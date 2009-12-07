#ifndef  __ShadowMaps_H__
#define  __ShadowMaps_H__

#include <d3dx9.h>
#include <boost/shared_ptr.hpp>
#include "fwd.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/TextureRenderTarget.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/CubeMapManager.hpp"

#include <assert.h>


class CShadowMap;
class CDirectionalLightShadowMap;
class CPointLightShadowMap;
class CSpotLightShadowMap;


class CShadowMapSceneRenderer
{
public:

	/// render objects that cast shadows
	virtual void RenderSceneToShadowMap( CCamera& camera ) = 0;

	/// render objects which are cast shadows by others
	virtual void RenderShadowReceivers( CCamera& camera ) = 0;
};


class CCubeShadowMapSceneRenderer : public CCubeMapSceneRenderer
{
	CShadowMapSceneRenderer *m_pSceneRenderer;

public:

	void SetSceneRenderer( CShadowMapSceneRenderer *pSceneRenderer ) { m_pSceneRenderer = pSceneRenderer; }

	void RenderSceneToCubeMap( CCamera& camera )
	{
		m_pSceneRenderer->RenderSceneToShadowMap( camera );
	}

	virtual Vector3 GetCameraPosition() { return Vector3(0,0,0); }
};


class CShadowMapVisitor
{
public:

	virtual ~CShadowMapVisitor() {}

	virtual void Visit( CDirectionalLightShadowMap& shadow_map ) {}
	virtual void Visit( CPointLightShadowMap& shadow_map ) {}
	virtual void Visit( CSpotLightShadowMap& shadow_map ) {}
};


class CShadowMap : public CGraphicsComponent
{
protected:

	CShadowMapSceneRenderer *m_pSceneRenderer;

	int m_ShadowMapSize;

	/// used to create camera matrix and projection matrix to render shadow map
	/// - Stores light direction and position
	CCamera m_LightCamera;

	CShaderHandle m_Shader;

	/// borrowed reference of scene camera
	/// - Used by CDirectionalLightShadowMap
	CCamera *m_pSceneCamera;

	bool m_DisplayShadowMapTexturesForDebugging;


	/// Stores the shader necessasry for shadowmap.
	/// The shader supposed to contain 2 techniques
//	CTextureRenderTarget m_SceneRenderTarget;
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

	CShadowMap()
		:
	m_pSceneRenderer(NULL),
	m_ShadowMapSize(512),
	m_ShadowMapTextureFormat(TextureFormat::R32F),
	m_DisplayShadowMapTexturesForDebugging(false),
	m_UseLightPosInWorldSpace(true)
	{}

//	CShadowMap( const CShaderHandle& shader );
//	CShadowMap( int texture_width, int texture_height );

	virtual ~CShadowMap();

	TextureFormat::Format GetShadowMapTextureFormat() const { return m_ShadowMapTextureFormat; }

	void SetShadowMapTextureFormat( TextureFormat::Format fmt ) { m_ShadowMapTextureFormat = fmt; }

	virtual bool CreateShadowMapTextures() = 0;

	virtual void SetSceneRenderer( CShadowMapSceneRenderer *pSceneRenderer ) { m_pSceneRenderer = pSceneRenderer; }

	virtual void RenderSceneToShadowMap( CCamera& camera );

	virtual void RenderShadowReceivers( CCamera& camera );

	/// returns true on success
	virtual bool Init() { return true; }

	void SetShader( CShaderHandle& shader ) { m_Shader = shader; }

	void SetSceneCamera( CCamera *pCamera ) { m_pSceneCamera = pCamera; }

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

	void LoadGraphicsResources( const CGraphicsParameters& rParam );

//	void SetLightDirection( const Vector3& vLightDir ) { m_LightCamera.SetOrientation( CreateOrientFromFwdDir( vLightDir ) ); }
//	void SetLightPosition( const Vector3& vLightPos ) { m_LightCamera.SetPosition( vLightPos ); }
	void SetLightCamera( const CCamera& camera ) { m_LightCamera = camera; }

	/// for visual debugging
	virtual void RenderShadowMapTexture( int sx, int sy, int ex, int ey ) {}
	void RenderSceneShadowMapTexture( int sx, int sy, int ex, int ey );
	void RenderSceneWithoutShadow( int sx, int sy, int ex, int ey );
	void RenderSceneWithShadow( int sx, int sy, int ex, int ey );

	virtual void UpdateDirectionalLight( CDirectionalLight& light ) {}
	virtual void UpdatePointLight( CPointLight& light ) {}
//	virtual void UpdateLight( CSpotLight& light ) {}

	virtual LPDIRECT3DTEXTURE9 GetShadowMapTexture() { return NULL; }

	/// For debugging.
	/// Strings that end with '/' are treated as directory paths
	/// and the function determines the filename for output image
	void SaveShadowMapTextureToFile( const std::string& file_or_directory_path );

	virtual std::string CreateTextureFilename() { return string(); }

	virtual void SaveShadowMapTextureToFileInternal( const std::string& filepath ) {}

	virtual void Accept( CShadowMapVisitor& v ) {}
/*
	void SetCameraDirection( const Vector3& vCamDir ) { m_SceneCamera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) ); }
	void SetCameraPosition( const Vector3& vCamPos ) { m_SceneCamera.SetPosition( vCamPos ); }
*/
};


class CFlatShadowMap : public CShadowMap
{
protected:

	/// Texture to which the shadow map is rendered
	/// Geometries that cast shadows to others (=shadow casters)
	/// are rendered to this texture.
	LPDIRECT3DTEXTURE9 m_pShadowMap;

	LPDIRECT3DSURFACE9 m_pShadowMapDepthBuffer; ///< Depth-stencil buffer for rendering to shadow map

//	LPDIRECT3DTEXTURE9 m_pShadowedView;
//	LPDIRECT3DSURFACE9 m_pDSShadowedView;

protected:

	void UpdateLightPositionAndDirection();

	void BeginSceneShadowReceivers();

	void SetWorldToLightSpaceTransformMatrix();

public:

	CFlatShadowMap()
		:
	m_pShadowMap(NULL),
	m_pShadowMapDepthBuffer(NULL)
	{}

	virtual ~CFlatShadowMap() {}

	virtual bool CreateShadowMapTextures();

	void ReleaseTextures();

	void BeginSceneShadowMap();

	void EndSceneShadowMap();

	void RenderShadowMapTexture( int sx, int sy, int ex, int ey );

	std::string CreateTextureFilename();

	LPDIRECT3DTEXTURE9 GetShadowMapTexture() { return m_pShadowMap; }

	virtual void SaveShadowMapTextureToFileInternal( const std::string& filepath);
};


class CDirectionalLightShadowMap : public CFlatShadowMap
{
//	boost::weak_ptr<CPhemisphericalDirectionalLight> m_DirectionalLight;

public:

	static float ms_fCameraShiftDistance;

	CDirectionalLightShadowMap();

	void UpdateDirectionalLight( CDirectionalLight& light );

	void Accept( CShadowMapVisitor& v ) { v.Visit( *this ); }
};


class CSpotLightShadowMap : public CFlatShadowMap
{
public:

	CSpotLightShadowMap()
	{}

	void Accept( CShadowMapVisitor& v ) { v.Visit( *this ); }
};


/**
 Renders the shadowmap to each face of the cube texture
 - Calls RenderShadowMapScene() of m_pSceneRenderer 6 times

*/
class CPointLightShadowMap : public CShadowMap
{
//	LPDIRECT3DCUBETEXTURE9 m_pCubeShadowMap;
//	LPDIRECT3DCUBETEXTURE9 m_pCubeShadowMapDepthBuffer;

	// or 

	CCubeMapManager *m_pCubeShadowMapManager;

	CCubeShadowMapSceneRenderer m_CubeShadowMapSceneRenderer;

//	boost::weak_ptr<CPhemisphericalPointLight> m_PointpLight;

public:

	CPointLightShadowMap();

	~CPointLightShadowMap();

	bool CreateShadowMapTextures();

	void SetSceneRenderer( CShadowMapSceneRenderer *pSceneRenderer )
	{
		CShadowMap::SetSceneRenderer( pSceneRenderer );

		m_CubeShadowMapSceneRenderer.SetSceneRenderer( pSceneRenderer );
	}

	void RenderSceneToShadowMap();

	void BeginSceneShadowMap();

	void EndSceneShadowMap();

	void UpdatePointLight( CPointLight& light );

	std::string CreateTextureFilename();

	LPDIRECT3DTEXTURE9 GetShadowMapTexture()
	{
		assert( !"How to set the cube shadowmap texture?" );
		return NULL;
	}

	void SaveShadowMapTextureToFileInternal( const std::string& filepath );

	void Accept( CShadowMapVisitor& v ) { v.Visit( *this ); }
};



#endif		/*  __ShadowMaps_H__  */
