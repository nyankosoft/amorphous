#include "GraphicsResourceLoaders.hpp"
#include "GraphicsResourceCacheManager.hpp"
#include "AsyncResourceLoader.hpp"
#include "D3DXMeshObjectBase.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Support/Profile.hpp"

using namespace std;
using namespace boost;


//===================================================================================
// CGraphicsResourceLoader
//===================================================================================

bool CGraphicsResourceLoader::Load()
{
	return LoadFromDisk();
}


bool CGraphicsResourceLoader::LoadFromDisk()
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
			return false; // the database is being used by someone else - retry later

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( src_filepath );

		target_filepath = src_filepath;
	}

	return loaded;
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

bool CDiskTextureLoader::LoadFromFile( const std::string& filepath )
{
	return m_Image.LoadFromFile( GetSourceFilepath() );

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
		bool image_loaded = m_Image.CreateFromImageArchive( img_archive );
		return image_loaded;
	}
	else
		return false;
}


bool CDiskTextureLoader::CopyLoadedContentToGraphicsResource()
{
	shared_ptr<CGraphicsResourceEntry> pEntry = GetResourceEntry();
	if( !pEntry )
		return false;

	shared_ptr<CTextureResource> pTexture = pEntry->GetTextureResource();

	if( !pTexture )
		return false;

	CLockedTexture locked_texture;
	bool retrieved = pTexture->GetLockedTexture( locked_texture );
	if( retrieved )
	{
		FillTexture( locked_texture );
		return true;
	}
	else
		return false;
}


void CDiskTextureLoader::FillResourceDesc()
{
	m_TextureDesc.Width  = m_Image.GetWidth();
	m_TextureDesc.Height = m_Image.GetHeight();

	m_TextureDesc.Format = TextureFormat::A8R8G8B8;
}


bool CDiskTextureLoader::AcquireResource()
{
	shared_ptr<CGraphicsResourceEntry> pHolder = GetResourceEntry();

	if( !pHolder )
		return false;

	shared_ptr<CGraphicsResource> pResource = GraphicsResourceCacheManager().GetCachedResource( m_TextureDesc );

	if( pResource )
	{
		// set texture resource to entry (holder)
		pHolder->SetResource( pResource );

		return true;
	}
	else
	{
		// create resource instance from desc
		// - Actual resource creation is not done in this call
		pResource = GraphicsResourceFactory().CreateGraphicsResource( m_TextureDesc );

		pHolder->SetResource( pResource );

		// desc has been set to the resource object
		//  - create the actual resource from the desc
		return pResource->Create();
	}
}


void CDiskTextureLoader::FillTexture( CLockedTexture& texture )
{
//	if( !m_Image.IsLoaded() )
//		return;

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = m_Image.GetWidth();
	const int h = m_Image.GetHeight();
	if( FreeImage_GetBPP( m_Image.GetFBITMAP() ) == 24 )
	{
		// probably an image without alpha channel
		RGBQUAD quad;
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
//				FreeImage_GetPixelColor( m_Image.GetFBITMAP(), x, y, &quad );
				FreeImage_GetPixelColor( m_Image.GetFBITMAP(), x, h - y - 1, &quad );
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
				texture.SetPixelARGB32( x, y, m_Image.GetPixelARGB32(x,y) );
			}
		}
	}
}


//===================================================================================
// CMeshLoader
//===================================================================================

CMeshLoader::CMeshLoader( boost::weak_ptr<CGraphicsResourceEntry> pEntry, const CMeshResourceDesc& desc )
:
CGraphicsResourceLoader(pEntry),
m_MeshDesc(desc)
{
	m_pArchive = boost::shared_ptr<C3DMeshModelArchive>( new C3DMeshModelArchive() );
}


bool CMeshLoader::LoadFromFile( const std::string& filepath )
{
	if( m_pArchive )
		return m_pArchive->LoadFromFile( GetSourceFilepath() );
	else
		return false;
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


void CMeshLoader::OnLoadingCompleted( boost::shared_ptr<CGraphicsResourceLoader> pSelf )
{
	FillResourceDesc();

	shared_ptr<CD3DXMeshLoaderBase> apLoader[3];
	apLoader[0] = shared_ptr<CD3DXMeshVerticesLoader>( new CD3DXMeshVerticesLoader(GetResourceEntry()) );
	apLoader[1] = shared_ptr<CD3DXMeshIndicesLoader>( new CD3DXMeshIndicesLoader(GetResourceEntry()) );
	apLoader[2] = shared_ptr<CD3DXMeshAttributeTableLoader>( new CD3DXMeshAttributeTableLoader(GetResourceEntry()) );

	for( int i=0; i<3; i++ )
	{
		apLoader[i]->m_pArchive   = m_pArchive;
//		apLoader[i]->m_pMeshEntry = m_pMeshEntry;
		CResourceLoadRequest req( CResourceLoadRequest::LoadFromDisk, apLoader[i], GetResourceEntry() );
		AsyncResourceLoader().AddResourceLoadRequest( req );
	}
}


void CMeshLoader::OnResourceLoadedOnGraphicsMemory()
{
	// Do nothing
	// - Avoid setting the resource state to GraphicsResourceState::LOADED
	// - Need to wait until all the subresources are locked, copied, and unlocked.
}


void CMeshLoader::FillResourceDesc()
{
	if( m_pArchive )
	{
		m_MeshDesc.NumVertices = m_pArchive->GetVertexSet().GetNumVertices();
		m_MeshDesc.NumIndices  = (int)m_pArchive->GetNumVertexIndices();

		m_MeshDesc.VertexFormatFlags = m_pArchive->GetVertexSet().m_VertexFormatFlag;

		vector<D3DVERTEXELEMENT9> vecVertElement; // buffer to temporarily hold vertex elements

		void *pVertexBufferContent = NULL;

		LoadVerticesForD3DXMesh(
			m_pArchive->GetVertexSet(),
			m_MeshDesc.vecVertElement,
			m_MeshDesc.VertexSize,
			pVertexBufferContent
			);

		SafeDelete( pVertexBufferContent );
	}
}



//===================================================================================
// CD3DXMeshVerticesLoader
//===================================================================================

bool CD3DXMeshVerticesLoader::LoadFromArchive()
{
	GetMesh()->LoadVertices( m_pVertexBufferContent, *(m_pArchive.get()) );

	return true;
}


bool CD3DXMeshVerticesLoader::CopyLoadedContentToGraphicsResource()
{
	if( m_pLockedVertexBuffer )
		memcpy( m_pLockedVertexBuffer, m_pVertexBufferContent, GetMesh()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );

	return true;
}


bool CD3DXMeshVerticesLoader::Lock()
{
	bool locked = GetMesh()->LockVertexBuffer( m_pLockedVertexBuffer );
	return locked;
}


bool CD3DXMeshVerticesLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockVertexBuffer();
	m_pLockedVertexBuffer = NULL;
	return unlocked;
}


void CD3DXMeshVerticesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( CMeshSubResource::VERTEX, GraphicsResourceState::LOADED );
}



//===================================================================================
// CD3DXMeshIndicesLoader
//===================================================================================

bool CD3DXMeshIndicesLoader::LoadFromArchive()
{
	unsigned short *pIBData;
	GetMesh()->LoadIndices( pIBData, *(m_pArchive.get()) );
	m_pIndexBufferContent = (void *)pIBData;
	return true;
}


bool CD3DXMeshIndicesLoader::Lock()
{
	bool locked = GetMesh()->LockIndexBuffer( m_pLockedIndexBuffer );
	return locked;
}


bool CD3DXMeshIndicesLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockIndexBuffer();
	m_pLockedIndexBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshIndicesLoader::CopyLoadedContentToGraphicsResource()
{
	if( m_pLockedIndexBuffer )
		memcpy( m_pLockedIndexBuffer, m_pIndexBufferContent, m_IndexBufferSize );

	return true;
}


void CD3DXMeshIndicesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( CMeshSubResource::INDEX, GraphicsResourceState::LOADED );
}



//===================================================================================
// CD3DXMeshAttributeTableLoader
//===================================================================================

bool CD3DXMeshAttributeTableLoader::Lock()
{
//	HRESULT hr = GetMesh()->SetAttributeTable( paAttribTable, GetMesh()->GetNumMaterials() );

//	SafeDeleteArray( paAttribTable );

	bool locked = GetMesh()->LockAttributeBuffer( m_pLockedAttributeBuffer );
	return locked;
}


bool CD3DXMeshAttributeTableLoader::Unlock()
{
	bool unlocked = GetMesh()->UnlockAttributeBuffer();
	m_pLockedAttributeBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshAttributeTableLoader::CopyLoadedContentToGraphicsResource()
{
	if( !m_pLockedAttributeBuffer )
		return false;

	const vector<CMMA_TriangleSet>& vecTriangleSet = m_pArchive->GetTriangleSet();

	DWORD *pdwBuffer = m_pLockedAttributeBuffer;
	DWORD face = 0;
	for( int i=0; i<GetMesh()->GetNumMaterials(); i++ )
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
