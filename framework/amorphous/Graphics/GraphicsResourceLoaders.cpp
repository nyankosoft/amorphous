#include "GraphicsResourceLoaders.hpp"
#include "GraphicsResourceCacheManager.hpp"
#include "AsyncResourceLoader.hpp"
#include "amorphous/base.hpp"
#include "amorphous/Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "amorphous/Graphics/TextureGenerators/TextureFillingAlgorithm.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/ImageArchiveAux.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/lfs.hpp"


namespace amorphous
{

using namespace std;


//===================================================================================
// GraphicsResourceLoader
//===================================================================================

Result::Name GraphicsResourceLoader::Load()
{
	const GraphicsResourceDesc *desc = this->GetDesc();
	if(desc) {
		if(desc->IsDiskResource())
			return LoadFromDisk();
		else
			return LoadFromDisk(); // TODO: create the resource from the desc
	}
	else
	{
		LOG_PRINT_ERROR("!desc");
		return Result::UNKNOWN_ERROR;
	}
}


Result::Name GraphicsResourceLoader::LoadFromDisk()
{
	bool loaded = false;
	string target_filepath;
	const string src_filepath = GetSourceFilepath();

	LOG_PRINT("src_filepath: " + src_filepath);

	if( is_db_filepath_and_keyname( src_filepath ) )
	{
		// decompose the string
		string db_filename, keyname;
		decompose_into_db_filepath_and_keyname( src_filepath, db_filename, keyname );

		string cwd = lfs::get_cwd();

		//if( !boost::filesystem::exists(  ) )
		//	return InvalidPath; db path is invalid - abort

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
		{
			LOG_PRINT_ERROR( "Failed to open the database file: " + db_filename );

			if( lfs::path_exists( db_filename ) )
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


bool GraphicsResourceLoader::AcquireResource()
{
	shared_ptr<GraphicsResourceEntry> pHolder = GetResourceEntry();

	if( !pHolder )
		return false;

	if( pHolder->GetResource() )
	{
		// The graphics resource already exists
		// - Happens when mipmaps of textures are being loaded.
		// - No need to create a new resource or draw one from cache.
		return true;
	}

	shared_ptr<GraphicsResource> pResource = GraphicsResourceCacheManager().GetCachedResource( *GetDesc() );

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
		pResource = GetGraphicsResourceFactory().CreateGraphicsResource( *GetDesc() );

		pHolder->SetResource( pResource );

		// desc has been set to the resource object
		//  - create the actual resource from the desc
		return pResource->Create();
	}
}


bool GraphicsResourceLoader::Lock()
{
	LOG_FUNCTION_SCOPE();

	shared_ptr<GraphicsResource> pResource = GetResource();
	if( pResource )
		return pResource->Lock();
	else
		return false;
}


bool GraphicsResourceLoader::Unlock()
{
	LOG_FUNCTION_SCOPE();

	shared_ptr<GraphicsResource> pResource = GetResource();
	if( pResource )
		return pResource->Unlock();
	else
		return false;
}


void GraphicsResourceLoader::OnLoadingCompleted( std::shared_ptr<GraphicsResourceLoader> pSelf )
{
	FillResourceDesc();

	// - send a lock request
	//   to copy the loaded resource to some graphics memory
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, pSelf, GetResourceEntry() );

	GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );
}


void GraphicsResourceLoader::OnResourceLoadedOnGraphicsMemory()
{
	if( GetResource() )
		GetResource()->SetState( GraphicsResourceState::LOADED );
}


bool GraphicsResourceLoader::LoadResourceAndCreateGraphicsResource()
{
	bool res = false;
	if( GetResource() )
		return GetResource()->Load();
	else
	{
		LOG_PRINT_ERROR("!GetResource()");
		return false;
	}
}



//===================================================================================
// DiskTextureLoader
//===================================================================================

static inline double log2( double scalar )
{
	return log( scalar ) / log( 2.0 );
}


shared_ptr<TextureResource> DiskTextureLoader::GetTextureResource()
{
	shared_ptr<GraphicsResourceEntry> pEntry = GetResourceEntry();
	if( !pEntry )
		return shared_ptr<TextureResource>();

	return pEntry->GetTextureResource();
}


bool DiskTextureLoader::InitImageArray( std::shared_ptr<BitmapImage> pBaseImage )
{
	if( !pBaseImage )
		return false;

	TextureResourceDesc& desc = m_TextureDesc;
	int num_mipmaps = 0;
	if( desc.MipLevels == 0 ) // complete mipmap chain
		num_mipmaps = take_min( (int)log2( (double)pBaseImage->GetWidth() ), (int)log2( (double)pBaseImage->GetHeight() ) );
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


bool DiskTextureLoader::CreateScaledImagesForMipmaps()
{
	int num_mipmaps = (int)m_vecpImage.size();
	for( int i=0; i<num_mipmaps - 1; i++ )
	{
		BitmapImage& src_img = *m_vecpImage[i];
		m_vecpImage[i+1] = src_img.GetRescaled( src_img.GetWidth() / 2, src_img.GetHeight() / 2 );

		if( !m_vecpImage[i+1] )
			return false;
	}

	return true;
}


bool DiskTextureLoader::LoadFromFile( const std::string& filepath )
{
	shared_ptr<BitmapImage> pBaseImage( new BitmapImage );
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


bool DiskTextureLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	ImageArchive img_archive;
	bool retrieved = db.GetData( keyname, img_archive );
	if( retrieved )
	{
		shared_ptr<BitmapImage> pBaseImage = CreateBitmapImageFromImageArchive( img_archive );

		if( !pBaseImage )
			return false;

		bool res = InitImageArray( pBaseImage );
		if( !res )
			return false;

		return CreateScaledImagesForMipmaps();
	}
	else
		return false;
}


bool DiskTextureLoader::CopyLoadedContentToGraphicsResource()
{
	shared_ptr<TextureResource> pTexture = GetTextureResource();

	if( !pTexture )
		return false;

	shared_ptr<LockedTexture> pLockedTexture;
	bool retrieved = pTexture->GetLockedTexture( pLockedTexture );
	if( retrieved && pLockedTexture )
	{
		FillTexture( *pLockedTexture );
		return true;
	}
	else
		return false;
}


bool DiskTextureLoader::Lock()
{
	shared_ptr<TextureResource> pTexture = GetTextureResource();

	if( !pTexture )
		return false;

	if( m_CurrentMipLevel < 0 || (int)m_vecpImage.size() <= m_CurrentMipLevel )
		return false;

	return pTexture->Lock( (uint)m_CurrentMipLevel );
}


void DiskTextureLoader::FillResourceDesc()
{
	if( m_vecpImage.empty() || !m_vecpImage[0] )
		return;

	BitmapImage& img = *m_vecpImage[0];

	m_TextureDesc.Width  = img.GetWidth();
	m_TextureDesc.Height = img.GetHeight();

	m_TextureDesc.Format = TextureFormat::A8R8G8B8;
}


void DiskTextureLoader::FillTexture( LockedTexture& texture )
{
	if( m_vecpImage.empty()
	 || (int)m_vecpImage.size() <= m_CurrentMipLevel
	 || !m_vecpImage[m_CurrentMipLevel] )
	{
		return;
	}

	BitmapImage& img = *m_vecpImage[m_CurrentMipLevel];

//	LOG_PRINT_VERBOSE( fmt_string(" Filling a texture (size: %dx%d)", img.GetWidth(), img.GetHeight() ) );

//	if( !img.IsLoaded() )
//		return;

//	texture.Clear( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.0f ) );

	const int w = img.GetWidth();
	const int h = img.GetHeight();
	//const int bits_per_pixel = FreeImage_GetBPP( img.GetFBITMAP() );
	const int bits_per_pixel = img.GetBitsPerPixel();
	if( bits_per_pixel == 24 )
	{
		// probably an image without alpha channel
		//RGBQUAD quad;
		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
//				FreeImage_GetPixelColor( img.GetFBITMAP(), x, y, &quad );
/*				FreeImage_GetPixelColor( img.GetFBITMAP(), x, h - y - 1, &quad );
				U32 argb32
					= 0xFF000000
					| quad.rgbRed   << 16
					| quad.rgbGreen <<  8
					| quad.rgbBlue;
*/
				U8 r=0, g=0, b=0, a=0;
				img.GetPixel( x, h - y - 1, r, g, b );
				U32 argb32 = 0xFF000000 | r << 16 | g <<  8 | b;

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


void DiskTextureLoader::OnResourceLoadedOnGraphicsMemory()
{
	m_CurrentMipLevel += 1;

	if( (int)m_vecpImage.size() <= m_CurrentMipLevel )
	{
		// Loaded all the mipmaps
		GraphicsResourceLoader::OnResourceLoadedOnGraphicsMemory();
	}
	else
	{
		// Load the next mipmap
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, m_pSelf.lock(), GetResourceEntry() );
		GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );
	}
}



//===================================================================================
// MeshLoader
//===================================================================================

MeshLoader::MeshLoader( std::weak_ptr<GraphicsResourceEntry> pEntry, const MeshResourceDesc& desc )
:
GraphicsResourceLoader(pEntry),
m_MeshDesc(desc),
m_MeshLoaderStateFlags(0)
{
	m_pArchive = std::shared_ptr<C3DMeshModelArchive>( new C3DMeshModelArchive() );
}


MeshLoader::~MeshLoader()
{
}


bool MeshLoader::LoadFromFile( const std::string& filepath )
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


bool MeshLoader::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	return db.GetData( keyname, *m_pArchive );
}


bool MeshLoader::CopyLoadedContentToGraphicsResource()
{
	// Done by sub resource loaders

	if( !GetResourceEntry() )
		return false;
/*
	MeshResource *pMesh = GetResourceEntry()->GetMeshResource();
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


bool MeshLoader::AcquireResource()
{
	bool res = GraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	GetResourceEntry()->GetMeshResource()->CreateMeshAndLoadNonAsyncResources( *(m_pArchive.get()) );

	return true;
}


void MeshLoader::RaiseStateFlags( U32 flags )
{
	m_MeshLoaderStateFlags |= flags;
}


void MeshLoader::SendLockRequestIfAllSubresourcesHaveBeenLoaded()
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


bool MeshLoader::LoadToGraphicsMemoryByRenderThread()
{
	LOG_PRINT( "Loading a mesh: " + m_MeshDesc.ResourcePath );

	if( !m_pArchive )
		return false;

	// Make the system create an empty mesh instance by settings vertices and indices to zeros 
	m_MeshDesc.NumVertices = 0;
	m_MeshDesc.NumIndices = 0;

	bool res = GraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	shared_ptr<GraphicsResourceEntry> pHolder = GetResourceEntry();
	if( !pHolder )
		return false;

	shared_ptr<MeshResource> pMeshResource = pHolder->GetMeshResource();
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



/// Cannot create an empty shader object and copy the content to it.
/// - Load from file
bool ShaderLoader::AcquireResource()
{
	shared_ptr<GraphicsResourceEntry> pHolder = GetResourceEntry();

	if( !pHolder )
		return false;

	pHolder->SetResource( GetGraphicsResourceFactory().CreateGraphicsResource( m_ShaderDesc ) );

	shared_ptr<ShaderResource> pShaderResource = pHolder->GetShaderResource();
	if( !pShaderResource )
		return false;

	bool loaded = pShaderResource->LoadFromFile( m_ShaderDesc.ResourcePath );

	return loaded;
}


bool ShaderLoader::LoadFromFile( const std::string& filepath )
{
	bool res = m_ShaderTextBuffer.LoadTextFile( filepath );

	return res;
}


void ShaderLoader::OnLoadingCompleted( std::shared_ptr<GraphicsResourceLoader> pSelf )
{
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::LoadToGraphicsMemoryByRenderThread, pSelf, GetResourceEntry() );
	GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );
	return;
}


bool ShaderLoader::LoadToGraphicsMemoryByRenderThread()
{
	bool res = GraphicsResourceLoader::AcquireResource();
	if( !res )
		return false;

	shared_ptr<GraphicsResourceEntry> pHolder = GetResourceEntry();
	if( !pHolder )
		return false;

	shared_ptr<ShaderResource> pShaderResource = pHolder->GetShaderResource();
	if( !pShaderResource )
		return false;

//	pShaderResource->CreateShaderFromTextBuffer( m_ShaderTextBuffer );

	res = pShaderResource->LoadFromFile( m_ShaderDesc.ResourcePath );

	return res;

/*	ShaderManager *pShader = pShaderResource->GetShaderManager();
	if( pShader )
		return pShader->LoadShaderFromText( m_ShaderTextBuffer );
	else
		return false;*/
}


} // namespace amorphous
