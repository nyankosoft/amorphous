#ifndef  __GLTextureRenderTarget_HPP__
#define  __GLTextureRenderTarget_HPP__


#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include <GL/gl.h>


namespace amorphous
{


class GLTextureRenderTarget : public TextureRenderTarget
{
private:

	GLuint m_Framebuffer;  // color render target

	GLuint m_DepthRenderBuffer;

	GLuint m_RenderTargetTextureID;

	GLint m_OrigFrameBuffer;

	Viewport m_OriginalViewport; // TODO: take this up to TextureRenderTarget


	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;

public:

	GLTextureRenderTarget();

	GLTextureRenderTarget( const TextureResourceDesc& texture_desc );

	~GLTextureRenderTarget();

	/// Creates the render target of the current screen size
	/// - The texture size is automatically resized to screen size (viewport size)
	///   in LoadGraphicsResources().
	bool InitScreenSizeRenderTarget();


//	void SetTextureWidth( const int width, const int height );

	void SetBackgroundColor( const SFloatRGBAColor& bg_color );

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface ); }

	bool LoadTextures();

	void ReleaseTextures();

	void OutputImageFile( const std::string& image_file_path );

	static boost::shared_ptr<TextureRenderTarget> Create() { boost::shared_ptr<GLTextureRenderTarget> p( new GLTextureRenderTarget ); return p; }
};

} // namespace amorphous



#endif		/*  __GLTextureRenderTarget_HPP__  */
