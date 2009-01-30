#ifndef  __CubeMapManager_H__
#define  __CubeMapManager_H__


#include <d3d9.h>
#include <d3dx9.h>
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/Camera.hpp"
#include "TextureFormat.hpp"


class STATUS
{
public:
	enum type
	{
		OK = 0,
//		ERROR = 1, // error - 'ERROR' is reserved
		Success = 0,
		Error = 1,
	};
};


class CCubeMapSceneRenderer
{
public:

	CCubeMapSceneRenderer() {}

	virtual ~CCubeMapSceneRenderer() {}

	virtual Vector3 GetCameraPosition() = 0;

	/// Called by CCubeMapManager in CCubeMapManager::RenderToCubeMap()
	/// to render the scene to the cube map surface.
	/// - called 6 times in total to render the scene to all the cube map surfaces
	/// - IDirect3DDevice9::BeginScene() & EndScene() pair is not called
	///   in CCubeMapManager::RenderToCubeMap() before and after the 
	///   RenderSceneToCubeMap() calls.
	///   User must call them in this functions?
	///   - changed: BeginScene() and EndScene() are called in CCubeMapManager::RenderToCubeMap()
	virtual void RenderSceneToCubeMap( CCamera& camera ) = 0;
};


class CCubeMapManager : public CGraphicsComponent
{
	int m_CubeTextureSize;

	TextureFormat::Format m_TextureFormat;

	LPDIRECT3DCUBETEXTURE9       m_apCubeMapFp[2];  ///< Floating point format cube map
	LPDIRECT3DCUBETEXTURE9       m_pCubeMap32;      ///< 32-bit cube map (for fallback)
	LPDIRECT3DSURFACE9           m_pDepthCube;      ///< Depth-stencil buffer for rendering to cube texture

	LPDIRECT3DCUBETEXTURE9       m_pCurrentCubeMap; ///< Cube map(s) to use based on current cubemap format

	/// temporarily hold the original render target
	LPDIRECT3DSURFACE9 m_pOrigRenderTarget;
	LPDIRECT3DSURFACE9 m_pOrigDepthStencilSurface;

	int m_NumCubes;

	CCamera m_Camera;

	/// borrowed reference
	CCubeMapSceneRenderer *m_pCubeMapSceneRenderer;

private:

	void CreateTextures( int tex_edge_length, TextureFormat::Format tex_format );

	void UpdateCameraOrientation( int face );

public:

	CCubeMapManager();
	~CCubeMapManager();

	void Init( int tex_edge_length = 256,
		       TextureFormat::Format tex_format = TextureFormat::A8R8G8B8 );

	bool IsReady();

	void RenderToCubeMap();
//	void BeginRenderToCubeMap( int face );
//	void EndRenderToCubeMap();

	inline int GetCubeTextureSize() const { return m_CubeTextureSize; }

	inline void SetCubeTextureSize( const int edge_length ) { m_CubeTextureSize = edge_length; }

	LPDIRECT3DCUBETEXTURE9 GetCubeTexture() { return m_pCurrentCubeMap; }

	void SaveCubeTextureToFile( const std::string& output_filename );

	inline void SetCubeMapSceneRenderer( CCubeMapSceneRenderer *pSceneRenderer ) { m_pCubeMapSceneRenderer = pSceneRenderer; }

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
	void ReleaseGraphicsResources();
};



#endif		/*  __CubeMapManager_H__  */
