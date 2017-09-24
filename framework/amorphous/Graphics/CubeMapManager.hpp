#ifndef  __CubeMapManager_H__
#define  __CubeMapManager_H__


#include <memory>
#include "fwd.hpp"
#include "Camera.hpp"
#include "SurfaceFormat.hpp"


namespace amorphous
{


class CCubeMapSceneRenderer
{
public:

	CCubeMapSceneRenderer() {}

	virtual ~CCubeMapSceneRenderer() {}

	virtual Vector3 GetCameraPosition() = 0;

	/// Called by CubeMapManager in CubeMapManager::RenderToCubeMap()
	/// to render the scene to the cube map surface.
	/// - called 6 times in total to render the scene to all the cube map surfaces
	/// - IDirect3DDevice9::BeginScene() & EndScene() pair is not called
	///   in CubeMapManager::RenderToCubeMap() before and after the 
	///   RenderSceneToCubeMap() calls.
	///   User must call them in this functions?
	///   - changed: BeginScene() and EndScene() are called in CubeMapManager::RenderToCubeMap()
	virtual void RenderSceneToCubeMap( Camera& camera ) = 0;
};


class CubeMapManager// : public GraphicsComponent
{
	Camera m_Camera;

	/// borrowed reference
	CCubeMapSceneRenderer *m_pCubeMapSceneRenderer;

	std::shared_ptr<CubeTextureRenderTarget> m_pCubeTextureRenderTarget;

private:

	void CreateTextures( int tex_edge_length, TextureFormat::Format tex_format );

	void UpdateCameraOrientation( int face );

public:

	CubeMapManager();
	~CubeMapManager();

	void Init( int tex_edge_length = 256,
		       TextureFormat::Format tex_format = TextureFormat::A8R8G8B8 );

	bool IsReady();

	void RenderToCubeMap();
//	void BeginRenderToCubeMap( int face );
//	void EndRenderToCubeMap();

	void SaveCubeTextureToFile( const std::string& output_filename );

	inline void SetCubeMapSceneRenderer( CCubeMapSceneRenderer *pSceneRenderer ) { m_pCubeMapSceneRenderer = pSceneRenderer; }
};


} // namespace amorphous



#endif		/*  __CubeMapManager_H__  */
