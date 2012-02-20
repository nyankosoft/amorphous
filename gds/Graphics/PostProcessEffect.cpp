#include "PostProcessEffect.hpp"
#include "PostProcessEffectManager.hpp"
#include "Graphics/SurfaceFormat.hpp"
#include "Graphics/Direct3D/D3DSurfaceFormat.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Log/DefaultLog.hpp"


using std::string;
using std::vector;
using namespace boost;


#define V(x) { hr = x; if( FAILED(hr) ) { LOG_PRINT_ERROR( string(#x) + " failed." ); } }
#define V_RETURN(x) { hr = x; if(FAILED(hr)) return hr; }


// Texture coordinate rectangle
struct CoordRect
{
    float fLeftU, fTopV;
    float fRightU, fBottomV;
};


// Screen quad vertex format
struct ScreenVertex
{
    D3DXVECTOR4 p; // position
    D3DXVECTOR2 t; // texture coordinate

    static const DWORD FVF;
};
const DWORD                 ScreenVertex::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;


const D3DSURFACE_DESC *GetD3D9BackBufferSurfaceDesc()
{
	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();

	// retrieve the back buffer size
	static D3DSURFACE_DESC s_back_buffer_desc;
	IDirect3DSurface9 *pBackBuffer;
	pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pBackBuffer->GetDesc( &s_back_buffer_desc );
	return &s_back_buffer_desc;
}


SRectangular GetBackBufferWidthAndHeight()
{
	const D3DSURFACE_DESC *pBB = GetD3D9BackBufferSurfaceDesc();
	return SRectangular( (int)(pBB->Width), (int)(pBB->Height) );
}


SRectangular GetCropWidthAndHeight()
{
	SRectangular bb = GetBackBufferWidthAndHeight();
	return SRectangular( bb.width - bb.width % 8, bb.height - bb.height % 8 );
}


void GetTextureRect( shared_ptr<CRenderTargetTextureHolder>& pSrc, RECT *pDest )
{
	pDest->left   = 0;
	pDest->top    = 0;
//	pDest->right  = pSrc->m_Desc.Width;
//	pDest->bottom = pSrc->m_Desc.Height;

	D3DSURFACE_DESC desc;
	pSrc->m_Texture.GetTexture()->GetLevelDesc( 0, &desc );
	pDest->right  = desc.Width;
	pDest->bottom = desc.Height;
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale4x4
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
Result::Name GetSampleOffsets_DownScale4x4( unsigned int width, unsigned int height, Vector2 avSampleOffsets[] )
{
    if( NULL == avSampleOffsets )
		return Result::INVALID_ARGS;

    float tU = 1.0f / (float)width;
    float tV = 1.0f / (float)height;

    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 4; y++ ) // y = { -1.5, -0.5, 0.5, 1.5 }
    {
        for( int x = 0; x < 4; x++ ) // x = { -1.5, -0.5, 0.5, 1.5 }
        {
            avSampleOffsets[ index ].x = ( x - 1.5f ) * tU;
            avSampleOffsets[ index ].y = ( y - 1.5f ) * tV;

            index++;
        }
    }

	return Result::SUCCESS;
}



//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale2x2
// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
//       pixel shader.
//-----------------------------------------------------------------------------
Result::Name GetSampleOffsets_DownScale2x2( unsigned int width, unsigned int height, Vector2 avSampleOffsets[] )
{
    if( NULL == avSampleOffsets )
		return Result::INVALID_ARGS;

    float tU = 1.0f / (float)width;
    float tV = 1.0f / (float)height;

    // Sample from the 4 surrounding points. Since the center point will be in
    // the exact center of 4 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 2; y++ )
    {
        for( int x = 0; x < 2; x++ )
        {
            avSampleOffsets[ index ].x = ( x - 0.5f ) * tU;
            avSampleOffsets[ index ].y = ( y - 0.5f ) * tV;

            index++;
        }
    }

	return Result::SUCCESS;
}


/**
 Draw a properly aligned quad covering the entire render target.
 Calculates the size of the rect to draw from the surface
 of the current render target.
*/
void DrawFullScreenQuad( float fLeftU, float fTopV, float fRightU, float fBottomV )
{
	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();

	D3DSURFACE_DESC dtdsdRT;
	PDIRECT3DSURFACE9 pSurfRT;
	HRESULT hr = S_OK;

	// Acquire render target width and height
	hr = pd3dDevice->GetRenderTarget( 0, &pSurfRT );
	hr = pSurfRT->GetDesc( &dtdsdRT );
	ULONG ref_count = pSurfRT->Release();

	// Ensure that we're directly mapping texels to pixels by offset by 0.5
	// For more info see the doc page titled "Directly Mapping Texels to Pixels"
	float fWidth5 = ( float )dtdsdRT.Width - 0.5f;
	float fHeight5 = ( float )dtdsdRT.Height - 0.5f;

	// Draw the quad
	ScreenVertex svQuad[4];

	svQuad[0].p = D3DXVECTOR4( -0.5f, -0.5f, 0.5f, 1.0f );
	svQuad[0].t = D3DXVECTOR2( fLeftU, fTopV );

	svQuad[1].p = D3DXVECTOR4( fWidth5, -0.5f, 0.5f, 1.0f );
	svQuad[1].t = D3DXVECTOR2( fRightU, fTopV );

	svQuad[2].p = D3DXVECTOR4( -0.5f, fHeight5, 0.5f, 1.0f );
	svQuad[2].t = D3DXVECTOR2( fLeftU, fBottomV );

	svQuad[3].p = D3DXVECTOR4( fWidth5, fHeight5, 0.5f, 1.0f );
	svQuad[3].t = D3DXVECTOR2( fRightU, fBottomV );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	hr = pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	hr = pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	hr = pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	hr = pd3dDevice->SetFVF( ScreenVertex::FVF );
	hr = pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof( ScreenVertex ) );
	hr = pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}


void DrawFullScreenQuad( const CoordRect& c )
{
    DrawFullScreenQuad( c.fLeftU, c.fTopV, c.fRightU, c.fBottomV );
}


void RenderFullScreenQuad( LPD3DXEFFECT pEffect, const CoordRect& c )
{
	HRESULT hr;
	UINT uiPassCount, uiPass;

	V( pEffect->Begin( &uiPassCount, 0 ) );

	for( uiPass = 0; uiPass < uiPassCount; uiPass++ )
	{
		hr = pEffect->BeginPass( uiPass );

		// Draw a fullscreen quad to sample the RT
		DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );

		hr = pEffect->EndPass();
	}

	hr = pEffect->End();
}


void RenderFullScreenQuad( LPD3DXEFFECT pEffect, float fLeftU, float fTopV, float fRightU, float fBottomV )
{
	CoordRect c;
	c.fLeftU   = fLeftU;
	c.fTopV    = fTopV;
	c.fRightU  = fRightU;
	c.fBottomV = fBottomV;

    RenderFullScreenQuad( pEffect, c );
}



/**
 Helper function for GetSampleOffsets function to compute the 
 2 parameter Gaussian distrubution using the given standard deviation rho.
*/
float GaussianDistribution( float x, float y, float rho )
{
    float g = 1.0f / sqrtf( 2.0f * (float)PI * rho * rho );
    g *= expf( -( x * x + y * y ) / ( 2 * rho * rho ) );

    return g;
}


/**
 Get the texture coordinate offsets to be used inside the GaussBlur5x5 pixel shader.
*/
HRESULT GetSampleOffsets_GaussBlur5x5( unsigned int dwD3DTexWidth,
                                       unsigned int dwD3DTexHeight,
                                       Vector2* avTexCoordOffset,
                                       Vector4* avSampleWeight,
                                       float fMultiplier = 1.0f )
{
    float tu = 1.0f / ( float )dwD3DTexWidth;
    float tv = 1.0f / ( float )dwD3DTexHeight;

    Vector4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );

    float totalWeight = 0.0f;
    int index = 0;
    for( int x = -2; x <= 2; x++ )
    {
        for( int y = -2; y <= 2; y++ )
        {
            // Exclude pixels with a block distance greater than 2. This will
            // create a kernel which approximates a 5x5 kernel using only 13
            // sample points instead of 25; this is necessary since 2.0 shaders
            // only support 16 texture grabs.
            if( abs( x ) + abs( y ) > 2 )
                continue;

            // Get the unscaled Gaussian intensity for this offset
            avTexCoordOffset[index] = Vector2( x * tu, y * tv );
            avSampleWeight[index] = vWhite * GaussianDistribution( ( float )x, ( float )y, 1.0f );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

    // Divide the current weight by the total weight of all the samples; Gaussian
    // blur kernels add to 1.0f to ensure that the intensity of the image isn't
    // changed when the blur occurs. An optional multiplier variable is used to
    // add or remove image intensity during the blur.
    for( int i = 0; i < index; i++ )
    {
        avSampleWeight[i] /= totalWeight;
        avSampleWeight[i] *= fMultiplier;
    }

    return S_OK;
}


/**
 Get the texture coordinate offsets to be used inside the Bloom pixel shader.
 By default, weights stored in avColorWeight are
  [0]  0.26596150
  [1]  0.25158879
  [2]  0.21296531
  [3]  0.16131380
  [4]  0.10934003
  [5]  0.066318087
  [6]  0.035993975
  [7]  0.017481256
  [8]  0.25158879
  [9]  0.21296531
  [10] 0.16131380
  [11] 0.10934003
  [12] 0.066318087
  [13] 0.035993975
  [14] 0.017481256
  [15] 0.00000000
  sum: approx. 1.976
*/
Result::Name GetSampleOffsets_Bloom( unsigned int dwD3DTexSize,
							   float afTexCoordOffset[15],
							   Vector4* avColorWeight,
							   float fDeviation,
							   float fMultiplier )
{
	int i = 0;
	float tu = 1.0f / ( float )dwD3DTexSize;

	// Fill the center texel
	float weight = fMultiplier * GaussianDistribution( 0, 0, fDeviation );
	avColorWeight[0] = Vector4( weight, weight, weight, 1.0f );

	afTexCoordOffset[0] = 0.0f;

	// Fill the first half
	for( i = 1; i < 8; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = fMultiplier * GaussianDistribution( ( float )i, 0, fDeviation );
		afTexCoordOffset[i] = i * tu;

		avColorWeight[i] = Vector4( weight, weight, weight, 1.0f );
	}

	// Mirror to the second half
	for( i = 8; i < 15; i++ )
	{
		avColorWeight[i] = avColorWeight[i - 7];
		afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
	}

	return Result::SUCCESS;
}


/**
 Get the texture coordinate offsets to be used inside the Bloom pixel shader.
*/
Result::Name GetSampleOffsets_Star( unsigned int dwD3DTexSize,
							  float afTexCoordOffset[15],
							  Vector4* avColorWeight,
							  float fDeviation )
{
	int i = 0;
	float tu = 1.0f / ( float )dwD3DTexSize;

	// Fill the center texel
	float weight = 1.0f * GaussianDistribution( 0, 0, fDeviation );
	avColorWeight[0] = Vector4( weight, weight, weight, 1.0f );

	afTexCoordOffset[0] = 0.0f;

	// Fill the first half
	for( i = 1; i < 8; i++ )
	{
		// Get the Gaussian intensity for this offset
		weight = 1.0f * GaussianDistribution( ( float )i, 0, fDeviation );
		afTexCoordOffset[i] = i * tu;

		avColorWeight[i] = Vector4( weight, weight, weight, 1.0f );
	}

	// Mirror to the second half
	for( i = 8; i < 15; i++ )
	{
		avColorWeight[i] = avColorWeight[i - 7];
		afTexCoordOffset[i] = -afTexCoordOffset[i - 7];
	}

	return Result::SUCCESS;
}


/**
 Get the texture coordinates to use when rendering into the destination
 texture, given the source and destination rectangles
*/
HRESULT GetTextureCoords( CTextureHandle& tex_src, RECT* pRectSrc,
						  CTextureHandle& tex_dest, RECT* pRectDest, CoordRect* pCoords )
{
	LPDIRECT3DTEXTURE9 pTexSrc  = tex_src.GetTexture();
	LPDIRECT3DTEXTURE9 pTexDest = tex_dest.GetTexture();

	HRESULT hr = S_OK;
	D3DSURFACE_DESC desc;
	float tU, tV;

	// Validate arguments
	if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
		return E_INVALIDARG;

	// Start with a default mapping of the complete source surface to complete 
	// destination surface
	pCoords->fLeftU = 0.0f;
	pCoords->fTopV = 0.0f;
	pCoords->fRightU = 1.0f;
	pCoords->fBottomV = 1.0f;

	// If not using the complete source surface, adjust the coordinates
	if( pRectSrc != NULL )
	{
		// Get destination texture description
		hr = pTexSrc->GetLevelDesc( 0, &desc );
		if( FAILED( hr ) )
			return hr;

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / desc.Width;
		tV = 1.0f / desc.Height;

		pCoords->fLeftU += pRectSrc->left * tU;
		pCoords->fTopV += pRectSrc->top * tV;
		pCoords->fRightU -= ( desc.Width - pRectSrc->right ) * tU;
		pCoords->fBottomV -= ( desc.Height - pRectSrc->bottom ) * tV;
	}

	// If not drawing to the complete destination surface, adjust the coordinates
	if( pRectDest != NULL )
	{
		// Get source texture description
		hr = pTexDest->GetLevelDesc( 0, &desc );
		if( FAILED( hr ) )
			return hr;

		// These delta values are the distance between source texel centers in 
		// texture address space
		tU = 1.0f / desc.Width;
		tV = 1.0f / desc.Height;

		pCoords->fLeftU -= pRectDest->left * tU;
		pCoords->fTopV -= pRectDest->top * tV;
		pCoords->fRightU += ( desc.Width - pRectDest->right ) * tU;
		pCoords->fBottomV += ( desc.Height - pRectDest->bottom ) * tV;
	}

	return S_OK;
}


LPD3DXEFFECT GetD3DXEffect(CPostProcessEffectFilter& filter)
{
	boost::shared_ptr<CPostProcessFilterShader> pFilterShader = filter.GetFilterShader();
	if( !pFilterShader )
		return NULL;

	CShaderManager *pShaderMgr = pFilterShader->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return NULL;

	return pShaderMgr->GetEffect();
}


//============================================================================
// CDownScale4x4Filter
//============================================================================

CDownScale4x4Filter::CDownScale4x4Filter()
{
	m_Technique.SetTechniqueName( "DownScale4x4" );

//	m_ScalingFactor = 0.25f;
}


Result::Name CDownScale4x4Filter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	return Result::SUCCESS;
}


/*
bool CDownScale4x4Filter::GetRenderTarget( CPostProcessEffectFilter& prev_filter, boost::shared_ptr<CRenderTargetTextureHolder>& pDest )
{
	int src_w = prev_filter.pDest->m_Desc.Width;
	int src_h = prev_filter.pDest->m_Desc.Height;
	int crop_width  = src_w - src_w % 8;
	int crop_height = src_h - src_h % 8;

	m_Desc.Width  = crop_width  / 4;
	m_Desc.Height = crop_height / 4;

	m_pDest = m_pCache->GetTexture( m_Desc );
	return m_pDest ? true : false;
}
*/


void CDownScale4x4Filter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);
	HRESULT hr = S_OK;

	Vector2 avSampleOffsets[MAX_SAMPLES];
	memset( avSampleOffsets, 0, sizeof(avSampleOffsets) );

//	const D3DSURFACE_DESC* pBackBufferDesc = GetD3D9BackBufferSurfaceDesc();
	int prev_scene_width  = m_pPrevScene->m_Desc.Width;// pBackBufferDesc->Width;
	int prev_scene_height = m_pPrevScene->m_Desc.Height;// pBackBufferDesc->Height;

//	m_Desc.Width  = prev_scene_width;
//	m_Desc.Height = prev_scene_height;
//	m_pDest = GetRenderTarget( m_Desc );
///	if( !m_pDest )
///		return;

//	DWORD dwCropWidth = ???;
//	DWORD dwCropHeight = ???;

	// Create a 1/4 x 1/4 scale copy of the HDR texture. Since bloom textures
	// are 1/8 x 1/8 scale, border texels of the HDR texture will be discarded 
	// to keep the dimensions evenly divisible by 8; this allows for precise 
	// control over sampling inside pixel shaders.
//	pEffect->SetTechnique( "DownScale4x4" ); // set in RenderBase()

	// Place the rectangle in the center of the back buffer surface
	RECT rectSrc;
//	rectSrc.left = ( pBackBufferDesc->Width  - dwCropWidth )  / 2;
//	rectSrc.top  = ( pBackBufferDesc->Height - dwCropHeight ) / 2;
//	rectSrc.right = rectSrc.left + dwCropWidth;
//	rectSrc.bottom = rectSrc.top + dwCropHeight;
//	RECT rectSrc = m_SourceRect;
	GetTextureRect( m_pPrevScene, &rectSrc );

	// rectSrc(l,t,r,b) = ( 0, 0, pBackBufferDesc->Width, pBackBufferDesc->Height ), when m_dwCropWidth and m_dwCropHeight are
	// same as those of the back buffer, which happens when back buffer width and height is divisible by 8.

	// Get the texture coordinates for the render target
	CoordRect coords;
	if( m_pDest )
	{
		GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, NULL, &coords );
	}
	else
	{
		coords.fLeftU   = 0;
		coords.fTopV    = 0;
		coords.fRightU  = 1;
		coords.fBottomV = 1;
	}

	// Get the sample offsets used within the pixel shader
	GetSampleOffsets_DownScale4x4( (unsigned int)prev_scene_width, (unsigned int)prev_scene_height, avSampleOffsets );
	hr = pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

//	hr = pd3dDevice->SetTexture( 0, m_pPrevScene->m_Texture.GetTexture() ); // done in RenderBase()
	hr = pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	hr = pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	hr = pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	hr = pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	RenderFullScreenQuad( pEffect, coords );
}


//============================================================================
// CDownScale2x2Filter
//============================================================================

CDownScale2x2Filter::CDownScale2x2Filter()
{
	m_Technique.SetTechniqueName( "DownScale2x2" );

//	m_ScalingFactor = 0.50f;
}


Result::Name CDownScale2x2Filter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	return Result::SUCCESS;
}


//-----------------------------------------------------------------------------
// Name: StarSource_To_BloomSource
// Desc: Scale down m_pTexStarSource by 1/2 x 1/2 and place the result in 
//       m_pTexBloomSource
//-----------------------------------------------------------------------------
void CDownScale2x2Filter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
//	LPD3DXEFFECT pEffect = m_pFilterShader->GetShader()->GetEffect();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;

	Vector2 avSampleOffsets[MAX_SAMPLES];
	memset( avSampleOffsets, 0, sizeof(avSampleOffsets) );

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
//	GetTextureRect( m_pPrevScene->m_Texture.GetTexture(), &rectSrc );
	GetTextureRect( m_pPrevScene, &rectSrc );
	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
//	GetTextureRect( m_pDest->m_Texture.GetTexture(), &rectDest );
	GetTextureRect( m_pDest, &rectDest );
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order
	// to sample from the source rectangle and render into the destination rectangle
	CoordRect coords;
	GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, &rectDest, &coords );

	// Get the sample offsets used within the pixel shader
	D3DSURFACE_DESC desc;
//	hr = m_pTexBrightPass->GetLevelDesc( 0, &desc );
	hr = m_pPrevScene->m_Texture.GetTexture()->GetLevelDesc( 0, &desc );
	if( FAILED( hr ) )
		return;// hr;

	GetSampleOffsets_DownScale2x2( (unsigned int)desc.Width, (unsigned int)desc.Height, avSampleOffsets );
	hr = pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

	// Create an exact 1/2 x 1/2 copy of the source texture
	//pEffect->SetTechnique( "DownScale2x2" );

//	pd3dDevice->SetTexture( 0, m_pTexStarSource );
	hr = pd3dDevice->SetScissorRect( &rectDest );
//	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ); // original D3D sample
	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	RenderFullScreenQuad( pEffect, coords );

	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
}



//============================================================================
// CHDRBrightPassFilter
//============================================================================

CHDRBrightPassFilter::CHDRBrightPassFilter()
{
	m_Technique.SetTechniqueName( "BrightPassFilter" );
}


Result::Name CHDRBrightPassFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	return Result::SUCCESS;
}


/// Run the bright-pass filter on source render target texture
/// and place the result in dest render target texture.
void CHDRBrightPassFilter::Render()
{
	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();
//	LPD3DXEFFECT pEffect = m_pFilterShader->GetShader()->GetEffect();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;

	D3DSURFACE_DESC desc;
	m_pPrevScene->m_Texture.GetTexture()->GetLevelDesc( 0, &desc );

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
//	GetTextureRect( m_pPrevScene->m_Texture.GetTexture(), &rectSrc );
	GetTextureRect( m_pPrevScene, &rectSrc );
	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
//	GetTextureRect( m_pDest->m_Texture.GetTexture(), &rectDest );
	GetTextureRect( m_pDest, &rectDest );
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	CoordRect coords;
	GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, &rectDest, &coords );

	// The bright-pass filter removes everything from the scene except lights and
	// bright reflections
	if( pEffect )
		hr = pEffect->SetTechnique( "BrightPassFilter" );

//	hr = pd3dDevice->SetRenderTarget( 0, m_pDest->m_pTexSurf );
//	hr = pd3dDevice->SetTexture( 0, m_pPrevScene->m_Texture.GetTexture() ); // done in RenderBase()

	hr = pd3dDevice->SetTexture( 1, m_pAdaptedLuminanceTexture->m_Texture.GetTexture() );

//	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ); // original D3D sample
	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
	hr = pd3dDevice->SetScissorRect( &rectDest );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

	RenderFullScreenQuad( pEffect, coords );

	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	return;
}



//=======================================================================
// CGaussianBlurFilter
//=======================================================================

CGaussianBlurFilter::CGaussianBlurFilter()
{
	m_Technique.SetTechniqueName( "GaussBlur5x5" );

	m_ExtraTexelBorderWidth = 1;
	m_UseTextureSizeDivisibleBy8 = true;
}


//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: Perform a 5x5 gaussian blur on m_pTexBrightPass and place the result
//       in m_pTexStarSource. The bright-pass filtered image is blurred before
//       being used for star operations to avoid aliasing artifacts.
//-----------------------------------------------------------------------------
void CGaussianBlurFilter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;

	Vector2 avSampleOffsets[MAX_SAMPLES];
	Vector4 avSampleWeights[MAX_SAMPLES];

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
//	GetTextureRect( m_pDest->m_Texture, &rectDest );
	if( m_pDest )
		GetTextureRect( m_pDest, &rectDest );
	else
		GetTextureRect( m_pCache->m_pOrigSceneHolder, &rectDest );

	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	CoordRect coords;
	if( m_pDest )
		GetTextureCoords( m_pPrevScene->m_Texture, NULL, m_pDest->m_Texture, &rectDest, &coords );
	else
	{
		coords.fLeftU = 0;
		coords.fTopV  = 0;
		coords.fRightU  = 1;
		coords.fBottomV = 1;
	}

	// Get the sample offsets used within the pixel shader
	D3DSURFACE_DESC desc;
	hr = m_pPrevScene->m_Texture.GetTexture()->GetLevelDesc( 0, &desc );
	if( FAILED( hr ) )
		return;// hr;


	GetSampleOffsets_GaussBlur5x5( desc.Width, desc.Height, avSampleOffsets, avSampleWeights );

	hr = pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );
	hr = pEffect->SetValue( "g_avSampleWeights", avSampleWeights, sizeof( avSampleWeights ) );

	// The gaussian blur smooths out rough edges to avoid aliasing effects
	// when the star effect is run
//	pEffect->SetTechnique( "GaussBlur5x5" );

//	pd3dDevice->SetRenderTarget( 0, m_pDest->pTexSurf );
	hr = pd3dDevice->SetTexture( 0, m_pPrevScene->m_Texture.GetTexture() );
	hr = pd3dDevice->SetScissorRect( &rectDest );
//	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ); // original D3D sample
	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	RenderFullScreenQuad( pEffect, coords );

	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	return;
}



//=======================================================================
// CBloomFilter
//=======================================================================

CBloomFilter::CBloomFilter()
{
	m_Technique.SetTechniqueName( "Bloom" );

	memset( m_avSampleOffsets, 0, sizeof( m_avSampleOffsets ) );
	memset( m_avSampleWeights, 0, sizeof( m_avSampleWeights ) );

	m_DoScissorTesting = false;

	// default value for deviation
	// - used when the filter is used for bloom in HDR lighting
	// - full screen blur filter may set different values
	m_fDeviation = 3.0f;

	m_fBloomFactor = 1.0f;

	m_UseForBlurFilter = false;
}


void CBloomFilter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);
	HRESULT hr;

	GetSampleOffsets();

	if( m_UseForBlurFilter )
	{
		// need to normalize weight
		float total_weight = 0.0f;
		for( int i = 0; i < MAX_SAMPLES; i++ )
			total_weight += m_avSampleWeights[i].x;

		for( int i = 0; i < MAX_SAMPLES; i++ )
			m_avSampleWeights[i] /= total_weight;
	}

//	pEffect->SetTechnique( "Bloom" );
	hr = pEffect->SetValue( "g_avSampleOffsets", m_avSampleOffsets, sizeof( m_avSampleOffsets ) );
	hr = pEffect->SetValue( "g_avSampleWeights", m_avSampleWeights, sizeof( m_avSampleWeights ) );

	// blur filter  -> set to 1/16
	// bloom filter -> set to 1
	hr = pEffect->SetFloat( "g_fBloomFactor", m_fBloomFactor );

//	pd3dDevice->SetRenderTarget( 0, pSurfTempBloom );
//	pd3dDevice->SetTexture( 0, m_apTexBloom[2] );

	// horizontal & vertical sets the tex coords differently
	// because the latter writes the scene texture that has not extra border texels

	RECT rectSrc;
//	GetTextureRect( m_pPrevScene->m_Texture.GetTexture(), &rectSrc );
	GetTextureRect( m_pPrevScene, &rectSrc );
	InflateRect( &rectSrc, -1, -1 );

	CoordRect coords;
	RECT rectDest;
	bool writing_to_texture_with_border_texels = m_DoScissorTesting;

	if( !m_pDest )
	{
		coords.fLeftU   = 0.0f;
		coords.fTopV    = 0.0f;
		coords.fRightU  = 1.0f;
		coords.fBottomV = 1.0f;
		memset( &rectDest, 0, sizeof(RECT) );
	}
	else if( writing_to_texture_with_border_texels )
	{
		// horizontal blur
//		GetTextureRect( m_pDest->m_Texture.GetTexture(), &rectDest );
		GetTextureRect( m_pDest, &rectDest );
		InflateRect( &rectDest, -1, -1 );

		GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, &rectDest, &coords );
	}
	else
	{
		// vertical blur settings
		GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, NULL, &coords );
	}


	if( m_DoScissorTesting )
	{
		pd3dDevice->SetScissorRect( &rectDest );
//		pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
		pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
	}

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

	RenderFullScreenQuad( pEffect, coords );

	if( m_DoScissorTesting )
		pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
}


CHorizontalBloomFilter::CHorizontalBloomFilter()
{
	m_DoScissorTesting = true;
}


/// The greater deviation value makes the image more blurrier
void CHorizontalBloomFilter::GetSampleOffsets()
{
//	float fDeviation = 3.0f; // original D3D sample
	static float fDeviation = 0.5f; // experiment
	memset( m_afSampleOffsets, 0, sizeof(m_afSampleOffsets) );
	memset( m_avSampleWeights, 0, sizeof(m_avSampleWeights) );
	HRESULT hr = GetSampleOffsets_Bloom( m_Desc.Width, m_afSampleOffsets, m_avSampleWeights, m_fDeviation, 2.0f );
	for( int i = 0; i < MAX_SAMPLES; i++ )
	{
		m_avSampleOffsets[i] = Vector2( m_afSampleOffsets[i], 0.0f );
	}
}


CVerticalBloomFilter::CVerticalBloomFilter()
{
	m_DoScissorTesting = false;
}


void CVerticalBloomFilter::GetSampleOffsets()
{
	memset( m_afSampleOffsets, 0, sizeof(m_afSampleOffsets) );
	memset( m_avSampleWeights, 0, sizeof(m_avSampleWeights) );
	HRESULT hr = GetSampleOffsets_Bloom( m_Desc.Height, m_afSampleOffsets, m_avSampleWeights, m_fDeviation, 2.0f );
	for( int i = 0; i < MAX_SAMPLES; i++ )
	{
		m_avSampleOffsets[i] = Vector2( 0.0f, m_afSampleOffsets[i] );
	}
}



CCombinedBloomFilter::CCombinedBloomFilter()
{
	SRectangular cbb = GetCropWidthAndHeight(); // cropped back buffer

	// based on the original Direct3D HDR Lighting sample
	m_BasePlane.width  = cbb.width  / 8;
	m_BasePlane.height = cbb.height / 8;
}


Result::Name CCombinedBloomFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	SRectangular bb = GetBackBufferWidthAndHeight();
	SRectangular cbb = GetCropWidthAndHeight(); // cropped back buffer

	const SRectangular base_plane = m_BasePlane;

	// shared settings for bloom textures
	CTextureResourceDesc desc;
	desc.UsageFlags = UsageFlag::RENDER_TARGET;
	desc.Format     = TextureFormat::A8R8G8B8;

	desc.Width      = base_plane.width  + 2;
	desc.Height     = base_plane.height + 2;

	int n = cache.GetNumTextures( desc );
	for( int i=n; i<2; i++ )
	{
		Result::Name res = cache.AddTexture( desc );
		if( res != Result::SUCCESS )
			LOG_PRINT_ERROR( " Failed to create a render target texture." );
	}

	desc.Width  = base_plane.width;
	desc.Height = base_plane.height;

	n = cache.GetNumTextures( desc );
	if( n == 0 )
	{
		// render target to store the result of bloom filter
		Result::Name res = cache.AddTexture( desc );
		if( res != Result::SUCCESS )
			LOG_PRINT_ERROR( " Failed to create a render target texture." );
	}

	m_pGaussianBlurFilter = shared_ptr<CGaussianBlurFilter>( new CGaussianBlurFilter );
	m_pGaussianBlurFilter->SetRenderTargetSize( base_plane.width + 2, base_plane.height + 2 );
	m_pGaussianBlurFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pGaussianBlurFilter->SetFilterShader( filter_shader_container.GetFilterShader( "GaussBlur5x5" ) );
	m_pGaussianBlurFilter->SetTextureCache( cache.GetSelfPtr().lock() );
	m_pGaussianBlurFilter->SetDebugImageFilenameExtraString( "-for-bloom" );

	m_pHBloomFilter = shared_ptr<CHorizontalBloomFilter>( new CHorizontalBloomFilter );
	m_pHBloomFilter->SetRenderTargetSize( base_plane.width + 2, base_plane.height + 2 );
	m_pHBloomFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pHBloomFilter->SetFilterShader( filter_shader_container.GetFilterShader( "Bloom" ) );
	m_pHBloomFilter->SetTextureCache( cache.GetSelfPtr().lock() );
	m_pHBloomFilter->SetDebugImageFilenameExtraString( "-for-horizontal-bloom" );

	m_pVBloomFilter = shared_ptr<CVerticalBloomFilter>( new CVerticalBloomFilter );
	m_pVBloomFilter->SetRenderTargetSize( base_plane.width,     base_plane.height );
	m_pVBloomFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pVBloomFilter->SetFilterShader( filter_shader_container.GetFilterShader( "Bloom" ) );
	m_pVBloomFilter->SetTextureCache( cache.GetSelfPtr().lock() );
	m_pVBloomFilter->SetDebugImageFilenameExtraString( "-for-vertical-bloom" );

	m_pGaussianBlurFilter->AddNextFilter( m_pHBloomFilter );
	m_pHBloomFilter->AddNextFilter( m_pVBloomFilter );

	// last filter of this interface filter
	m_pLastFilter = m_pVBloomFilter;

	return Result::SUCCESS;
}



void CCombinedBloomFilter::RenderBase( CPostProcessEffectFilter& prev_filter )
{
	return m_pGaussianBlurFilter->RenderBase( prev_filter );
}


void CCombinedBloomFilter::UseAsGaussianBlurFilter( bool use_as_gauss_blur )
{
//	float fGaussianWeightsSum = 1.976f;
//	m_pHBloomFilter->SetBloomFactor( 1.0f / fGaussianWeightsSum );
//	m_pVBloomFilter->SetBloomFactor( 1.0f / fGaussianWeightsSum );
	m_pHBloomFilter->SetUseForBlurFilter( true );
	m_pVBloomFilter->SetUseForBlurFilter( true );
	m_pHBloomFilter->SetDebugImageFilenameExtraString( "-for-horizontal-blur" );
	m_pVBloomFilter->SetDebugImageFilenameExtraString( "-for-vertical-blur" );
}


void CCombinedBloomFilter::SetBlurStrength( float strength )
{
	m_pHBloomFilter->SetDeviation( strength );
	m_pVBloomFilter->SetDeviation( strength );
}


/*
void CCombinedBloomFilter::AddNextFilter( boost::shared_ptr<CPostProcessEffectFilter> pFilter )
{
	m_vecpNextFilter.push_back( pFilter );
}
*/

void GetLuminanceTextureDesc( CTextureResourceDesc& dest )
{
	LPDIRECT3D9 pD3D = DIRECT3D9.GetD3D();
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    D3DCAPS9 caps;
	D3DFORMAT luminance_format = D3DFMT_R16F;
    D3DDISPLAYMODE display_mode;
    pd3dDevice->GetDeviceCaps( &caps );
    pd3dDevice->GetDisplayMode( 0, &display_mode );
    // IsDeviceAcceptable already ensured that one of D3DFMT_R16F or D3DFMT_R32F is available.
    if( FAILED( pD3D->CheckDeviceFormat( caps.AdapterOrdinal, caps.DeviceType,
                                         display_mode.Format, D3DUSAGE_RENDERTARGET,
                                         D3DRTYPE_TEXTURE, D3DFMT_R16F ) ) )
        luminance_format = D3DFMT_R32F;
    else
        luminance_format = D3DFMT_R16F;

	dest.Format = FromD3DSurfaceFormat( luminance_format );
	dest.UsageFlags = UsageFlag::RENDER_TARGET;
}



//=======================================================================
// CLuminanceCalcFilter
//=======================================================================

CLuminanceCalcFilter::CLuminanceCalcFilter( const std::string& technique_name, int num_samples, int render_target_size )
{
	m_Technique.SetTechniqueName( technique_name.c_str() );
	m_NumSamples = num_samples;
	m_RenderTargetSize = render_target_size;

//	CTextureResourceDesc tex_desc;
	GetLuminanceTextureDesc( m_Desc );
	m_Desc.Width  = m_RenderTargetSize;
	m_Desc.Height = m_RenderTargetSize;
}


Result::Name CLuminanceCalcFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	Result::Name res = Result::SUCCESS;
	if( m_pCache->GetNumTextures( m_Desc ) == 0 )
		res = m_pCache->AddTexture( m_Desc );

	return res;
}


bool CLuminanceCalcFilter::GetRenderTarget( CPostProcessEffectFilter& prev_filter, boost::shared_ptr<CRenderTargetTextureHolder>& pDest )
{
	m_pDest = m_pCache->GetTexture( m_Desc );

	if( m_pDest )
		return true;
	else
		return false;
}


void CLuminanceCalcFilter::GetSampleOffsets_DownScale3x3( int width, int height, Vector2 avSampleOffsets[] )
{
    if( NULL == avSampleOffsets )
        return;

	// Initialize the sample offsets for the initial luminance pass.
	float tU, tV;
	tU = 1.0f / ( 3.0f * width );
	tV = 1.0f / ( 3.0f * height );

	int index = 0;
	int x = 0, y = 0;
	for( x = -1; x <= 1; x++ )
	{
		for( y = -1; y <= 1; y++ )
		{
			avSampleOffsets[index].x = x * tU;
			avSampleOffsets[index].y = y * tV;

			index++;
		}
	}
}


//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_DownScale4x4
// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
//       pixel shader.
//-----------------------------------------------------------------------------
void CLuminanceCalcFilter::GetSampleOffsets_DownScale4x4( int width, int height, Vector2 avSampleOffsets[] )
{
    if( NULL == avSampleOffsets )
        return;

    float tU = 1.0f / (float)width;
    float tV = 1.0f / (float)height;

    // Sample from the 16 surrounding points. Since the center point will be in
    // the exact center of 16 texels, a 0.5f offset is needed to specify a texel
    // center.
    int index = 0;
    for( int y = 0; y < 4; y++ )
    {
        for( int x = 0; x < 4; x++ )
        {
            avSampleOffsets[ index ].x = ( x - 1.5f ) * tU;
            avSampleOffsets[ index ].y = ( y - 1.5f ) * tV;

            index++;
        }
    }
}


void CLuminanceCalcFilter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);
	HRESULT hr = S_OK;

	Vector2 avSampleOffsets[MAX_SAMPLES];
	memset( avSampleOffsets, 0, sizeof(avSampleOffsets) );

	int w = m_pDest->m_Desc.Width;
	int h = m_pDest->m_Desc.Height;

	switch( m_NumSamples )
	{
	case 9:  GetSampleOffsets_DownScale3x3( w, h, avSampleOffsets ); break;
	case 16: GetSampleOffsets_DownScale4x4( w, h, avSampleOffsets ); break;
	default:
		break;
	}

	hr = pEffect->SetValue( "g_avSampleOffsets", avSampleOffsets, sizeof( avSampleOffsets ) );

//	pd3dDevice->SetRenderTarget( 0, apSurfToneMap[dwCurTexture] );
//	pd3dDevice->SetTexture( 0, g_pTexSceneScaled );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	RenderFullScreenQuad( pEffect, 0.0f, 0.0f, 1.0f, 1.0f );
}



//=======================================================================
// CAdaptationCalcFilter
//=======================================================================

CAdaptationCalcFilter::CAdaptationCalcFilter()
:
m_fElapsedTime(0.01666666667f),
m_fLuminanceAdaptationRate(0.02f)
{
	m_Technique.SetTechniqueName( "CalculateAdaptedLum" );
}


CAdaptationCalcFilter::~CAdaptationCalcFilter()
{
	if( m_pTexAdaptedLuminanceLast )
		m_pTexAdaptedLuminanceLast->DecrementLockCount();

	if( m_pTexAdaptedLuminanceCur )
		m_pTexAdaptedLuminanceCur->DecrementLockCount();
}


Result::Name CAdaptationCalcFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	m_Desc.Width  = 1;
	m_Desc.Height = 1;
	GetLuminanceTextureDesc( m_Desc );

	int num_cached_texes = cache.GetNumTextures( m_Desc );

//	shared_ptr<CRenderTargetTextureHolder> apHolder[2];
	for( int i=num_cached_texes; i<2; i++ )
	{
		cache.AddTexture( m_Desc );
	}

	m_pTexAdaptedLuminanceLast = cache.GetTexture( m_Desc );
	m_pTexAdaptedLuminanceCur  = cache.GetTexture( m_Desc );

	// Lock at this init phase and always keep them.

	if( m_pTexAdaptedLuminanceLast )
		m_pTexAdaptedLuminanceLast->IncrementLockCount();
	else
		LOG_PRINT_ERROR( "Failed to get texture for storing adapted luminance (last)." );

	if( m_pTexAdaptedLuminanceCur )
		m_pTexAdaptedLuminanceCur->IncrementLockCount();
	else
		LOG_PRINT_ERROR( "Failed to get texture for storing adapted luminance (current)." );

	return Result::SUCCESS;
}


/**
 Increment the user's adapted luminance
*/
void CAdaptationCalcFilter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;
//	UINT uiPass, uiPassCount;

	// Swap current & last luminance
	shared_ptr<CRenderTargetTextureHolder> pTexSwap = m_pTexAdaptedLuminanceLast;
	m_pTexAdaptedLuminanceLast = m_pTexAdaptedLuminanceCur;
	m_pTexAdaptedLuminanceCur = pTexSwap;

	m_pDest = m_pTexAdaptedLuminanceCur;
	hr = m_pDest->m_Texture.GetTexture()->GetSurfaceLevel( 0, &(m_pDest->m_pTexSurf) );

	/// increment the lock count to avoid decrement it to be zero-ed by the next filter
	m_pDest->IncrementLockCount();

//	PDIRECT3DSURFACE9 pSurfAdaptedLum = NULL;
//	V( m_pTexAdaptedLuminanceCur->GetSurfaceLevel( 0, &pSurfAdaptedLum ) );

	// This simulates the light adaptation that occurs when moving from a 
	// dark area to a bright area, or vice versa. The g_pTexAdaptedLuminance
	// texture stores a single texel cooresponding to the user's adapted 
	// level.
//	pEffect->SetTechnique( "CalculateAdaptedLum" );
	pEffect->SetFloat( "g_fElapsedTime", m_fElapsedTime );
	pEffect->SetFloat( "g_fElapsedTime", m_fLuminanceAdaptationRate );

//	hr = pd3dDevice->SetRenderTarget( 0, pSurfAdaptedLum );
//	hr = pd3dDevice->SetRenderTarget( 0, m_pTexAdaptedLuminanceCur->m_pTexSurf );
	hr = pd3dDevice->SetRenderTarget( 0, m_pDest->m_pTexSurf );
	hr = pd3dDevice->SetTexture( 0, m_pTexAdaptedLuminanceLast->m_Texture.GetTexture() );
//	hr = pd3dDevice->SetTexture( 1, g_apTexToneMap[0] );
	hr = pd3dDevice->SetTexture( 1, m_pPrevScene->m_Texture.GetTexture() );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );

	RenderFullScreenQuad( pEffect, 0.0f, 0.0f, 1.0f, 1.0f );

//	SAFE_RELEASE( pSurfAdaptedLum );
//	return S_OK;
}



//====================================================================================
// CHDRLightingFinalPassFilter
//====================================================================================

CHDRLightingFinalPassFilter::CHDRLightingFinalPassFilter()
:
m_fKeyValue(0.5f),
m_ToneMappingEnabled(true),
m_StarEffectEnabled(false)
{
	m_Technique.SetTechniqueName( "FinalScenePass" );

	// Create a small black texture which is used when the star effect is off
	CTextureResourceDesc desc;
	desc.Width  = 16;
	desc.Height = 16;
	desc.Format = TextureFormat::A8R8G8B8;
	desc.pLoader = shared_ptr<CSignleColorTextureFilling>( new CSignleColorTextureFilling( SFloatRGBAColor::Black() ) );
	m_BlancTextureForDisabledStarEffect.Load( desc );
}


bool CHDRLightingFinalPassFilter::IsReadyToRender()
{
	if( m_pBloom
	 && ( m_pStar || !m_StarEffectEnabled )
	 && m_pAdaptedLuminance
	 && m_pPrevResult )
		return true;
	else
		return false;
}


void CHDRLightingFinalPassFilter::StorePrevFilterResults( CPostProcessEffectFilter& prev_filter )
{
	switch( prev_filter.GetFilterType() )
	{
//	case CPostProcessEffectFilter::TYPE_COMBINED_BLOOM:
	case CPostProcessEffectFilter::TYPE_VERTICAL_BLOOM: // last filter of CCombinedBloomFilter
		m_pBloom = prev_filter.GetDestRenderTarget();
		break;
	case CPostProcessEffectFilter::TYPE_STAR:
		m_pStar = prev_filter.GetDestRenderTarget();
		break;
	case CPostProcessEffectFilter::TYPE_ADAPTATION_CALC:
		m_pAdaptedLuminance = prev_filter.GetDestRenderTarget();
		break;
	default:
		// none of the above - assume this is the prev scene
		m_pPrevResult = prev_filter.GetDestRenderTarget();
		break;
	}
}


void CHDRLightingFinalPassFilter::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;

//	m_pLumCalcFilter->RenderBase();

//	m_pBrightPassFilter->SetAvarageLuminanceTexture( m_pLumCalcFilter->GetAverageLuminanceTexture() );

//	m_pBrightPassFilter->RenderBase();

	// Draw the high dynamic range scene texture to the low dynamic range
	// back buffer. As part of this final pass, the scene will be tone-mapped
	// using the user's current adapted luminance, blue shift will occur
	// if the scene is determined to be very dark, and the post-process lighting
	// effect textures will be added to the scene.

//	V( pEffect->SetTechnique( "FinalScenePass" ) );
	V( pEffect->SetFloat( "g_fMiddleGray", m_fKeyValue ) );
	V( pEffect->SetBool( "g_bEnableToneMap", m_ToneMappingEnabled ) );

//	V( pd3dDevice->SetRenderTarget( 0, pSurfLDR ) );
	V( pd3dDevice->SetTexture( 0, m_pPrevResult->m_Texture.GetTexture() ) );
	V( pd3dDevice->SetTexture( 1, m_pBloom->m_Texture.GetTexture() ) );
	V( pd3dDevice->SetTexture( 3, m_pAdaptedLuminance->m_Texture.GetTexture() ) );

	if( m_pStar )
	{
		V( pd3dDevice->SetTexture( 2, m_pStar->m_Texture.GetTexture() ) );
	}
	else
	{
		V( pd3dDevice->SetTexture( 2, m_BlancTextureForDisabledStarEffect.GetTexture() ) );
	}

	V( pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );
	V( pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );
	V( pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
	V( pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
	V( pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ) );
	V( pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR ) );
	V( pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) );
	V( pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT ) );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	RenderFullScreenQuad( pEffect, 0.0f, 0.0f, 1.0f, 1.0f );

	V( pd3dDevice->SetTexture( 1, NULL ) );
	V( pd3dDevice->SetTexture( 2, NULL ) );
	V( pd3dDevice->SetTexture( 3, NULL ) );

	m_pBloom->DecrementLockCount();
	m_pAdaptedLuminance->DecrementLockCount();

	if( m_pStar )
		m_pStar->DecrementLockCount();

	m_pBloom = shared_ptr<CRenderTargetTextureHolder>();
	m_pStar = shared_ptr<CRenderTargetTextureHolder>();
	m_pAdaptedLuminance = shared_ptr<CRenderTargetTextureHolder>();
}



//=======================================================================
// CHDRLightingFilter
//=======================================================================

CHDRLightingFilter::CHDRLightingFilter()
:
//m_fKeyValue(0)
m_EnableStarFilter(false)
{
}


Result::Name CHDRLightingFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	// first, calculate the luminance
//	m_vecpChildFilter.push_back( m_pLumCalcFilter );

	// bloom and star effect
//	m_vecpChildFilter.push_back( m_pBrightPassFilter );

	Result::Name res = Result::SUCCESS;

	SRectangular cbb = GetCropWidthAndHeight();

	m_pDownScale4x4Filter = shared_ptr<CDownScale4x4Filter>( new CDownScale4x4Filter );
	m_pDownScale4x4Filter->SetRenderTargetSize( cbb.width / 4, cbb.height / 4 );
	m_pDownScale4x4Filter->SetRenderTargetSurfaceFormat( TextureFormat::A16R16G16B16F );
	m_pDownScale4x4Filter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pDownScale4x4Filter->Init( cache, filter_shader_container );

	m_pBrightPassFilter = shared_ptr<CHDRBrightPassFilter>( new CHDRBrightPassFilter );
	m_pBrightPassFilter->SetRenderTargetSize( cbb.width / 4 + 2, cbb.height / 4 + 2 );
	m_pBrightPassFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pBrightPassFilter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pBrightPassFilter->Init( cache, filter_shader_container );
//	m_pBrightPassFilter->SetExtraTexelBorderWidth( 1 );

	m_pGaussianBlurFilter = shared_ptr<CGaussianBlurFilter>( new CGaussianBlurFilter );
	m_pGaussianBlurFilter->SetRenderTargetSize( cbb.width / 4 + 2, cbb.height / 4 + 2 );
	m_pGaussianBlurFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pGaussianBlurFilter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pGaussianBlurFilter->Init( cache, filter_shader_container );
	m_pGaussianBlurFilter->SetDebugImageFilenameExtraString( "-for-hdrl" );
//	m_pGaussianBlurFilter->Init();
//	m_pDownScale2x2Filter->SetExtraTexelBorderWidth( 1 );

	m_pDownScale2x2Filter = shared_ptr<CDownScale2x2Filter>( new CDownScale2x2Filter );
	m_pDownScale2x2Filter->SetRenderTargetSize( cbb.width / 8 + 2, cbb.height / 8 + 2 );
	m_pDownScale2x2Filter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pDownScale2x2Filter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pDownScale2x2Filter->Init( cache, filter_shader_container );
//	m_pDownScale2x2Filter->SetExtraTexelBorderWidth( 1 );

	if( m_EnableStarFilter )
	{
		m_pStarFilter = shared_ptr<CStarFilter>( new CStarFilter );
	}

	m_apLumCalcFilter[0] = shared_ptr<CLuminanceCalcFilter>( new CLuminanceCalcFilter( "SampleAvgLum",  9, 64 ) );

	m_apLumCalcFilter[1] = shared_ptr<CLuminanceCalcFilter>( new CLuminanceCalcFilter( "ResampleAvgLum", 16, 16 ) );

	m_apLumCalcFilter[2] = shared_ptr<CLuminanceCalcFilter>( new CLuminanceCalcFilter( "ResampleAvgLum", 16,  4 ) );

	m_apLumCalcFilter[3] = shared_ptr<CLuminanceCalcFilter>( new CLuminanceCalcFilter( "ResampleAvgLumExp", 16,  1 ) );

	for( int i=0; i<NUM_TONEMAP_TEXTURES; i++ )
	{
		m_apLumCalcFilter[i]->Init( cache, filter_shader_container );
		m_apLumCalcFilter[i]->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	}

	m_pAdaptationCalcFilter = shared_ptr<CAdaptationCalcFilter>( new CAdaptationCalcFilter() );
	m_pAdaptationCalcFilter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pAdaptationCalcFilter->Init( cache, filter_shader_container );

	m_pBloomFilter = shared_ptr<CCombinedBloomFilter>( new CCombinedBloomFilter );
	res = m_pBloomFilter->Init( cache, filter_shader_container );

//	m_pHorizontalBloomFilter = shared_ptr<CDownScale2x2Filter>( new CDownScale2x2Filter );
//	m_pVerticalBloomFilter   = shared_ptr<CDownScale2x2Filter>( new CDownScale2x2Filter );

	m_pFinalPassFilter = shared_ptr<CHDRLightingFinalPassFilter>( new CHDRLightingFinalPassFilter );
//	m_pFinalPassFilter->SetRenderTargetSize( cbb.width / 4 + 2, cbb.height / 4 + 2 );
	m_pFinalPassFilter->SetRenderTargetSize( cbb.width, cbb.height );
	m_pFinalPassFilter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pFinalPassFilter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pFinalPassFilter->Init( cache, filter_shader_container );

	//
	// create textures to use as render targets of filters
	//

	cbb = GetCropWidthAndHeight();
	CTextureResourceDesc tex_desc;
	tex_desc.UsageFlags = UsageFlag::RENDER_TARGET;

	// for 4x4 scaled down scene
	tex_desc.Width  = cbb.width  / 4;
	tex_desc.Height = cbb.height / 4;
	tex_desc.Format = TextureFormat::A16R16G16B16F;
	if( m_pCache->GetNumTextures( tex_desc ) == 0 )
		m_pCache->AddTexture( tex_desc );

	// for bright pass and gaussian blur
	tex_desc.Width  = cbb.width  / 4 + 2;
	tex_desc.Height = cbb.height / 4 + 2;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	int num = m_pCache->GetNumTextures( tex_desc );
	for( int i=num; i<2; i++ )
		m_pCache->AddTexture( tex_desc );

	// for combined bloom filter
	tex_desc.Width  = cbb.width  / 8;
	tex_desc.Height = cbb.height / 8;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	if( m_pCache->GetNumTextures( tex_desc ) == 0 )
		m_pCache->AddTexture( tex_desc );

	tex_desc.Width  = cbb.width  / 8 + 2;
	tex_desc.Height = cbb.height / 8 + 2;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	num = m_pCache->GetNumTextures( tex_desc );
	for( int i=num; i<3; i++ )
		m_pCache->AddTexture( tex_desc );

	// for final pass filter
	// - need this when there are subsequent filters after this HDR lighting filter
	tex_desc.Width  = cbb.width;
	tex_desc.Height = cbb.height;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	if( m_pCache->GetNumTextures( tex_desc ) == 0 )
		m_pCache->AddTexture( tex_desc );


	//
	// set up filter lists
	//

	m_apLumCalcFilter[0]->AddNextFilter( m_apLumCalcFilter[1] );
	m_apLumCalcFilter[1]->AddNextFilter( m_apLumCalcFilter[2] );
	m_apLumCalcFilter[2]->AddNextFilter( m_apLumCalcFilter[3] );
	m_apLumCalcFilter[3]->AddNextFilter( m_pAdaptationCalcFilter );
	m_pAdaptationCalcFilter->AddNextFilter( m_pFinalPassFilter );

	// preprocessing for bloom and star
	m_pDownScale4x4Filter->AddNextFilter( m_pBrightPassFilter );
	m_pBrightPassFilter->AddNextFilter( m_pGaussianBlurFilter );

	// bloom filters
	m_pGaussianBlurFilter->AddNextFilter( m_pDownScale2x2Filter );
	m_pDownScale2x2Filter->AddNextFilter( m_pBloomFilter );
	m_pBloomFilter->AddNextFilter( m_pFinalPassFilter );

	// star filters
	if( m_pStarFilter )
	{
		m_pGaussianBlurFilter->AddNextFilter( m_pStarFilter );
		m_pStarFilter->AddNextFilter( m_pFinalPassFilter );
	}

	m_pLastFilter = m_pFinalPassFilter;

	// set shader params
	shared_ptr<CPostProcessFilterShader> pShader = filter_shader_container.GetFilterShader( "HDRPostProcessor" );
	if( pShader && pShader->GetShader().GetShaderManager() )
	{
		CShaderManager *pShaderMgr = pShader->GetShader().GetShaderManager();
		CShaderParameter< vector<float> > bloom_scale( "g_fBloomScale" ), star_scale( "g_fStarScale" );
		bloom_scale.Parameter().resize(1);
		star_scale.Parameter().resize(1);
		bloom_scale.Parameter()[0] = 1.0f;
		star_scale.Parameter()[0] = 0.5f;
		pShaderMgr->SetParam( bloom_scale );
		pShaderMgr->SetParam( star_scale );
	}

	return Result::SUCCESS;
}


void CHDRLightingFilter::RenderBase( CPostProcessEffectFilter& prev_filter )
{
	m_apLumCalcFilter[0]->RenderBase( prev_filter );

	m_pBrightPassFilter->SetAdaptedLuminanceTexture( m_pAdaptationCalcFilter->GetAdaptedLuminanceTexture() );

	// scale down by 4x4 -> bright pass -> gaussian blur
	//   -> scale down by 2x2 -> bloom -> final HDR lighting pass
	//   -> star -> final HDR lighting pass
	m_pDownScale4x4Filter->RenderBase( prev_filter );

	m_pFinalPassFilter->RenderBase( prev_filter );
}



//============================================================================
// CFullScreenBlurFilter
//============================================================================

CFullScreenBlurFilter::CFullScreenBlurFilter()
:
m_fBlurStrength(1.0f)
{
}


Result::Name CFullScreenBlurFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	Result::Name res;
	const SRectangular cbb = GetCropWidthAndHeight();

	m_pDownScale4x4Filter = shared_ptr<CDownScale4x4Filter>( new CDownScale4x4Filter );
	m_pDownScale4x4Filter->SetRenderTargetSize( cbb.width / 4, cbb.height / 4 );
	m_pDownScale4x4Filter->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
	m_pDownScale4x4Filter->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
	res = m_pDownScale4x4Filter->Init( cache, filter_shader_container );
	m_pDownScale4x4Filter->SetDebugImageFilenameExtraString( "-for-gaussblur" );
/*
	for( int i=0; i<2; i++ )
	{
		m_apHorizontalBloomFilter[i] = shared_ptr<CHorizontalBloomFilter>( new CHorizontalBloomFilter );
		m_apHorizontalBloomFilter[i]->SetRenderTargetSize( cbb.width / 4, cbb.height / 4 );
		m_apHorizontalBloomFilter[i]->SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
		m_apHorizontalBloomFilter[i]->SetFilterShader( filter_shader_container.GetFilterShader( "HDRPostProcessor" ) );
		m_apHorizontalBloomFilter[i]->SetTextureCache( cache.GetSelfPtr().lock() );
	}
	res = m_pDownScale4x4Filter->Init( cache, filter_shader_container );
*/

	m_pBloomFilter = shared_ptr<CCombinedBloomFilter>( new CCombinedBloomFilter );
	m_pBloomFilter->SetBasePlane( SRectangular( cbb.width / 4, cbb.height / 4 ) );
	res = m_pBloomFilter->Init( cache, filter_shader_container );
	m_pBloomFilter->UseAsGaussianBlurFilter( true );

	CTextureResourceDesc tex_desc;
	tex_desc.Width  = cbb.width  / 4;
	tex_desc.Height = cbb.height / 4;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	tex_desc.UsageFlags = UsageFlag::RENDER_TARGET;
	int num = m_pCache->GetNumTextures( tex_desc );
	for( int i=num; i<2; i++ )
		m_pCache->AddTexture( tex_desc );

	tex_desc.Width  = cbb.width  / 4 + 2;
	tex_desc.Height = cbb.height / 4 + 2;
	tex_desc.Format = TextureFormat::A8R8G8B8;
	tex_desc.UsageFlags = UsageFlag::RENDER_TARGET;
	num = m_pCache->GetNumTextures( tex_desc );
	for( int i=num; i<2; i++ )
		m_pCache->AddTexture( tex_desc );

//	m_pDownScale4x4Filter->AddNextFilter( m_pBloomFilter );

	m_pLastFilter = m_pBloomFilter;

	// test the 4x4 down scale filter
	m_pLastFilter = m_pDownScale4x4Filter;

	return Result::SUCCESS;
}


void CFullScreenBlurFilter::RenderBase( CPostProcessEffectFilter& prev_filter )
{
	m_pBloomFilter->SetBlurStrength( m_fBlurStrength );

	m_pDownScale4x4Filter->RenderBase( prev_filter );

//	m_pBloomFilter->RenderBase( prev_filter );
}



//============================================================================
// CMonochromeColorFilter
//============================================================================

CMonochromeColorFilter::CMonochromeColorFilter()
{
	m_Technique.SetTechniqueName( "MonochromeColor" );

	// set the render target size to that of back buffer by default
	const SRectangular cbb = GetCropWidthAndHeight();
	SetRenderTargetSize( cbb.width, cbb.height );

	SetRenderTargetSurfaceFormat( TextureFormat::A8R8G8B8 );
}


Result::Name CMonochromeColorFilter::Init( CRenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();
	SetFilterShader( filter_shader_container.GetShader( "HDRPostProcessor" ) );

	return Result::SUCCESS;
}


void CMonochromeColorFilter::Render()
{
	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXEFFECT pEffect = GetD3DXEffect(*this);

	HRESULT hr = S_OK;
/*
	D3DSURFACE_DESC desc;
	m_pPrevScene->m_Texture.GetTexture()->GetLevelDesc( 0, &desc );

	// Get the rectangle describing the sampled portion of the source texture.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectSrc;
//	GetTextureRect( m_pPrevScene->m_Texture.GetTexture(), &rectSrc );
	GetTextureRect( m_pPrevScene, &rectSrc );
	InflateRect( &rectSrc, -1, -1 );

	// Get the destination rectangle.
	// Decrease the rectangle to adjust for the single pixel black border.
	RECT rectDest;
//	GetTextureRect( m_pDest->m_Texture.GetTexture(), &rectDest );
	GetTextureRect( m_pDest, &rectDest );
	InflateRect( &rectDest, -1, -1 );

	// Get the correct texture coordinates to apply to the rendered quad in order 
	// to sample from the source rectangle and render into the destination rectangle
	CoordRect coords;
	GetTextureCoords( m_pPrevScene->m_Texture, &rectSrc, m_pDest->m_Texture, &rectDest, &coords );
*/
	CoordRect coords;
	coords.fLeftU   = 0.0f;
	coords.fTopV    = 0.0f;
	coords.fRightU  = 1.0f;
	coords.fBottomV = 1.0f;

	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
//	hr = pd3dDevice->SetScissorRect( &rectDest );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

	RenderFullScreenQuad( pEffect, coords );

	hr = pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	return;
}
