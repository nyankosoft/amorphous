#include "D3DGraphicsResources.hpp"

#include "Graphics/Direct3D/D3DSurfaceFormat.hpp"
#include "Graphics/Direct3D/Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/Direct3D/Shader/D3DShaderManager.hpp"
#include "Graphics/Direct3D/Shader/D3DCgEffect.hpp"
#include "Graphics/Direct3D/Shader/D3DFixedFunctionPipelineManager.hpp"
#include "Graphics/TextureGenerators/TextureFillingAlgorithm.hpp"
#include "Graphics/TextureGenerators/TextureFilter.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{
using namespace serialization;

using std::string;
using std::vector;
using std::map;
using namespace boost;


static inline D3DXIMAGE_FILEFORMAT ArchiveImgFmt2D3DImgFmt( ImageArchive::ImageFormat img_archive_format )
{
	switch(img_archive_format)
	{
	case ImageArchive::IMGFMT_BMP24: return D3DXIFF_BMP;
	case ImageArchive::IMGFMT_BMP32: return D3DXIFF_BMP;
	case ImageArchive::IMGFMT_JPEG:  return D3DXIFF_JPG;
	case ImageArchive::IMGFMT_TGA:   return D3DXIFF_TGA;
	case ImageArchive::IMGFMT_PNG:   return D3DXIFF_PNG;
//	case ImageArchive::IMGFMT_ : return D3DXIFF_DDS,
//	case ImageArchive::IMGFMT_ : return D3DXIFF_PPM,
//	case ImageArchive::IMGFMT_ : return D3DXIFF_DIB,
//	case ImageArchive::IMGFMT_ : return D3DXIFF_HDR,       //high dynamic range formats
//	case ImageArchive::IMGFMT_ : return D3DXIFF_PFM,       //
	default: return D3DXIFF_BMP;
	}

	return D3DXIFF_BMP;
}



//==================================================================================================
// CD3DLockedTexture
//==================================================================================================

class CD3DLockedTexture : public LockedTexture
{
	void *m_pBits;

	int m_Width;

	int m_Height;

public:

	CD3DLockedTexture() : m_pBits(NULL), m_Width(0), m_Height(0) {}

	int GetWidth() { return m_Width; }

	int GetHeight() { return m_Height; }

	bool IsValid() const { return (m_pBits != NULL); }

	void GetPixel( int x, int y, SFloatRGBAColor& dest ) { U32 argb = ((U32 *)m_pBits)[ y * m_Width + x ]; dest.SetARGB32( argb ); }

	virtual void SetPixelARGB32( int x, int y, U32 argb_color ) { ((U32 *)m_pBits)[ y * m_Width + x ] = argb_color; }

	/// \param alpha [0,255]
	void SetAlpha( int x, int y, U8 alpha )
	{
		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	void Clear( U32 argb_color )
	{
		const int num_bytes_per_pixel = sizeof(U32);
		if( argb_color == 0 )
			memset( m_pBits, 0, m_Width * m_Height * num_bytes_per_pixel );
		else if( argb_color == 0xFFFFFFFF )
			memset( m_pBits, 0xFF, m_Width * m_Height * num_bytes_per_pixel );
		else
		{
			int w = m_Width;
			int h = m_Height;
			for( int y=0; y<h; y++ )
			{
				for( int x=0; x<w; x++ )
				{
					SetPixelARGB32( x, y, argb_color );
				}
			}
		}
	}

	void Clear( const SFloatRGBAColor& color ) { Clear( color.GetARGB32() ); }

	friend class CD3DTextureResource;
};



//==================================================================================================
// CD3DTextureResource
//==================================================================================================

CD3DTextureResource::CD3DTextureResource( const TextureResourceDesc *pDesc )
:
TextureResource(pDesc),
m_pTexture(NULL)
{
}


CD3DTextureResource::~CD3DTextureResource()
{
	Release();
}


static HRESULT LoadTextureFromImageArchive( ImageArchive& src_img, LPDIRECT3DTEXTURE9& pTexture )
{
	HRESULT hr = 
			D3DXCreateTextureFromFileInMemory(
			DIRECT3D9.GetDevice(),
			&src_img.m_Buffer.buffer()[0],
			(UINT)src_img.m_Buffer.buffer().size(),
			&pTexture
			);

	return hr;
}


/**
 Load the texture specified by resource path in the current desc
 does not change the reference count
*/
bool CD3DTextureResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SAFE_RELEASE( m_pTexture );

	string image_archive_key = keyname;

	HRESULT hr = S_OK;

	ImageArchive img;
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

		hr = LoadTextureFromImageArchive( img, m_pTexture );
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


bool CD3DTextureResource::SaveTextureToImageFile( const std::string& image_filepath )
{
	if( m_pTexture )
	{
		D3DXIMAGE_FILEFORMAT img_fmt = GetD3DXImageFormatFromFilepath( image_filepath );
		HRESULT hr = D3DXSaveTextureToFile( image_filepath.c_str(), img_fmt, m_pTexture, NULL );
		if( SUCCEEDED(hr) )
			return true;
		else
			return false;
	}
	else
		return false;
}


HRESULT CD3DTextureResource::CreateD3DTextureFromFile( const std::string& filepath )
{
	SAFE_RELEASE( m_pTexture );
	return D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );
}


LPDIRECT3DSURFACE9 CD3DTextureResource::GetPrimaryTextureSurface()
{
	if( !m_pTexture )
		return NULL;

	LPDIRECT3DSURFACE9 pSurfaceLevel = NULL;
	HRESULT surf_hr = m_pTexture->GetSurfaceLevel( 0, &pSurfaceLevel );

	return pSurfaceLevel;
}


// May only be called by render thread
// Used in synchronous loading
bool CD3DTextureResource::LoadFromFile( const std::string& filepath )
{
	char title[1024];
	sprintf( title, "D3DXCreateTextureFromFile (file: %s)", filepath.c_str() );
	LOG_SCOPE( title );

	SAFE_RELEASE( m_pTexture );

	HRESULT hr = E_FAIL;

	// For a system that does not use ImageArchive, this suffices.
//	hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );

	if( lfs::get_ext(filepath) == "ia" )
	{
		ImageArchive img;
		bool loaded = img.LoadFromFile( filepath );
		if( loaded )
			hr = LoadTextureFromImageArchive( img, m_pTexture );
//		if( SUCCEEDED(hr) )
//			m_LoadedFromImageArchive = 1;
	}
	else
	{
		// Load regular image files, such as .bmp and .png
		hr = CreateD3DTextureFromFile( filepath );
	//	hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pTexture );

		if( FAILED(hr) )
		{
			string ia_filepath( filepath );
			lfs::change_ext( ia_filepath, "ia" );
			ImageArchive img( ia_filepath );
			if( img.IsValid() )
				hr = LoadTextureFromImageArchive( img, m_pTexture );
//			if( SUCCEEDED(hr) )
//				m_LoadedFromImageArchive = 1;
		}
	}

	// Retrieve the width and hight of the top level texture, and set them to the desc
	if( SUCCEEDED(hr) )
	{
		LPDIRECT3DSURFACE9 pSurfaceLevel = GetPrimaryTextureSurface();
		if( /*SUCCEEDED(surf_hr) &&*/ pSurfaceLevel )
		{
			D3DSURFACE_DESC surf_desc;
			pSurfaceLevel->GetDesc( &surf_desc );
			pSurfaceLevel->Release();
			m_TextureDesc.Width  = (int)surf_desc.Width;
			m_TextureDesc.Height = (int)surf_desc.Height;
		}
	}

	return SUCCEEDED(hr) ? true : false;
}


HRESULT CD3DTextureResource::CreateD3DTexture( const TextureResourceDesc& desc, DWORD usage, D3DPOOL pool )
{
	HRESULT hr = D3DXCreateTexture( DIRECT3D9.GetDevice(),
	                                (UINT)desc.Width,
									(UINT)desc.Height,
									(UINT)desc.MipLevels,
									usage,
									ConvertTextureFormatToD3DFORMAT( desc.Format ),
									pool,
									&m_pTexture );

	return hr;
}


extern const char *hr_d3d_error_to_string(HRESULT hr);

bool CD3DTextureResource::Create()
{
	SAFE_RELEASE( m_pTexture );

	const TextureResourceDesc& desc = m_TextureDesc;

	HRESULT hr = E_FAIL;
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

		hr = CreateD3DTexture( desc, usage, pool );
	}

	if( FAILED(hr) || !GetD3DBaseTexture() )
	{
		LOG_PRINT_ERROR( " D3DXCreateTexture() failed. Error: " + string(hr_d3d_error_to_string(hr)) );
		return false;
	}
	else
		return true;
}


bool CD3DTextureResource::CreateFromDesc()
{
	const TextureResourceDesc& desc = m_TextureDesc;

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
		SetState( GraphicsResourceState::LOADED );
		return true;
	}

	if( Lock( 0 ) )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		boost::shared_ptr<TextureFillingAlgorithm> pLoader = desc.pLoader;
		if( pLoader && m_pLockedTexture )
		{
			pLoader->FillTexture( *m_pLockedTexture );

			for( size_t i=0; i<pLoader->m_pFilters.size(); i++ )
			{
				if( pLoader->m_pFilters[i] )
					pLoader->m_pFilters[i]->ApplyFilter( *m_pLockedTexture );
			}
		}

		bool unlocked = Unlock();

		SetState( GraphicsResourceState::LOADED );

		HRESULT hr = S_OK;

//		hr = D3DXFilterTexture( m_pTexture, NULL, 0, D3DX_FILTER_TRIANGLE );
		hr = D3DXFilterTexture( GetD3DBaseTexture(), NULL, 0, D3DX_FILTER_TRIANGLE );

//		hr = D3DXSaveTextureToFile( string(desc.ResourcePath + ".dds").c_str(), D3DXIFF_DDS, m_pTexture, NULL );

		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to lock the texture: " + desc.ResourcePath );
		return false;
	}

//	return SUCCEEDED(hr) ? true : false;
}


bool CD3DTextureResource::Lock( uint mip_level )
{
	if( !m_pTexture )
		return false;

	const TextureResourceDesc& desc = m_TextureDesc;

	// Lock a surface and get the pointer to the first texel of the texture
	D3DLOCKED_RECT locked_rect;
	HRESULT hr = m_pTexture->LockRect( mip_level, &locked_rect, NULL, 0 );

	if( FAILED(hr) )
	{
		LOG_PRINT_WARNING( " Failed to lock the texture (mip level: %d)" + to_string(mip_level) );
		return false;
	}

	shared_ptr<CD3DLockedTexture> pLockedTexture( new CD3DLockedTexture() );
	CD3DLockedTexture& tex = *pLockedTexture;
	tex.m_pBits  = locked_rect.pBits;
	int div = (int)pow( 2.0, double(mip_level) );
	tex.m_Width  = desc.Width  / div;
	tex.m_Height = desc.Height / div;
	m_pLockedTexture = pLockedTexture;

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
/*
//	hr = D3DXFilterTexture( m_pTexture, NULL, 0, D3DX_FILTER_TRIANGLE );

//	m_pTexture->PreLoad();

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( "D3DXFilterTexture() failed." );
	}*/

	return true;
}


void CD3DTextureResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SAFE_RELEASE( m_pTexture );

	SetState( GraphicsResourceState::RELEASED );
}


//==================================================================================================
// CD3DCubeTextureResource
//==================================================================================================


HRESULT CD3DCubeTextureResource::CreateD3DTexture( const TextureResourceDesc& desc, DWORD usage, D3DPOOL pool )
{
	SAFE_RELEASE( m_pCubeTexture );

	HRESULT hr = D3DXCreateCubeTexture( DIRECT3D9.GetDevice(),
	                                (UINT)desc.Width,
									(UINT)desc.MipLevels,
									usage,
									ConvertTextureFormatToD3DFORMAT( desc.Format ),
									pool,
									&m_pCubeTexture );

	return hr;
}


HRESULT CD3DCubeTextureResource::CreateD3DTextureFromFile( const std::string& filepath )
{
	SAFE_RELEASE( m_pCubeTexture );
	return D3DXCreateCubeTextureFromFile( DIRECT3D9.GetDevice(), filepath.c_str(), &m_pCubeTexture );
}


LPDIRECT3DSURFACE9 CD3DCubeTextureResource::GetPrimaryTextureSurface()
{
	if( !m_pCubeTexture )
		return NULL;

	LPDIRECT3DSURFACE9 pSurfaceLevel = NULL;
	HRESULT surf_hr = m_pCubeTexture->GetCubeMapSurface( D3DCUBEMAP_FACE_POSITIVE_X, 0, &pSurfaceLevel );

	return pSurfaceLevel;
}


CD3DCubeTextureResource::CD3DCubeTextureResource( const TextureResourceDesc *pDesc )
:
CD3DTextureResource(pDesc), m_pCubeTexture(NULL)
{
}


//==================================================================================================
// CD3DShaderResource
//==================================================================================================

CD3DShaderResource::CD3DShaderResource( const ShaderResourceDesc *pDesc )
:
ShaderResource( pDesc )
{
}


CD3DShaderResource::~CD3DShaderResource()
{
}


ShaderManager *CD3DShaderResource::CreateShaderManager()
{
	const string& resource_path = m_ShaderDesc.ResourcePath;
	if( 3 < resource_path.length() )
	{
		if( resource_path.find(".fx") == resource_path.length() - 3 )
			return new CHLSLShaderManager;
		else if( resource_path.find(".cgfx") == resource_path.length() - 5 )
			return new CD3DCgEffect;
		else
			return new CD3DCgEffect;
	}
	else
		return new CHLSLShaderManager; // generic shaders are implemented in HLSL
}


ShaderManager *CD3DShaderResource::CreateFixedFunctionPipelineManager()
{
	return new CD3DFixedFunctionPipelineManager;
}


} // namespace amorphous
