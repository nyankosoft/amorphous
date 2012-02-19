#ifndef __PostProcessEffectFilter_HPP__
#define __PostProcessEffectFilter_HPP__


#include <vector>
#include <d3dx9.h>
#include "../base.hpp"
#include "../3DMath/Rectangular.hpp"
#include "Direct3D/Direct3D9.hpp"
#include "TextureHandle.hpp"
#include "SurfaceFormat.hpp"
#include "GraphicsResourceDescs.hpp"
#include "GraphicsComponentCollector.hpp"
#include "Shader/ShaderTechniqueHandle.hpp"


class CPostProcessFilterShader;
class CFilterShaderContainer;


const D3DSURFACE_DESC *GetD3D9BackBufferSurfaceDesc();
SRectangular GetBackBufferWidthAndHeight();
SRectangular GetCropWidthAndHeight();


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

	boost::shared_ptr<CRenderTargetTextureHolder> m_pOrigSceneHolder;

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

	Result::Name AddTexture( int width, int height, TextureFormat::Format format );

	boost::shared_ptr<CRenderTargetTextureHolder> GetTexture( const CTextureResourceDesc& desc );

	void SetSelfPtr( boost::weak_ptr<CRenderTargetTextureCache> pSelf ) { m_pSelf = pSelf; }

	boost::weak_ptr<CRenderTargetTextureCache> GetSelfPtr() { return m_pSelf; }
};


//--------------------------------------------------------------------------------------


/// A class that represents an instance of a post-process to be applied
/// to the scene.
/// Render target size is determined by the preceeding filter and CPostProcessEffectFilter::m_fScalingFactor
class CPostProcessEffectFilter
{
protected:

	boost::shared_ptr<CPostProcessFilterShader> m_pFilterShader;

//    int m_nFxIndex;

//	D3DXVECTOR4 m_avParam[CPostProcess::NUM_PARAMS];

	float m_fScaleX, m_fScaleY;

	boost::shared_ptr<CRenderTargetTextureHolder> m_pDest;

	boost::shared_ptr<CRenderTargetTextureHolder> m_pPrevScene;

	/// The specification of the render target.
	CTextureResourceDesc m_Desc;

	RECT m_SourceRect;

	/// default: false
	bool m_UseTextureSizeDivisibleBy8;

	/// Set this to 1 to set the render target which is wider by 1 texel on all sides than the source texture.
	/// default: 0
	int m_ExtraTexelBorderWidth;

	CShaderTechniqueHandle m_Technique;

	float m_fScalingFactor;

	std::vector< boost::shared_ptr<CPostProcessEffectFilter> > m_vecpNextFilter;

	boost::shared_ptr<CRenderTargetTextureCache> m_pCache;

	std::string m_DebugImageFilenameExtraString;

private:

	Result::Name CPostProcessEffectFilter::SetRenderTarget( CPostProcessEffectFilter& prev_filter );

protected:

	virtual bool GetRenderTarget( CPostProcessEffectFilter& prev_filter );

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

	inline CPostProcessEffectFilter( int fx_index = -1 ) :
//		m_nFxIndex(fx_index),
		m_fScaleX(1),
		m_fScaleY(1),
		m_UseTextureSizeDivisibleBy8(false),
		m_fScalingFactor(1.0f),
		m_ExtraTexelBorderWidth(0)
	{
//		ZeroMemory( m_avParam, sizeof( m_avParam ) );
	}

	virtual ~CPostProcessEffectFilter()
	{}

	virtual FilterType GetFilterType() const = 0;

	virtual Result::Name Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container );

	void SetTextureCache( boost::shared_ptr<CRenderTargetTextureCache> pCache ) { m_pCache = pCache; }

	inline void SetScale( float fScaleX, float fScaleY ) { m_fScaleX = fScaleX; m_fScaleY = fScaleY; }

	void SetExtraTexelBorderWidth( int width ) { m_ExtraTexelBorderWidth = width; }

//	void SetScalingFactor( int scaling_factor ) { m_fScalingFactor = scaling_factor; }

	void SetSourceRect( RECT rect ) { m_SourceRect = rect; }

//	void Render( CPostProcessEffectFilter& prev_filter );

	/// \brief Sets the render target of the previous filter, i.e. the input for this filter,
	/// to the stage 0 of the fixed function pipeline.
	virtual void RenderBase( CPostProcessEffectFilter& prev_filter );

	/// \brief Renders the post processsed scene to the render target texture.
	/// The render target of the previous filter, i.e. the input for this filter,
	/// is set in RenderBase() before this function is called.
	virtual void Render() {}

	virtual void StorePrevFilterResults( CPostProcessEffectFilter& prev_filter );

	virtual bool IsReadyToRender() { return true; }

	void SetRenderTargetSurfaceFormat( TextureFormat::Format fmt ) { m_Desc.Format = fmt; }

	void SetRenderTargetSize( int width, int height ) { m_Desc.Width = width; m_Desc.Height = height; }

	virtual void AddNextFilter( boost::shared_ptr<CPostProcessEffectFilter> pFilter ) { m_vecpNextFilter.push_back( pFilter ); }

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



#endif /* __PostProcessEffectFilter_HPP__ */
