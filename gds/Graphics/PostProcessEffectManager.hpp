#ifndef  __PostProcessEffectManager_HPP__
#define  __PostProcessEffectManager_HPP__


#include "ShaderHandle.hpp"
#include "PostProcessEffect.hpp"


//class CHDRLightingFilter;


class CHDRLightingParams
{
public:

	enum Flags
	{
		TONE_MAPPING              = (1 << 0),
		KEY_VALUE                 = (1 << 1),
		BULE_SHIFT                = (1 << 2),
		LUMINANCE_ADAPTATION_RATE = (1 << 3),
//		STAR_EFFECT_TYPE          = (1 << 4),
	};

	float key_value;
	bool blue_shift;
	bool tone_mapping;

	/// How quickly camera adjust the brightness
	/// - range: (0.0,1.0]
	/// - brightness is adjusted without delay when this is set to 1.0
	float luminance_adaptation_rate;

//	CStarEffectType::Name start_effect;

	CHDRLightingParams()
		:
	key_value(0.5f),
	blue_shift(false),
	tone_mapping(false),
	luminance_adaptation_rate(0.02f)
//	start_effect()
	{}
};


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

	boost::shared_ptr<CFullScreenBlurFilter> m_pFullScreenBlurFilter;

	boost::shared_ptr<CMonochromeColorFilter> m_pMonochromeColorFilter;


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

	/// Displays adapted luminance on a small rectangle at the top-left corner of the display.
	/// - Used for debugging
	void DisplayAdaptedLuminance();

public:

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

	Result::Name InitBlurFilter();

	Result::Name InitMonochromeColorFilter();
	
	Result::Name EnableHDRLighting( bool enable );

	Result::Name EnableBlur( bool enable );

	Result::Name EnableEffect( U32 effect_flags );

	Result::Name DisableEffect( U32 effect_flags );

	bool IsEnabled( U32 flag ) const { return (m_EnabledEffectFlags & flag) ? true : false; }

	void SetHDRLightingParams( U32 hdr_lighting_param_flags, const CHDRLightingParams& params );

	void SetBlurStrength( float fBlurStrength );

	void SetMonochromeColorOffset( const SFloatRGBColor& color );

	void ReleaseGraphicsResources();

	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif  /* __PostProcessEffectManager_HPP__ */
