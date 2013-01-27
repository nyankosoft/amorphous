#include "GLGraphicsResources.hpp"

#include "Graphics/TextureGenerators/TextureFillingAlgorithm.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "Graphics/OpenGL/glext.h" // GL_BGR
#include "Graphics/OpenGL/Shader/GLCgEffect.hpp"
#include "Graphics/OpenGL/Shader/GLShader.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/BitmapImage.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{
using namespace serialization;

using namespace std;


class CGLLockedTexture : public LockedTexture
{
	boost::shared_ptr<CBitmapImage> m_pImage;

public:

	CGLLockedTexture( boost::shared_ptr<CBitmapImage>& pImage )
		:
	m_pImage(pImage)
	{}

	int GetWidth() { return (int)m_pImage->GetWidth(); }

	int GetHeight() { return (int)m_pImage->GetHeight(); }

	bool IsValid() const { return (m_pImage && 0 < m_pImage->GetWidth() && 0 < m_pImage->GetHeight()); }

	void GetPixel( int x, int y, SFloatRGBAColor& dest )
	{
		U8 r=0, g=0, b=0, a=255;
		m_pImage->GetPixel( x, y, r, g, b, a );
		dest.SetRGBA(
			(float)r/255.0f,
			(float)g/255.0f,
			(float)b/255.0f,
			(float)a/255.0f
			);
	}

	void SetPixelARGB32( int x, int y, U32 argb_color )
	{
		SFloatRGBAColor color;
		color.SetARGB32( argb_color );
		m_pImage->SetPixel( x, y, color );
	}

	void SetAlpha( int x, int y, U8 alpha )
	{
//		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	void Clear( U32 argb_color )
	{
		SFloatRGBAColor color;
		color.SetARGB32( argb_color );

		Clear( color );
	}

	void Clear( const SFloatRGBAColor& color ) { m_pImage->FillColor( color ); }
};


CGLTextureResource::CGLTextureResource( const TextureResourceDesc *pDesc )
:
TextureResource(pDesc)
{
	m_TextureID = 0;
}


CGLTextureResource::~CGLTextureResource()
{
	Release();
}


/**
 Load the texture specified by resource path in the current desc
 does not change the reference count
*/
bool CGLTextureResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	return false;
}


bool CGLTextureResource::SaveTextureToImageFile( const std::string& image_filepath )
{
	return false;
}


GLint ToGLInternalFormat( TextureFormat::Format fmt )
{
	switch( fmt )
	{
	case TextureFormat::A8R8G8B8:      return GL_RGBA8;
	case TextureFormat::X8R8G8B8:      return GL_RGB8;
	case TextureFormat::A16R16G16B16F: return GL_RGBA16;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
	default:
		return GL_RGB8;
	}
}

/**
 \param [out] src_format
 \param [out] src_type
*/
Result::Name GetSrcPixelTypeAndFormat( CBitmapImage& img, GLenum& src_format, GLenum& src_type )
{
	src_format = GL_RGB;
	src_type   = GL_UNSIGNED_BYTE;

	FIBITMAP *pFIBitmap = img.GetFBITMAP();
	if( !pFIBitmap )
		return Result::UNKNOWN_ERROR;

	bool supported = true;

	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType( pFIBitmap );
	unsigned int bpp = FreeImage_GetBPP( pFIBitmap );
	switch( color_type )
	{
	case FIC_RGB:
//		src_format = GL_RGB;
		src_format = GL_BGR;
		switch( bpp )
		{
		case 24:
			src_type = GL_UNSIGNED_BYTE;
			break;
		default:
			supported = false;
			break;
		}
		break;
	case FIC_RGBALPHA:
		src_format = GL_RGBA;
		switch( bpp )
		{
		case 32:
			src_type = GL_UNSIGNED_BYTE;
			break;
		default:
			supported = false;
			break;
		}
		break;
	default:
		supported = false;
		LOG_PRINT_ERROR( fmt_string( " An unsupported image format: color type = %s", img.GetColorTypeName() ) );
		break;
	}

	return supported ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


inline static int CalcNumMipmaps( int src )
{
	return (1 < src) ? (CalcNumMipmaps(src/2)+1) : 1;
}


inline static int GetNumMipmaps( const TextureResourceDesc& desc )
{
	if( 0 < desc.MipLevels )
		return desc.MipLevels;
	else
		return take_min( CalcNumMipmaps(desc.Width), CalcNumMipmaps(desc.Height) );
}


// Create texture from a bitmap image
// \param src_img [in] the source image. NOTE: the image is altered by one or more scaling operations to create mipmap textures.
bool CGLTextureResource::CreateGLTextureFromBitmapImage( CBitmapImage& src_image )
{
	GLenum src_format = GL_RGB;
	GLenum src_type   = GL_UNSIGNED_BYTE;
	Result::Name res = GetSrcPixelTypeAndFormat( src_image, src_format, src_type );
	if( res != Result::SUCCESS )
		return false;

	int num_mipmaps = GetNumMipmaps( m_TextureDesc );

	int next_width  = m_TextureDesc.Width;
	int next_height = m_TextureDesc.Height;
	for( int i=0; i<num_mipmaps; i++ )
	{
		if( 0 < i )
		{
			// Scale the image to the half in width and height to create the mipmap texture(s).
			next_width  /= 2;
			next_height /= 2;
			bool rescaled = src_image.Rescale( next_width, next_height );
			if( !rescaled )
			{
				LOG_PRINT_ERROR( fmt_string(" Failed to scale an image for mipmap texture(s): level=%d, path=%s", i, m_TextureDesc.ResourcePath.c_str() ) );
//				LOG_PRINT_ERROR( fmt_string(" Failed to scale an image for mipmap texture(s): level=%d", i ) );
				break;
			}
		}

		bool res = UpdateGLTextureImage( GL_TEXTURE_2D, i, next_width, next_height, src_format, src_type, FreeImage_GetBits(src_image.GetFBITMAP()) );
	}

	return true;//CreateGLTexture( GL_TEXTURE_2D, src_format, src_type, FreeImage_GetBits(img.GetFBITMAP()) );
}


/*
FIC_MINISBLACK Monochrome bitmap (1-bit) : first palette entry is black. Palletised bitmap (4 or 8-bit) and single channel non standard bitmap: the bitmap has a greyscale palette
FIC_MINISWHITE Monochrome bitmap (1-bit) : first palette entry is white. Palletised bitmap (4 or 8-bit) : the bitmap has an inverted greyscale palette
FIC_PALETTE Palettized bitmap (1, 4 or 8 bit)
FIC_RGB High-color bitmap (16, 24 or 32 bit), RGB16 or RGBF
FIC_RGBALPHA High-color bitmap with an alpha channel (32 bit bitmap, RGBA16 or RGBAF)
FIC_CMYK CMYK bitmap (32 bit only)
*/
// May only be called by render thread
// Used in synchronous loading
bool CGLTextureResource::LoadFromFile( const std::string& filepath )
{
	CBitmapImage img;
	bool loaded = img.LoadFromFile( filepath );
	if( !loaded )
		return false;

	bool res = img.FlipVertical();

	m_TextureDesc.Width  = img.GetWidth();
	m_TextureDesc.Height = img.GetHeight();

//	GetSourcePixelType( img );

	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	return CreateGLTextureFromBitmapImage( img );
}


/// Creates an empty texture
bool CGLTextureResource::Create()
{
	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	int level = 0;
	return UpdateGLTextureImage( GL_PROXY_TEXTURE_2D, level, m_TextureDesc.Width, m_TextureDesc.Height, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
}


bool CGLTextureResource::UpdateGLTextureImage( GLenum target, int level, int width, int height, const GLenum& src_format, const GLenum& src_type, void *pImageData )
{
//	glGenTextures( 1, &m_TextureID );

//	LOG_GL_ERROR( "glGenTextures() failed." );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );

	LOG_GL_ERROR( "glBindTexture() failed." );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	TextureResourceDesc& desc = m_TextureDesc;

//	GLint level = 0;
//	GLint level = (0 < desc.MipLevels) ? (desc.MipLevels - 1) : min( mip_level_calc(desc.Width), mip_level_calc(desc.Height) );

	GLint internal_format = ToGLInternalFormat( desc.Format );
	internal_format = GL_RGBA8;

	LOG_GL_ERROR( "Clearing error before glTexImage2D()" );

//	vector<uchar> fake_img_data;
//	fake_img_data.resize( desc.Width * desc.Height * 3, 0x80 );

	if( true )//desc.MipLevels == 1 )
	{
		glTexImage2D( target,                 // GLenum target,
					  level,                  // GLint level,
					  internal_format,        // GLint internalformat,
					  width,                  // GLsizei width,
					  height,                 // GLsizei height,
					  0,                      // GLint border,
					  src_format,             // GLenum format,
					  src_type,               // GLenum type
					  pImageData // const GLvoid *pixels
					  //&(fake_img_data[0]) // const GLvoid *pixels
					  );

		LOG_GL_ERROR( fmt_string( "glTexImage2D() failed: level=%d, width=%d, height=%d", level, width, height ).c_str() );
	}
/*	else
	{
		// Build the complete mipmap chain
		gluBuild2DMipmaps(
			target,
			internal_format,
			desc.Width,
			desc.Height,
			src_format,
			src_type,
			pImageData );
	}*/


//	LOG_GL_ERROR( "error in glTexImage2D(): " );

	return true;
/*
	SAFE_RELEASE( m_pTexture );

	const CGLTextureResourceDesc& desc = m_TextureDesc;

	HRESULT hr;
	DWORD usage = 0;
	D3DPOOL pool = D3DPOOL_MANAGED;
//	DWORD usage = D3DUSAGE_DYNAMIC;
//	D3DPOOL pool = D3DPOOL_DEFAULT;
*/
}


bool CGLTextureResource::CreateFromDesc()
{
	const TextureResourceDesc& desc = m_TextureDesc;

	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	// create an empty texture
//	bool created = Create();
//	if( !created )
//		return false;

	int bpp = 32;
//	if( desc.Format == TextureFormat::X8R8G8B8 )
//		bpp = 24;

	m_pLockedImage.reset( new CBitmapImage( m_TextureDesc.Width, m_TextureDesc.Height, bpp ) );

	boost::shared_ptr<CGLLockedTexture> pLockedTex( new CGLLockedTexture(m_pLockedImage) );

	if( pLockedTex->IsValid() )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		boost::shared_ptr<TextureFillingAlgorithm> pLoader = desc.pLoader;
		if( pLoader )
		{
			// Let the user-defined routine to fill the texture
			pLoader->FillTexture( *(pLockedTex.get()) );
		}

		CreateGLTextureFromBitmapImage( *m_pLockedImage );

//		Unlock();

		SetState( GraphicsResourceState::LOADED );

		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to lock the texture: " + desc.ResourcePath );
		return false;
	}
}

U32 ARGB32toRGBA32( U32 src )
{
	U32 a = src >> 24;
	U32 rgb = src << 8;
	return ( rgb | a );
}


bool CGLTextureResource::Lock( uint mip_level )
{
	return true;
}


bool CGLTextureResource::Unlock()
{
	return true;
}


void CGLTextureResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	glDeleteTextures( 1, &m_TextureID );

	SetState( GraphicsResourceState::RELEASED );
}



//==================================================================================================
// CGLShaderResource
//==================================================================================================

CGLShaderResource::CGLShaderResource( const ShaderResourceDesc *pDesc )
:
ShaderResource( pDesc )
{
}


CGLShaderResource::~CGLShaderResource()
{
}


CShaderManager *CGLShaderResource::CreateShaderManager()
{
	switch( m_ShaderDesc.ShaderType )
	{
	case CShaderType::VERTEX_SHADER:
		return new CGLVertexShader;
	case CShaderType::PIXEL_SHADER:
		return new CGLFragmentShader;
	case CShaderType::NON_PROGRAMMABLE:
		return NULL;//CGLFixedFunctionPipelineManager;
	case CShaderType::PROGRAMMABLE:
		{
			const string& resource_path = m_ShaderDesc.ResourcePath;
			if( resource_path.find(".cgfx") == resource_path.length() - 5 )
				return new CGLCgEffect;
			else
				return new CGLProgram;
		}
	default:
		LOG_PRINT_ERROR( "An invalid shader type: " + to_string( (int)m_ShaderDesc.ShaderType ) );
		return NULL;
	}

	return NULL;
}


CShaderManager *CGLShaderResource::CreateFixedFunctionPipelineManager()
{
	return new CGLFixedFunctionPipelineManager;
}


/*
bool CGLShaderResource::LoadFromFile( const std::string& filepath )
{
	return false;
}
*/


} // namespace amorphous
