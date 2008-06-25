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


inline bool str_includes( const std::string& src, const std::string& target )
{
	if( src.find(target.c_str()) != std::string::npos )
		return true;
	else
		return false;
}



//==================================================================================================
// CGraphicsResourceDesc and its derived classes
//==================================================================================================

CGraphicsResourceDesc::CGraphicsResourceDesc()
{}


CMeshResourceDesc::CMeshResourceDesc()
	:
MeshType(CD3DXMeshObjectBase::TYPE_MESH)
{}


//==================================================================================================
// CGraphicsResourceEntry
//==================================================================================================

CGraphicsResourceEntry::CGraphicsResourceEntry()
:
m_iRefCount(0),
m_LastModifiedTimeOfFile(0)
{}


CGraphicsResourceEntry::~CGraphicsResourceEntry()
{
//	Release();	// LNK2019 - why???
}

/*
void CGraphicsResourceEntry::Release()
{
}
*/

/**
  Release and reload resource if the file has been modified since the last time is was load
   - This method does not change the reference count
 */
void CGraphicsResourceEntry::Refresh()
{
	if( m_LastModifiedTimeOfFile < fnop::get_last_modified_time( m_Filename ) )
	{
		Release();
		Load();
	}
}


/*
 Load the resource when the reference count is incremented from 0 to 1
*/
void CGraphicsResourceEntry::IncRefCount()
{
	if( m_iRefCount == 0 )
	{
		bool res = Load();
		if( res )
			m_iRefCount = 1;
	}
	else
	{
		// resource has already been loaded - just increment the reference count
		m_iRefCount++;
	}
}


/*
 Release the resource when the reference count is decremented from 1 to 0
*/
void CGraphicsResourceEntry::DecRefCount()
{
	if( m_iRefCount == 0 )
	{
		LOG_PRINT_WARNING( " - A redundant call: ref count is already 0 (resource name: " + m_Filename + ")" );
		return;	// error
	}

	m_iRefCount--;

	if( m_iRefCount == 0 )
		Release();
}


bool CGraphicsResourceEntry::Load()
{
	if( IsDiskResource() )
	{
		return LoadFromDisk();
	}
	else
	{
		// create from non-disk resource
		// - e.g.) empty texture
		return CreateFromDesc();
	}
}


bool CGraphicsResourceEntry::LoadFromDisk()
{
	bool loaded = false;
	size_t pos = m_Filename.find( "::" );
	string target_filepath;

	if( pos != string::npos )
	{
		// not supported yet
		// found "::" in filename
		// - "(binary database filename)::(key)"
		string db_filename       = m_Filename.substr( 0, pos );
		string keyname = m_Filename.substr( pos + 2, m_Filename.length() );

		string cwd = fnop::get_cwd();

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
			return false;

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( m_Filename );

		target_filepath = m_Filename;
	}

/*		pos = m_Filename.find( ":" )
		if( pos != string::npos )
		{
			// pack file - not supported yet
			CPackedFile pakfile;
			string pakfilename = m_Filename.substr( 0, pos );
			string texfilename = m_Filename.substr( pos + 1, 1024 );
			pakfile.Load( pakfilename );
			vector<unsigned char> vecPixel;
			pakfile.GetPackedFile( texfilename, vecPixel );
			hr = D3DXCreateTextureFromFileInMemory( DIRECT3D9.GetDevice(), &vecPixel[0], vecPixel.size(), &m_pTexture );
		}
		else
*/

	if( loaded )
	{
		// record the time of last modification of the texture file
		m_LastModifiedTimeOfFile = fnop::get_last_modified_time(target_filepath);
	}

	return loaded;
}


bool CGraphicsResourceEntry::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( GetResourceType() == desc.GetResourceType()
	 && GetFilename() == desc.Filename )
		return true;
	else
		return false;
}


//==================================================================================================
// CTextureEntry
//==================================================================================================

CTextureEntry::CTextureEntry( const CTextureResourceDesc *pDesc )
:
m_pTexture(NULL)
{
	if( pDesc )
		m_TextureDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An imcompatible resource desc" );
}


CTextureEntry::~CTextureEntry()
{
	Release();
}


static inline D3DXIMAGE_FILEFORMAT ArchiveImgFmt2D3DImgFmt( int img_archive_format )
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
 load the texture specified by m_Filename
 does not change the reference count
*/
bool CTextureEntry::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SAFE_RELEASE( m_pTexture );

	string image_archive_key = keyname;

	HRESULT hr;

//	string cwd = fnop::get_cwd();

	CImageArchive img;
	db.GetData( image_archive_key, img );

	{
		char title[1024];
		sprintf( title, "D3DXCreateTextureFromFileInMemory (keyname: %s)", keyname.c_str() );
		LOG_SCOPE( title );

	hr = D3DXCreateTextureFromFileInMemory( DIRECT3D9.GetDevice(), &img.m_vecData[0], (UINT)img.m_vecData.size(), &m_pTexture );
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


bool CTextureEntry::LoadFromFile( const std::string& filepath )
{
	char title[1024];
	sprintf( title, "D3DXCreateTextureFromFile (file: %s)", filepath.c_str() );
	LOG_SCOPE( title );

	SAFE_RELEASE( m_pTexture );

	HRESULT hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );

	return SUCCEEDED(hr) ? true : false;
}


bool CTextureEntry::CreateFromDesc()
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
		return false;

	D3DLOCKED_RECT locked_rect;
	hr = m_pTexture->LockRect( 0, &locked_rect, NULL, 0);	// Lock and get the pointer to the first texel of the texture

	CLockedTexture tex;
	tex.m_pBits  = locked_rect.pBits;
	tex.m_Width  = desc.Width;
	tex.m_Height = desc.Height;

	// An empty texture has been created
	// - fill the texture if loader was specified
	shared_ptr<CTextureLoader> pLoader = desc.pLoader.lock();
	if( pLoader )
	{
		pLoader->FillTexture( tex );
	}

	m_pTexture->UnlockRect(0);

	D3DXFilterTexture( m_pTexture, NULL, 0, D3DX_FILTER_TRIANGLE );

	D3DXSaveTextureToFile( string(desc.Filename + ".dds").c_str(), D3DXIFF_DDS, m_pTexture, NULL );

	return true;

//	return SUCCEEDED(hr) ? true : false;
}


void CTextureEntry::Release()
{
	SAFE_RELEASE( m_pTexture );
}


bool CTextureEntry::IsDiskResource() const
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


//==================================================================================================
// CMeshObjectEntry
//==================================================================================================

CMeshObjectEntry::CMeshObjectEntry( const CMeshResourceDesc *pDesc )
:
m_pMeshObject(NULL)
{
	if( pDesc )
		m_MeshDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An imcompatible resource desc" );
}

/*
CMeshObjectEntry::CMeshObjectEntry( int mesh_type )
:
m_pMeshObject(NULL),
m_MeshType(mesh_type)
{}
*/

CMeshObjectEntry::~CMeshObjectEntry()
{
	Release();
}


bool CMeshObjectEntry::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SafeDelete( m_pMeshObject );

	string mesh_archive_key = keyname;

	// retrieve mesh archive from db
	C3DMeshModelArchive mesh_archive;
	db.GetData( mesh_archive_key, mesh_archive );

	CMeshObjectFactory factory;
	m_pMeshObject = factory.LoadMeshObjectFromArchvie( mesh_archive, m_Filename, m_MeshDesc.MeshType );

	return ( m_pMeshObject ? true : false );
}


bool CMeshObjectEntry::LoadFromFile( const std::string& filepath )
{
	SafeDelete( m_pMeshObject );

	CMeshObjectFactory factory;
	m_pMeshObject = factory.LoadMeshObjectFromFile( m_Filename, m_MeshDesc.MeshType );
	if( !m_pMeshObject )
		return false;

	return ( m_pMeshObject ? true : false );
}


void CMeshObjectEntry::Release()
{
	SafeDelete( m_pMeshObject );
}


bool CMeshObjectEntry::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( desc.GetResourceType() != CGraphicsResourceDesc::RT_MESHOBJECT )
		return false;

	const CMeshResourceDesc *pMeshDesc = dynamic_cast<const CMeshResourceDesc *>(&desc);
	if( !pMeshDesc )
		return false;

	if( CGraphicsResourceEntry::CanBeSharedAsSameResource(desc)
	 && GetMeshType() == pMeshDesc->MeshType )
		return true;
	else
		return false;
}


//==================================================================================================
// CShaderManagerEntry
//==================================================================================================

CShaderManagerEntry::CShaderManagerEntry( const CShaderResourceDesc *pDesc )
:
m_pShaderManager(NULL)
{
	if( pDesc )
		m_ShaderDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An imcompatible resource desc" );
}


CShaderManagerEntry::~CShaderManagerEntry()
{
//	LOG_FUNCTION_SCOPE();

	Release();
}


bool CShaderManagerEntry::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SafeDelete( m_pShaderManager );

	LOG_PRINT_ERROR( " - Not implemented!" );

	return false;

	// retrieve mesh archive from db
//	db.GetData( keyname, shader_archive );

//	m_pShaderManager = CreateShaderFromArchive( shader_archive );

//	return ( m_pShaderManager ? true : false );
}


bool CShaderManagerEntry::LoadFromFile( const std::string& filepath )
{
	SafeDelete( m_pShaderManager );

	// load a shader file
	m_pShaderManager = new CShaderManager();
	bool loaded = m_pShaderManager->LoadShaderFromFile( filepath );

	return loaded;
}


void CShaderManagerEntry::Release()
{
//	LOG_FUNCTION_SCOPE();

	SafeDelete( m_pShaderManager );
}
