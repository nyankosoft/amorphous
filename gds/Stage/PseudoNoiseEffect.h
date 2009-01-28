#ifndef  __PSEUDONOISEEFFECT_H__
#define  __PSEUDONOISEEFFECT_H__


#include "Graphics/3DRect.h"
#include "Graphics/FVF_TLVertex.h"
#include "Graphics/GraphicsComponentCollector.h"
#include "Graphics/TextureHandle.h"


#define NUM_NVNOISE_TEXTURES 4

class CPseudoNoiseEffect : public CGraphicsComponent
{
	TLVERTEX2 m_avTextureRect[4];

	CTextureHandle m_aNoiseTexture[NUM_NVNOISE_TEXTURES];

//	LPDIRECT3DTEXTURE9 m_apNVNoiseTexture[NUM_NVNOISE_TEXTURES];

	/// an extra texture which is rendered with noise effect
//	LPDIRECT3DTEXTURE9 m_pExtraTexture;

	int m_iScreenWidth;
	int m_iScreenHeight;

public:

	CPseudoNoiseEffect();
	~CPseudoNoiseEffect();

//	void SetPosition( D3DXVECTOR3& rvMin, D3DXVECTOR3& rvMax );

	bool LoadNoiseTextures();

//	void LoadExtraTexture( const char *pcTextureFilename, float fScale );

	void RenderNoiseEffect();

	void SetNoiseTexture();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();

	void UpdateScreenSize();


};


#endif  /*  __PSEUDONOISEEFFECT_H__  */
