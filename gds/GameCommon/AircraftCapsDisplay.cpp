
#include "AircraftCapsDisplay.h"
#include "Item/GI_Aircraft.h"

#include "Support/memory_helpers.h"
#include "Support/msgbox.h"

#include "Graphics/2DPrimitive/2DRect.h"
#include "Graphics/2DPrimitive/2DFrameRect.h"


CAircraftCapsDisplay::CAircraftCapsDisplay()
:
m_pAircraft(NULL)
{
	for( int i=0; i<NUM_CAPS; i++ )
	{
		m_vecCapsVal[i].SetZeroState();
		m_vecCapsVal[i].vel = 10.0f;
		m_vecCapsVal[i].smooth_time = 0.2f;
	}

	m_pAmmoDesc = NULL;
	m_pAmmoName = NULL;
}


CAircraftCapsDisplay::~CAircraftCapsDisplay()
{
}


void CAircraftCapsDisplay::DrawDesc()
{
/*	CGI_Aircraft& aircraft = *m_pAircraft;

	int lang_id = GetCurrentLanguageID();
	CFont& font = ;

	SPoint pos = m_vPos[NAME];
	font.DrawText( aircraft.GetName().c_str(), pos.x, pos.y );

	const string& desc_text = aircraft.GetDesc( lang_id );

	pos = m_vPos[DESC];
	font.DrawText( desc_text.c_str(), pos.x, pos.y );
*/
}


void CAircraftCapsDisplay::UpdateCaps()
{

	m_vecCapsVal[SPEED].target = 0.5f;//m_pAircraft->GetStealthiness();

	m_vecCapsVal[MANEUVERABILITY].target = m_pAircraft->GetManeuverability();

	m_vecCapsVal[DURABILITY].target = m_pAircraft->GetArmorScaled();

	m_vecCapsVal[AIR_TO_AIR].target = 0.5f;//m_pAircraft->GetStealthiness();

	m_vecCapsVal[AIR_TO_GROUND].target = 0.5f;//m_pAircraft->GetStealthiness();

	m_vecCapsVal[STEALTHINESS].target = m_pAircraft->GetStealthiness();

}

/*
void CAircraftCapsDisplay::Update( float dt )
{
	CGM_ControlRendererManager::Update( dt );

//	for(  )
//	{
		for( int i=0; i<NUM_PERFS; i++ )
			m_vecCapsVal[i].Update( dt );
//	}
}


void CAircraftCapsDisplay::Render()
{
	CGM_ControlRendererManager::Render();
}
*/

CGM_ControlRenderer *CAircraftCapsDisplay::CreateStaticRenderer( CGM_Static *pStatic )
{
	return CGM_StdControlRendererManager::CreateStaticRenderer( pStatic );
}

/*
void CAircraftCapsDisplay::InitRenderParamStatic( CGM_Static& static_ctrl )
{
	CGM_ControlRenderMgr_Default::InitRenderParamStatic( static_ctrl );

	int id = static_ctrl.GetID();
	if( CAircraftCaps::CID_PERF_SPEED <= id && id <= CAircraftCaps::CID_PERF_STEALTHINESS )
	{
		CControlRenderParam& param = GetRenderParam( static_ctrl.m_UserIndex );
//		CControlRenderParam& param = ControlRenderParam().back();

		param.m_Font.m_FontIndex = 1;
		param.m_TextLayoutH = CControlRenderParam::TL_LEFT;
		SetTextOffset( param );
	}

	if( static_ctrl.GetText() == "AmmoName" ) m_pAmmoName = &static_ctrl;
	if( static_ctrl.GetText().find( "AmmoDesc" ) == 0 ) m_pAmmoDesc = &static_ctrl;

	if( static_ctrl.GetText().find("Desc") != string::npos
	 || static_ctrl.GetText().find("Name") != string::npos )
	{
		CControlRenderParam& param = GetRenderParam( static_ctrl.m_UserIndex );

		if( static_ctrl.GetText().find("Desc") != string::npos )
			param.m_Font.m_FontIndex = 3;

		param.m_TextLayoutH = CControlRenderParam::TL_LEFT;
		SetTextOffset( param );
	}
}
*/
/*
void CAircraftCapsDisplay::RenderStatic( CGM_Static& static_ctrl )
{
	int id = static_ctrl.GetID();
	if( CAircraftCaps::CID_PERF_SPEED <= id && id <= CAircraftCaps::CID_PERF_STEALTHINESS )
	{
		// performance items - draw perf bars
		const int offset = CAircraftCaps::CID_PERF_SPEED;
		int index = id - offset;
		const SRect& rect = static_ctrl.GetBoundingBox();
		C2DRect bar;
		bar.SetPositionLTWH(
			rect.left,
			rect.top,
//			rect.GetWidth(),
			(int)(rect.GetWidth() * m_vecCapsVal[index].current),
			rect.GetHeight() );

//		SetAdditiveAlphaBlend();

///		bar.SetDestAlphaBlendMode( D3DBLEND_ONE );
		bar.SetDestAlphaBlendMode( AlphaBlend::One );
		bar.SetColor( 0x803030F0 );
		bar.Draw();

//		SetDefaultAlphaBlend();
	}

	if( static_ctrl.GetText().find("bg:") == 0 )
	{
		// render background rect just like dialog
		CControlRenderParam& render_param = GetRenderParam( static_ctrl.m_UserIndex );
		CGM_TextureRectElement& tex_rect = render_param.m_vecTexRectElement[0];

		// render background rect
		SRect& src_rect = tex_rect.m_Rect;
		C2DRect rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );
		C2DFrameRect frame_rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );

		const SFloatRGBAColor src_color = SFloatRGBAColor( 0.0, 0.0, 0.0, 0.5 ); //tex_rect.m_Color.m_CurrentColor;
		rect.SetColor( src_color.GetARGB32() );

		frame_rect.SetBorderWidth( GetRectBorderWidth() );
		frame_rect.SetColor( 0xFF10FF10 );

		int tex_index = tex_rect.m_TextureIndex;
		if( 0 <= tex_index )
			rect.Draw( GetTexture(tex_index).GetTexture() );
		else
			rect.Draw();

		frame_rect.Draw();

//		DrawDialogTitle( dialog );

		// do not run the usual render routine
		return;
	}

	// render title
	CGM_ControlRenderMgr_Default::RenderStatic( static_ctrl );
}
*/
