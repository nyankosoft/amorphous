#ifndef  __PostProcessEffect_HPP__
#define  __PostProcessEffect_HPP__


#include "PostProcessEffectFilter.hpp"
#include "../3DMath/Vector2.hpp"
#include "../3DMath/Vector4.hpp"


namespace amorphous
{


class OriginalSceneFilter : public PostProcessEffectFilter
{
public:

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_ORIGINAL_SCENE; }

	OriginalSceneFilter( boost::shared_ptr<RenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}

	void SetSceneRenderTarget( boost::shared_ptr<RenderTargetTextureHolder> pHolder )
	{
		m_pDest = pHolder;
	}
};


class CombinedFilter : public PostProcessEffectFilter
{
protected:

//	std::vector< boost::shared_ptr<PostProcessEffectFilter> > m_vecpChildFilter;

	boost::shared_ptr<PostProcessEffectFilter> m_pLastFilter;

public:

	virtual ~CombinedFilter() {}

	void AddNextFilter( boost::shared_ptr<PostProcessEffectFilter> pFilter ) { if(m_pLastFilter) { m_pLastFilter->AddNextFilter( pFilter ); } }

	void ClearNextFilters() { if(m_pLastFilter) { m_pLastFilter->ClearNextFilters(); } }
};


class GaussianBlurFilter : public PostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	GaussianBlurFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_GAUSSIAN_BLUR; }

	void Render();

	void SetWidth( float fWidth ) {}
	void SetHeight( float fHeight ) {}

//	void Update();
};


class BloomFilter : public PostProcessEffectFilter
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

	BloomFilter();

	virtual ~BloomFilter() {}

	void Render();

	void SetBloomFactor( float factor ) { m_fBloomFactor = factor; }

	void SetUseForBlurFilter( bool use_for_blur_filter ) { m_UseForBlurFilter = use_for_blur_filter; }

	void SetDeviation( float deviation ) { m_fDeviation = deviation; }
};


class HorizontalBloomFilter : public BloomFilter
{
public:

	HorizontalBloomFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_HORIZONTAL_BLOOM; }

	void GetSampleOffsets();
};


class VerticalBloomFilter : public BloomFilter
{
public:

	VerticalBloomFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_VERTICAL_BLOOM; }

	void GetSampleOffsets();
};


class CombinedBloomFilter : public CombinedFilter
{
	boost::shared_ptr<GaussianBlurFilter> m_pGaussianBlurFilter;
	boost::shared_ptr<HorizontalBloomFilter> m_pHBloomFilter;
	boost::shared_ptr<VerticalBloomFilter>   m_pVBloomFilter;

	SRectangular m_BasePlane;

public:

	CombinedBloomFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_COMBINED_BLOOM; }

//	Result::Name Init( RenderTargetTextureCache& cache );
	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void RenderBase( PostProcessEffectFilter& prev_filter );

	void SetBasePlane( SRectangular base_plane ) { m_BasePlane = base_plane; }

	void UseAsGaussianBlurFilter( bool use_as_gauss_blur );

	void SetBlurStrength( float strength );

//	void AddNextFilter( boost::shared_ptr<PostProcessEffectFilter> pFilter );
};


class StarFilter : public PostProcessEffectFilter
{
public:

	StarFilter() {}

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_STAR; }

	void Render() {}
};


class DownScale4x4Filter : public PostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	DownScale4x4Filter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_DOWN_SCALE_4x4; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


class DownScale2x2Filter : public PostProcessEffectFilter
{
	enum Params
	{
		MAX_SAMPLES = 16,
	};

public:

	DownScale2x2Filter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_DOWN_SCALE_2x2; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

//	void Update();

	void Render();
};


/// Uses square render target
class LuminanceCalcFilter : public PostProcessEffectFilter
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

	LuminanceCalcFilter( const std::string& technique_name, int num_samples, int render_target_size );

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_LUMINANCE_CALC; }

	void Render();
};


class AdaptationCalcFilter : public PostProcessEffectFilter
{
	boost::shared_ptr<RenderTargetTextureHolder> m_pTexAdaptedLuminanceLast;
	boost::shared_ptr<RenderTargetTextureHolder> m_pTexAdaptedLuminanceCur;

	float m_fElapsedTime;

	float m_fLuminanceAdaptationRate;

private:

	Result::Name SetRenderTarget( PostProcessEffectFilter& prev_filter );

public:

	AdaptationCalcFilter();

	~AdaptationCalcFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_ADAPTATION_CALC; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void Render();

	void SetElapsedTime( float fElapsedTime ) { m_fElapsedTime = fElapsedTime; }

	void SetLuminanceAdaptationRate( float fRate ) { m_fLuminanceAdaptationRate = fRate; }

	boost::shared_ptr<RenderTargetTextureHolder> GetAdaptedLuminanceTexture() { return m_pTexAdaptedLuminanceCur; }
};


class HDRBrightPassFilter : public PostProcessEffectFilter
{
	boost::shared_ptr<RenderTargetTextureHolder> m_pAdaptedLuminanceTexture;

public:

	HDRBrightPassFilter();

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_HDR_BRIGHT_PASS; }

	void Render();

	void SetAdaptedLuminanceTexture( boost::shared_ptr<RenderTargetTextureHolder> pHolder ) { m_pAdaptedLuminanceTexture = pHolder; }
};


class HDRLightingFinalPassFilter : public PostProcessEffectFilter
{
public:

	boost::shared_ptr<RenderTargetTextureHolder> m_pBloom;
	boost::shared_ptr<RenderTargetTextureHolder> m_pStar;
	boost::shared_ptr<RenderTargetTextureHolder> m_pAdaptedLuminance;
	boost::shared_ptr<RenderTargetTextureHolder> m_pPrevResult;

	float m_fKeyValue;

	bool m_ToneMappingEnabled;

	bool m_StarEffectEnabled;

	TextureHandle m_BlancTextureForDisabledStarEffect;

public:

	HDRLightingFinalPassFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_HDR_LIGHTING_FINAL_PASS; }

	bool IsReadyToRender();

	void StorePrevFilterResults( PostProcessEffectFilter& prev_filter );

	void Render();

	void SetToneMappingKeyValue( float fKeyValue ) { m_fKeyValue = fKeyValue; }

	void EnableToneMapping( bool enable ) { m_ToneMappingEnabled = enable; }

	void EnableStarEffect( bool enable ) { m_StarEffectEnabled = enable; }
};


class HDRLightingFilter : public CombinedFilter
{
	enum Params
	{
		NUM_TONEMAP_TEXTURES = 4,
	};

//	boost::shared_ptr<CHDROverlayEffectFilter> m_pOverlayEffectFilter;

	boost::shared_ptr<LuminanceCalcFilter> m_apLumCalcFilter[NUM_TONEMAP_TEXTURES];

	boost::shared_ptr<AdaptationCalcFilter> m_pAdaptationCalcFilter;

	boost::shared_ptr<DownScale4x4Filter> m_pDownScale4x4Filter;

	boost::shared_ptr<HDRBrightPassFilter> m_pBrightPassFilter;

	boost::shared_ptr<GaussianBlurFilter> m_pGaussianBlurFilter;

	boost::shared_ptr<DownScale2x2Filter> m_pDownScale2x2Filter;

	boost::shared_ptr<CombinedBloomFilter> m_pBloomFilter;

	boost::shared_ptr<HDRLightingFinalPassFilter> m_pFinalPassFilter;

	boost::shared_ptr<StarFilter> m_pStarFilter;

	bool m_EnableStarFilter;

public:
	
	HDRLightingFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_HDR_LIGHTING; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void RenderBase( PostProcessEffectFilter& prev_filter );

	void EnableToneMapping( bool enable ) { if(m_pFinalPassFilter) m_pFinalPassFilter->EnableToneMapping( enable ); }

	void SetToneMappingKeyValue( float fKeyValue ) { if(m_pFinalPassFilter) m_pFinalPassFilter->SetToneMappingKeyValue( fKeyValue ); }

	void SetLuminanceAdaptationRate( float fRate ) { if(m_pAdaptationCalcFilter) m_pAdaptationCalcFilter->SetLuminanceAdaptationRate(fRate); }

	void LockPrevRenderTarget( boost::shared_ptr<RenderTargetTextureHolder> pHolder ) { pHolder->IncrementLockCount(); }

	void UnlockPrevRenderTarget( boost::shared_ptr<RenderTargetTextureHolder> pHolder ) { pHolder->DecrementLockCount(); }

	/// used to display adapted luminance for debugging
	boost::shared_ptr<AdaptationCalcFilter> GetAdaptationCalcFilter() { return m_pAdaptationCalcFilter; }
};


class FullScreenBlurFilter : public CombinedFilter
{
	float m_fBlurStrength;

	boost::shared_ptr<DownScale4x4Filter> m_pDownScale4x4Filter;

//	boost::shared_ptr<HorizontalBloomFilter> m_apHorizontalBloomFilter[2];
//	boost::shared_ptr<VerticalBloomFilter> m_apVerticalBloomFilter[2];

	boost::shared_ptr<CombinedBloomFilter> m_pBloomFilter;

public:

	FullScreenBlurFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_FULL_SCREEN_BLUR; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void RenderBase( PostProcessEffectFilter& prev_filter );

	void Render() {}

	void SetBlurStrength( float strength ) { m_fBlurStrength = strength; }
};


class MonochromeColorFilter : public PostProcessEffectFilter
{
public:

	MonochromeColorFilter();

	FilterType GetFilterType() const { return PostProcessEffectFilter::TYPE_MONOCHROME_COLOR; }

	Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void SetColorOffset( float* pafRGB ) {}

	void Render();
};


/*
class GlareFilter : public CombinedFilter
{
public:

	void SetThreashold( float fThreashold ) {}

	void Render();
};
*/
} // namespace amorphous



#endif  /* __PostProcessEffect_HPP__ */
