#ifndef  __ShadowMaps_H__
#define  __ShadowMaps_H__

#include "fwd.hpp"
#include "ShaderHandle.hpp"
#include "GraphicsComponentCollector.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"
#include "Shader/GenericShaderDesc.hpp"
#include "Camera.hpp"
#include "CubeMapManager.hpp"
#include "MeshObjectHandle.hpp"

#include <assert.h>


namespace amorphous
{


class ShadowMap;
//class DirectionalLightShadowMap;
class OrthoShadowMap;
class PointLightShadowMap;
class SpotlightShadowMap;


class ShadowMapSceneRenderer
{
public:

	/// render objects that cast shadows
	virtual void RenderSceneToShadowMap( Camera& camera ) = 0;

	/// render objects which are cast shadows by others
	virtual void RenderShadowReceivers( Camera& camera ) = 0;
};


class CCubeShadowMapSceneRenderer : public CCubeMapSceneRenderer
{
	ShadowMapSceneRenderer *m_pSceneRenderer;

public:

	void SetSceneRenderer( ShadowMapSceneRenderer *pSceneRenderer ) { m_pSceneRenderer = pSceneRenderer; }

	void RenderSceneToCubeMap( Camera& camera )
	{
		m_pSceneRenderer->RenderSceneToShadowMap( camera );
	}

	virtual Vector3 GetCameraPosition() { return Vector3(0,0,0); }
};


class ShadowMapVisitor
{
public:

	virtual ~ShadowMapVisitor() {}

//	virtual void Visit( DirectionalLightShadowMap& shadow_map ) {}
	virtual void Visit( OrthoShadowMap& shadow_map ) {}
	virtual void Visit( SpotlightShadowMap& shadow_map ) {}
	virtual void Visit( PointLightShadowMap& shadow_map ) {}
};


class ShadowMap : public GraphicsComponent
{
protected:

	ShadowMapSceneRenderer *m_pSceneRenderer;

	int m_ShadowMapSize;

	/// used to create camera matrix and projection matrix to render shadow map
	/// - Stores light direction and position
	Camera m_LightCamera;

	ShaderHandle m_Shader;

	/// borrowed reference of scene camera
	/// - Used by DirectionalLightShadowMap
	Camera *m_pSceneCamera;

	bool m_DisplayShadowMapTexturesForDebugging;


	/// Stores the shader necessasry for shadowmap.
	/// The shader supposed to contain 2 techniques
//	TextureRenderTarget m_SceneRenderTarget;
//	static std::string ms_strDefaultShaderFilename;
//	std::string m_ShadowMapShaderFilename;

protected:

	bool m_UseLightPosInWorldSpace;

	TextureFormat::Format m_ShadowMapTextureFormat;

//	void SetDefault();

//	bool CreateSceneShadowMapTextures();
//	virtual D3DFORMAT GetShadowMapTextureFormat() { return D3DFMT_R32F; }

	virtual void BeginSceneShadowReceivers() {}

public:

	ShadowMap()
		:
	m_pSceneRenderer(NULL),
	m_ShadowMapSize(1024),
	m_ShadowMapTextureFormat(TextureFormat::R32F),
	m_DisplayShadowMapTexturesForDebugging(false),
	m_UseLightPosInWorldSpace(true)
	{}

//	ShadowMap( const ShaderHandle& shader );
//	ShadowMap( int texture_width, int texture_height );

	virtual ~ShadowMap();

	TextureFormat::Format GetShadowMapTextureFormat() const { return m_ShadowMapTextureFormat; }

	void SetShadowMapTextureFormat( TextureFormat::Format fmt ) { m_ShadowMapTextureFormat = fmt; }

	virtual bool CreateShadowMapTextures() = 0;

	virtual void SetSceneRenderer( ShadowMapSceneRenderer *pSceneRenderer ) { m_pSceneRenderer = pSceneRenderer; }

	virtual void RenderSceneToShadowMap( Camera& camera );

	virtual void RenderShadowReceivers( Camera& camera );

	/// returns true on success
	virtual bool Init() { return true; }

	void SetShader( ShaderHandle& shader ) { m_Shader = shader; }

	void SetSceneCamera( Camera *pCamera ) { m_pSceneCamera = pCamera; }

//	void Init( int texture_width, int texture_height );

	/// Set width and height of shadow map texture
	void SetShadowMapSize( int size ) { m_ShadowMapSize = size; }

	virtual void BeginSceneShadowMap() = 0;

	/// why virtual?
	/// - See CVarianceShadowMapManager
	virtual void EndSceneShadowMap() = 0;


	virtual void UpdateLightPositionAndDirection() {}

	virtual void ReleaseTextures() {}

	void ReleaseGraphicsResources() { ReleaseTextures(); }

	void LoadGraphicsResources( const GraphicsParameters& rParam );

//	void SetLightDirection( const Vector3& vLightDir ) { m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) ); }
//	void SetLightPosition( const Vector3& vLightPos ) { m_LightCamera.SetPosition( vLightPos ); }
	void SetLightCamera( const Camera& camera ) { m_LightCamera = camera; }

	/// for visual debugging
	virtual void RenderShadowMapTexture( int sx, int sy, int ex, int ey ) {}
	void RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneWithoutShadow( int sx, int sy, int ex, int ey );
	void RenderSceneWithShadow( int sx, int sy, int ex, int ey );

	virtual void UpdateDirectionalLight( const DirectionalLight& light ) {}
	virtual void UpdatePointLight( const PointLight& light ) {}
	virtual void UpdateSpotlight( const Spotlight& light ) {}
//	virtual void UpdateLight( const CSpotLight& light ) {}

	virtual ShaderTechniqueHandle& ShadowMapTechnique( CVertexBlendType::Name vertex_blend_type = CVertexBlendType::NONE ) = 0;
	virtual ShaderTechniqueHandle& DepthTestTechnique( CVertexBlendType::Name vertex_blend_type = CVertexBlendType::NONE ) = 0;

	virtual LPDIRECT3DTEXTURE9 GetShadowMapTexture() { return NULL; }

	/// For debugging.
	/// Strings that end with '/' are treated as directory paths
	/// and the function determines the filename for output image
	void SaveShadowMapTextureToFile( const std::string& file_or_directory_path );

	virtual std::string CreateTextureFilename() { return std::string(); }

	virtual void SaveShadowMapTextureToFileInternal( const std::string& filepath ) {}

	virtual void Accept( ShadowMapVisitor& v ) {}
/*
	void SetCameraDirection( const Vector3& vCamDir ) { m_SceneCamera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) ); }
	void SetCameraPosition( const Vector3& vCamPos ) { m_SceneCamera.SetPosition( vCamPos ); }
*/

	static int ms_DebugShadowMap;
};


/// Shouldn't this be called "CSingleTextureShadowMap" ?
class FlatShadowMap : public ShadowMap
{
protected:

	ShaderTechniqueHandle m_ShadowMapTechnique;
	ShaderTechniqueHandle m_DepthTestTechnique;
	ShaderTechniqueHandle m_VertexBlendShadowMapTechnique;
	ShaderTechniqueHandle m_VertexBlendDepthTestTechnique;

	/// Texture to which the shadow map is rendered.
	/// Geometries that cast shadows to others (=shadow casters)
	/// are rendered to this texture.
//	LPDIRECT3DTEXTURE9 m_pShadowMap;

//	LPDIRECT3DSURFACE9 m_pShadowMapDepthBuffer; ///< Depth-stencil buffer for rendering to shadow map

	boost::shared_ptr<TextureRenderTarget> m_pShadowmapRenderTarget;

	MeshHandle m_ShadowCasterBoundingBox;

protected:

	void UpdateLightPositionAndDirection();

	void BeginSceneShadowReceivers();

	virtual void SetWorldToLightSpaceTransformMatrix() = 0;

	virtual void UpdateShadowMapSettings() = 0;

public:

	FlatShadowMap();

	virtual ~FlatShadowMap() {}

	ShaderTechniqueHandle& ShadowMapTechnique( CVertexBlendType::Name vertex_blend_type );
	ShaderTechniqueHandle& DepthTestTechnique( CVertexBlendType::Name vertex_blend_type );

	virtual bool CreateShadowMapTextures();

	void ReleaseTextures();

	void BeginSceneShadowMap();

	void EndSceneShadowMap();

	void RenderShadowMapTexture( int sx, int sy, int ex, int ey );

	std::string CreateTextureFilename();

//	LPDIRECT3DTEXTURE9 GetShadowMapTexture() { return m_pShadowMap; }

	virtual void SaveShadowMapTextureToFileInternal( const std::string& filepath );
};


/// Use this for directional light?
class OrthoShadowMap : public FlatShadowMap
{
	void UpdateShadowMapSettings();

	void UpdateLightPositionAndDirection();

	void SetWorldToLightSpaceTransformMatrix();

public:

	static float ms_fCameraShiftDistance;

public:

	OrthoShadowMap();

	void UpdateDirectionalLight( const DirectionalLight& light );

	void Accept( ShadowMapVisitor& v ) { v.Visit( *this ); }
};

/*
class DirectionalLightShadowMap : public FlatShadowMap
{
//	boost::weak_ptr<CHemisphericalDirectionalLight> m_DirectionalLight;

public:

	DirectionalLightShadowMap();

	void UpdateDirectionalLight( DirectionalLight& light );

	void Accept( ShadowMapVisitor& v ) { v.Visit( *this ); }
};*/


class SpotlightShadowMap : public FlatShadowMap
{
	void UpdateShadowMapSettings();

	void SetWorldToLightSpaceTransformMatrix();

public:

	/// used when 
	static float ms_fCameraShiftDistance;

public:

	SpotlightShadowMap();

	/// Called when shadow of directional light is done with spotlight shadow map
	void UpdateDirectionalLight( const DirectionalLight& light );

	void UpdateSpotlight( const Spotlight& light );

	void Accept( ShadowMapVisitor& v ) { v.Visit( *this ); }
};


/**
 Renders the shadowmap to each face of the cube texture
 - Calls RenderShadowMapScene() of m_pSceneRenderer 6 times

*/
class PointLightShadowMap : public ShadowMap
{
//	LPDIRECT3DCUBETEXTURE9 m_pCubeShadowMap;
//	LPDIRECT3DCUBETEXTURE9 m_pCubeShadowMapDepthBuffer;

	// or 

	CubeMapManager *m_pCubeShadowMapManager;

	CCubeShadowMapSceneRenderer m_CubeShadowMapSceneRenderer;

//	boost::weak_ptr<CPhemisphericalPointLight> m_PointpLight;

public:

	PointLightShadowMap();

	~PointLightShadowMap();

	bool CreateShadowMapTextures();

	void SetSceneRenderer( ShadowMapSceneRenderer *pSceneRenderer )
	{
		ShadowMap::SetSceneRenderer( pSceneRenderer );

		m_CubeShadowMapSceneRenderer.SetSceneRenderer( pSceneRenderer );
	}

	void RenderSceneToShadowMap();

	void BeginSceneShadowMap();

	void EndSceneShadowMap();

	void UpdatePointLight( const PointLight& light );

	std::string CreateTextureFilename();

	LPDIRECT3DTEXTURE9 GetShadowMapTexture()
	{
		assert( !"How to set the cube shadowmap texture?" );
		return NULL;
	}

	void SaveShadowMapTextureToFileInternal( const std::string& filepath );

	void Accept( ShadowMapVisitor& v ) { v.Visit( *this ); }
};


} // namespace amorphous



#endif		/*  __ShadowMaps_H__  */
