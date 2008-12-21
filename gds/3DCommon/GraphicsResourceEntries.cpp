#include "GraphicsResourceEntries.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DXMeshObjectBase.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "Support/SafeDelete.h"
#include "Support/fnop.h"
#include "Support/ImageArchive.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

using namespace std;
using namespace boost;


static inline D3DXIMAGE_FILEFORMAT ArchiveImgFmt2D3DImgFmt( CImageArchive::ImageFormat img_archive_format )
{
	switch(img_archive_format)
	{
	case CImageArchive::IMGFMT_BMP24: return D3DXIFF_BMP;
	case CImageArchive::IMGFMT_BMP32: return D3DXIFF_BMP;
	case CImageArchive::IMGFMT_JPEG:  return D3DXIFF_JPG;
	case CImageArchive::IMGFMT_TGA:   return D3DXIFF_TGA;
	case CImageArchive::IMGFMT_PNG:   return D3DXIFF_PNG;
//	case CImageArchive::IMGFMT_ : return D3DXIFF_DDS,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_PPM,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_DIB,
//	case CImageArchive::IMGFMT_ : return D3DXIFF_HDR,       //high dynamic range formats
//	case CImageArchive::IMGFMT_ : return D3DXIFF_PFM,       //
	default: return D3DXIFF_BMP;
	}

	return D3DXIFF_BMP;
}


/**
 Graphics Resource
 - Graphics resources are not sharable if the loading modes are different
   even if all the other properties are the same.
 Rationale:
 - Sharing resources with different loading methods complicates loading process.
   - What if the user tries to load a resource(A) synchronously and the same resource(B) is being loaded asynchronously.
     1. The user wants to load (A) synchronously. i.e., no rendering before loading the resource
     2. (B) is being loaded asynchronously. If the system simply cancel the synchronous loading of (A),
	    some rendering may be done before loading (B).
		-> User's request mentioned in 1. is not satisfied.

*/


inline bool str_includes( const std::string& src, const std::string& target )
{
	if( src.find(target.c_str()) != std::string::npos )
		return true;
	else
		return false;
}



//==================================================================================================
// CGraphicsResourceEntry
//==================================================================================================

void CGraphicsResourceEntry::GetStatus( char *pDestBuffer )
{
	sprintf( pDestBuffer, "ref: %02d", m_iRefCount );

	if( GetResource() )
	{
		char buffer[512];
		GetResource()->GetStatus( buffer );
		strcat( pDestBuffer, buffer );
	}
}



//==================================================================================================
// CGraphicsResource
//==================================================================================================

CGraphicsResource::CGraphicsResource()
:
m_LastModifiedTimeOfFile(0),
m_State(GraphicsResourceState::RELEASED),
m_IsCachedResource(false),
m_Index(0)
{}


CGraphicsResource::~CGraphicsResource()
{
//	Release();	// LNK2019 - why???
}


/*
void CGraphicsResource::Release()
{}
*/


/**
  Release and reload resource if the file has been modified since the last time is was load
   - This method does not change the reference count
 */
void CGraphicsResource::Refresh()
{
	const string& resource_path = GetDesc().ResourcePath;
	string filepath, keyname;

	if( is_db_filepath_and_keyname( resource_path ) )
		decompose_into_db_filepath_and_keyname( resource_path, filepath, keyname );
	else
		filepath = resource_path;

	if( m_LastModifiedTimeOfFile < fnop::get_last_modified_time( filepath ) )
	{
		Release();
		Load();
	}
}


bool CGraphicsResource::Load()
{
	if( IsDiskResource() )
	{
		return LoadFromDisk();
	}
	else
	{
		// create resource from non-disk resource
		// - e.g.) empty texture that gets filled by user-defined a routine
		// - Simply create an empty texture if no loader is set to desc. This happens
		//   when an unused cached texture needs to be loaded after being released
		//   in order to reconfigure graphics devices for resolution changes or something.
		return CreateFromDesc();
	}
}


void CGraphicsResource::ReleaseNonChachedResource()
{
	if( !m_IsCachedResource )
		Release();
}


void CGraphicsResource::ReleaseCachedResource()
{
	if( m_IsCachedResource )
		Release();
}


bool CGraphicsResource::LoadFromDisk()
{
	bool loaded = false;
	string target_filepath;
	const string resource_path = GetDesc().ResourcePath;

	if( is_db_filepath_and_keyname( resource_path ) )
	{
		// decompose the string
		string db_filename, keyname;
		decompose_into_db_filepath_and_keyname( resource_path, db_filename, keyname );

		string cwd = fnop::get_cwd();

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
			return false; // the database is being used by someone else - retry later

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( resource_path );

		target_filepath = resource_path;
	}


	if( loaded )
	{
		SetState( GraphicsResourceState::LOADED );

		// record the time of last modification of the texture file
		m_LastModifiedTimeOfFile = fnop::get_last_modified_time(target_filepath);
	}

	return loaded;
}


bool CGraphicsResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( GetResourceType()      == desc.GetResourceType()
	 && GetDesc().ResourcePath == desc.ResourcePath )
		return true;
	else
		return false;
}


void CGraphicsResource::GetStatus( char *pDestBuffer )
{
	const CGraphicsResourceDesc& desc = GetDesc();

	sprintf( pDestBuffer, " / %s",	desc.ResourcePath.c_str() );
}



//==================================================================================================
// CTextureResource
//==================================================================================================

CTextureResource::CTextureResource( const CTextureResourceDesc *pDesc )
:
m_pTexture(NULL)
{
	if( pDesc )
		m_TextureDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


CTextureResource::~CTextureResource()
{
	Release();
}


/**
 Load the texture specified by resource path in the current desc
 does not change the reference count
*/
bool CTextureResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SAFE_RELEASE( m_pTexture );

	string image_archive_key = keyname;

	HRESULT hr;

//	string cwd = fnop::get_cwd();

	CImageArchive img;
	bool img_found = db.GetData( image_archive_key, img );

	if( !img_found )
	{
		LOG_PRINT_ERROR( " - Cannot find the image archive with the specified key '" + image_archive_key + "' in the database" );
		return false;
	}

	{
		char title[1024];
		sprintf( title, "D3DXCreateTextureFromFileInMemory (keyname: %s)", keyname.c_str() );
		LOG_SCOPE( title );

		hr = D3DXCreateTextureFromFileInMemory(
			DIRECT3D9.GetDevice(),
			&img.m_Buffer.buffer()[0],
			(UINT)img.m_Buffer.buffer().size(),
			&m_pTexture
			);
	}

/*	D3DXIMAGE_INFO img_info;
	memset( &img_info, 0, sizeof(D3DXIMAGE_INFO) );
	img_info.Width           = img.m_Width;
	img_info.Height          = img.m_Height;
	img_info.Depth           = 1;//24;
	img_info.MipLevels       = 1;
	img_info.Format          = D3DFMT_A8B8G8R8;
	img_info.ResourceType    = D3DRTYPE_TEXTURE;
	img_info.ImageFileFormat = ArchiveImgFmt2D3DImgFmt( img.m_Format );

	hr = D3DXCreateTextureFromFileInMemoryEx(
	DIRECT3D9.GetDevice(), //	0, //LPDIRECT3DDEVICE9 pDevice,
	&img.m_vecData[0], //	0, //LPCVOID pSrcData,
	(UINT)img.m_vecData.size(), //	0, //UINT SrcDataSize,
	img.m_Width,     //	0, //UINT Width,
	img.m_Height,    //	0, //UINT Height,
	0,               //	0, //UINT MipLevels,
	0,               //	0, //DWORD Usage,
	D3DFMT_A8R8G8B8, //	0, //D3DFORMAT Format,
	D3DPOOL_MANAGED, //	0, //D3DPOOL Pool,
	0,               //	0, //DWORD Filter,
	0,               //	0, //DWORD MipFilter,
	0,               //	0, //D3DCOLOR ColorKey,
	&img_info,       //	0, //D3DXIMAGE_INFO * pSrcInfo,
	NULL,            //	0, //PALETTEENTRY * pPalette,
	&m_pTexture     //	0 //LPDIRECT3DTEXTURE9 * ppTexture
	);*/

	return SUCCEEDED(hr) ? true : false;
}


static inline D3DXIMAGE_FILEFORMAT SuffixToD3DImgFmt( const std::string& suffix )
{
	if( suffix == "bmp" ) return D3DXIFF_BMP;
	else if( suffix == "jpg" ) return D3DXIFF_JPG;
	else if( suffix == "tga" ) return D3DXIFF_TGA;
	else if( suffix == "png" ) return D3DXIFF_PNG;
	else if( suffix == "dds" ) return D3DXIFF_DDS;
	else return D3DXIFF_BMP;
}


bool CTextureResource::SaveTextureToImageFile( const std::string& image_filepath )
{
	if( m_pTexture )
	{
		D3DXIMAGE_FILEFORMAT img_fmt = SuffixToD3DImgFmt( image_filepath.substr( image_filepath.length() - 3, 3 ) );
		HRESULT hr = D3DXSaveTextureToFile( image_filepath.c_str(), img_fmt, m_pTexture, NULL );
		if( SUCCEEDED(hr) )
			return true;
		else
			return false;
	}
	else
		return false;
}


// May only be called by render thread
// Used in synchronous loading
bool CTextureResource::LoadFromFile( const std::string& filepath )
{
	char title[1024];
	sprintf( title, "D3DXCreateTextureFromFile (file: %s)", filepath.c_str() );
	LOG_SCOPE( title );

	SAFE_RELEASE( m_pTexture );

	HRESULT hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );

	return SUCCEEDED(hr) ? true : false;
}


bool CTextureResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	return CGraphicsResource::CanBeSharedAsSameResource( desc );
}


int CTextureResource::CanBeUsedAsCache( const CGraphicsResourceDesc& desc )
{
	if( GetState() != GraphicsResourceState::RELEASED )
		return 0;

	return desc.CanBeUsedAsTextureCache( m_TextureDesc );
}


bool CTextureResource::Create()
{
	SAFE_RELEASE( m_pTexture );

	const CTextureResourceDesc& desc = m_TextureDesc;

	HRESULT hr;
	DWORD usage = 0;
	D3DPOOL pool = D3DPOOL_MANAGED;
	{
		char title[1024];
		sprintf( title, "D3DXCreateTexture (%dx%d)", desc.Width, desc.Height );
		LOG_SCOPE( title );

	hr = D3DXCreateTexture( DIRECT3D9.GetDevice(),
	                                (UINT)desc.Width,
									(UINT)desc.Height,
									(UINT)desc.MipLevels,
									usage,
									ConvertTextureFormatToD3DFORMAT( desc.Format ),
									pool,
									&m_pTexture );
	}

	if( FAILED(hr) || !m_pTexture )
	{
		LOG_PRINT_ERROR( " D3DXCreateTexture() failed." );
		return false;
	}
	else
		return true;
}


bool CTextureResource::CreateFromDesc()
{
	const CTextureResourceDesc& desc = m_TextureDesc;

	// create an empty texture
	Create();

	if( Lock() )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		shared_ptr<CTextureLoader> pLoader = desc.pLoader.lock();
		if( pLoader )
		{
			pLoader->FillTexture( *(m_pLockedTexture.get()) );
		}

		Unlock();

		SetState( GraphicsResourceState::LOADED );

		D3DXSaveTextureToFile( string(desc.ResourcePath + ".dds").c_str(), D3DXIFF_DDS, m_pTexture, NULL );

		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to lock the texture: " + desc.ResourcePath );
		return false;
	}

//	return SUCCEEDED(hr) ? true : false;
}


void CTextureResource::UpdateDescForCachedResource( const CGraphicsResourceDesc& desc )
{
	desc.UpdateCachedTextureResourceDesc( m_TextureDesc );
}


bool CTextureResource::Lock()
{
	if( !m_pTexture )
		return false;

	const CTextureResourceDesc& desc = m_TextureDesc;

	D3DLOCKED_RECT locked_rect;
	HRESULT hr = m_pTexture->LockRect( 0, &locked_rect, NULL, 0);	// Lock and get the pointer to the first texel of the texture

	if( FAILED(hr) )
	{
		LOG_PRINT_WARNING( " Failed to lock the texture" );
		return false;
	}

	m_pLockedTexture = shared_ptr<CLockedTexture>( new CLockedTexture() );
	CLockedTexture& tex = *(m_pLockedTexture.get());
	tex.m_pBits  = locked_rect.pBits;
	tex.m_Width  = desc.Width;
	tex.m_Height = desc.Height;

	return true;
}


bool CTextureResource::Unlock()
{
	if( !m_pTexture )
		return false;

	m_pLockedTexture.reset();

	HRESULT hr = S_OK;
	hr = m_pTexture->UnlockRect(0);

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( "IDirect3DTexture9::UnlockRect() failed." );
		return false;
	}

//	hr = D3DXFilterTexture( m_pTexture, NULL, 0, D3DX_FILTER_TRIANGLE );

//	m_pTexture->PreLoad();

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( "D3DXFilterTexture() failed." );
	}

	return true;
}


bool CTextureResource::GetLockedTexture( CLockedTexture& texture )
{
	// TODO: increment the ref count when the async loading process is started
//	m_iRefCount++;

	if( !m_pLockedTexture )
		return false;

	texture = *m_pLockedTexture;
	return true;
}


void CTextureResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SAFE_RELEASE( m_pTexture );

	SetState( GraphicsResourceState::RELEASED );
}


bool CTextureResource::IsDiskResource() const
{
	if( 0 < m_TextureDesc.Width
	 && 0 < m_TextureDesc.Height
	 && m_TextureDesc.Format != TextureFormat::Invalid )
	{
		return false;
	}
	else
		return true;
}


void CTextureResource::GetStatus( char *pDestBuffer )
{
	CGraphicsResource::GetStatus( pDestBuffer );

	char buffer[256];
	const CTextureResourceDesc& desc = m_TextureDesc;

	sprintf( buffer, " / %d x %d", desc.Width, desc.Height );
	strcat( pDestBuffer, buffer );
}



//==================================================================================================
// CMeshResource
//==================================================================================================

CMeshResource::CMeshResource( const CMeshResourceDesc *pDesc )
{
	if( pDesc )
		m_MeshDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An incompatible resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


CMeshResource::~CMeshResource()
{
	Release();
}


bool CMeshResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	m_pMeshObject.reset();

	string mesh_archive_key = keyname;

	// retrieve mesh archive from db
	C3DMeshModelArchive mesh_archive;
	db.GetData( mesh_archive_key, mesh_archive );

	CMeshObjectFactory factory;
	CD3DXMeshObjectBase *pMesh = factory.LoadMeshObjectFromArchive( mesh_archive, keyname, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );
	m_pMeshObject = boost::shared_ptr<CD3DXMeshObjectBase>( pMesh );

	return ( m_pMeshObject ? true : false );
}


bool CMeshResource::LoadFromFile( const std::string& filepath )
{
	m_pMeshObject.reset();

	CMeshObjectFactory factory;
	CD3DXMeshObjectBase *pMeshObject
		= factory.LoadMeshObjectFromFile( filepath, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<CD3DXMeshObjectBase>( pMeshObject );
		return true;
	}
	else
		return false;
}


void CMeshResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	m_pMeshObject.reset();

	SetState( GraphicsResourceState::RELEASED );
}


bool CMeshResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	return desc.CanBeSharedAsSameMeshResource( m_MeshDesc );
}


int CMeshResource::CanBeUsedAsCache( const CGraphicsResourceDesc& desc )
{
	if( GetState() != GraphicsResourceState::RELEASED )
		return 0;

	return desc.CanBeUsedAsMeshCache( m_MeshDesc );
}

/*
struct IDAndString
{
	int id;
	const char* text;
};


#define ID_AND_STRING(id) { id, #id }


IDAndString g_MeshTypes[] =
{
	ID_AND_STRING(CMeshType::BASIC),
	ID_AND_STRING(CMeshType::PROGRESSIVE),
	ID_AND_STRING(CMeshType::SKELETAL)
};
*/

void CMeshResource::GetStatus( char *pDestBuffer )
{
	CGraphicsResource::GetStatus( pDestBuffer );

	char buffer[256];
	const CMeshResourceDesc& desc = m_MeshDesc;

	sprintf( buffer, " / %d", desc.MeshType );
	strcat( pDestBuffer, buffer );
}


//==================================================================================================
// CShaderManagerEntry
//==================================================================================================

CShaderResource::CShaderResource( const CShaderResourceDesc *pDesc )
:
m_pShaderManager(NULL)
{
	if( pDesc )
		m_ShaderDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


CShaderResource::~CShaderResource()
{
//	LOG_FUNCTION_SCOPE();

	Release();
}


bool CShaderResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SafeDelete( m_pShaderManager );

	LOG_PRINT_ERROR( " - Not implemented!" );

	return false;

	// retrieve mesh archive from db
//	db.GetData( keyname, shader_archive );

//	m_pShaderManager = CreateShaderFromArchive( shader_archive );

//	return ( m_pShaderManager ? true : false );
}


bool CShaderResource::LoadFromFile( const std::string& filepath )
{
	SafeDelete( m_pShaderManager );

	// load a shader file
	m_pShaderManager = new CShaderManager();
	bool loaded = m_pShaderManager->LoadShaderFromFile( filepath );

	return loaded;
}


void CShaderResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SafeDelete( m_pShaderManager );

	SetState( GraphicsResourceState::RELEASED );
}
