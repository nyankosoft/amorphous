#ifndef  __PostProcessEffect_HPP__
#define  __PostProcessEffect_HPP__


#include <vector>
#include <d3dx9.h>
#include "../base.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/TextureFormat.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"


class CPostProcessFilterShader;
class CFilterShaderContainer;


template<typename T>
class TPlane2
{
public:

	T width, height;

public:

	TPlane2() : width(0), height(0) {}
	TPlane2( int w, int h ) : width(w), height(h) {}
};


typedef TPlane2<float> Plane2;
typedef TPlane2<int> SPlane2;


const D3DSURFACE_DESC *GetD3D9BackBufferSurfaceDesc();
SPlane2 GetBackBufferWidthAndHeight();
SPlane2 GetCropWidthAndHeight();


class CPostProcessEffect
{
public:
	enum TypeFlag
	{
		TF_HDR_LIGHTING       = (1 << 0),
		TF_BLUR               = (1 << 1),
		TF_MONOCHROME_COLOR   = (1 << 2),
	};
};


/// the vertex format used with the quad during post-process.
struct PPVERT;


//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


// PPCOUNT is the number of postprocess effects supported
//#define PPCOUNT (sizeof(g_aszFxFile) / sizeof(g_aszFxFile[0]))



// RT_COUNT is the number of simultaneous render targets used in the sample.
enum eRT_TYPE
{
	RT_COLOR = 0,
//	RT_NORMAL,
//	RT_POSITION,
	RT_COUNT
};


class CRenderTargetTextureHolder
{
	int m_NumLocks;

public:
	LPDIRECT3DSURFACE9 m_pTexSurf;
	CTextureHandle m_Texture;
	CTextureResourceDesc m_Desc;

	CRenderTargetTextureHolder()
		:
	m_pTexSurf(NULL),
//	m_pTexture(NULL),
	m_NumLocks(0)
	{}

	/// call this after rendering
	void ReleaseSurface()
	{
		SAFE_RELEASE( m_pTexSurf );
	}

	void IncrementLockCount()
	{
		m_NumLocks++;
	}

	void DecrementLockCount()
	{
		if( m_NumLocks <= 0 )
		{
			LOG_PRINT_WARNING( "Redundant calls. m_NumLocks is already 0." );
			return;
		}

		m_NumLocks--;
	}

	int GetLockCount() const { return m_NumLocks; }
};


class CRenderTargetTextureCache
{
public:

	std::vector< boost::shared_ptr<CRenderTargetTextureHolder> > m_vecpHolder;

	LPDIRECT3DSURFACE9 m_pOrigRenderTarget;

	boost::weak_ptr<CRenderTargetTextureCache> m_pSelf;

public:

	CRenderTargetTextureCache()
		:
	m_pOrigRenderTarget(NULL)
	{}

	/// Returns the number of textures that match the description of desc
	/// and currently exist in the cache.
	/// Checks only width, height and surface format of the desc
	int GetNumTextures( const CTextureResourceDesc& desc );

	Result::Name AddTexture( const CTextureResourceDesc& desc );

	boost::shared_ptr<CRenderTargetTextureHolder> GetTexture( const CTextureResourceDesc& desc );

	void SetSelfPtr( boost::weak_ptr<CRenderTargetTextureCache> pSelf ) { m_pSelf = pSelf; }

	boost::weak_ptr<CRenderTargetTextureCache> GetSelfPtr() { return m_pSelf; }
};


//--------------------------------------------------------------------------------------


/// A class that represents an instance of a post-process to be applied
/// to the scene.
/// Render target size is determined by the preceeding filter and CFilter::m_fScalingFactor
class CFilter
{
protected:

	boost::shared_ptr<CPostProcessFilterShader> m_pFilterShader;

//    int m_nFxIndex;

//	D3DXVECTOR4 m_avParam[CPostProcess::NUM_PARAMS];

	float m_fScaleX, m_fScaleY;

	boost::shared_ptr<CRenderTargetTextureHolder> m_pDest;

	boost::shared_ptr<CRenderTargetTextureHolder> m_pPrevScene;

	CTextureResourceDesc m_Desc;

	RECT m_SourceRect;

	/// default: false
	bool m_UseTextureSizeDivisibleBy8;

	/// Set this to 1 to set the render target which is wider by 1 texel on all sides than the source texture.
	/// default: 0
	int m_ExtraTexelBorderWidth;

	CShaderTechniqueHandle m_Technique;

	float m_fScalingFactor;

	std::vector< boost::shared_ptr<CFilter> > m_vecpNextFilter;

	boost::shared_ptr<CRenderTargetTextureCache> m_pCache;

	std::string m_DebugImageFilenameExtraString;

private:

	Result::Name CFilter::SetRenderTarget( CFilter& prev_filter );

protected:

	virtual bool GetRenderTarget( CFilter& prev_filter );

//	LPD3DXEFFECT GetD3DXEffect();

public:

	enum FilterType
	{
		TYPE_DOWN_SCALE_4x4,
		TYPE_DOWN_SCALE_2x2,
		TYPE_COMBINED_BLOOM,
		TYPE_HORIZONTAL_BLOOM,
		TYPE_VERTICAL_BLOOM,
		TYPE_STAR,
		TYPE_GAUSSIAN_BLUR,
		TYPE_LUMINANCE_CALC,
		TYPE_ADAPTATION_CALC,
		TYPE_HDR_BRIGHT_PASS,
		TYPE_HDR_LIGHTING_FINAL_PASS,
		TYPE_HDR_LIGHTING,
		TYPE_FULL_SCREEN_BLUR,
		TYPE_MONOCHROME_COLOR,
		TYPE_ORIGINAL_SCENE,
		NUM_FILTER_TYPES
	};

	inline CFilter( int fx_index = -1 ) :
//		m_nFxIndex(fx_index),
		m_fScaleX(1),
		m_fScaleY(1),
		m_UseTextureSizeDivisibleBy8(false),
		m_fScalingFactor(1.0f),
		m_ExtraTexelBorderWidth(0)
	{
//		ZeroMemory( m_avParam, sizeof( m_avParam ) );
	}

	virtual ~CFilter()
	{}

	virtual FilterType GetFilterType() const = 0;

	virtual Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void SetTextureCache( boost::shared_ptr<CRenderTargetTextureCache> pCache ) { m_pCache = pCache; }

	inline void SetScale( float fScaleX, float fScaleY ) { m_fScaleX = fScaleX; m_fScaleY = fScaleY; }

	void SetExtraTexelBorderWidth( int width ) { m_ExtraTexelBorderWidth = width; }

//	void SetScalingFactor( int scaling_factor ) { m_fScalingFactor = scaling_factor; }

	void SetSourceRect( RECT rect ) { m_SourceRect = rect; }

//	void Render( CFilter& prev_filter );

	virtual void RenderBase( CFilter& prev_filter );

	virtual void Render() {}

	virtual void StorePrevFilterResults( CFilter& prev_filter );

	virtual bool IsReadyToRender() { return true; }

	void SetRenderTargetSurfaceFormat( TextureFormat::Format fmt ) { m_Desc.Format = fmt; }

	void SetRenderTargetSize( int width, int height ) { m_Desc.Width = width; m_Desc.Height = height; }

	virtual void AddNextFilter( boost::shared_ptr<CFilter> pFilter ) { m_vecpNextFilter.push_back( pFilter ); }

	virtual void ClearNextFilters() { m_vecpNextFilter.resize( 0 ); }

	void SetFilterShader(  boost::shared_ptr<CPostProcessFilterShader> pFilterShader ) { m_pFilterShader = pFilterShader; }

	boost::shared_ptr<CPostProcessFilterShader> GetFilterShader() { return m_pFilterShader; }

	boost::shared_ptr<CRenderTargetTextureHolder> GetDestRenderTarget() { return m_pDest; }

	/// rationale to make this virtual: combined filters may not need to lock prev render target
	virtual void LockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->IncrementLockCount(); }

	virtual void UnlockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->DecrementLockCount(); }

	const std::string& GetDebugImageFilenameExtraString() { return m_DebugImageFilenameExtraString; }

	void SetDebugImageFilenameExtraString( const std::string& str ) { m_DebugImageFilenameExtraString = str; }

	static int ms_SaveFilterResultsAtThisFrame;
};


class COriginalSceneFilter : public CFilter
{
public:

	FilterType GetFilterType() const { return CFilter::TYPE_ORIGINAL_SCENE; }

	COriginalSceneFilter( boost::shared_ptr<CRenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}

	void SetSceneRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}
};


class CCombinedFilter : public CFilter
{
protected:

	std::vector< boost::shared_ptr<CFilter> > m_vecpChildFilter;

	boost::shared_ptr<CFilter> m_pLastFilter;

public:

	virtual ~CCombinedFilter() {}

	void AddNextFilter( boost::shared_ptr<CFilter> pFilter ) { if(m_pLastFilter) { m_pLastFilter->AddNextFilter( pFilter ); } }

	void ClearNextFilters() { if(m_pLastFilter) { m_pLastFilter->ClearNextFilters(); } }
};


class CGaussianBlurFilter : public CFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CGaussianBlurFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_GAUSSIAN_BLUR; }

	void Render();

	void SetWidth( float fWidth ) {}
	void SetHeight( float fHeight ) {}

	void Update();
};


class CBloomFilter : public CFilter
{
protected:

	enum Params
	{
		MAX_SAMPLES = 16,
	};

	float m_afSampleOffsets[MAX_SAMPLES];
	D3DXVECTOR2 m_avSampleOffsets[MAX_SAMPLES];
	D3DXVECTOR4 m_avSampleWeights[MAX_SAMPLES];

	bool m_DoScissorTesting;

	virtual void GetSampleOffsets() = 0;

	float m_fDeviation;

	float m_fBloomFactor;

	bool m_UseForBlurFilter;

public:

	CBloomFilter();

	virtual ~CBloomFilter() {}

	void Render();

	void SetBloomFactor( float factor ) { m_fBloomFactor = factor; }

	void SetUseForBlurFilter( bool use_for_blur_filter ) { m_UseForBlurFilter = use_for_blur_filter; }

	void SetDeviation( float deviation ) { m_fDeviation = deviation; }
};


class CHorizontalBloomFilter : public CBloomFilter
{
public:

	CHorizontalBloomFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_HORIZONTAL_BLOOM; }

	void GetSampleOffsets();
};


class CVerticalBloomFilter : public CBloomFilter
{
public:

	CVerticalBloomFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_VERTICAL_BLOOM; }

	void GetSampleOffsets();
};


class CCombinedBloomFilter : public CCombinedFilter
{
	boost::shared_ptr<CGaussianBlurFilter> m_pGaussianBlurFilter;
	boost::shared_ptr<CHorizontalBloomFilter> m_pHBloomFilter;
	boost::shared_ptr<CVerticalBloomFilter>   m_pVBloomFilter;

	SPlane2 m_BasePlane;

public:

	CCombinedBloomFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_COMBINED_BLOOM; }

//	Result::Name Init( CRenderTargetTextureCache& cache );
	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CFilter& prev_filter );

	void SetBasePlane( SPlane2 base_plane ) { m_BasePlane = base_plane; }

	void UseAsGaussianBlurFilter( bool use_as_gauss_blur );

	void SetBlurStrength( float strength );

//	void AddNextFilter( boost::shared_ptr<CFilter> pFilter );
};


class CStarFilter : public CFilter
{
public:

	CStarFilter() {}

	FilterType GetFilterType() const { return CFilter::TYPE_STAR; }

	void Render() {}
};


class CDownScale4x4Filter : public CFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CDownScale4x4Filter();

	FilterType GetFilterType() const { return CFilter::TYPE_DOWN_SCALE_4x4; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


class CDownScale2x2Filter : public CFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CDownScale2x2Filter();

	FilterType GetFilterType() const { return CFilter::TYPE_DOWN_SCALE_2x2; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


/// Uses square render target
class CLuminanceCalcFilter : public CFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

	int m_NumSamples;

	int m_RenderTargetSize;

private:

	bool GetRenderTarget( CFilter& prev_filter, boost::shared_ptr<CRenderTargetTextureHolder>& pDest );

	void GetSampleOffsets_DownScale3x3( int width, int height, D3DXVECTOR2 avSampleOffsets[] );

	void GetSampleOffsets_DownScale4x4( int width, int height, D3DXVECTOR2 avSampleOffsets[] );

public:

	CLuminanceCalcFilter( const std::string& technique_name, int num_samples, int render_target_size );

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return CFilter::TYPE_LUMINANCE_CALC; }

	void Render();
};


class CAdaptationCalcFilter : public CFilter
{
	boost::shared_ptr<CRenderTargetTextureHolder> m_pTexAdaptedLuminanceLast;
	boost::shared_ptr<CRenderTargetTextureHolder> m_pTexAdaptedLuminanceCur;

	float m_fElapsedTime;

	float m_fLuminanceAdaptationRate;

public:

	CAdaptationCalcFilter();

	~CAdaptationCalcFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_ADAPTATION_CALC; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void Render();

	void SetElapsedTime( float fElapsedTime ) { m_fElapsedTime = fElapsedTime; }

	void SetLuminanceAdaptationRate( float fRate ) { m_fLuminanceAdaptationRate = fRate; }

	boost::shared_ptr<CRenderTargetTextureHolder> GetAdaptedLuminanceTexture() { return m_pTexAdaptedLuminanceCur; }
};


class CHDRBrightPassFilter : public CFilter
{
	boost::shared_ptr<CRenderTargetTextureHolder> m_pAdaptedLuminanceTexture;

public:

	CHDRBrightPassFilter();

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return CFilter::TYPE_HDR_BRIGHT_PASS; }

	void Render();

	void SetAdaptedLuminanceTexture( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { m_pAdaptedLuminanceTexture = pHolder; }
};


class CHDRLightingFinalPassFilter : public CFilter
{
public:

	boost::shared_ptr<CRenderTargetTextureHolder> m_pBloom;
	boost::shared_ptr<CRenderTargetTextureHolder> m_pStar;
	boost::shared_ptr<CRenderTargetTextureHolder> m_pAdaptedLuminance;
	boost::shared_ptr<CRenderTargetTextureHolder> m_pPrevResult;

	float m_fKeyValue;

	bool m_ToneMappingEnabled;

	bool m_StarEffectEnabled;

	CTextureHandle m_BlancTextureForDisabledStarEffect;

public:

	CHDRLightingFinalPassFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_HDR_LIGHTING_FINAL_PASS; }

	bool IsReadyToRender();

	void StorePrevFilterResults( CFilter& prev_filter );

	void Render();

	void SetToneMappingKeyValue( float fKeyValue ) { m_fKeyValue = fKeyValue; }

	void EnableToneMapping( bool enable ) { m_ToneMappingEnabled = enable; }

	void EnableStarEffect( bool enable ) { m_StarEffectEnabled = enable; }
};


class CHDRLightingFilter : public CCombinedFilter
{
	enum Params
	{
		NUM_TONEMAP_TEXTURES = 4,
	};

//	boost::shared_ptr<CHDROverlayEffectFilter> m_pOverlayEffectFilter;

	boost::shared_ptr<CLuminanceCalcFilter> m_apLumCalcFilter[NUM_TONEMAP_TEXTURES];

	boost::shared_ptr<CAdaptationCalcFilter> m_pAdaptationCalcFilter;

	boost::shared_ptr<CDownScale4x4Filter> m_pDownScale4x4Filter;

	boost::shared_ptr<CHDRBrightPassFilter> m_pBrightPassFilter;

	boost::shared_ptr<CGaussianBlurFilter> m_pGaussianBlurFilter;

	boost::shared_ptr<CDownScale2x2Filter> m_pDownScale2x2Filter;

	boost::shared_ptr<CCombinedBloomFilter> m_pBloomFilter;

	boost::shared_ptr<CHDRLightingFinalPassFilter> m_pFinalPassFilter;

	boost::shared_ptr<CStarFilter> m_pStarFilter;

	bool m_EnableStarFilter;

public:
	
	CHDRLightingFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_HDR_LIGHTING; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CFilter& prev_filter );

	void EnableToneMapping( bool enable ) { if(m_pFinalPassFilter) m_pFinalPassFilter->EnableToneMapping( enable ); }

	void SetToneMappingKeyValue( float fKeyValue ) { if(m_pFinalPassFilter) m_pFinalPassFilter->SetToneMappingKeyValue( fKeyValue ); }

	void SetLuminanceAdaptationRate( float fRate ) { if(m_pAdaptationCalcFilter) m_pAdaptationCalcFilter->SetLuminanceAdaptationRate(fRate); }

	void LockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->IncrementLockCount(); }

	void UnlockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->DecrementLockCount(); }
};


class CFullScreenBlurFilter : public CCombinedFilter
{
	float m_fBlurStrength;

	boost::shared_ptr<CDownScale4x4Filter> m_pDownScale4x4Filter;

//	boost::shared_ptr<CHorizontalBloomFilter> m_apHorizontalBloomFilter[2];
//	boost::shared_ptr<CVerticalBloomFilter> m_apVerticalBloomFilter[2];

	boost::shared_ptr<CCombinedBloomFilter> m_pBloomFilter;

public:

	CFullScreenBlurFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_FULL_SCREEN_BLUR; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CFilter& prev_filter );

	void Render() {}

	void SetBlurStrength( float strength ) { m_fBlurStrength = strength; }
};


class CMonochromeColorFilter : public CFilter
{
public:

	CMonochromeColorFilter();

	FilterType GetFilterType() const { return CFilter::TYPE_MONOCHROME_COLOR; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void SetColorOffset( float* pafRGB ) {}

	void Render();
};


/*
class CGlareFilter : public CCombinedFilter
{
public:

	void SetThreashold( float fThreashold ) {}

	void Render();
};
*/

#endif  /* __PostProcessEffect_HPP__ */
