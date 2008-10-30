#ifndef  __VarianceShadowMapManager_H__
#define  __VarianceShadowMapManager_H__


#include "ShadowMapManager.h"
#include "3DCommon/ShaderHandle.h"


/**
 */
class CVarianceShadowMapManager : public CShadowMapManager
{
	LPDIRECT3DTEXTURE9 m_pHBlurredShadowMap;

	LPDIRECT3DTEXTURE9 m_pBlurredShadowMap;

	CShaderHandle m_BlurShader;

private:

	D3DFORMAT GetShadowMapTextureFormat() { return D3DFMT_G16R16F; }

public:

	CVarianceShadowMapManager();

	CVarianceShadowMapManager( int texture_width, int texture_height );

	~CVarianceShadowMapManager();

	/// returns true on success
	bool Init();

	/// Creates the blurred shadowmap from the shadowmap
	void EndSceneShadowMap();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif		/*  __VarianceShadowMapManager_H__  */
