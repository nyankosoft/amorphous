#include "D3DGraphicsResources.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
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


//==================================================================================================
// CD3DTextureResource
//==================================================================================================

CD3DTextureResource::CD3DTextureResource( const CTextureResourceDesc *pDesc )
:
CTextureResource(pDesc),
m_pTexture(NULL)
{
}


CD3DTextureResource::~CD3DTextureResource()
{
	Release();
}


/**
 Load the texture specified by resource path in the current desc
 does not change the reference count
*/
bool CD3DTextureResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SAFE_RELEASE( m_pTexture );

	string image_archive_key = keyname;

	HRESULT hr;

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


bool CD3DTextureResource::SaveTextureToImageFile( const std::string& image_filepath )
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
bool CD3DTextureResource::LoadFromFile( const std::string& filepath )
{
	char title[1024];
	sprintf( title, "D3DXCreateTextureFromFile (file: %s)", filepath.c_str() );
	LOG_SCOPE( title );

	SAFE_RELEASE( m_pTexture );

	HRESULT hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );

	return SUCCEEDED(hr) ? true : false;
}


bool CD3DTextureResource::Create()
{
	SAFE_RELEASE( m_pTexture );

	const CTextureResourceDesc& desc = m_TextureDesc;

	HRESULT hr;
	DWORD usage = 0;
	D3DPOOL pool = D3DPOOL_MANAGED;
//	DWORD usage = D3DUSAGE_DYNAMIC;
//	D3DPOOL pool = D3DPOOL_DEFAULT;

	if( desc.UsageFlags & UsageFlag::RENDER_TARGET )
	{
		usage = D3DUSAGE_RENDERTARGET;
		pool = D3DPOOL_DEFAULT;
	}
	else
	{
		usage = 0;
		pool = D3DPOOL_MANAGED;
	}

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


bool CD3DTextureResource::CreateFromDesc()
{
	const CTextureResourceDesc& desc = m_TextureDesc;

	// create an empty texture
	bool created = Create();

	if( !created )
		return false;

	if( desc.UsageFlags & UsageFlag::RENDER_TARGET )
	{
		SetState( GraphicsResourceState::LOADED );
		return true;
	}

	if( !desc.pLoader )
	{
		LOG_PRINT_WARNING( " Created an empty texture that is not a render target and does not have a texture loader." );
		return true;
	}

	if( Lock() )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		shared_ptr<CTextureFillingAlgorithm> pLoader = desc.pLoader;
		if( pLoader )
		{
			pLoader->FillTexture( *(m_pLockedTexture.get()) );
		}

		bool unlocked = Unlock();

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


bool CD3DTextureResource::Lock()
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


bool CD3DTextureResource::Unlock()
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

/*
bool CD3DTextureResource::GetLockedTexture( CLockedTexture& texture )
{
	// TODO: increment the ref count when the async loading process is started
//	m_iRefCount++;

	if( !m_pLockedTexture )
		return false;

	texture = *m_pLockedTexture;
	return true;
}
*/

void CD3DTextureResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SAFE_RELEASE( m_pTexture );

	SetState( GraphicsResourceState::RELEASED );
}
