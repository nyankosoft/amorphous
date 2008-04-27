#ifndef  __IMAGECAPTURE_H__
#define  __IMAGECAPTURE_H__

#include <d3d9.h>
#include <d3dx9.h>


class CImageCapture
{
	int m_iImageWidth;
	int m_iImageHeight;

	// for drawing glare image
	LPDIRECT3DTEXTURE9 m_pRenderTargetTexture;	// used as render targets
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface;
	LPDIRECT3DSURFACE9 m_pRenderTargetDepthSurface;

	D3DVIEWPORT9 m_OriginalViewport;
	PDIRECT3DSURFACE9 m_pOriginalSurface;
	PDIRECT3DSURFACE9 m_pOriginalDepthSurface;
//	TLVERTEX m_avTextureRect[4];
//	TEXTUREVERTEX m_avTextureRect2[4];

public:
	CImageCapture();
	~CImageCapture();

	void SetImageSize( int iWidth, int iHeight );
	void EnterCaptureMode();
	void ExitCaptureMode();
	bool GetCapturedImage( DWORD *pdwImageData );
	void Release();

};



#endif		/*  __IMAGECAPTURE_H__  */
