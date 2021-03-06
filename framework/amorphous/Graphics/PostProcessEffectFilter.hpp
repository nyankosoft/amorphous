#ifndef __PostProcessEffectFilter_HPP__
#define __PostProcessEffectFilter_HPP__


#include "amorphous/base.hpp"
#include "amorphous/3DMath/Rectangular.hpp"
#include "TextureHandle.hpp"
#include "SurfaceFormat.hpp"
#include "GraphicsResourceDescs.hpp"
#include "GraphicsComponentCollector.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"


namespace amorphous
{


class PostProcessFilterShader;
class FilterShaderContainer;


//const D3DSURFACE_DESC *GetD3D9BackBufferSurfaceDesc();
SRectangular GetBackBufferWidthAndHeight();
SRectangular GetCropWidthAndHeight();


class PostProcessEffect
{
public:
	enum TypeFlag
	{
		TF_HDR_LIGHTING       = (1 << 0),
		TF_BLUR               = (1 << 1),
		TF_MONOCHROME_COLOR   = (1 << 2),
	};
};


//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 


// RT_COUNT is the number of simultaneous render targets used in the sample.
enum eRT_TYPE
{
	RT_COLOR = 0,
//	RT_NORMAL,
//	RT_POSITION,
	RT_COUNT
};


class RenderTargetTextureHolder
{
	int m_NumLocks;

public:
//	LPDIRECT3DSURFACE9 m_pTexSurf;
//	TextureHandle m_Texture;
	TextureResourceDesc m_Desc;

	std::shared_ptr<TextureRenderTarget> m_pTextureRenderTarget;

	RenderTargetTextureHolder()
		:
//	m_pTexSurf(NULL),
	m_NumLocks(0)
	{}

	/// call this after rendering
	void ReleaseSurface()
	{
//		SAFE_RELEASE( m_pTexSurf );
	}

	void IncrementLockCount()
	{
		m_NumLocks++;
	}

	void DecrementLockCount()
	{
		if( m_NumLocks <= 0 )
		{
//			LOG_PRINT_WARNING( "Redundant calls. m_NumLocks is already 0." );
			return;
		}

		m_NumLocks--;
	}

	int GetLockCount() const { return m_NumLocks; }

	TextureHandle GetTexture();

	TextureHandle& Texture();
};


class RenderTargetTextureCache
{
public:

	std::vector< std::shared_ptr<RenderTargetTextureHolder> > m_vecpHolder;

//	LPDIRECT3DSURFACE9 m_pOrigRenderTarget;

	std::shared_ptr<RenderTargetTextureHolder> m_pOrigSceneHolder;

	std::weak_ptr<RenderTargetTextureCache> m_pSelf;

public:

	RenderTargetTextureCache()
//		:
//	m_pOrigRenderTarget(NULL)
	{}

	/// Returns the number of textures that match the description of desc
	/// and currently exist in the cache.
	/// Checks only width, height and surface format of the desc
	int GetNumTextures( const TextureResourceDesc& desc );

	Result::Name AddTexture( const TextureResourceDesc& desc );

	Result::Name AddTexture( int width, int height, TextureFormat::Format format );

	std::shared_ptr<RenderTargetTextureHolder> GetTexture( const TextureResourceDesc& desc );

	void SetSelfPtr( std::weak_ptr<RenderTargetTextureCache> pSelf ) { m_pSelf = pSelf; }

	std::weak_ptr<RenderTargetTextureCache> GetSelfPtr() { return m_pSelf; }
};


//--------------------------------------------------------------------------------------


/// A class that represents an instance of a post-process to be applied
/// to the scene.
/// Render target size is determined by the preceeding filter and PostProcessEffectFilter::m_fScalingFactor
class PostProcessEffectFilter
{
protected:

	std::shared_ptr<PostProcessFilterShader> m_pFilterShader;

//    int m_nFxIndex;

//	D3DXVECTOR4 m_avParam[CPostProcess::NUM_PARAMS];

	float m_fScaleX, m_fScaleY;

	std::shared_ptr<RenderTargetTextureHolder> m_pDest;

	std::shared_ptr<RenderTargetTextureHolder> m_pPrevScene;

	/// The specification of the render target.
	TextureResourceDesc m_Desc;

	SRect m_SourceRect;

	/// default: false
	bool m_UseTextureSizeDivisibleBy8;

	/// Set this to 1 to set the render target which is wider by 1 texel on all sides than the source texture.
	/// default: 0
	int m_ExtraTexelBorderWidth;

	char m_SetSamplerParameters[4];
	uint m_MaxInputTextureIndex;
	TextureFilter::Name m_MagFilters[4];
	TextureFilter::Name m_MinFilters[4];

	char m_SetTextureWrapParameters[4];
	TextureAddressMode::Name m_TextureWrapAxis0[4];
	TextureAddressMode::Name m_TextureWrapAxis1[4];

	ShaderTechniqueHandle m_Technique;

	float m_fScalingFactor;

	std::vector< std::shared_ptr<PostProcessEffectFilter> > m_vecpNextFilter;

	std::shared_ptr<RenderTargetTextureCache> m_pCache;

	std::string m_DebugImageFilenameExtraString;

private:

	virtual Result::Name SetRenderTarget( PostProcessEffectFilter& prev_filter );

	/// For debugging
	void SaveProcessedSceneToImageFile();

protected:

	virtual bool GetRenderTarget( PostProcessEffectFilter& prev_filter );

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

	inline PostProcessEffectFilter( int fx_index = -1 ) :
//		m_nFxIndex(fx_index),
		m_fScaleX(1),
		m_fScaleY(1),
		m_UseTextureSizeDivisibleBy8(false),
		m_fScalingFactor(1.0f),
		m_ExtraTexelBorderWidth(0)
	{
		m_Desc.MipLevels = 1;
//		ZeroMemory( m_avParam, sizeof( m_avParam ) );

		for( int i=0; i<numof(m_MagFilters); i++ )
		{
			m_MagFilters[i] = TextureFilter::LINEAR;
			m_MinFilters[i] = TextureFilter::LINEAR;
		}

		for( int i=0; i<numof(m_TextureWrapAxis0); i++ ) m_TextureWrapAxis0[i] = TextureAddressMode::CLAMP_TO_EDGE;
		for( int i=0; i<numof(m_TextureWrapAxis1); i++ ) m_TextureWrapAxis1[i] = TextureAddressMode::CLAMP_TO_EDGE;

		// Mag & min filters for the texture 0 are set by default.
		m_SetSamplerParameters[0] = 1;
		for( int i=1; i<numof(m_SetSamplerParameters); i++ )
			m_SetSamplerParameters[i] = 0;

		for( int i=0; i<numof(m_SetTextureWrapParameters); i++ )
			m_SetTextureWrapParameters[i] = 0;

		m_MaxInputTextureIndex = 0;
	}

	virtual ~PostProcessEffectFilter()
	{}

	virtual FilterType GetFilterType() const = 0;

	virtual Result::Name Init( RenderTargetTextureCache& cache, FilterShaderContainer& filter_shader_container );

	void SetTextureCache( std::shared_ptr<RenderTargetTextureCache> pCache ) { m_pCache = pCache; }

	inline void SetScale( float fScaleX, float fScaleY ) { m_fScaleX = fScaleX; m_fScaleY = fScaleY; }

	void SetExtraTexelBorderWidth( int width ) { m_ExtraTexelBorderWidth = width; }

//	void SetScalingFactor( int scaling_factor ) { m_fScalingFactor = scaling_factor; }

	void SetSourceRect( SRect rect ) { m_SourceRect = rect; }

//	void Render( PostProcessEffectFilter& prev_filter );

	/// \brief Sets the render target of the previous filter, i.e. the input for this filter,
	/// to the stage 0 of the fixed function pipeline.
	virtual void RenderBase( PostProcessEffectFilter& prev_filter );

	/// \brief Renders the post processsed scene to the render target texture.
	/// The render target of the previous filter, i.e. the input for this filter,
	/// is set in RenderBase() before this function is called.
	virtual void Render() {}

	virtual void StorePrevFilterResults( PostProcessEffectFilter& prev_filter );

	virtual bool IsReadyToRender() { return true; }

	void SetRenderTargetSurfaceFormat( TextureFormat::Format fmt ) { m_Desc.Format = fmt; }

	void SetRenderTargetSize( int width, int height ) { m_Desc.Width = width; m_Desc.Height = height; }

	virtual void AddNextFilter( std::shared_ptr<PostProcessEffectFilter> pFilter ) { m_vecpNextFilter.push_back( pFilter ); }

	virtual void ClearNextFilters() { m_vecpNextFilter.resize( 0 ); }

	void SetFilterShader(  std::shared_ptr<PostProcessFilterShader> pFilterShader ) { m_pFilterShader = pFilterShader; }

	std::shared_ptr<PostProcessFilterShader> GetFilterShader() { return m_pFilterShader; }

	std::shared_ptr<RenderTargetTextureHolder> GetDestRenderTarget() { return m_pDest; }

	/// rationale to make this virtual: combined filters may not need to lock prev render target
	virtual void LockPrevRenderTarget( std::shared_ptr<RenderTargetTextureHolder> pHolder ) { pHolder->IncrementLockCount(); }

	virtual void UnlockPrevRenderTarget( std::shared_ptr<RenderTargetTextureHolder> pHolder ) { pHolder->DecrementLockCount(); }

	const std::string& GetDebugImageFilenameExtraString() { return m_DebugImageFilenameExtraString; }

	void SetDebugImageFilenameExtraString( const std::string& str ) { m_DebugImageFilenameExtraString = str; }

	static int ms_SaveFilterResultsAtThisFrame;
};


} // namespace amorphous



#endif /* __PostProcessEffectFilter_HPP__ */
