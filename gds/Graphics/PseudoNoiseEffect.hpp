#ifndef  __PseudoNoiseEffect_H__
#define  __PseudoNoiseEffect_H__


#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/TextureHandle.hpp"


// Implementaions of noise effects
// - Create a noise look-up texture
//   - No need to use programmable shader
// - Use rand function in HLSL
//   - Saves texture memory
class CPseudoNoiseEffect : public CGraphicsComponent
{
	C2DRect m_FullscreenRect;

	CTextureHandle m_NoiseTexture;

	uint m_NoiseTextureSize;

	uint m_NoisePixelSize;

	/// an extra texture which is rendered with noise effect
//	CTextureHandle m_ExtraTexture;

	int m_iScreenWidth;
	int m_iScreenHeight;

public:

	CPseudoNoiseEffect();
	~CPseudoNoiseEffect();

	void Init( float strength, uint noise_pixel_size );

//	void SetPosition( Vector2& rvMin, Vector2& rvMax );

	bool LoadNoiseTextures();

//	void LoadExtraTexture( const char *pcTextureFilename, float fScale );

	void RenderNoiseEffect();

	void SetNoiseTexture();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	void UpdateScreenSize();
};


#endif  /*  __PseudoNoiseEffect_H__  */
