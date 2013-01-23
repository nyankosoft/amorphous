#include "PostProcessEffectFilter.hpp"
#include "PostProcessEffectManager.hpp"
#include "Graphics/SurfaceFormat.hpp"
#include "Graphics/Direct3D/D3DSurfaceFormat.hpp"
#include "Graphics/GraphicsResourceDescs.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using std::string;
using std::vector;
using namespace boost;


/// Does not consider the current lock count
int RenderTargetTextureCache::GetNumTextures( const CTextureResourceDesc& desc )
{
	int num_matches = 0;
	size_t i=0;
	for( i=0; i<m_vecpHolder.size(); i++ )
	{
		CTextureResourceDesc& current = m_vecpHolder[i]->m_Desc;
		if( current.Width     == desc.Width
		 && current.Height    == desc.Height
		 && current.Format    == desc.Format
		 && current.MipLevels == desc.MipLevels )
		{
			num_matches++;
		}
	}

	return num_matches;
}


Result::Name RenderTargetTextureCache::AddTexture( const CTextureResourceDesc& desc )
{
	TextureHandle new_tex;
	bool created = new_tex.Load( desc );
	if( !created )
		return Result::UNKNOWN_ERROR;

	shared_ptr<CRenderTargetTextureHolder> pHolder( new CRenderTargetTextureHolder );
	m_vecpHolder.push_back( pHolder );
	m_vecpHolder.back()->m_Desc    = desc;
	m_vecpHolder.back()->m_Texture = new_tex;

	// debug - check the texture which was just created.
	shared_ptr<CTextureResource> pTex = new_tex.GetEntry()->GetTextureResource();
	D3DSURFACE_DESC surf_desc;
	if( pTex && pTex->GetTexture() )
	{
		pTex->GetTexture()->GetLevelDesc( 0, &surf_desc );
	}

	return Result::SUCCESS;
}


Result::Name RenderTargetTextureCache::AddTexture( int width, int height, TextureFormat::Format format )
{
	CTextureResourceDesc tex_desc;
	tex_desc.Width     = width;
	tex_desc.Height    = height;
	tex_desc.Format    = format;
	tex_desc.MipLevels = 1;
	tex_desc.UsageFlags = UsageFlag::RENDER_TARGET;
	return AddTexture( tex_desc );
}


boost::shared_ptr<CRenderTargetTextureHolder> RenderTargetTextureCache::GetTexture( const CTextureResourceDesc& desc )
{
	size_t i=0;
	for( i=0; i<m_vecpHolder.size(); i++ )
	{
		const CTextureResourceDesc& current = m_vecpHolder[i]->m_Desc;
		if( current.Width     == desc.Width
		 && current.Height    == desc.Height
		 && current.Format    == desc.Format
		 && current.MipLevels == desc.MipLevels
		 && m_vecpHolder[i]->GetLockCount() == 0 )
		{
			return m_vecpHolder[i];
		}
	}

	return boost::shared_ptr<CRenderTargetTextureHolder>();
}



//============================================================================
// PostProcessEffectFilter
//============================================================================


int PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame = 0;


//bool PostProcessEffectFilter::GetRenderTarget( PostProcessEffectFilter& prev_filter, boost::shared_ptr<CRenderTargetTextureHolder>& pDest )
bool PostProcessEffectFilter::GetRenderTarget( PostProcessEffectFilter& prev_filter )
{
/*	int width  = prev_filter.m_pDest->m_Desc.Width;
	int height = prev_filter.m_pDest->m_Desc.Height;

	if( m_UseTextureSizeDivisibleBy8 )
	{
		width  = width  - width  % 8;
		height = height - height % 8;
	}

	m_Desc.Width  = (int)(width  * m_fScalingFactor) + m_ExtraTexelBorderWidth * 2;
	m_Desc.Height = (int)(height * m_fScalingFactor) + m_ExtraTexelBorderWidth * 2;
*/
	m_pDest = m_pCache->GetTexture( m_Desc );

	if( m_pDest )
	{
		m_pDest->IncrementLockCount();
//		for( size_t i=0; i<m_vecpNextFilter.size(); i++ )
//			m_pDest->IncrementLockCount();
		return true;
	}
	else
		return false;
}


/// default implementation - use the current texture desc to specify the render target
//Result::Name PostProcessEffectFilter::SetRenderTarget( PostProcessEffectFilter& prev_filter, boost::shared_ptr<CRenderTargetTextureHolder>& pDest )
Result::Name PostProcessEffectFilter::SetRenderTarget( PostProcessEffectFilter& prev_filter )
{
	HRESULT hr = S_OK;
	if( 0 < m_vecpNextFilter.size() )
	{
		// Find a texture render target currently available, and set it to m_pDest
		/*bool found = */GetRenderTarget( prev_filter );
//		if( !found )
//			return Result::UNKNOWN_ERROR;


		if( m_pDest )
		{
			// Note that this increases the internal reference count of m_pDest->m_pTexSurf
			hr = m_pDest->m_Texture.GetTexture()->GetSurfaceLevel( 0, &(m_pDest->m_pTexSurf) );

			ULONG ref_count = 0;
			if( FAILED( hr ) )
				return Result::UNKNOWN_ERROR;	// return DXUT_ERR( L"GetSurfaceLevel", hr );
			else
				ref_count = m_pDest->m_pTexSurf->Release(); // Decrement the reference count

			hr = DIRECT3D9.GetDevice()->SetRenderTarget( 0, m_pDest->m_pTexSurf );

			// set viewport
			D3DSURFACE_DESC surf_desc;
			hr = m_pDest->m_pTexSurf->GetDesc( &surf_desc );
			D3DVIEWPORT9 viewport = { 0, 0, surf_desc.Width, surf_desc.Height, 0.0f, 1.0f };
			hr = DIRECT3D9.GetDevice()->SetViewport( &viewport );
		}
		else
		{
			// Do not treat this as error
			// - adaptation calc filter sets the render target in Render()
			return Result::SUCCESS;
		}
	}
	else
	{
		// The last filter - render to the original render target

/*		hr = m_SceneRenderTarget.GetTexture()->GetSurfaceLevel( 0, &pTexSurf );
		if( FAILED( hr ) )
			return false;	// return DXUT_ERR( L"GetSurfaceLevel", hr );

		DIRECT3D9.GetDevice()->SetRenderTarget( 0, pTexSurf );
*/
		hr = DIRECT3D9.GetDevice()->SetRenderTarget( 0, m_pCache->m_pOrigRenderTarget );
	}

	return SUCCEEDED(hr) ? Result::SUCCESS : Result::UNKNOWN_ERROR;
}


void PostProcessEffectFilter::StorePrevFilterResults( PostProcessEffectFilter& prev_filter )
{
}


Result::Name PostProcessEffectFilter::Init( RenderTargetTextureCache& cache, CFilterShaderContainer& filter_shader_container )
{
	m_pCache = cache.GetSelfPtr().lock();

	return Result::SUCCESS;
}


void PostProcessEffectFilter::RenderBase( PostProcessEffectFilter& prev_filter )
{
//	RenderChildFilters();

	m_pPrevScene = prev_filter.m_pDest;

	// Store the scene of the previous filter
	// - Used when the filter has more than 1 input sources
	StorePrevFilterResults( prev_filter );


	// See if the filter has all the necessary input textures
	// - This is used when the filter has more than 1 input sources
	if( !IsReadyToRender() )
		return;

	if( !m_pFilterShader )
		return;

	IDirect3DDevice9* pd3dDevice = DIRECT3D9.GetDevice();

	// find render target
//	bool found = GetRenderTarget( prev_filter, m_pDest );
//	if( !found )
//		LOG_PRINT_ERROR( " Could not find render target." );

	Result::Name res = SetRenderTarget( prev_filter );
	if( res != Result::SUCCESS )
		return;

	CShaderManager *pShaderMgr = m_pFilterShader->GetShader().GetShaderManager();
	if( !pShaderMgr )
		return;

	res = pShaderMgr->SetTechnique( m_Technique ); // set in RenderBase()
	if( res != Result::SUCCESS )
		int failed_to_set_technique = 1;

	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTexture = m_pPrevScene->m_Texture.GetTexture();
	if( !pTexture )
		int texture_is_not_loaded = 1;

	hr = pd3dDevice->SetTexture( 0, pTexture );
	if( FAILED(hr) )
		int failed_to_set_texture = 1;
	hr = pd3dDevice->SetTexture( 1, pTexture );

	hr = pShaderMgr->GetEffect()->CommitChanges();

	if( GetDebugImageFilenameExtraString() == "-for-gaussblur" )
	{
		Render();
/*		static TextureHandle test_tex;
		static bool tex_loaded = false;
		ONCE( tex_loaded = test_tex.Load( "debug/post-process_effect/test_tex.png" ) );
		C2DRect test_rect( 0, 0, 199, 149, SFloatRGBAColor::White().GetARGB32() );
		test_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		pd3dDevice->SetVertexShader( NULL );
		pd3dDevice->SetPixelShader( NULL );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		test_rect.draw();
//		test_rect.draw( m_pPrevScene->m_Texture );
//		test_rect.Draw();
//		test_rect.Draw( test_tex );
		if( 0 < PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame )
			m_pPrevScene->m_Texture.SaveTextureToImageFile( "debug/post-process_effect/prev_scene_of_gauss_blur.png" );
*/	}
	else
		Render();

	if( 0 < PostProcessEffectFilter::ms_SaveFilterResultsAtThisFrame && m_pDest )
	{
		SaveProcessedSceneToImageFile();
	}

///	if( m_pDest )
///		m_pDest->ReleaseSurface();
//	m_pDest = shared_ptr<CRenderTargetTextureHolder>();

	m_pPrevScene->DecrementLockCount();
	m_pPrevScene = shared_ptr<CRenderTargetTextureHolder>();

	size_t i=0;
	for( i=0; i<m_vecpNextFilter.size(); i++ )
	{
		m_vecpNextFilter[i]->RenderBase( *this );
	}
}


void PostProcessEffectFilter::SaveProcessedSceneToImageFile()
{
//	D3DXIMAGE_FILEFORMAT img_fmt;
	string ext;
/*	if( m_pDest->m_Desc.Format == TextureFormat::A8R8G8B8)
	{*/
//		img_fmt = D3DXIFF_PNG;
		ext = ".png";
/*	}
	else
	{
		img_fmt = D3DXIFF_PFM;
		ext = ".pfm";
	}*/

//	boost::filesystem::create_directories( "debug/post-process_effect" ); // Done in PostProcessEffectManager::RenderPostProcessEffects()
	string image_pathname = "debug/post-process_effect/filter-" + string(m_Technique.GetTechniqueName()) + GetDebugImageFilenameExtraString() + ext;
	bool saved = m_pDest->m_Texture.SaveTextureToImageFile( image_pathname );
//	hr = D3DXSaveTextureToFile( img_fmt, m_pDest->m_Texture.GetTexture(), NULL );
}


} // namespace amorphous
