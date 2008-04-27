#ifndef  __TEXTURERENDERTARGET_H__
#define  __TEXTURERENDERTARGET_H__

#include <d3d9.h>
#include <d3dx9.h>
//#include "3DCommon/FVF_TLVertex.h"
//#include "3DCommon/FVF_TextureVertex.h"
#include "3DCommon/GraphicsComponentCollector.h"



class CTextureRenderTarget : public CGraphicsComponent
{
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

	int m_iTextureWidth;
	int m_iTextureHeight;

	DWORD m_dwBackgroundColor;

	// Turned on when an instance is initialized by InitScreenSizeRenderTarget().
	// The texture size is automatically resized to screen size (viewport size)
	// in LoadGraphicsResources()
	bool m_bScreenSizeRenderTarget;
	
public:

	CTextureRenderTarget();

	CTextureRenderTarget( int texture_width, int texture_height );

	~CTextureRenderTarget();

	void Init( int texture_width, int texture_height );

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
