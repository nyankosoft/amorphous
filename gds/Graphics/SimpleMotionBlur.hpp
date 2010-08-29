#ifndef  __SimpleMotionBlur_HPP__
#define  __SimpleMotionBlur_HPP__


#include <boost/shared_ptr.hpp>
#include "Graphics/fwd.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"


/**

 CSimpleMotionBlur blur;

 blur.Begin();
 
 /// render scene ///

 blur.End();

 // render the scene with motion blur
 blur.Render();
 
 NOTE: derived from CGraphicsComponent to retrieve the screen resolution in InitForScreenSize()
 */
class CSimpleMotionBlur : public CGraphicsComponent
{
	/// used to render the scene
	boost::shared_ptr<CTextureRenderTarget> m_pSceneRenderTarget;

	boost::shared_ptr<CTextureRenderTarget> m_apTexRenderTarget[2];

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

	/// Initialize for fullscreen motion blur effect.
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

	void ReleaseGraphicsResources() {}

	void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}
};


#endif		/*  __SimpleMotionBlur_HPP__  */
