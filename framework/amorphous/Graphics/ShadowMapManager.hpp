#ifndef  __ShadowMapManager_H__
#define  __ShadowMapManager_H__

#include <map>
#include "../3DMath/3DGameMath.hpp"
#include "GraphicsComponentCollector.hpp"
#include "TextureRenderTarget.hpp"
#include "Camera.hpp"
#include "GraphicsDevice.hpp"
#include "ShadowMaps.hpp"


namespace amorphous
{


/**
 * Still in an early stage of the development
 * - requires "Shader/ShadowMap.fx" to run
 *
 * usage notes:
 * steps
 * //1. for 
 * 1. Call ShadowMapManager::BeginSceneShadowMap().
 * 2. render objects that cast shadow to others using the shader technique "ShadowMap".
 *    Set the technique through the effect obtained by
 *    CShader::Get()->GetShaderManager()->GetEffect()
 *    e.g.) CShader::Get()->GetShaderManager()->GetEffect()->SetTechnique( "..." ).
 * 3. Call ShadowMapManager::EndSceneShadowMap().
 *
 * 4. Call ShadowMapManager::BeginSceneDepthMap().
 * 5. Render objects that receive shadow from the objects rendered in step 2
 *    using the shader technique "SceneShadowMap".
 * 6. Call ShadowMapManager::EndSceneDepthMap()
 *
 * 7. Call ShadowMapManager::BeginScene()
 * 8. Render objects using original shaders
 * 9. Call ShadowMapManager::EndScene()
 *
 */
class ShadowMapManager : public GraphicsComponent
{
protected:

	enum Params
	{
		NUM_MAX_SCENE_SHADOW_TEXTURES = 2,
	};

	typedef std::map< int, std::shared_ptr<ShadowMap> > IDtoShadowMap;

	IDtoShadowMap m_mapIDtoShadowMap;

	std::shared_ptr<ShadowMapSceneRenderer> m_pSceneRenderer;

	// Temporarily stores the shadow map to return shadowmap/depth test technique
	std::shared_ptr<ShadowMap> m_pCurrentShadowMap;

	int m_IDCounter;

	/// used to temporarily hold original surfaces
//	LPDIRECT3DSURFACE9 m_pOriginalSurface;
//	LPDIRECT3DSURFACE9 m_pOriginalDepthSurface;

	Viewport m_OriginalViewport;

	int m_iTextureWidth;
	int m_iTextureHeight;

	int m_ShadowMapSize;

	/// Holds textures(s) with the original screen size.
	/// Shadows of the scene are rendered to this texture, then overlayed
	/// to the original, non-shadowed scene.
	std::shared_ptr<TextureRenderTarget> m_apShadowTexture[NUM_MAX_SCENE_SHADOW_TEXTURES];

	Camera m_SceneCamera;

	/// Stores the shader necessasry for shadowmap.
	/// The shader supposed to contain 2 techniques
	/// - "ShadowMap": for shadow map rendering. renders the shadow casters to shadow map texture. m_LightCamera is used to calculate  
	ShaderHandle m_Shader;

	std::shared_ptr<TextureRenderTarget> m_pSceneRenderTarget;

//	static std::string ms_strDefaultShaderFilename;

	std::string m_ShadowMapShaderFilename;

	bool m_DisplayShadowMapTexturesForDebugging;

	ShaderHandle m_RectDrawShader;

protected:

	void SetDefault();

	bool CreateShadowMapTextures();

	bool CreateSceneShadowMapTextures();

	virtual TextureFormat::Format GetShadowMapTextureFormat() { return TextureFormat::R32F; }

public:

	ShadowMapManager();

	~ShadowMapManager();

	/// returns true on success
	virtual bool Init();

	/// The system sets the size to the screen width and height by default.
	void SetSceneShadowTextureSize( int texture_width, int texture_height );

	void SetSceneRenderer( std::shared_ptr<ShadowMapSceneRenderer> pSceneRenderer );

	std::map< int, std::shared_ptr<ShadowMap> >::iterator CreateShadowMap( U32 id, const Light& light );

	Result::Name UpdateLightForShadow( U32 id, const Light& light );

	void RemoveShadowForLight( int shadowmap_id );

	void UpdateLight( int shadowmap_id, const Light& light );

	void RenderShadowCasters( Camera& camera );

	void RenderShadowReceivers( Camera& camera );

	ShaderHandle& GetShader() { return m_Shader; }

	ShaderTechniqueHandle ShaderTechniqueForShadowCaster( VertexBlendTypeName vertex_blend_type = VertexBlendTypeName::NONE );

	ShaderTechniqueHandle ShaderTechniqueForShadowReceiver( VertexBlendTypeName vertex_blend_type = VertexBlendTypeName::NONE );

	ShaderTechniqueHandle ShaderTechniqueForNonShadowedCasters( VertexBlendTypeName vertex_blend_type = VertexBlendTypeName::NONE );

//	void Init( int texture_width, int texture_height );

//	void SetTextureWidth( const int width, const int height );

//	void BeginSceneForShadowCaster();
//	void EndSceneForShadowCaster();
//	void BeginSceneForShadowReceiver();
//	void EndSceneForShadowReceiver();

	/// sets m_ShaderManager to CShader (singleton)
	void BeginSceneShadowMap();

	/// why virtual?
	/// - See VarianceShadowMapManager
	virtual void EndSceneShadowMap();

	virtual void PostProcessShadowMap( ShadowMap& shadow_map ) {}

//	virtual void UpdateLightPositionAndDirection();


	virtual void BeginSceneDepthMap();

	void EndSceneDepthMap();

	/// sets the render target texture for the scene
	void BeginScene();
	void EndScene();

	bool HasShadowMap() const { return !(m_mapIDtoShadowMap.empty()); }

	void RenderSceneWithShadow();

	void UpdateScreenSize();

	void ReleaseTextures();

	virtual void ReleaseGraphicsResources() {}

	virtual void LoadGraphicsResources( const GraphicsParameters& rParam ) {}

	void SetCameraDirection( const Vector3& vCamDir ) { m_SceneCamera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) ); }
	void SetCameraPosition( const Vector3& vCamPos ) { m_SceneCamera.SetPosition( vCamPos ); }
	void SetSceneCamera( const Camera& camera ) { m_SceneCamera = camera; }

	Camera& SceneCamera() { return m_SceneCamera; }

	/// for visual debugging
//	void RenderShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneWithoutShadow( int sx, int sy, int ex, int ey );
	void RenderSceneWithShadow( int sx, int sy, int ex, int ey );

//	std::shared_ptr<TextureRenderTarget> GetSceneShadowTexture() { return m_apShadowTexture[0]; }

	TextureHandle GetSceneShadowTexture();

//	static void SetDefaultShaderFilename( const std::string& filename ) { ms_strDefaultShaderFilename = filename; }
	void SetShadowMapShaderFilename( const std::string& filename ) { m_ShadowMapShaderFilename = filename; }

	//
	// For debugging
	//

	void SaveShadowMapTexturesToImageFiles( const std::string& output_directory_path );

	void SaveSceneTextureToFile( const std::string& filename );
};

} // namespace amorphous



#endif		/*  __ShadowMapManager_H__  */
