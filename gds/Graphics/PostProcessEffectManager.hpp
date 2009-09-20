#ifndef  __PostProcessEffectManager_HPP__
#define  __PostProcessEffectManager_HPP__


#include "Graphics/ShaderHandle.hpp"
#include "PostProcessEffect.hpp"


//class CHDRLightingFilter;


//--------------------------------------------------------------------------------------
// struct CPostProcess
// A struct that encapsulates aspects of a render target postprocess
// technique.
//--------------------------------------------------------------------------------------
class CPostProcessFilterShader
{
	enum eParam
	{
		// NUM_PARAMS is the maximum number of changeable parameters supported
		// in an effect.
		NUM_PARAMS = 2,
	};

	/// Effect object for this technique
	CShaderHandle m_Shader;
//	LPD3DXEFFECT m_pEffect;

	/// filepath of the HLSL effect file for the effect interface above
	std::string m_ShaderFilename;

	/// PostProcess technique handle
	D3DXHANDLE   m_hTPostProcess;

	/// Render target channel this PP outputs
//	int          m_nRenderTarget;

	D3DXHANDLE   m_hTexSource[4];        ///< Handle to the post-process source textures

	D3DXHANDLE   m_hTexScene[4];         ///< Handle to the saved scene texture

	/// Indicates whether the post-process technique
	///   outputs data for this render target.
	bool         m_bWrite[4];            
	                                     

	WCHAR        m_awszParamName[NUM_PARAMS][MAX_PATH]; ///< Names of changeable parameters

	WCHAR        m_awszParamDesc[NUM_PARAMS][MAX_PATH]; ///< Description of parameters

	D3DXHANDLE   m_ahParam[NUM_PARAMS];  ///< Handles to the changeable parameters

	/// Size of the parameter. Indicates how many
	/// components of float4 are used.
	int          m_anParamSize[NUM_PARAMS];

	D3DXVECTOR4  m_avParamDef[NUM_PARAMS]; ///< Parameter default

public:

	CPostProcessFilterShader();

	inline ~CPostProcessFilterShader() {}//{ Cleanup(); }

	Result::Name Init( const std::string& filename );
	
	const std::string& GetShaderFilename() { return m_ShaderFilename; }

	CShaderHandle GetShader() { return m_Shader; }

//	inline void Cleanup() { SAFE_RELEASE( m_pEffect ); }
//	LPD3DXEFFECT GetEffect() { return m_pEffect; }
//	HRESULT OnLostDevice();
//	HRESULT OnResetDevice( DWORD dwWidth, DWORD dwHeight );
//	HRESULT SetScale( float scale_x, float scale_y );
};


class CFilterShaderContainer
{
	std::vector< boost::shared_ptr<CPostProcessFilterShader> > m_vecpShader;
public:

	Result::Name AddShader( const std::string& name );

	boost::shared_ptr<CPostProcessFilterShader> GetFilterShader( const std::string& name );

	boost::shared_ptr<CPostProcessFilterShader> GetShader( const std::string& technique_name );

//	boost::shared_ptr<CPostProcessFilterShader> GetShaderFromFilename( const std::string& filename );
};


struct CRenderTargetChain
{
	// erased
};



//======================================================================================
// CPostProcessEffectManager
//  A class that manages post process effects
//  Only the color channel is available
//======================================================================================

class CPostProcessEffectManager : public CGraphicsComponent
{
	boost::shared_ptr<CHDRLightingFilter> m_pHDRLightingFilter;

	boost::shared_ptr<CRenderTargetTextureCache> m_pTextureCache;

	CFilterShaderContainer m_FilterShaderContainer;

	/// render target to render the scene
	CTextureHandle m_SceneRenderTarget;

	boost::shared_ptr<COriginalSceneFilter> m_pOriginalSceneFilter;

	boost::shared_ptr<CFilter> m_pFilter;

	boost::shared_ptr<CRenderTargetTextureHolder> m_pOrigSceneHolder;


	U32 m_EnabledEffectFlags;

	bool m_IsRedering;

	bool m_bUseMultiSampleFloat16;

	PDIRECT3DSURFACE9 m_pFloatMSRT;
	PDIRECT3DSURFACE9 m_pFloatMSDS;

	PDIRECT3DSURFACE9 m_pSurfLDR; /// Low dynamic range surface for final output (original render target)
	PDIRECT3DSURFACE9 m_pSurfDS;  /// Low dynamic range depth stencil surface

public:

	/// set a texture render target on which the scene is to be rendered.
	/// Thus, this method must be called before the entire scene is rendered.
	/// Before calling this method, user is responsible for saving the original render target
	/// by calling GetRenderTarget()
/*	inline HRESULT SetTextureRenderTarget( IDirect3DDevice9 *pd3dDevice, UINT pass )
	{
		HRESULT hr;
		// if pass == 0 && m_nRtUsed == 0, this is equal to
		// pd3dDevice->SetRenderTarget( 0, m_pSceneSave[0] )
		for( int rt = 0; rt < m_nRtUsed; ++rt )
		{
			hr = pd3dDevice->SetRenderTarget( rt, m_aRtTable[pass].pRT[rt] );
			if( FAILED(hr) )
				return hr;
		}

		return S_OK;
	}*/
/*
	inline HRESULT SetTextureRenderTarget( UINT pass )
	{
		// removed
	}

	inline HRESULT ResetTextureRenderTarget()
	{
		// removed
	}


	/// after rendering the scene, call this method to apply post process effects
	HRESULT PerformPostProcess();

	/// output the final result image onto the backbuffer.
	/// after calling PerformPostProcess(), the user is suppoed to restore the original
	/// render target and call this method
	/// 18:37 2008-02-11 changed: restores the original render target
	/// 22:25 2008-02-11 commented out. replaced by DrawSceneWithPostProcessEffects() below
	///                  also commented out the BeginScene() & EndScene() pair
//	HRESULT RenderPostProcess( IDirect3DDevice9 *pd3dDevice );
	HRESULT DrawSceneWithPostProcessEffects();
//	HRESULT OnCreateDevice( IDirect3DDevice9* pd3dDevice,
//							const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
//							const std::string& shader_filename,
//							void* pUserContext );

	/// Calls this for initialization
	/// - Returns true on success
    Result::Name OnCreateDevice( const std::string& shader_filename );
	/// Used to release the resources to change the resolution of the back buffer
    void OnDestroyDevice();
	void OnLostDevice( void* pUserContext );

//	HRESULT Init( const std::string& filename );
    HRESULT OnResetDevice();
*/
    CPostProcessEffectManager();

	~CPostProcessEffectManager();

	/// Returns the index of the added shader.
	/// Returns -1 on failure
	int AddPostProcessShader( const std::string& shader_filename );

//	std::vector<CPProcInstance>& GetPostProcessInstance() { return m_vecPPInstance; }
//	CPostProcess& GetPostProcess( int index ) { return m_aPostProcess[index]; }

	Result::Name BeginRender();

	Result::Name EndRender();

	Result::Name RenderPostProcessEffects();

	void Release();

	void SetFirstFilterParams();

//	Result::Name Init( const std::string& filename );

	Result::Name Init( const std::string& base_shader_directory_path = "" );

	Result::Name InitHDRLightingFilter();
	
	Result::Name EnableHDRLighting( bool enable );

	Result::Name EnableBlur( bool enable );

	bool IsEnabled( U32 flag ) const { return (m_EnabledEffectFlags & flag) ? true : false; }

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /* __PostProcessEffectManager_HPP__ */
