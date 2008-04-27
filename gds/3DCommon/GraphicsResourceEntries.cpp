#include "GraphicsResourceEntries.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DXMeshObjectBase.h"
#include "Support/SafeDelete.h"
#include "Support/fnop.h"
#include "Support/ImageArchive.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

using namespace std;


inline bool str_includes( const std::string& src, const std::string& target )
{
	if( src.find(target.c_str()) != std::string::npos )
		return true;
	else
		return false;
}



//==================================================================================================
// CGraphicsResourceDesc
//==================================================================================================

CGraphicsResourceDesc::CGraphicsResourceDesc( int resource_type )
:
ResourceType(resource_type),
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
  Release and reload resource if the file has been modified since the last load
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
		g_Log.Print( WL_WARNING, "CGraphicsResourceEntry::DecRefCount() - redundant call: ref count is already 0" );
		return;	// error
	}

	m_iRefCount--;

	if( m_iRefCount == 0 )
		Release();
}


bool CGraphicsResourceEntry::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( GetResourceType() == desc.ResourceType
	 && GetFilename() == desc.Filename )
		return true;
	else
		return false;
}


//==================================================================================================
// CTextureEntry
//==================================================================================================

CTextureEntry::CTextureEntry()
:
m_pTexture(NULL)
{}


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
bool CTextureEntry::Load()
{
	SAFE_RELEASE( m_pTexture );

	HRESULT hr;
	size_t pos = m_Filename.find( "::" );
	if( pos != string::npos )
	{
		// not supported yet
		// found "::" in filename
		// - "(binary database filename)::(key)"
		string db_filename       = m_Filename.substr( 0, pos );
		string image_archive_key = m_Filename.substr( pos + 2, m_Filename.length() );

		string cwd = fnop::get_cwd();

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
			return false;

		CImageArchive img;
		db.GetData( image_archive_key, img );

		hr = D3DXCreateTextureFromFileInMemory( DIRECT3D9.GetDevice(), &img.m_vecData[0], (UINT)img.m_vecData.size(), &m_pTexture );

/*		D3DXIMAGE_INFO img_info;
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

//		hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), &(img.vecPixel[0], vecPixel.size(), &m_pTexture );
	}
	else
	{
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
		{*/
			hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), m_Filename.c_str(), &m_pTexture );
/*		}*/
	}

	if( FAILED( hr ) )
		return false;

	// record the time of last modification of the texture file
	m_LastModifiedTimeOfFile = fnop::get_last_modified_time(m_Filename);

	return true;
}


void CTextureEntry::Release()
{
	SAFE_RELEASE( m_pTexture );
}


//==================================================================================================
// CMeshObjectEntry
//==================================================================================================

CMeshObjectEntry::CMeshObjectEntry()
:
m_pMeshObject(NULL),
m_MeshType(CD3DXMeshObjectBase::TYPE_MESH)
{}


CMeshObjectEntry::CMeshObjectEntry( int mesh_type )
:
m_pMeshObject(NULL),
m_MeshType(mesh_type)
{}


CMeshObjectEntry::~CMeshObjectEntry()
{
	Release();
}


bool CMeshObjectEntry::Load()
{
	SafeDelete( m_pMeshObject );

//	HRESULT hr;
	size_t pos = m_Filename.find( "::" );
	if( pos != string::npos )
	{
		// found "::" in filename
		// - "(binary database filename)::(key)"
		string db_filename      = m_Filename.substr( 0, pos );
		string mesh_archive_key = m_Filename.substr( pos + 2, m_Filename.length() );

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
		{
			LOG_PRINT_ERROR( string(" - unable to open database: ") + db_filename );
			return false;
		}

		// retrieve mesh archive from db
		C3DMeshModelArchive mesh_archive;
		db.GetData( mesh_archive_key, mesh_archive );

		CMeshObjectFactory factory;
		m_pMeshObject = factory.LoadMeshObjectFromArchvie( mesh_archive, m_Filename, m_MeshType );
		if( !m_pMeshObject )
			return false;
	}
	else
	{
/*		pos = m_strMeshObjectFilename.find( "::" )
		if( pos != string::npos )
		{
			// pack file - not supported yet
			CPackedFile pakfile;
			string pakfilename = m_strMeshObjectFilename.substr( 0, pos );
			string texfilename = m_strMeshObjectFilename.substr( pos + 1, 1024 );
			pakfile.Load( pakfilename );
			vector<unsigned char> vecPixel;
			pakfile.GetPackedFile( texfilename, vecPixel );
			hr = D3DXCreateMeshObjectFromFileInMemory( DIRECT3D9.GetDevice(), &vecPixel[0], vecPixel.size(), &m_pMeshObject );
		}
		else
		{*/
			CMeshObjectFactory factory;
			m_pMeshObject = factory.LoadMeshObjectFromFile( m_Filename, m_MeshType );
			if( !m_pMeshObject )
				return false;
/*		}*/
	}

	// record the time of last modification of the texture file
	m_LastModifiedTimeOfFile = fnop::get_last_modified_time(m_Filename);

	return true;

}


void CMeshObjectEntry::Release()
{
	SafeDelete( m_pMeshObject );
}


bool CMeshObjectEntry::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( CGraphicsResourceEntry::CanBeSharedAsSameResource(desc)
	 && m_MeshType == desc.MeshType )
		return true;
	else
		return false;
}



/*
bool CTextureEntry::LoadTexture( const char *pcTextureFilename )
{
	if( !pcTextureFilename || strlen(pcTextureFilename) == 0 )
		return false;	// invalid filename

	// Save texture filename so that we can reload it after we lost D3D device and re-create it
	m_Filename = pcTextureFilename;

	if( Load() )
	{
		m_iRefCount = 1;
		return true;
	}
	else
		return false;
}*/

/*
bool CTextureEntry::LoadTextureFromMemory( void *pSrcData, const char *name )
{
	if( !name || strlen(name) == 0 )
		return false;	// invalid filename

	// Save texture filename so that we can reload it after we lost D3D device and re-create it
	m_Filename = name;

	HRESULT hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), pcTextureFilename, &m_pTexture );
    if( FAILED( hr ) )
	{
        m_pTexture = NULL;
		return false;
	}

	m_iRefCount = 1;

	return true;
}
*/
