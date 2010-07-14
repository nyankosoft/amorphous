#ifndef  __GLTextureRenderTargetImpl_HPP__
#define  __GLTextureRenderTargetImpl_HPP__


#include "Graphics/SurfaceFormat.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include "Graphics/TextureHandle.hpp"
#include <GL/gl.h>


class CTextureRenderTargetImpl : public CGraphicsComponent
{

public:

	enum OptionFlags
	{
		OPTFLG_NO_DEPTH_BUFFER    = ( 1 << 0 ),
		OPTFLG_ANOTHER_OPTION     = ( 1 << 1 ),
		OPTFLG_YET_ANOTHER_OPTION = ( 1 << 2 )
	};

	CTextureHandle m_Texture;


	SFloatRGBAColor m_BackgroundColor;

public:

	CTextureRenderTargetImpl();

	CTextureRenderTargetImpl( int texture_width, int texture_height, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	CTextureRenderTargetImpl( const CTextureResourceDesc& texture_desc );

	virtual ~CTextureRenderTargetImpl() {}

	/// Returns true on success
	bool Init( int texture_width,
		       int texture_height,
			   TextureFormat::Format texture_format = TextureFormat::A8R8G8B8,
			   uint option_flags = 0 );

	bool Init( const CTextureResourceDesc& texture_desc );

	/// Creates the render target of the current screen size
	/// - The texture size is automatically resized to screen size (viewport size)
	///   in LoadGraphicsResources().
	bool InitScreenSizeRenderTarget();


//	void SetTextureWidth( const int width, const int height );

	void SetBackgroundColor( const SFloatRGBAColor& bg_color ) { m_BackgroundColor = bg_color; }

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface ); }

//	inline LPDIRECT3DTEXTURE9 GetRenderTargetTexture() { return m_RenderTargetTexture; }

	/// returns the lockable texture of the scene
//	inline LPDIRECT3DTEXTURE9 GetRenderTargetCopyTexture() { return m_RenderTargetCopyTexture; }

	void UpdateScreenSize();

	bool LoadTextures();

	void ReleaseTextures();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OutputImageFile( const char* filename );
};



/*
// TODO: rename this to CGLTextureRenderTargetImpl
class CGLTextureRenderTargetImplBase : public CGraphicsComponent
{
public:

//	CGLTextureRenderTargetImpl( TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

//	CGLTextureRenderTargetImpl( int texture_width, int texture_height, TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

	virtual void Init(
		int texture_width,
		int texture_height,
		TextureFormat::Format texture_format = TextureFormat::A8R8G8B8,
		uint option_flags = 0 )
	{}
};
*/

// TODO: change this to class CD3DGLTextureRenderTargetImpl : public CGLTextureRenderTargetImpl
class CGLTextureRenderTargetImpl : public CTextureRenderTargetImpl
{
public:

	enum OptionFlags
	{
		OPTFLG_NO_DEPTH_BUFFER    = ( 1 << 0 ),
		OPTFLG_ANOTHER_OPTION     = ( 1 << 1 ),
		OPTFLG_YET_ANOTHER_OPTION = ( 1 << 2 )
	};

private:

	GLuint m_Framebuffer;  // color render target

	GLuint m_DepthRenderBuffer;

	GLuint m_RenderTargetTexture;

//	D3DVIEWPORT9 m_OriginalViewport;

	CTextureResourceDesc m_TextureDesc;


	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;

private:

	void Release();
	
public:

	CGLTextureRenderTargetImpl();

	CGLTextureRenderTargetImpl( int texture_width, int texture_height, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	CGLTextureRenderTargetImpl( const CTextureResourceDesc& texture_desc );

	~CGLTextureRenderTargetImpl();

	/// Returns true on success
	bool Init( int texture_width,
		       int texture_height,
			   TextureFormat::Format texture_format = TextureFormat::A8R8G8B8,
			   uint option_flags = 0 );

	bool Init( const CTextureResourceDesc& texture_desc );

	/// Creates the render target of the current screen size
	/// - The texture size is automatically resized to screen size (viewport size)
	///   in LoadGraphicsResources().
	bool InitScreenSizeRenderTarget();


//	void SetTextureWidth( const int width, const int height );

	void SetBackgroundColor( const SFloatRGBAColor bg_color ) { m_BackgroundColor = bg_color; }

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface ); }

//	inline LPDIRECT3DTEXTURE9 GetRenderTargetTexture() { return m_RenderTargetTexture; }

	/// returns the lockable texture of the scene
//	inline LPDIRECT3DTEXTURE9 GetRenderTargetCopyTexture() { return m_RenderTargetCopyTexture; }

	void UpdateScreenSize();

	bool LoadTextures();

	void ReleaseTextures();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OutputImageFile( const char* filename );
};


#endif		/*  __GLTextureRenderTargetImpl_HPP__  */
