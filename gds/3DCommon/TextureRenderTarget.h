#ifndef  __TEXTURERENDERTARGET_H__
#define  __TEXTURERENDERTARGET_H__

#include <d3d9.h>
#include <d3dx9.h>
#include "3DCommon/TextureFormat.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DCommon/GraphicsResourceDescs.h"

/*
// TODO: rename this to CTextureRenderTarget
class CTextureRenderTargetBase : public CGraphicsComponent
{
public:

//	CTextureRenderTarget( TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

//	CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format = A8R8G8B8, uint option_flags = 0 );

	virtual void Init(
		int texture_width,
		int texture_height,
		TextureFormat::Format texture_format = TextureFormat::A8R8G8B8,
		uint option_flags = 0 )
	{}
};
*/

// TODO: change this to class CD3DTextureRenderTarget : public CTextureRenderTarget
class CTextureRenderTarget : public CGraphicsComponent
{
public:

	enum OptionFlags
	{
		OPTFLG_NO_DEPTH_BUFFER    = ( 1 << 0 ),
		OPTFLG_ANOTHER_OPTION     = ( 1 << 1 ),
		OPTFLG_YET_ANOTHER_OPTION = ( 1 << 2 )
	};

private:

	LPDIRECT3DTEXTURE9 m_pRenderTargetTexture;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface;
	LPDIRECT3DSURFACE9 m_pRenderTargetDepthSurface;

	/// buffer to hold the content of render target surface
	/// need to access the scene image because the texture created
	/// as render target is not lockable
	LPDIRECT3DTEXTURE9 m_pRenderTargetCopyTexture;
	LPDIRECT3DSURFACE9 m_pRenderTargetCopySurface;

	/// used to temporarily hold original surfaces
	LPDIRECT3DSURFACE9 m_pOriginalSurface;
	LPDIRECT3DSURFACE9 m_pOriginalDepthSurface;

	D3DVIEWPORT9 m_OriginalViewport;

	CTextureResourceDesc m_TextureDesc;



	DWORD m_dwBackgroundColor;

	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;
	
public:

	CTextureRenderTarget();

	CTextureRenderTarget( int texture_width, int texture_height, TextureFormat::Format texture_format = TextureFormat::A8R8G8B8, uint option_flags = 0 );

	CTextureRenderTarget( const CTextureResourceDesc& texture_desc );

	~CTextureRenderTarget();

	/// Returns true on success
	bool Init( int texture_width,
		       int texture_height,
			   TextureFormat::Format texture_format = TextureFormat::A8R8G8B8,
			   uint option_flags = 0 );

	bool Init( const CTextureResourceDesc& texture_desc );

	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	void InitScreenSizeRenderTarget();


//	void SetTextureWidth( const int width, const int height );

	void SetBackgroundColor( const DWORD dwBGColor ) { m_dwBackgroundColor = dwBGColor; }

	void SetRenderTarget();

	void ResetRenderTarget();

	void CopyRenderTarget();// { GetRenderTargetData( m_pRenderTargetSurface, m_pRenderTargetCopySurface ); }

	inline LPDIRECT3DTEXTURE9 GetRenderTargetTexture() { return m_pRenderTargetTexture; }

	/// returns the lockable texture of the scene
	inline LPDIRECT3DTEXTURE9 GetRenderTargetCopyTexture() { return m_pRenderTargetCopyTexture; }

	void UpdateScreenSize();

	bool LoadTextures();

	void ReleaseTextures();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void OutputImageFile( const char* filename );
};


#endif		/*  __TEXTURERENDERTARGET_H__  */
