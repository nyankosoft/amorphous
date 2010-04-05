#include "GraphicsResourceLoaders.hpp"
#include "GraphicsResourceCacheManager.hpp"
#include "AsyncResourceLoader.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Profile.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost;


//===================================================================================
// CGraphicsResourceLoader
//===================================================================================

Result::Name CGraphicsResourceLoader::Load()
{
	return LoadFromDisk();
}


Result::Name CGraphicsResourceLoader::LoadFromDisk()
{
	bool loaded = false;
	string target_filepath;
	const string src_filepath = GetSourceFilepath();

	if( is_db_filepath_and_keyname( src_filepath ) )
	{
		// decompose the string
		string db_filename, keyname;
		decompose_into_db_filepath_and_keyname( src_filepath, db_filename, keyname );

		string cwd = fnop::get_cwd();

		//if( !boost::filesystem::exists(  ) )
		//	return InvalidPath; db path is invalid - abort

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
		{
			LOG_PRINT_ERROR( "Failed to open the database file: " + db_filename );

			if( boost::filesystem::exists( db_filename ) )
				return Result::RESOURCE_IN_USE; // the database is being used by someone else - retry later
			else
				return Result::RESOURCE_NOT_FOUND;
		}

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( src_filepath );

		target_filepath = src_filepath;
	}

	if( !loaded )
		LOG_PRINT_ERROR( "Failed to load a graphics resource: " + src_filepath );

	return loaded ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


bool CGraphicsResourceLoader::AcquireResource()
{
	shared_ptr<CGraphicsResourceEntry> pHolder = GetResourceEntry();

	if( !pHolder )
		return false;

	if( pHolder->GetResource() )
	{
		// The graphics resource already exists
		// - Happens when mipmaps of textures are being loaded.
		// - No need to create a new resource or draw one from cache.
		return true;
	}

	shared_ptr<CGraphicsResource> pResource = GraphicsResourceCacheManager().GetCachedResource( *GetDesc() );

	if( pResource )
	{
		// set resource to entry (holder)
		pHolder->SetResource( pResource );

		return true;
	}
	else
	{
		// create resource instance from desc
		// - Actual resource creation is not done in this call
		pResource = GraphicsResourceFactory().CreateGraphicsResource( *GetDesc() );

		pHolder->SetResource( pResource );

		// desc has been set to the resource object
		//  - create the actual resource from the desc
		return pResource->Create();
	}
}


bool CGraphicsResourceLoader::Lock()
{
	LOG_FUNCTION_SCOPE();

	shared_ptr<CGraphicsResource> pResource = GetResource();
	if( pResource )
		return pResource->Lock();
	else
		return false;
}


bool CGraphicsResourceLoader::Unlock()
{
	LOG_FUNCTION_SCOPE();

	shared_ptr<CGraphicsResource> pResource = GetResource();
	if( pResource )
		return pResource->Unlock();
	else
		return false;
}


void CGraphicsResourceLoader::OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf )
{
	FillResourceDesc();

	// - send a lock request
	//   to copy the loaded resource to some graphics memory
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, pSelf, GetResourceEntry() );

	AsyncResourceLoader().AddGraphicsDeviceRequest( req );
}


void CGraphicsResourceLoader::OnResourceLoadedOnGraphicsMemory()
{
	if( GetResource() )
		GetResource()->SetState( GraphicsResourceState::LOADED );
}



//===================================================================================
// CDiskTextureLoader
//===================================================================================

static inline double log2( double scalar )
{
	return log( scalar ) / log( 2.0 );
}


shared_ptr<CTextureResource> CDiskTextureLoader::GetTextureResource()
{
	shared_ptr<CGraphicsResourceEntry> pEntry = GetResourceEntry();
	if( !pEntry )
		return shared_ptr<CTextureResource>();

	return pEntry->GetTextureResource();
}


bool CDiskTextureLoader::InitImageArray( boost::shared_ptr<CBitmapImage> pBaseImage )
{
	if( !pBaseImage )
		return false;

	CTextureResourceDesc& desc = m_TextureDesc;
	int num_mipmaps = 0;
	if( desc.MipLevels == 0 ) // complete mipmap chain
		num_mipmaps = min( (int)log2( (double)pBaseImage->GetWidth() ), (int)log2( (double)pBaseImage->GetHeight() ) );
	else if( 0 < desc.MipLevels )
		num_mipmaps = desc.MipLevels;
	else
		return false;

	if( num_mipmaps == 0 )
		return false;

	m_CurrentMipLevel = 0;

	m_vecpImage.resize( num_mipmaps );
	m_vecpImage[0] = pBaseImage;

	return true;
}


bool CDiskTextureLoader::CreateScaledImagesForMipmaps()
{
	int num_mipmaps = (int)m_vecpImage.size();
	for( int i=0; i<num_mipmaps - 1; i++ )
	{
		CBitmapImage& src_img = *m_vecpImage[i];
		m_vecpImage[i+1] = src_img.GetRescaled( src_img.GetWidth() / 2, src_img.GetHeight() / 2 );

		if( !m_vecpImage[i+1] )
			return false;
	}

	return true;
}


bool CDiskTextureLoader::LoadFromFile( const std::string& filepath )
{
	shared_ptr<CBitmapImage> pBaseImage( new CBitmapImage );
	bool image_loaded = pBaseImage->LoadFromFile( GetSourceFilepath() );

	if( !image_loaded )
		return false;

	bool res = InitImageArray( pBaseImage );
	if( !res )
		return false;

	return CreateScaledImagesForMipmaps();

	//>>> async loading debug
/*	bool loaded = m_Image.LoadFromFile( GetSourceFilepath() );
	if( loaded )
	{
		// See if the image is properly loaded
		string dest_filepath = GetSourceFilepath();
		fnop::append_to_body( dest_filepath, "_loaded" );
		m_Image.SaveToFile( dest_filepath );
	}
	return loaded;*/
	//<<< async loading debug
}


bool CDiskTextureLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	CImageArchive img_archive;
	bool retrieved = db.GetData( keyname, img_archive );
	if( retrieved )
	{
		shared_ptr<CBitmapImage> pBaseImage( new CBitmapImage );
		bool image_loaded = pBaseImage->CreateFromImageArchive( img_archive );

		if( !image_loaded )
			return false;

		bool res = InitImageArray( pBaseImage );
		if( !res )
			return false;

		return CreateScaledImagesForMipmaps();
	}
	else
		return false;
}


bool CDiskTextureLoader::CopyLoadedContentToGraphicsResource()
{
	shared_ptr<CTextureResource> pTexture = GetTextureResource();

	if( !pTexture )
		return false;

	shared_ptr<CLockedTexture> pLockedTexture;
	bool retrieved = pTexture->GetLockedTexture( pLockedTexture );
	if( retrieved && pLockedTexture )
	{
		FillTexture( *pLockedTexture );
		return true;
	}
	else
		return false;
}


bool CDiskTextureLoader::Lock()
{
	shared_ptr<CTextureResource> pTexture = GetTextureResource();

	if( !pTexture )
		return false;

	if( m_CurrentMipLevel < 0 || (int)m_vecpImage.size() <= m_CurrentMipLevel )
		return false;

	return pTexture->Lock( (uint)m_CurrentMipLevel );
}


void CDiskTextureLoader::FillResourceDesc()
{
	if( m_vecpImage.empty() || !m_vecpImage[0] )
		return;

	CBitmapImage& img = *m_vecpImage[0];

	m_TextureDesc.Width  = img.GetWidth();
	m_TextureDesc.Height = img.GetHeight();

	m_TextureDesc.Format = TextureFormat::A8R8G8B8;
}


void CDiskTextureLoader::FillTexture( CLockedTexture& texture )
{
	if( m_vecpImage.empty()
	 || (int)m_vecpImage.size() <= m_CurrentMipLevel
	 || !m_vecpImage[m_CurrentMipLevel] )
	{
		return;
	}

	CBitmapImage& img = *m_vecpImage[m_CurrentMipLevel];

//	LOG_PRINT_VERBOSE( fmt_string(" Filling a texture (size: %dx%d)", img.GetWidth(), img.GetHeight() ) );

//	if( !img.IsLoaded() )
//		return;

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = img.GetWidth();
	const int h = img.GetHeight();
	if( FreeImage_GetBPP( img.GetFBITMAP() ) == 24 )
	{
		// probably an image without alpha channel
		RGBQUAD quad;
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
//				FreeImage_GetPixelColor( img.GetFBITMAP(), x, y, &quad );
				FreeImage_GetPixelColor( img.GetFBITMAP(), x, h - y - 1, &quad );
				U32 argb32
					= 0xFF000000
					| quad.rgbRed   << 16
					| quad.rgbGreen <<  8
					| quad.rgbBlue;

				texture.SetPixelARGB32( x, y, argb32 );
//				texture.SetPixelARGB32( x, y, 0xFF00FF00 ); / debug
			}
		}
	}
	else
	{
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				texture.SetPixelARGB32( x, y, img.GetPixelARGB32(x,y) );
			}
		}
	}
}


void CDiskTextureLoader::OnResourceLoadedOnGraphicsMemory()
{
	m_CurrentMipLevel += 1;

	if( (int)m_vecpImage.size() <= m_CurrentMipLevel )
	{
		// Loaded all the mipmaps
		CGraphicsResourceLoader::OnResourceLoadedOnGraphicsMemory();
	}
	else
	{
		// Load the next mipmap
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, m_pSelf.lock(), GetResourceEntry() );
		AsyncResourceLoader().AddGraphicsDeviceRequest( req );
	}
}



//===================================================================================
// CMeshLoader
//===================================================================================

CMeshLoader::CMeshLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry, const CMeshResourceDesc& desc )
:
CGraphicsResourceLoader(pEntry),
m_MeshDesc(desc),
m_MeshLoaderStateFlags(0),
m_pVertexBufferContent(NULL)
{
	m_pArchive = boost::shared_ptr<C3DMeshModelArchive>( new C3DMeshModelArchive() );
}


CMeshLoader::~CMeshLoader()
{
	SafeDelete( m_pVertexBufferContent );
}


bool CMeshLoader::LoadFromFile( const std::string& filepath )
{
	// load mesh archive from file
	bool res = false;
	if( m_pArchive )
		res = m_pArchive->LoadFromFile( GetSourceFilepath() );

	if( !res )
		return false;

	// load vertices, vertex size, vertex elements
	// from the vertex set of the mesh archive

	// load indices

	LoadMeshSubresources();

	return true;
}


bool CMeshLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	return db.GetData( keyname, *m_pArchive );
}


bool CMeshLoader::CopyLoadedContentToGraphicsResource()
{
	// Done by sub resource loaders

	if( !GetResourceEntry() )
		return false;
/*
	CMeshResource *pMesh = GetResourceEntry()->GetMeshResource();
	if( !pMeshEntry )
		return false;

	CD3DXMeshObjectBase *pMesh = pMeshEntry->GetMeshObject();
	if( !pMesh )
		return false;
*/
	// copy vertices to VB / IB?

//	pMesh->


	return true;
}


bool CMeshLoader::AcquireResource()
{
	bool res = CGraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	GetResourceEntry()->GetMeshResource()->CreateMeshAndLoadNonAsyncResources( *(m_pArchive.get()) );

	return true;
}


/// Called by I/O thread after the mesh archive is loaded and stored to 'm_pArchive'
/// - Usually loaded from disk
void CMeshLoader::OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf )
{
	// change this to true if async loading is fixed
	bool preferred_async_loading_method = false;

	if( !preferred_async_loading_method )
	{
		LOG_PRINT( "Sending a LoadToGraphicsMemoryByRenderThread request for a mesh: " + m_MeshDesc.ResourcePath );
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::LoadToGraphicsMemoryByRenderThread, pSelf, GetResourceEntry() );
		AsyncResourceLoader().AddGraphicsDeviceRequest( req );
		return;
	}

	// - send a lock request
	//   - Actually does not lock. Send the lock request to call AcquireResource.
	//   - Must be processed before the lock requrests of the subresources below.
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, pSelf, GetResourceEntry() );
	AsyncResourceLoader().AddGraphicsDeviceRequest( req );

	// create subresource loaders
	shared_ptr<CD3DXMeshLoaderBase> apLoader[3];
	apLoader[0] = shared_ptr<CD3DXMeshVerticesLoader>( new CD3DXMeshVerticesLoader(GetResourceEntry()) );
	apLoader[1] = shared_ptr<CD3DXMeshIndicesLoader>( new CD3DXMeshIndicesLoader(GetResourceEntry()) );
	apLoader[2] = shared_ptr<CD3DXMeshAttributeTableLoader>( new CD3DXMeshAttributeTableLoader(GetResourceEntry()) );

	for( int i=0; i<3; i++ )
	{
		// set the pointer to mesh archive to each subresource loader
		apLoader[i]->m_pArchive   = m_pArchive;
//		apLoader[i]->m_pMeshEntry = m_pMeshEntry;

		apLoader[i]->m_pMeshLoader = m_pSelf.lock();

		// add requests to load subresource from the mesh archive
//		CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, apLoader[i], GetResourceEntry() );
//		AsyncResourceLoader().AddResourceLoadRequest( req );

		// subresources have been loaded
		// - send lock requests for each mesh sub resource
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, apLoader[i], GetResourceEntry() );
		AsyncResourceLoader().AddGraphicsDeviceRequest( req );
	}

	// create mesh instance
	// load resources that needs to be loaded synchronously
//	GetResourceEntry()->GetMeshResource()->CreateMeshAndLoadNonAsyncResources( *(m_pArchive.get()) );
}


void CMeshLoader::OnResourceLoadedOnGraphicsMemory()
{
	// Do nothing
	// - Avoid setting the resource state to GraphicsResourceState::LOADED
	// - Need to wait until all the subresources are locked, copied, and unlocked.
}


void CMeshLoader::LoadMeshSubresources()
{
	if( m_pArchive )
	{
		m_MeshDesc.NumVertices = m_pArchive->GetVertexSet().GetNumVertices();
		m_MeshDesc.NumIndices  = (int)m_pArchive->GetNumVertexIndices();

		m_MeshDesc.VertexFormatFlags = m_pArchive->GetVertexSet().m_VertexFormatFlag;

		LoadVerticesForD3DXMesh(
			m_pArchive->GetVertexSet(),
			m_MeshDesc.vecVertElement,
			m_MeshDesc.VertexSize,
			m_pVertexBufferContent
			);

//		SafeDelete( pVertexBufferContent );

		LoadIndices( *(m_pArchive.get()), m_vecIndexBufferContent );

		GetAttributeTableFromTriangleSet( m_pArchive->GetTriangleSet(), m_vecAttributeRange );
	}
}


void CMeshLoader::RaiseStateFlags( U32 flags )
{
	m_MeshLoaderStateFlags |= flags;
}


void CMeshLoader::SendLockRequestIfAllSubresourcesHaveBeenLoaded()
{
	if( m_MeshLoaderStateFlags & VERTICES_LOADED
	 && m_MeshLoaderStateFlags & INDICES_LOADED
	 && m_MeshLoaderStateFlags & ATTRIB_TABLES_LOADED )
	{
		return;
	}
	else
	{
		return;
	}
}


bool CMeshLoader::LoadToGraphicsMemoryByRenderThread()
{
	LOG_PRINT( "Loading a mesh: " + m_MeshDesc.ResourcePath );

	if( !m_pArchive )
		return false;

	// Make the system create an empty mesh instance by settings vertices and indices to zeros 
	m_MeshDesc.NumVertices = 0;
	m_MeshDesc.NumIndices = 0;

	bool res = CGraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	shared_ptr<CGraphicsResourceEntry> pHolder = GetResourceEntry();
	if( !pHolder )
		return false;

	shared_ptr<CMeshResource> pMeshResource = pHolder->GetMeshResource();
	if( !pMeshResource )
		return false;
/*
	shared_ptr<CD3DXMeshObjectBase> pMesh = pMeshResource->GetMesh();
	if( pMesh )
		return pMesh->LoadFromArchive( *(m_pArchive.get()), m_MeshDesc.ResourcePath );
	else
		return false;*/

	res = pMeshResource->LoadMeshFromArchive( *(m_pArchive.get()) );

	return res;
}


//===================================================================================
// CD3DXMeshLoaderBase
//===================================================================================

CD3DXMeshObjectBase *CD3DXMeshLoaderBase::GetD3DMeshImpl()
{
	return dynamic_cast<CD3DXMeshObjectBase *>( GetMesh() );
}


//===================================================================================
// CD3DXMeshVerticesLoader
//===================================================================================

Result::Name CD3DXMeshLoaderBase::Load()
{
	return ( LoadFromArchive() ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}



//===================================================================================
// CD3DXMeshVerticesLoader
//===================================================================================

bool CD3DXMeshVerticesLoader::LoadFromArchive()
{
/*	// load the vertices from the mesh archive (m_pArchive), and store it
	// to the buffer (m_pVertexBufferContent).
	GetMesh()->LoadVertices( m_pVertexBufferContent, *(m_pArchive.get()) );

//	LoadVerticesForD3DXMesh( m_pArchive->GetVertexSet(), elems, size, dest_buffer );

	m_pMeshLoader->RaiseStateFlags( CMeshLoader::VERTICES_LOADED );
	m_pMeshLoader->SendLockRequestIfAllSubresourcesHaveBeenLoaded();
*/
	return true;
}


bool CD3DXMeshVerticesLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( m_pLockedVertexBuffer )
	{
//		memcpy( m_pLockedVertexBuffer, m_pVertexBufferContent, GetMesh()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );

		memcpy( m_pLockedVertexBuffer,
			m_pMeshLoader->VertexBufferContent(),
			GetD3DMeshImpl()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );
	}

	return true;
}


bool CD3DXMeshVerticesLoader::Lock()
{
	LOG_PRINT( "" );

	CD3DXMeshObjectBase *pMesh = GetD3DMeshImpl();
	if( pMesh )
		return pMesh->LockVertexBuffer( m_pLockedVertexBuffer );
	else
		return false;
}


bool CD3DXMeshVerticesLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockVertexBuffer();
	m_pLockedVertexBuffer = NULL;
	return unlocked;
}


void CD3DXMeshVerticesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( CMeshSubResource::VERTEX, GraphicsResourceState::LOADED );
}


bool CD3DXMeshVerticesLoader::IsReadyToLock() const
{
	return true;
}



//===================================================================================
// CD3DXMeshIndicesLoader
//===================================================================================

bool CD3DXMeshIndicesLoader::LoadFromArchive()
{
/*	unsigned short *pIBData;
	GetMesh()->LoadIndices( pIBData, *(m_pArchive.get()) );
	m_pIndexBufferContent = (void *)pIBData;

	m_pMeshLoader->RaiseStateFlags( CMeshLoader::INDICES_LOADED );
	m_pMeshLoader->SendLockRequestIfAllSubresourcesHaveBeenLoaded();
*/
	return true;
}


bool CD3DXMeshIndicesLoader::Lock()
{
	LOG_PRINT( "" );

	CD3DXMeshObjectBase *pMesh = GetD3DMeshImpl();
	if( pMesh )
		return pMesh->LockIndexBuffer( m_pLockedIndexBuffer );
	else
		return false;
}


bool CD3DXMeshIndicesLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockIndexBuffer();
	m_pLockedIndexBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshIndicesLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( m_pLockedIndexBuffer )
	{
//		memcpy( m_pLockedIndexBuffer, m_pIndexBufferContent, m_IndexBufferSize );

		memcpy( m_pLockedIndexBuffer,
			&(m_pMeshLoader->IndexBufferContent()[0]),
			m_pMeshLoader->IndexBufferContent().size() );
	}


	return true;
}


void CD3DXMeshIndicesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( CMeshSubResource::INDEX, GraphicsResourceState::LOADED );
}


bool CD3DXMeshIndicesLoader::IsReadyToLock() const
{
	return GetSubResourceState( CMeshSubResource::VERTEX ) == GraphicsResourceState::LOADED;
}



//===================================================================================
// CD3DXMeshAttributeTableLoader
//===================================================================================

bool CD3DXMeshAttributeTableLoader::Lock()
{
	LOG_PRINT( "" );

	HRESULT hr = GetMesh()->GetMesh()->SetAttributeTable(
		&(m_pMeshLoader->AttributeTable()[0]),
		(DWORD)(m_pMeshLoader->AttributeTable().size()) );

	bool locked = GetD3DMeshImpl()->LockAttributeBuffer( m_pLockedAttributeBuffer );
	return locked;
}


bool CD3DXMeshAttributeTableLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockAttributeBuffer();
	m_pLockedAttributeBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshAttributeTableLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( !m_pLockedAttributeBuffer )
		return false;

	const vector<CMMA_TriangleSet>& vecTriangleSet = m_pArchive->GetTriangleSet();

	DWORD *pdwBuffer = m_pLockedAttributeBuffer;
	DWORD face = 0;
	const int num_materials = GetMesh()->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i];

		DWORD face_start = triangle_set.m_iStartIndex / 3;
		DWORD num_faces = triangle_set.m_iNumTriangles;
		for( face=face_start; face<face_start + num_faces; face++ )
		{
			pdwBuffer[face] = i;
		}
	}

	return true;
}


void CD3DXMeshAttributeTableLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( CMeshSubResource::ATTRIBUTE_TABLE, GraphicsResourceState::LOADED );
}


bool CD3DXMeshAttributeTableLoader::IsReadyToLock() const
{
	return GetSubResourceState( CMeshSubResource::INDEX ) == GraphicsResourceState::LOADED;
}



/// Cannot create an empty shader object and copy the content to it.
/// - Load from file
bool CShaderLoader::AcquireResource()
{
	shared_ptr<CGraphicsResourceEntry> pHolder = GetResourceEntry();

	if( !pHolder )
		return false;

	pHolder->SetResource( GraphicsResourceFactory().CreateGraphicsResource( m_ShaderDesc ) );

	shared_ptr<CShaderResource> pShaderResource = pHolder->GetShaderResource();
	if( !pShaderResource )
		return false;

	bool loaded = pShaderResource->LoadFromFile( m_ShaderDesc.ResourcePath );

	return loaded;
}


bool CShaderLoader::LoadFromFile( const std::string& filepath )
{
	bool res = m_ShaderTextBuffer.LoadTextFile( filepath );

	return res;
}


void CShaderLoader::OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf )
{
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::LoadToGraphicsMemoryByRenderThread, pSelf, GetResourceEntry() );
	AsyncResourceLoader().AddGraphicsDeviceRequest( req );
	return;
}


bool CShaderLoader::LoadToGraphicsMemoryByRenderThread()
{
	bool res = CGraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	shared_ptr<CGraphicsResourceEntry> pHolder = GetResourceEntry();
	if( !pHolder )
		return false;

	shared_ptr<CShaderResource> pShaderResource = pHolder->GetShaderResource();
	if( !pShaderResource )
		return false;

//	pShaderResource->CreateShaderFromTextBuffer( m_ShaderTextBuffer );

	res = pShaderResource->LoadFromFile( m_ShaderDesc.ResourcePath );

	return res;

/*	CShaderManager *pShader = pShaderResource->GetShaderManager();
	if( pShader )
		return pShader->LoadShaderFromText( m_ShaderTextBuffer );
	else
		return false;*/
}
