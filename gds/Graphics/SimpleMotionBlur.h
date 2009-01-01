#ifndef  __SimpleMotionBlur_H__
#define  __SimpleMotionBlur_H__


#include <d3d9.h>
#include <d3dx9.h>
#include "Graphics/GraphicsComponentCollector.h"
#include "Graphics/TextureRenderTarget.h"


/**

 CSimpleMotionBlur blur;

 blur.Begin();
 
 /// render scene ///

 blur.End();

 // render the scene with motion blur
 blur.Render();
 
 */
class CSimpleMotionBlur : public CGraphicsComponent
{
	/// used to render the scene
	CTextureRenderTarget *m_pSceneRenderTarget;

	CTextureRenderTarget *m_apTexRenderTarget[2];

	int m_TextureWidth;
	int m_TextureHeight;

	int m_TargetTexIndex;

//	DWORD m_dwBackgroundColor;

	float m_fBlurWeight;

	bool m_bFirst;
	
private:

	void InitTextureRenderTargetBGColors();
	
public:

	CSimpleMotionBlur();

	CSimpleMotionBlur( int texture_width, int texture_height );

	~CSimpleMotionBlur();

	void Init( int texture_width, int texture_height );

	/// Initialize as screen size motion blur effect.
	/// Texture render targets are automatically resized when the screen resolution is changed.
	void InitForScreenSize();

//	void SetBackgroundColor( const DWORD dwBGColor ) { m_dwBackgroundColor = dwBGColor; }
//	void SetRenderTarget();
//	void ResetRenderTarget();

	void Begin();

	void End();

	/// draw fullscreen rect with blurrred scene texture
	void Render();

	void SetBlurWeight( float weight ) { m_fBlurWeight = weight; }

	void UpdateScreenSize();

	bool LoadTextures();

	void ReleaseTextures();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif		/*  __SimpleMotionBlur_H__  */
