#include "GLGraphicsResources.hpp"

#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "Graphics/OpenGL/glext.h" // GL_BGR
#include "Graphics/OpenGL/Shader/GLShader.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/BitmapImage.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

using namespace std;
using namespace boost;


CGLTextureResource::CGLTextureResource( const CTextureResourceDesc *pDesc )
:
CTextureResource(pDesc)
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


static inline D3DXIMAGE_FILEFORMAT SuffixToD3DImgFmt( const std::string& suffix )
{
	if( suffix == "bmp" ) return D3DXIFF_BMP;
	else if( suffix == "jpg" ) return D3DXIFF_JPG;
	else if( suffix == "tga" ) return D3DXIFF_TGA;
	else if( suffix == "png" ) return D3DXIFF_PNG;
	else if( suffix == "dds" ) return D3DXIFF_DDS;
	else return D3DXIFF_BMP;
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
void GetSrcPixelTypeAndFormat( CBitmapImage& img, GLenum& src_format, GLenum& src_type )
{
	src_format = GL_RGB;
	src_type   = GL_UNSIGNED_BYTE;

	FIBITMAP *pFIBitmap = img.GetFBITMAP();
	if( !pFIBitmap )
		return;

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
			break;
		}
		break;
	default:
		break;
	}
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

	FIBITMAP *pFIBitmap = img.GetFBITMAP();

	if( !pFIBitmap )
	{
		return false;
	}

	BOOL res = FreeImage_FlipVertical( pFIBitmap );

	m_TextureDesc.Width  = img.GetWidth();
	m_TextureDesc.Height = img.GetHeight();

	GLenum src_format = GL_RGB;
	GLenum src_type   = GL_UNSIGNED_BYTE;
	GetSrcPixelTypeAndFormat( img, src_format, src_type );

//	GetSourcePixelType( img );

	return CreateGLTexture( GL_TEXTURE_2D, src_format, src_type, FreeImage_GetBits(img.GetFBITMAP()) );
}


/// Creates an empty texture
bool CGLTextureResource::Create()
{
	return CreateGLTexture( GL_PROXY_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
}


static int to_gl_mip_level( int texture_desc_mip_levels )
{
	if( texture_desc_mip_levels == 0 )
	{
		// Create a complete mipmap chain
		return 8;
	}
	else
	{
		return texture_desc_mip_levels - 1;
	}
}


bool CGLTextureResource::CreateGLTexture( GLenum target, const GLenum& src_format, const GLenum& src_type, void *pImageData )
{
	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );

	LOG_GL_ERROR( "glBindTexture() failed." );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	CTextureResourceDesc& desc = m_TextureDesc;

	GLint level = 0;
//	GLint level = to_gl_mip_level( desc.MipLevels );

	GLint internal_format = ToGLInternalFormat( desc.Format );
	internal_format = GL_RGBA8;

	LOG_GL_ERROR( "Clearing error before glTexImage2D()" );

//	vector<uchar> fake_img_data;
//	fake_img_data.resize( desc.Width * desc.Height * 3, 0x80 );

	glTexImage2D( target,                 // GLenum target,
				  level,                  // GLint level,
				  internal_format,        // GLint internalformat,
				  desc.Width,             // GLsizei width,
				  desc.Height,            // GLsizei height,
				  0,                      // GLint border,
				  src_format,             // GLenum format,
				  src_type,               // GLenum type
				  pImageData // const GLvoid *pixels
				  //&(fake_img_data[0]) // const GLvoid *pixels
				  );

	LOG_GL_ERROR( "error in glTexImage2D(): " );

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
	const CTextureResourceDesc& desc = m_TextureDesc;

	// create an empty texture
	bool created = Create();

	if( Lock( 0 ) && m_pLockedTexture->IsValid() )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		shared_ptr<CTextureFillingAlgorithm> pLoader = desc.pLoader;
		if( pLoader )
		{
			// Let the user-defined routine to fill the texture
			pLoader->FillTexture( *(m_pLockedTexture.get()) );
		}

		Unlock();

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


class CGLLockedTextureRGBA32 : public CLockedTexture
{
	shared_ptr< C2DArray<U32> > m_pImageBuffer;

public:

	CGLLockedTextureRGBA32( shared_ptr< C2DArray<U32> > pImgBuffer )
		:
	m_pImageBuffer(pImgBuffer)
	{}

	int GetWidth() { return (int)m_pImageBuffer->size_x(); };

	int GetHeight() { return (int)m_pImageBuffer->size_y(); };

	bool IsValid() const { return (m_pImageBuffer && 0 < m_pImageBuffer->size_x() && 0 < m_pImageBuffer->size_y()); }

	void SetPixelARGB32( int x, int y, U32 argb_color )
	{
//		C2DArray<SFloatRGBAColor>& img_buffer = (*m_pImageBuffer.get());
		(*m_pImageBuffer)(x,y) = ARGB32toRGBA32( argb_color );
	}

	void SetAlpha( int x, int y, U8 alpha )
	{
//		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	void Clear( U32 argb_color )
	{
		U32 rgba_color = ARGB32toRGBA32( argb_color );

		C2DArray<U32>& img_buffer = *m_pImageBuffer;
		img_buffer.resize( img_buffer.size_x(), img_buffer.size_y(), rgba_color );
	}

	void Clear( const SFloatRGBAColor& color ) { Clear( color.GetARGB32() ); }
};


class CGLLockedTexture : public CLockedTexture
{
	shared_ptr< C2DArray<SFloatRGBAColor> > m_pImageBuffer;

public:

	CGLLockedTexture( shared_ptr< C2DArray<SFloatRGBAColor> > pImgBuffer )
		:
	m_pImageBuffer(pImgBuffer)
	{}

	int GetWidth() { return (int)m_pImageBuffer->size_x(); };

	int GetHeight() { return (int)m_pImageBuffer->size_y(); };

	bool IsValid() const { return (m_pImageBuffer && 0 < m_pImageBuffer->size_x() && 0 < m_pImageBuffer->size_y()); }

	void SetPixelARGB32( int x, int y, U32 argb_color )
	{
		C2DArray<SFloatRGBAColor>& img_buffer = (*m_pImageBuffer.get());

		SFloatRGBAColor color;
		color.SetARGB32( argb_color );
		img_buffer(x,y) = color;
	}

	void SetAlpha( int x, int y, U8 alpha )
	{
//		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	void Clear( U32 argb_color )
	{
		SFloatRGBAColor color;
		color.SetARGB32( argb_color );

		C2DArray<SFloatRGBAColor>& img_buffer = *m_pImageBuffer;
		img_buffer.resize( img_buffer.size_x(), img_buffer.size_y(), color );
	}

	void Clear( const SFloatRGBAColor& color ) { Clear( color.GetARGB32() ); }
};


bool CGLTextureResource::Lock( uint mip_level )
{
	// allocate buffer

	// CBitmapImage <<< currently has no function to create an empty image?

	if( true /*format == RGBA32*/ )
	{
		m_pLockedTextureRGBA32ImageBuffer = shared_ptr< C2DArray<U32> >( new C2DArray<U32> );
		m_pLockedTextureRGBA32ImageBuffer->resize( m_TextureDesc.Width, m_TextureDesc.Height );
		m_pLockedTexture = shared_ptr<CGLLockedTextureRGBA32>( new CGLLockedTextureRGBA32(m_pLockedTextureRGBA32ImageBuffer) );
	}
	else
	{
		m_pLockedTextureImageBuffer = shared_ptr< C2DArray<SFloatRGBAColor> >( new C2DArray<SFloatRGBAColor> );
		m_pLockedTextureImageBuffer->resize( m_TextureDesc.Width, m_TextureDesc.Height );
		m_pLockedTexture = shared_ptr<CGLLockedTexture>( new CGLLockedTexture(m_pLockedTextureImageBuffer) );
	}

	return true;
}


bool CGLTextureResource::Unlock()
{
//	m_pImageBuffer.reset();

	if( m_TextureDesc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		GLint level = 0;
//		GLint level = to_gl_mip_level( m_TextureDesc.MipLevels );

		void *pixels = NULL;
		if( true /*format == RGBA32*/ )
		{
			if( m_pLockedTextureRGBA32ImageBuffer )
				pixels = &((*m_pLockedTextureRGBA32ImageBuffer)(0,0));
		}
		else
		{
			if( m_pLockedTextureImageBuffer )
				pixels = &((*m_pLockedTextureImageBuffer)(0,0));
		}

//		vector<char> dummy;
//		dummy.resize( m_TextureDesc.Width * m_TextureDesc.Height * sizeof(U32), 0 );

		glBindTexture( GL_TEXTURE_2D, m_TextureID );

		glTexImage2D( GL_TEXTURE_2D,          // GLenum target,
					  level,                  // GLint level,
					  GL_RGBA8,               // GLint internalformat,
					  m_TextureDesc.Width,    // GLsizei width,
					  m_TextureDesc.Height,   // GLsizei height,
					  0,                      // GLint border,
					  GL_RGBA,                // GLenum format,
					  GL_UNSIGNED_BYTE,       // GLenum type
					  pixels                  // const GLvoid *pixels
					  //&dummy[0]
					  );
	}

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

CGLShaderResource::CGLShaderResource( const CShaderResourceDesc *pDesc )
:
CShaderResource( pDesc )
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
		return new CGLProgram;
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
