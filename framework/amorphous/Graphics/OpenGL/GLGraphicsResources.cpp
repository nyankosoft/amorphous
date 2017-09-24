#include "GLGraphicsResources.hpp"
#include "GLTextureUtilities.hpp"
#include "amorphous/Graphics/TextureGenerators/TextureFillingAlgorithm.hpp"
#include "amorphous/Graphics/TextureGenerators/TextureFilter.hpp"
#include "amorphous/Graphics/TextureGenerators/SingleColorTextureGenerator.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/OpenGL/GLGraphicsDevice.hpp"
//#include "amorphous/Graphics/OpenGL/Shader/GLCgEffect.hpp"
#include "amorphous/Graphics/OpenGL/Shader/GLShader.hpp"
#include "amorphous/Support/ImageArchiveAux.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Serialization/BinaryDatabase.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{
using namespace serialization;
using namespace std;
using std::shared_ptr;


class CGLLockedTexture : public LockedTexture
{
	shared_ptr<BitmapImage> m_pImage;

public:

	CGLLockedTexture( shared_ptr<BitmapImage>& pImage )
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

	inline void ARGB32toR8G8B8A8( U32 src, U8& red, U8& green, U8& blue, U8& alpha )
	{
		alpha = (U8)( (src >> 24) & 0x000000FF );
		red   = (U8)( (src >> 16) & 0x000000FF );
		green = (U8)( (src >>  8) & 0x000000FF );
		blue  = (U8)( (src)       & 0x000000FF );
	}

	void SetPixelARGB32( int x, int y, U32 argb_color )
	{
		SFloatRGBAColor color;
		color.SetARGB32( argb_color );
		U8 r=0,g=0,b=0,a=0;
		ARGB32toR8G8B8A8( argb_color, r, g, b, a );
		m_pImage->SetPixel( x, y, r, g, b, a );
	}

	void SetAlpha( int x, int y, U8 alpha )
	{
//		((U32 *)m_pBits)[ y * m_Width + x ] &= ( (alpha << 24) | 0x00FFFFFF );
	}

	// TODO: add FillColorARGB32(), a performance friendly alternative, to BitmapImage
//	void Clear( U32 argb_color ) { m_pImage->FillColorARGB32( argb_color ); }

	void Clear( const SFloatRGBAColor& color ) { m_pImage->FillFRGBAColor( color ); }
};


CGLTextureResource::CGLTextureResource( const TextureResourceDesc *pDesc )
:
GLTextureResourceBase(pDesc)
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
	if( !m_TextureDesc.IsValid() )
		return false;

	return SaveGL2DTextureToImageFile( m_TextureID, m_TextureDesc.Width, m_TextureDesc.Height, m_SourceFormat, m_SourceType, image_filepath );
}


SDim2 CGLTextureResource::GetSize2D( unsigned int level )
{
	if( m_TextureID == 0 )
		return SDim2(0,0);

	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	LOG_GL_ERROR( "glBindTexture() returned an error." );

	GLint width = 0, height = 0;

	glGetTexLevelParameteriv( GL_TEXTURE_2D, (GLint)level, GL_TEXTURE_WIDTH,  &width );
	LOG_GL_ERROR( fmt_string("glGetTexLevelParameteriv() returned an error (level: %u, called to retrieve width).", level).c_str() );

	glGetTexLevelParameteriv( GL_TEXTURE_2D, (GLint)level, GL_TEXTURE_HEIGHT, &height );
	LOG_GL_ERROR( fmt_string("glGetTexLevelParameteriv() returned an error (level: %u, called to retrieve height).", level).c_str() );

	return SDim2( (int)width, (int)height );
}


Result::Name CGLTextureResource::SetSamplingParameter( SamplingParameter::Name param, uint value )
{
	if( m_TextureID == 0 )
		return Result::UNKNOWN_ERROR;

	LOG_GL_ERROR( "Clearing OpenGL error(s)..." );

	glBindTexture( GL_TEXTURE_2D, m_TextureID );
	LOG_GL_ERROR( "glBindTexture() returned an error." );

	GLenum pname;
	switch( param )
	{
	case SamplingParameter::TEXTURE_WRAP_AXIS_0: pname = GL_TEXTURE_WRAP_S; break;
	case SamplingParameter::TEXTURE_WRAP_AXIS_1: pname = GL_TEXTURE_WRAP_T; break;
	case SamplingParameter::TEXTURE_WRAP_AXIS_2: pname = GL_TEXTURE_WRAP_R; break;
	case SamplingParameter::MIN_FILTER:          pname = GL_TEXTURE_MIN_FILTER; break;
	case SamplingParameter::MAG_FILTER:          pname = GL_TEXTURE_MAG_FILTER; break;
	default:
		LOG_PRINTF_ERROR(( " An unknown sampling parameter (%d)", (int)param ));
		return Result::UNKNOWN_ERROR;
	}

	GLint dest_value = 0;
	if( pname == GL_TEXTURE_WRAP_S
	 || pname == GL_TEXTURE_WRAP_T
	 || pname == GL_TEXTURE_WRAP_R )
	{
		switch(value)
		{
		case TextureAddressMode::REPEAT:          dest_value = GL_REPEAT;   break;
//		case TextureAddressMode::MIRRORED_REPEAT: dest_value = D3DTADDRESS_MIRROR; break;
		case TextureAddressMode::CLAMP_TO_BORDER: dest_value = GL_CLAMP; break;
		case TextureAddressMode::CLAMP_TO_EDGE:   dest_value = GL_CLAMP_TO_EDGE;  break;
		default:
			LOG_PRINTF_ERROR(( " An unsupported texture address mode (%d)", (int)value ));
			return Result::UNKNOWN_ERROR;
		}
	}
	else if( pname == GL_TEXTURE_MAG_FILTER )
	{
		switch(value)
		{
		case TextureFilter::NEAREST:          dest_value = GL_NEAREST;  break;
		case TextureFilter::LINEAR:           dest_value = GL_LINEAR;   break;
		default:
			LOG_PRINTF_ERROR(( " An unsupported texture mag filter (%d)", (int)value ));
			return Result::UNKNOWN_ERROR;
		}
	}
	else if( pname == GL_TEXTURE_MIN_FILTER )
	{
		switch(value)
		{
		case TextureFilter::NEAREST:          dest_value = GL_NEAREST; break;
		case TextureFilter::LINEAR:           dest_value = GL_LINEAR;  break;
		default:
			LOG_PRINTF_ERROR(( " An unsupported texture min filter (%d)", (int)value ));
			return Result::UNKNOWN_ERROR;
		}
	}
	else
	{
		LOG_PRINTF_ERROR(( " An unsupported parameter name (%d)", (int)pname ));
		return Result::UNKNOWN_ERROR;
	}

	glTexParameteri( GL_TEXTURE_2D, pname, dest_value );
	LOG_GL_ERROR( "glTexParameteri() returned an error." );

	return Result::SUCCESS;
}


GLint ToGLInternalFormat( TextureFormat::Format fmt )
{
	switch( fmt )
	{
	case TextureFormat::A8R8G8B8:      return GL_RGBA;
	case TextureFormat::X8R8G8B8:      return GL_RGB;
	case TextureFormat::R32F:          return GL_R32F;
	case TextureFormat::A16R16G16B16F: return GL_RGBA16F;
//	case TextureFormat::A32R32G32B32F: return GL_RGBA32F;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
//	case TextureFormat: return ;
	case TextureFormat::Invalid:
	default:
		return GL_RGBA;
	}
}


static Result::Name GetSrcPixelTypeAndFormat( const TextureResourceDesc& desc, GLenum& src_format, GLenum& src_type )
{
	bool is_render_target = (desc.UsageFlags & UsageFlag::RENDER_TARGET);

	src_format = is_render_target ? GL_RGBA : GL_BGRA;

	if( desc.Format == TextureFormat::R32F )
		src_type = GL_FLOAT;
	else
		src_type = GL_UNSIGNED_BYTE;

	return Result::SUCCESS;
}


/**
 \param[in]  img
 \param[in]  is_render_target
 \param[out] src_format
 \param[out] src_type
*/
static Result::Name GetSrcPixelTypeAndFormat( const TextureResourceDesc& desc, BitmapImage& img, bool is_render_target, GLenum& src_format, GLenum& src_type )
{
//	if( desc.UsageFlags & UsageFlag::RENDER_TARGET )
//		bool is_render_target;

	src_format = GL_RGB;
	src_type   = GL_UNSIGNED_BYTE;

	FIBITMAP *pFIBitmap = img.GetFBITMAP();
	if( !pFIBitmap )
		return Result::UNKNOWN_ERROR;

	bool supported = true;

	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType( pFIBitmap );
	unsigned int bpp = FreeImage_GetBPP( pFIBitmap );

	// Consider the format as the RGBA if the BPP is 32
	// Rationale: FreeImage seems to return FIC_RGB if the all the alpha channel bytes are 0xFFs.
	// See the definition of FreeImage_GetColorType() in Source/FreeImage/BitmapAccess.cpp
	// in the source code of FreeImage library.
	if( bpp == 32 )
		color_type = FIC_RGBALPHA;

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
		case 32:
			src_type = GL_UNSIGNED_BYTE;
			break;
		default:
			supported = false;
			break;
		}
		break;
	case FIC_RGBALPHA:
//		src_format = GL_RGBA; // Works when the texture is used as a render target.
//		src_format = GL_BGRA; // Works when an image file is used as a texture.
		src_format = is_render_target ? GL_RGBA : GL_BGRA;
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
		return take_max( CalcNumMipmaps(desc.Width), CalcNumMipmaps(desc.Height) );
}


// Create texture from a bitmap image
// \param src_img [in] the source image. NOTE: the image is altered by one or more scaling operations to create mipmap textures.
bool GLTextureResourceBase::CreateGLTextureFromBitmapImage( GLenum target, BitmapImage& src_img, GLuint& texture_id )
{
	const bool is_render_target = (m_TextureDesc.UsageFlags & UsageFlag::RENDER_TARGET);

	if( m_TextureDesc.Height == 1024 )
		int break_here = 1;

	GLenum src_format = GL_RGB;
	GLenum src_type   = GL_UNSIGNED_BYTE;

	Result::Name res = Result::UNKNOWN_ERROR;
	if( 0 < src_img.GetWidth() )
	{
		// An image for the texture has been loaded from a file, or generated with an algorithm
		res = GetSrcPixelTypeAndFormat( m_TextureDesc, src_img, is_render_target, src_format, src_type );
	}
	else
	{
		// We are creating an empty texture
		res = GetSrcPixelTypeAndFormat( m_TextureDesc, src_format, src_type );
	}

	if( res != Result::SUCCESS )
	{
		LOG_PRINT_ERROR( " GetSrcPixelTypeAndFormat() failed." );
		return false;
	}

	m_SourceFormat = src_format;
	m_SourceType   = src_type;

	int num_mipmaps = GetNumMipmaps( m_TextureDesc );

	m_NumMipmaps = (uint)num_mipmaps;

	shared_ptr<BitmapImage> pImageCopy;
	if( 0 < src_img.GetWidth() )
	{
		pImageCopy = src_img.CreateCopy();

		if( !pImageCopy )
		{
			string stat;
			GetStatus(stat);
			LOG_PRINT_ERROR( " Failed to create a copy of the source image (source: " + stat + ")." );
			return false;
		}
	}

	BitmapImage empty_image;

	BitmapImage& image_copy = pImageCopy ? (*pImageCopy) : empty_image;

	int next_width  = m_TextureDesc.Width;
	int next_height = m_TextureDesc.Height;
	if( next_width == 1024 && next_height == 1024 )
		int break_here = 1;

	for( int i=0; i<num_mipmaps; i++ )
	{
		if( 0 < i )
		{
			// Scale the image to the half in width and height to create the mipmap texture(s).
			next_width  /= 2;
			next_height /= 2;

			if( next_width == 0 )
				next_width = 1;

			if( next_height == 0 )
				next_height = 1;

			// Scale the image to half of its current size, if we have actually an image
			// Sometimes people need to create an empty texture with a complete mipmap chain,
			// and in such a case rescaling is unnecessary.
			if( 0 < image_copy.GetWidth() )
			{
				bool rescaled = image_copy.Rescale( next_width, next_height );
				//LOG_PRINTF((" %s - target scale: %d x %d, rescaled: %s", m_TextureDesc.ResourcePath.c_str(), next_width, next_height, rescaled ? "true" : "false" ));
				if( !rescaled )
				{
					LOG_PRINT_ERROR( fmt_string(" Failed to scale an image for mipmap texture(s): level=%d, path=%s", i, m_TextureDesc.ResourcePath.c_str() ) );
//					LOG_PRINT_ERROR( fmt_string(" Failed to scale an image for mipmap texture(s): level=%d", i ) );
					break;
				}

				// Uncomment this line to visualize the effect of the mipmaps
//				image_copy.FillColor( SFloatRGBAColor::Magenta() );
			}
		}

		image_copy.SaveToFile( fmt_string(".debug/gl_textures/%s_%d.png",boost::filesystem::path(m_TextureDesc.ResourcePath).leaf().string().c_str(),i) );

		void *pImageData = FreeImage_GetBits(image_copy.GetFBITMAP());
		bool res = UpdateGLTextureImage( target, i, next_width, next_height, src_format, src_type, pImageData, texture_id );
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
	using namespace boost::filesystem;

	BitmapImage img;
	bool loaded = false;

	if( path(filepath).extension() == ".ia" )
	{
		ImageArchive ia;
		ia.LoadFromFile(filepath);
		loaded = LoadBitmapImageFromImageArchive(ia,img);
	}
	else
	{
		loaded = img.LoadFromFile( filepath );
	}

	if( !loaded )
		return false;

	// FreeImage loads the image upside down, so we vertically flip the image data
//	bool res = img.FlipVertical();

	m_TextureDesc.Width  = img.GetWidth();
	m_TextureDesc.Height = img.GetHeight();

//	GetSourcePixelType( img );

	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	return CreateGLTextureFromBitmapImage( GL_TEXTURE_2D, img, m_TextureID );
}


/// Creates an empty texture
bool CGLTextureResource::Create()
{
	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	int level = 0;
	return UpdateGLTextureImage( GL_PROXY_TEXTURE_2D, level, m_TextureDesc.Width, m_TextureDesc.Height, GL_RGBA, GL_UNSIGNED_BYTE, NULL, m_TextureID );
}


GLTextureResourceBase::GLTextureResourceBase( const TextureResourceDesc *pDesc )
:
TextureResource(pDesc),
m_SourceFormat(GL_RGB),
m_SourceType(GL_UNSIGNED_BYTE),
m_NumMipmaps(0)
{}


/**
 \param pImageData NULL if an empty texture is being created.
 */
bool GLTextureResourceBase::UpdateGLTextureImage(
	GLenum target,
	int level,
	int width,
	int height,
	const GLenum& src_format,
	const GLenum& src_type,
	void *pImageData,
	GLuint texture_id
	)
{
	glBindTexture( GL_TEXTURE_2D, texture_id );

	LOG_GL_ERROR( "glBindTexture() failed." );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	TextureResourceDesc& desc = m_TextureDesc;

	GLint internal_format = ToGLInternalFormat( desc.Format );

	// TODO: respect the specified format
//	internal_format = GL_RGBA;

	LOG_GL_ERROR( "Clearing error before glTexImage2D()" );

	LOG_PRINTF(( "glTexImage2D() level: %d, width: %d, height: %d", level, width, height ));

	if( true )
	{
		glTexImage2D( target,                 // GLenum target,
					  level,                  // GLint level,
					  internal_format,        // GLint internalformat,
					  width,                  // GLsizei width,
					  height,                 // GLsizei height,
					  0,                      // GLint border,
					  src_format,             // GLenum format,
					  src_type,               // GLenum type
					  pImageData              // const GLvoid *pixels
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

	return true;
}


bool CGLTextureResource::CreateFromDesc()
{
	const TextureResourceDesc& desc = m_TextureDesc;

	LOG_GL_ERROR( "clearing errors (if there is any) before calling glGenTextures()." );

	glGenTextures( 1, &m_TextureID );

	LOG_GL_ERROR( "glGenTextures() failed." );

	if( m_TextureID == 0 )
	{
		int break_here = 1;
	}

	// create an empty texture
//	bool created = Create();
//	if( !created )
//		return false;

	int bpp = 32;
//	if( desc.Format == TextureFormat::X8R8G8B8 )
//		bpp = 24;

//	if( m_TextureDesc.Height == 512 )
//		m_TextureDesc.Height = 1024;
/*
	m_pLockedImage.reset( new BitmapImage( m_TextureDesc.Width, m_TextureDesc.Height, bpp ) );

	shared_ptr<CGLLockedTexture> pLockedTex( new CGLLockedTexture(m_pLockedImage) );

	if( pLockedTex->IsValid() )
	{
		// An empty texture has been created
		// - fill the texture if loader was specified
		shared_ptr<TextureFillingAlgorithm> pLoader = desc.pLoader;

		if( !pLoader )
			pLoader.reset( new SingleColorTextureGenerator );

		if( pLoader )
		{
			// Let the user-defined routine to fill the texture
			pLoader->FillTexture( *pLockedTex );

			for( size_t i=0; i<pLoader->m_pFilters.size(); i++ )
			{
				if( pLoader->m_pFilters[i] )
					pLoader->m_pFilters[i]->ApplyFilter( *pLockedTex );
			}
		}

		bool res = CreateGLTextureFromBitmapImage( GL_TEXTURE_2D, *m_pLockedImage, m_TextureID );

		if( res )
		{
//			Unlock();
			SetState( GraphicsResourceState::LOADED );
			return true;
		}
		else
		{
			LOG_PRINT_ERROR( " CreateGLTextureFromBitmapImage() failed (" + desc.ResourcePath + ").");
			return false;
		}
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to lock the texture: " + desc.ResourcePath );
		return false;
	}
*/
	shared_ptr<TextureFillingAlgorithm> pLoader = desc.pLoader;

//	if( !pLoader )
//		pLoader.reset( new SingleColorTextureGenerator );

	if( pLoader )
	{
		m_pLockedImage.reset( new BitmapImage( m_TextureDesc.Width, m_TextureDesc.Height, bpp ) );

		shared_ptr<CGLLockedTexture> pLockedTex( new CGLLockedTexture(m_pLockedImage) );

		if( pLockedTex->IsValid() )
		{
			// An empty texture has been created
			// - fill the texture if loader was specified
			shared_ptr<TextureFillingAlgorithm> pLoader = desc.pLoader;

			// Let the user-defined routine to fill the texture
			pLoader->FillTexture( *pLockedTex );

			for( size_t i=0; i<pLoader->m_pFilters.size(); i++ )
			{
				if( pLoader->m_pFilters[i] )
					pLoader->m_pFilters[i]->ApplyFilter( *pLockedTex );
			}
		}
	}

	BitmapImage empty_image;
	BitmapImage& src_image = m_pLockedImage ? (*m_pLockedImage) : empty_image;

	bool res = CreateGLTextureFromBitmapImage( GL_TEXTURE_2D, src_image, m_TextureID );

	if( res )
	{
//		Unlock();
		SetState( GraphicsResourceState::LOADED );
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " CreateGLTextureFromBitmapImage() failed (" + desc.ResourcePath + ").");
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
	if( m_pLockedImage )
	{
		int w = m_pLockedImage->GetWidth();
		int h = m_pLockedImage->GetHeight();
		m_pLockedTexture.reset( new CGLLockedTexture( m_pLockedImage ) );
		return true;
	}
	else
	{
		m_pLockedImage.reset( new BitmapImage(m_TextureDesc.Width,m_TextureDesc.Height,32) );
		m_pLockedImage->FillFRGBAColor( SFloatRGBAColor::White() );
		return true;
	}

	return false;
}


bool CGLTextureResource::Unlock()
{
	m_pLockedTexture.reset();

	if( !m_pLockedImage )
		return false;

	BitmapImage& locked_image = *m_pLockedImage;

//	bool res = CreateGLTextureFromBitmapImage( GL_TEXTURE_2D, *m_pLockedImage, m_TextureID );

//	static int s_count = 0;
//	if( s_count < 5 )
//	{
//		locked_image.SaveToFile( "m_pLockedImage" + to_string(s_count) + ".png" );
//		SaveTextureToImageFile( "locked_texture_before_update" + to_string(s_count) + ".png" );
//		s_count += 1;
//	}

	shared_ptr<BitmapImage> pCopiedImage = locked_image.CreateCopy();
	if( !pCopiedImage )
		return false;

	BitmapImage& copied_image = *pCopiedImage;

	for( uint level=0; level<m_NumMipmaps; level++ )
	{
		if( 1 <= level )
		{
			int next_width  = copied_image.GetWidth()  / 2;
			int next_height = copied_image.GetHeight() / 2;

			if( next_width <= 0 || next_height <= 0 )
				return false;

			copied_image.Rescale( next_width, next_height );
		}

//		int level = 2;
		int tex_width  = copied_image.GetWidth();  //locked_image.GetWidth();
		int tex_height = copied_image.GetHeight(); //locked_image.GetHeight();
		unsigned int bpp = FreeImage_GetBPP( copied_image.GetFBITMAP() );
		unsigned int num_colors = FreeImage_GetColorsUsed( copied_image.GetFBITMAP() );
//		GLenum format = GL_RGBA;
		GLenum format = GL_BGRA; // GL_BGRA is set because is not specified as a render target
		const GLvoid *data = FreeImage_GetBits(m_pLockedImage->GetFBITMAP());

//		vector<U8> u8buffer;
//		u8buffer.resize( tex_width * tex_height * 4, 255 );
//		U8 r=0, g=0, b=0, a=0;
//		for( int y=0; y<tex_height; y++ )
//		{
//			for( int x=0; x<tex_width; x++ )
//			{
//				int offset = (y * tex_width + x) * 4;
//				copied_image.GetPixel(x,y,r,g,b,a);
//				u8buffer[offset + 0] = b;
//				u8buffer[offset + 1] = g;
//				u8buffer[offset + 2] = r;
//				u8buffer[offset + 3] = a;
//			}
//		}
//		const GLvoid *data = (GLvoid*)&u8buffer[0];

		if( !data )
			return false;

		LOG_GL_ERROR( "Clearing error before glBindTexture() in CGLTextureResource::Unlock()" );

		glBindTexture( GL_TEXTURE_2D, m_TextureID );

		LOG_GL_ERROR( "Clearing error before glTexSubImage2D() in CGLTextureResource::Unlock()" );

		glTexSubImage2D(
			GL_TEXTURE_2D,
			level,            // GLint level
			0,                // GLint xoffset
			0,                // GLint yoffset
			tex_width,        // GLsizei width
			tex_height,       // GLsizei height
			format,           // GLenum format
			GL_UNSIGNED_BYTE, // GLenum type
			(GLvoid*)data     //const GLvoid * data
			);

		LOG_GL_ERROR( fmt_string( "glTexSubImage2D() failed: level=%d, width=%d, height=%d", level, tex_width, tex_height ).c_str() );
	}

	return true;
}


void CGLTextureResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	glDeleteTextures( 1, &m_TextureID );

	m_TextureID = 0;

	m_pLockedImage.reset();

	SetState( GraphicsResourceState::RELEASED );
}


//==================================================================================================
// CGLCubeTextureResource
//==================================================================================================

bool CGLCubeTextureResource::LoadFromFile( const std::string& filepath )
{
	if( filepath.find( "%u" ) == string::npos )
	{
		LOG_PRINT_ERROR( "'%u' was not found in the input pathname." );
		return false;
	}

	glGenTextures( NUM_CUBE_MAP_FACES, m_TextureIDs );

	LOG_GL_ERROR( "glGenTextures() failed." );

	const GLenum cube_map_targets[] =
	{
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	int prev_width = 0, prev_height = 0;
	for( uint i=0; i<NUM_CUBE_MAP_FACES; i++ )
	{
		string image_pathname = fmt_string( filepath.c_str(), i );

		BitmapImage img;
		bool loaded = img.LoadFromFile( image_pathname );
		if( !loaded )
			return false;

		if( i == 0 )
		{
			prev_width  = img.GetWidth();
			prev_height = img.GetHeight();
		}
		else
		{
			if( prev_width  != img.GetWidth()
			 || prev_height != img.GetHeight() )
			{
				LOG_PRINT_ERROR( " All the cube map images must have the same widths and heights." );
				return false;
			}
		}

//		bool res = img.FlipVertical();

		bool res = CreateGLTextureFromBitmapImage( cube_map_targets[i], img, m_TextureIDs[i] );
		if( !res )
			return false;
	}

	m_TextureDesc.Width  = prev_width;
	m_TextureDesc.Height = prev_height;

	return true;
}


bool CGLCubeTextureResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	LOG_PRINT_ERROR( " Not implemented yet." );
	return false;
}


bool CGLCubeTextureResource::Create()
{
	LOG_PRINT_ERROR( " Not implemented yet." );
	return false;
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


ShaderManager *CGLShaderResource::CreateShaderManager()
{
	switch( m_ShaderDesc.ShaderType )
	{
	case ShaderType::VERTEX_SHADER:
		return new CGLVertexShader;
	case ShaderType::PIXEL_SHADER:
		return new CGLFragmentShader;
	case ShaderType::NON_PROGRAMMABLE:
		return NULL;//CGLFixedFunctionPipelineManager;
	case ShaderType::PROGRAMMABLE:
		{
//			const string& resource_path = m_ShaderDesc.ResourcePath;
//			if( resource_path.find(".cgfx") == resource_path.length() - 5 )
//				return new CGLCgEffect;
//			else
				return new CGLProgram;
		}
		LOG_PRINT_ERROR( " An unsupported shader type: " + to_string((int)m_ShaderDesc.ShaderType) );
		return NULL;
	default:
		LOG_PRINT_ERROR( "An invalid shader type: " + to_string( (int)m_ShaderDesc.ShaderType ) );
		return NULL;
	}

	return NULL;
}


ShaderManager *CGLShaderResource::CreateFixedFunctionPipelineManager()
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
