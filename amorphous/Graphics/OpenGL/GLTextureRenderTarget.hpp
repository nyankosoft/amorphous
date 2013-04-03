#ifndef  __GLTextureRenderTarget_HPP__
#define  __GLTextureRenderTarget_HPP__


#include "../TextureRenderTarget.hpp"
#include <GL/gl.h>


namespace amorphous
{


class CGLTextureRenderTarget : public TextureRenderTarget
{
private:

	GLuint m_Framebuffer;  // color render target

	GLuint m_DepthRenderBuffer;

	GLuint m_RenderTargetTextureID;

//	D3DVIEWPORT9 m_OriginalViewport;


	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;

private:

	void Release();
	
public:

	CGLTextureRenderTarget();

	CGLTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	CGLTextureRenderTarget( const TextureResourceDesc& texture_desc );

	~CGLTextureRenderTarget();

	/// Returns true on success
	bool Init( int texture_width,
		       int texture_height,
			   TextureFormat::Format texture_format,
			   uint option_flags );

	bool Init( const TextureResourceDesc& texture_desc );

	/// Creates the render target of the current screen size
	/// - The texture size is automatically resized to screen size (viewport size)
	///   in LoadGraphicsResources().
	bool InitScreenSizeRenderTarget();


//	void SetTextureWidth( const int width, const int height );

	void SetBackgroundColor( const SFloatRGBAColor bg_color ) { m_BackgroundColor = bg_color; }

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface ); }

	bool LoadTextures();

	void ReleaseTextures();

	void OutputImageFile( const std::string& image_file_path );

	static boost::shared_ptr<TextureRenderTarget> Create() { boost::shared_ptr<CGLTextureRenderTarget> p( new CGLTextureRenderTarget ); return p; }
};

} // namespace amorphous



#endif		/*  __GLTextureRenderTarget_HPP__  */
