#ifndef  __TextureRenderTarget_HPP__
#define  __TextureRenderTarget_HPP__


#include "FloatRGBAColor.hpp"
#include "SurfaceFormat.hpp"
#include "GraphicsComponentCollector.hpp"
#include "GraphicsResourceDescs.hpp"
#include "TextureHandle.hpp"


namespace amorphous
{


class CTextureRenderTarget : public GraphicsComponent
{
protected:

	enum OptionFlags
	{
		OPTFLG_NO_DEPTH_BUFFER    = ( 1 << 0 ),
		OPTFLG_ANOTHER_OPTION     = ( 1 << 1 ),
		OPTFLG_YET_ANOTHER_OPTION = ( 1 << 2 )
	};

	CTextureResourceDesc m_TextureDesc;

	TextureHandle m_RenderTargetTexture;

	SFloatRGBAColor m_BackgroundColor;

	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;

public:

	CTextureRenderTarget();

	CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	CTextureRenderTarget( const CTextureResourceDesc& texture_desc );

	virtual ~CTextureRenderTarget();

//	CTextureRenderTarget( TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

//	CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

	/// Returns true on success
	bool Init(
		int texture_width,
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

	virtual void SetRenderTarget() {}

	virtual void ResetRenderTarget() {}

	virtual void CopyRenderTarget() {}// { GetRenderTargetData( m_RenderTargetSurface, m_RenderTargetCopySurface ); }

	TextureHandle GetRenderTargetTexture() { return m_RenderTargetTexture; }

	virtual LPDIRECT3DTEXTURE9 GetD3DRenderTargetTexture() { return NULL; }

	/// returns the lockable texture of the scene
	virtual LPDIRECT3DTEXTURE9 GetD3DRenderTargetCopyTexture() { return NULL; }

	virtual bool LoadTextures() { return false; }

	virtual void ReleaseTextures() {}

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const GraphicsParameters& rParam );

	virtual void OutputImageFile( const std::string& image_file_path ) {}

	static boost::shared_ptr<CTextureRenderTarget> (*ms_pCreateTextureRenderTarget)(void);

	static void SetInstanceCreationFunction( boost::shared_ptr<CTextureRenderTarget> (*CreateTextureRenderTarget)(void) ) { ms_pCreateTextureRenderTarget = CreateTextureRenderTarget; }

	static boost::shared_ptr<CTextureRenderTarget> Create();
};


} // namespace amorphous



#endif		/*  __TextureRenderTarget_HPP__  */
