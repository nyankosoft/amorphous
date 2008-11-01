#ifndef  __ShadowMapManager_H__
#define  __ShadowMapManager_H__

#include <d3d9.h>
#include <d3dx9.h>
#include "3DMath/Vector3.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/TextureRenderTarget.h"
#include "3DCommon/Camera.h"
#include "3DCommon/3DGameMath.h"


/**
 * Still in an early stage of the development
 * - requires "Shader/ShadowMap.fx" to run
 *
 * usage notes:
 * steps
 * //1. for 
 * 1. Call CShadowMapManager::BeginSceneShadowMap().
 * 2. render objects that cast shadow to others using the shader technique "ShadowMap".
 *    Set the technique through the effect obtained by
 *    CShader::Get()->GetShaderManager()->GetEffect()
 *    e.g.) CShader::Get()->GetShaderManager()->GetEffect()->SetTechnique( "..." ).
 * 3. Call CShadowMapManager::EndSceneShadowMap().
 *
 * 4. Call CShadowMapManager::BeginSceneDepthMap().
 * 5. Render objects that receive shadow from the objects rendered in step 2
 *    using the shader technique "SceneShadowMap".
 * 6. Call CShadowMapManager::EndSceneDepthMap()
 *
 * 7. Call CShadowMapManager::BeginScene()
 * 8. Render objects using original shaders
 * 9. Call CShadowMapManager::EndScene()
 *
 */
class CShadowMapManager : public CGraphicsComponent
{
protected:

	/// used to temporarily hold original surfaces
	LPDIRECT3DSURFACE9 m_pOriginalSurface;
	LPDIRECT3DSURFACE9 m_pOriginalDepthSurface;

	D3DVIEWPORT9 m_OriginalViewport;

	int m_iTextureWidth;
	int m_iTextureHeight;

	int m_ShadowMapSize;

	LPDIRECT3DTEXTURE9 m_pShadowMap;    ///< Texture to which the shadow map is rendered
	LPDIRECT3DSURFACE9 m_pDSShadow;     ///< Depth-stencil buffer for rendering to shadow

	LPDIRECT3DTEXTURE9 m_pShadowedView;
	LPDIRECT3DSURFACE9 m_pDSShadowedView;

	/// used to create camera matrix and projection matrix to render shadow map
	/// - Stores light direction and position
	CCamera m_LightCamera;

	CCamera m_SceneCamera;

	/// Stores the shader necessasry for shadowmap.
	/// The shader supposed to contain 2 techniques
	/// - "ShadowMap": for shadow map rendering. renders the shadow casters to shadow map texture. m_LightCamera is used to calculate  
	CShaderManager m_ShaderManager;

	CTextureRenderTarget m_SceneRenderTarget;

//	static std::string ms_strDefaultShaderFilename;

	std::string m_ShadowMapShaderFilename;

	bool m_DisplayShadowMapTexturesForDebugging;

protected:

	void SetDefault();

	bool CreateShadowMapTextures();

	bool CreateSceneShadowMapTextures();

	virtual D3DFORMAT GetShadowMapTextureFormat() { return D3DFMT_R32F; }

public:

	CShadowMapManager();

	CShadowMapManager( int texture_width, int texture_height );

	~CShadowMapManager();

	/// returns true on success
	virtual bool Init();

//	void Init( int texture_width, int texture_height );

//	void SetTextureWidth( const int width, const int height );

//	void BeginSceneForShadowCaster();
//	void EndSceneForShadowCaster();
//	void BeginSceneForShadowReceiver();
//	void EndSceneForShadowReceiver();

	/// sets m_ShaderManager to CShader (singleton)
	void BeginSceneShadowMap();

	/// why virtual?
	/// - See CVarianceShadowMapManager
	virtual void EndSceneShadowMap();


	virtual void UpdateLightPositionAndDirection();


	virtual void BeginSceneDepthMap();

	void EndSceneDepthMap();

	/// sets the render target texture for the scene
	void BeginScene();
	void EndScene();

	void RenderSceneWithShadow();

	void UpdateScreenSize();

	void ReleaseTextures();

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	void SetLightDirection( const Vector3& vLightDir ) { m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) ); }
	void SetLightPosition( const Vector3& vLightPos ) { m_LightCamera.SetPosition( vLightPos ); }
	void SetLightCamera( const CCamera& camera ) { m_LightCamera = camera; }

	void SetCameraDirection( const Vector3& vCamDir ) { m_SceneCamera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) ); }
	void SetCameraPosition( const Vector3& vCamPos ) { m_SceneCamera.SetPosition( vCamPos ); }
	void SetSceneCamera( const CCamera& camera ) { m_SceneCamera = camera; }

	CCamera& SceneCamera() { return m_SceneCamera; }

	/// for visual debugging
	void RenderShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneWithoutShadow( int sx, int sy, int ex, int ey );
	void RenderSceneWithShadow( int sx, int sy, int ex, int ey );

//	static void SetDefaultShaderFilename( const std::string& filename ) { ms_strDefaultShaderFilename = filename; }
	void SetShadowMapShaderFilename( const std::string& filename ) { m_ShadowMapShaderFilename = filename; }
};


#endif		/*  __ShadowMapManager_H__  */
