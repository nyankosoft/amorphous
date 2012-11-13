#ifndef  __PostProcessEffect_HPP__
#define  __PostProcessEffect_HPP__


#include "PostProcessEffectFilter.hpp"
#include "../3DMath/Vector2.hpp"
#include "../3DMath/Vector4.hpp"


class COriginalSceneFilter : public CPostProcessEffectFilter
{
public:

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_ORIGINAL_SCENE; }

	COriginalSceneFilter( boost::shared_ptr<CRenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}

	void SetSceneRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}
};


class CCombinedFilter : public CPostProcessEffectFilter
{
protected:

//	std::vector< boost::shared_ptr<CPostProcessEffectFilter> > m_vecpChildFilter;

	boost::shared_ptr<CPostProcessEffectFilter> m_pLastFilter;

public:

	virtual ~CCombinedFilter() {}

	void AddNextFilter( boost::shared_ptr<CPostProcessEffectFilter> pFilter ) { if(m_pLastFilter) { m_pLastFilter->AddNextFilter( pFilter ); } }

	void ClearNextFilters() { if(m_pLastFilter) { m_pLastFilter->ClearNextFilters(); } }
};


class CGaussianBlurFilter : public CPostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CGaussianBlurFilter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_GAUSSIAN_BLUR; }

	void Render();

	void SetWidth( float fWidth ) {}
	void SetHeight( float fHeight ) {}

//	void Update();
};


class CBloomFilter : public CPostProcessEffectFilter
{
protected:

	enum Params
	{
		MAX_SAMPLES = 16,
	};

	float m_afSampleOffsets[MAX_SAMPLES];
	Vector2 m_avSampleOffsets[MAX_SAMPLES];
	Vector4 m_avSampleWeights[MAX_SAMPLES];

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

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_HORIZONTAL_BLOOM; }

	void GetSampleOffsets();
};


class CVerticalBloomFilter : public CBloomFilter
{
public:

	CVerticalBloomFilter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_VERTICAL_BLOOM; }

	void GetSampleOffsets();
};


class CCombinedBloomFilter : public CCombinedFilter
{
	boost::shared_ptr<CGaussianBlurFilter> m_pGaussianBlurFilter;
	boost::shared_ptr<CHorizontalBloomFilter> m_pHBloomFilter;
	boost::shared_ptr<CVerticalBloomFilter>   m_pVBloomFilter;

	SRectangular m_BasePlane;

public:

	CCombinedBloomFilter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_COMBINED_BLOOM; }

//	Result::Name Init( CRenderTargetTextureCache& cache );
	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CPostProcessEffectFilter& prev_filter );

	void SetBasePlane( SRectangular base_plane ) { m_BasePlane = base_plane; }

	void UseAsGaussianBlurFilter( bool use_as_gauss_blur );

	void SetBlurStrength( float strength );

//	void AddNextFilter( boost::shared_ptr<CPostProcessEffectFilter> pFilter );
};


class CStarFilter : public CPostProcessEffectFilter
{
public:

	CStarFilter() {}

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_STAR; }

	void Render() {}
};


class CDownScale4x4Filter : public CPostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CDownScale4x4Filter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_DOWN_SCALE_4x4; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


class CDownScale2x2Filter : public CPostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	CDownScale2x2Filter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_DOWN_SCALE_2x2; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


/// Uses square render target
class CLuminanceCalcFilter : public CPostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

	int m_NumSamples;

	int m_RenderTargetSize;

private:

	void GetSampleOffsets_DownScale3x3( int width, int height, Vector2 avSampleOffsets[] );

	void GetSampleOffsets_DownScale4x4( int width, int height, Vector2 avSampleOffsets[] );

public:

	CLuminanceCalcFilter( const std::string& technique_name, int num_samples, int render_target_size );

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_LUMINANCE_CALC; }

	void Render();
};


class CAdaptationCalcFilter : public CPostProcessEffectFilter
{
	boost::shared_ptr<CRenderTargetTextureHolder> m_pTexAdaptedLuminanceLast;
	boost::shared_ptr<CRenderTargetTextureHolder> m_pTexAdaptedLuminanceCur;

	float m_fElapsedTime;

	float m_fLuminanceAdaptationRate;

public:

	CAdaptationCalcFilter();

	~CAdaptationCalcFilter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_ADAPTATION_CALC; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void Render();

	void SetElapsedTime( float fElapsedTime ) { m_fElapsedTime = fElapsedTime; }

	void SetLuminanceAdaptationRate( float fRate ) { m_fLuminanceAdaptationRate = fRate; }

	boost::shared_ptr<CRenderTargetTextureHolder> GetAdaptedLuminanceTexture() { return m_pTexAdaptedLuminanceCur; }
};


class CHDRBrightPassFilter : public CPostProcessEffectFilter
{
	boost::shared_ptr<CRenderTargetTextureHolder> m_pAdaptedLuminanceTexture;

public:

	CHDRBrightPassFilter();

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_HDR_BRIGHT_PASS; }

	void Render();

	void SetAdaptedLuminanceTexture( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { m_pAdaptedLuminanceTexture = pHolder; }
};


class CHDRLightingFinalPassFilter : public CPostProcessEffectFilter
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

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_HDR_LIGHTING_FINAL_PASS; }

	bool IsReadyToRender();

	void StorePrevFilterResults( CPostProcessEffectFilter& prev_filter );

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

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_HDR_LIGHTING; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CPostProcessEffectFilter& prev_filter );

	void EnableToneMapping( bool enable ) { if(m_pFinalPassFilter) m_pFinalPassFilter->EnableToneMapping( enable ); }

	void SetToneMappingKeyValue( float fKeyValue ) { if(m_pFinalPassFilter) m_pFinalPassFilter->SetToneMappingKeyValue( fKeyValue ); }

	void SetLuminanceAdaptationRate( float fRate ) { if(m_pAdaptationCalcFilter) m_pAdaptationCalcFilter->SetLuminanceAdaptationRate(fRate); }

	void LockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->IncrementLockCount(); }

	void UnlockPrevRenderTarget( boost::shared_ptr<CRenderTargetTextureHolder> pHolder ) { pHolder->DecrementLockCount(); }

	/// used to display adapted luminance for debugging
	boost::shared_ptr<CAdaptationCalcFilter> GetAdaptationCalcFilter() { return m_pAdaptationCalcFilter; }
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

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_FULL_SCREEN_BLUR; }

	Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void RenderBase( CPostProcessEffectFilter& prev_filter );

	void Render() {}

	void SetBlurStrength( float strength ) { m_fBlurStrength = strength; }
};


class CMonochromeColorFilter : public CPostProcessEffectFilter
{
public:

	CMonochromeColorFilter();

	FilterType GetFilterType() const { return CPostProcessEffectFilter::TYPE_MONOCHROME_COLOR; }

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
