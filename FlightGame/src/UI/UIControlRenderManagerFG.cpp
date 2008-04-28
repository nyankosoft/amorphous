
#include "UIControlRenderManagerFG.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Font.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/2DFrameRect.h"

#include "Support/memory_helpers.h"
#include "Support/StringAux.h"
#include "Support/msgbox.h"


CUIControlRenderManagerFG::CUIControlRenderManagerFG()
{
	// TODO: check if the texture handle work under internal frees&news of vector
	m_vecTextureCache.reserve(8);

	m_DefaultRenderParam.m_vecTexRectElement.resize( 1 );

//	m_DefaultRenderParam.m_vecTexRectElement[0].m_TextureIndex = 0;
    CGM_BlendColor& blend_color = m_DefaultRenderParam.m_vecTexRectElement[0].m_Color;
	blend_color.m_aColor[CGM_Control::STATE_NORMAL]			= SFloatRGBAColor( 0.8f, 0.8f, 0.8f, 0.80f );
    blend_color.m_aColor[CGM_Control::STATE_DISABLED]		= SFloatRGBAColor( 0.3f, 0.3f, 0.3f, 0.65f );
    blend_color.m_aColor[CGM_Control::STATE_HIDDEN]			= SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 0.55f );
    blend_color.m_aColor[CGM_Control::STATE_FOCUS]			= SFloatRGBAColor( 0.2f, 1.0f, 0.2f, 0.85f );
    blend_color.m_aColor[CGM_Control::STATE_MOUSEOVER]		= SFloatRGBAColor( 0.2f, 0.7f, 0.2f, 0.85f );
    blend_color.m_aColor[CGM_Control::STATE_PRESSED]		= SFloatRGBAColor( 0.6f, 1.0f, 0.6f, 0.85f );
    blend_color.m_aColor[CGM_Control::STATE_SUBDIALOGOPEN]	= SFloatRGBAColor( 0.5f, 0.9f, 0.5f, 0.85f );
    blend_color.m_CurrentColor					= blend_color.m_aColor[CGM_Control::STATE_NORMAL];

	m_DefaultRenderParam.m_Font.m_Color = blend_color;

	m_fColorBlendRate = 0.2f;

	m_fElapsedTime = 0.02f;

	m_RectBorderWidth = 2;

	m_vecControlRenderParam.reserve(16);
}


CUIControlRenderManagerFG::~CUIControlRenderManagerFG()
{
}


//============================= init routine for each control =============================
/*
CControlRenderParam& CUIControlRenderManagerFG::InitRenderParamBase( CGM_Control& ctrl )
{
	CControlRenderParam param = m_DefaultRenderParam;

	const SRect& rect = ctrl.GetBoundingBox();
	param.m_vecTexRectElement[0].m_Rect = rect;

	ctrl.m_UserIndex = (int)(m_vecControlRenderParam.size());

	m_vecControlRenderParam.push_back( param );

	return m_vecControlRenderParam.back();
}*/


/*
void CUIControlRenderManagerFG::InitRenderParamStatic( CGM_Static& static_ctrl )
{
	CGM_ControlRenderMgr_Default::InitRenderParamStatic( static_ctrl );
//	CControlRenderParam& param = InitRenderParamBase( static_ctrl );
//	param.m_Text = static_ctrl.GetText();
//	SetTextOffset( param );
}


void CUIControlRenderManagerFG::InitRenderParamButton( CGM_Button& button )
{
	CGM_ControlRenderMgr_Default::InitRenderParamButton( button );
//	CControlRenderParam& param = InitRenderParamBase( button );
//	param.m_Text = button.GetText();
//	SetTextOffset( param );
}


void CUIControlRenderManagerFG::InitRenderParamCheckBox( CGM_CheckBox& checkbox )
{
	CGM_ControlRenderMgr_Default::InitRenderParamCheckBox

//	InitRenderParamButton( checkbox );

	// no center alignment for checkbox titles
//	m_vecControlRenderParam.back().m_vTextOffset = D3DXVECTOR2( 0,0 );
}


void CUIControlRenderManagerFG::InitRenderParamRadioButton( CGM_RadioButton& radio_button )
{
	InitRenderParamButton( radio_button );

	// no center align for radio button titles
	m_vecControlRenderParam.back().m_vTextOffset = D3DXVECTOR2( 0,0 );
}


void CUIControlRenderManagerFG::InitRenderParamSlider( CGM_Slider& slider )
{
}


void CUIControlRenderManagerFG::InitRenderParamSubDialogButton( CGM_SubDialogButton& subdlg_button )
{
	InitRenderParamButton( subdlg_button );
}


void CUIControlRenderManagerFG::InitRenderParamDialogCloseButton( CGM_DialogCloseButton& button )
{
	InitRenderParamButton( button );
}


void CUIControlRenderManagerFG::InitRenderParamListBox( CGM_ListBox& listbox )
{
	CControlRenderParam& param = InitRenderParamBase( listbox );
}


void CUIControlRenderManagerFG::InitRenderParamDialog( CGM_Dialog& dialog )
{
	CControlRenderParam param = m_DefaultRenderParam;

	const SRect& dlg_rect = dialog.GetBoundingBox();
	param.m_vecTexRectElement[0].m_Rect = dlg_rect;

	dialog.m_UserIndex = (int)(m_vecControlRenderParam.size());

	m_vecControlRenderParam.push_back( param );
}



void CUIControlRenderManagerFG::BlendFontColor( CGM_Control* pControl )
{
	CGM_FontElement& font_element = GetRenderParam( pControl->m_UserIndex ).m_Font;
	font_element.m_Color.Blend( pControl->GetState(), m_fElapsedTime, m_fColorBlendRate );
}*/

/*
void CUIControlRenderManagerFG::DrawDialogTitle( CGM_Dialog& dialog )
{
	CFontBase *pFont = GetFont( 0 );
	const string& title = dialog.GetTitle();

	const SRect& src_rect = dialog.GetBoundingBox();
	D3DXVECTOR2 vTextPos;
	vTextPos.x = (float)src_rect.left + (src_rect.GetWidth() - title.length() * pFont->GetFontWidth()) * 0.5f;
	vTextPos.y = (float)src_rect.top;

	SetDefaultTextureStageStates();

	DWORD dialog_title_color = 0xFF20FF20;
	pFont->SetFontColor( dialog_title_color );
///	pFont->DrawText( title.c_str(), vTextPos );

	pFont->DrawText( fmt_string( "%d,%d,%d,%d", src_rect.left, src_rect.top, src_rect.right, src_rect.bottom ).c_str(),
		vTextPos, 0xFF00C0C0 );

	const SRect& rend_rect = GetRenderParam( dialog.m_UserIndex ).m_vecTexRectElement[0].m_Rect;
	pFont->DrawText( fmt_string( "%d,%d,%d,%d", rend_rect.left, rend_rect.top, rend_rect.right, rend_rect.bottom ).c_str(),
		vTextPos + D3DXVECTOR2(-1,-1), 0xFFFFFF00 );
}*/


//============================= render routine for each control =============================



void CUIControlRenderManagerFG::RenderStatic( CGM_Static& static_ctrl )
{
	unsigned int state = static_ctrl.GetState();

	CControlRenderParam& render_param = GetRenderParam( static_ctrl.m_UserIndex );

	CGM_FontElement& font_element = render_param.m_Font;
	font_element.m_Color.Blend( state, m_fElapsedTime, m_fColorBlendRate );

	SetDefaultAlphaBlend();
	SetDefaultTextureStageStates();

	const SRect& rect = static_ctrl.GetBoundingBox();
	D3DXVECTOR2 vTextPos = D3DXVECTOR2((float)rect.left, (float)rect.top);
	CFontBase *pFont = GetFont(font_element.m_FontIndex);
	pFont->SetFontColor( font_element.m_Color.m_CurrentColor.GetARGB32() );
//	render_param.m_vTextOffset.x = 0;
	pFont->DrawText( static_ctrl.GetText().c_str(), vTextPos + render_param.m_vTextOffset );

//	D3DXVECTOR2 pos = render_param.m_vTextOffset;
/*	pFont->DrawText( fmt_string( "%df, %d, %df, %d", rect.left, rect.top, rect.right, rect.bottom ).c_str(),
		vTextPos + render_param.m_vTextOffset, 0xFF0000FF );
	const SRect& rend_rect = render_param.m_vecTexRectElement[0].m_Rect;
	pFont->DrawText( fmt_string( "%df, %d, %df, %d", rend_rect.left, rend_rect.top, rend_rect.right, rend_rect.bottom ).c_str(),
		vTextPos + render_param.m_vTextOffset + D3DXVECTOR2(-1,-1), 0xFFFF0000 );
*/
}


void CUIControlRenderManagerFG::RenderButton( CGM_Button& button )
{
	unsigned int state = button.GetState();

	CControlRenderParam& render_param = GetRenderParam( button.m_UserIndex );

	CGM_TextureRectElement& tex_rect = render_param.m_vecTexRectElement[0];

	tex_rect.m_Color.Blend( state, m_fElapsedTime, m_fColorBlendRate );
	SFloatRGBAColor& src_color = tex_rect.m_Color.m_CurrentColor;

	SRect& src_rect = tex_rect.m_Rect;
	C2DRect rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );

	C2DFrameRect frame_rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );
	frame_rect.SetBorderWidth( m_RectBorderWidth );
	frame_rect.SetColor( src_color.GetARGB32() );

//	rect.SetColor( src_color.GetARGB32() );
	rect.SetColor( 0x80000000 );

	int tex_index = tex_rect.m_TextureIndex;
	if( 0 <= tex_index )
        rect.Draw( GetTexture(tex_index).GetTexture() );
	else
		rect.Draw();

	frame_rect.Draw();

	int d = m_RectBorderWidth * 2;
	SRect deflated_rect = src_rect;
	deflated_rect.Inflate( -d, -d );
	C2DRect top_rect( deflated_rect, 0x60909090 );
//	C2DRect top_rect( deflated_rect, 0xFFFF0000 );
	top_rect.SetColor( 0x60404040, 2 );
	top_rect.SetColor( 0x60404040, 3 );
	top_rect.Draw();

	CGM_FontElement& font_element = render_param.m_Font;
	font_element.m_Color.Blend( state, m_fElapsedTime, m_fColorBlendRate );

	SetDefaultAlphaBlend();
	SetDefaultTextureStageStates();

	CFontBase *pFont = GetFont(font_element.m_FontIndex);
	pFont->SetFontColor( font_element.m_Color.m_CurrentColor.GetARGB32() );
	pFont->DrawText( button.GetText().c_str(), rect.GetPosition2D(0) + render_param.m_vTextOffset );

	// draw label for the button

}


void CUIControlRenderManagerFG::RenderCheckBox( CGM_CheckBox& checkbox )
{
	CGM_FontElement& font_element = GetRenderParam(checkbox.m_UserIndex).m_Font;

    BlendFontColor( &checkbox );

	CFontBase *pFont = GetFont(font_element.m_FontIndex);

	const SRect& src_rect = checkbox.GetBoundingBox();
	D3DXVECTOR2 vTextPos = D3DXVECTOR2( (float)(src_rect.left), (float)(src_rect.top) );

	string text;
	if( checkbox.IsChecked() )
		text = "[x] " + checkbox.GetText();
	else
		text = "[ ] " + checkbox.GetText();

	SetDefaultAlphaBlend();
	SetDefaultTextureStageStates();

	pFont->SetFontColor( font_element.m_Color.m_CurrentColor.GetARGB32() );
	pFont->DrawText( text.c_str(), vTextPos );
}


void CUIControlRenderManagerFG::RenderRadioButton( CGM_RadioButton& radio_button )
{
	CGM_FontElement& font_element = GetRenderParam(radio_button.m_UserIndex).m_Font;

    BlendFontColor( &radio_button );

	CFontBase *pFont = GetFont(font_element.m_FontIndex);

	const SRect& src_rect = radio_button.GetBoundingBox();
	D3DXVECTOR2 vTextPos = D3DXVECTOR2( (float)(src_rect.left), (float)(src_rect.top) );

	string text;
	if( radio_button.IsChecked() )
		text = "(x) " + radio_button.GetText();
	else
		text = "( ) " + radio_button.GetText();

	SetDefaultAlphaBlend();
	SetDefaultTextureStageStates();
	pFont->SetFontColor( font_element.m_Color.m_CurrentColor.GetARGB32() );
	pFont->DrawText( text.c_str(), vTextPos );
}


void CUIControlRenderManagerFG::RenderDialogCloseButton( CGM_DialogCloseButton& button )
{
	RenderButton( button );
}


void CUIControlRenderManagerFG::RenderSlider( CGM_Slider& slider )
{
}


void CUIControlRenderManagerFG::RenderListBox( CGM_ListBox& listbox )
{
	CGM_FontElement& font_element = GetRenderParam(listbox.m_UserIndex).m_Font;
    BlendFontColor( &listbox );
	CFontBase *pFont = GetFont(font_element.m_FontIndex);

	SetDefaultAlphaBlend();
	SetDefaultTextureStageStates();

	const SRect text_rect = listbox.GetTextRect();
	const int text_height = listbox.GetTextHeight();
	size_t i, num_items = listbox.GetNumItems();
	C2DRect rect;
	rect.SetPositionLTRB( text_rect.left, 0, text_rect.right, 0 );
	rect.SetColor( 0x35F0F0F0 );
	for( i=0; i<num_items; i++ )
	{
		int sepbar_y = text_rect.top + (i+1) * text_height;
		rect.SetPositionLTRB( text_rect.left, sepbar_y, text_rect.right, sepbar_y + 1 );
		rect.Draw();

		CGM_ListBoxItem& item = *listbox.GetItem(i);
		const string& text = item.text;
//		pFont->SetFontColor( font_element.m_Color.m_CurrentColor.GetARGB32() );

		SetDefaultAlphaBlend();
		SetDefaultTextureStageStates();
		pFont->SetFontColor( 0xFFF0F0F0 );
		pFont->DrawText( text.c_str(), text_rect.left, text_rect.top + text_height * i );
	}

	int selected_index = listbox.GetSelectedIndex();
	rect.SetColor( 0x3020E020 );
	rect.SetPositionLTRB(
		text_rect.left,		text_rect.top + selected_index * text_height,
		text_rect.right,	text_rect.top + (selected_index+1) * text_height );
	rect.Draw();

	if( listbox.HasFocus() )
		pFont->SetFontColor( 0xF0FF1010 );
	pFont->DrawText( to_string(selected_index).c_str(), text_rect.left, text_rect.top + text_height * selected_index );

	if( listbox.HasFocus() )
	{
		CFontBase* pDescFont = GetFont( 3 );
		CGM_ListBoxItem* pSelected = listbox.GetSelectedItem();

		if( pDescFont && pSelected )
		{
			float scale = 1.0f;
			D3DXVECTOR2 pos = D3DXVECTOR2( 90, 530 ) * scale;

			pDescFont->DrawText( pSelected->desc.c_str(), pos );
		}
	}
}


void CUIControlRenderManagerFG::RenderSubDialogButton( CGM_SubDialogButton& subdlg_button )
{
	RenderButton( subdlg_button );
}


void CUIControlRenderManagerFG::RenderDialog( CGM_Dialog& dialog )
{
//	CheckScope( "UIRenderMgr::RenderDialog()" );

	CControlRenderParam& render_param = GetRenderParam( dialog.m_UserIndex );

	CGM_TextureRectElement& tex_rect = render_param.m_vecTexRectElement[0];

	// render background rect
	SRect& src_rect = tex_rect.m_Rect;
	C2DRect rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );
	C2DFrameRect frame_rect( src_rect.left, src_rect.top, src_rect.right, src_rect.bottom );

	const SFloatRGBAColor src_color = SFloatRGBAColor( 0.0, 0.0, 0.0, 0.5 ); //tex_rect.m_Color.m_CurrentColor;
	rect.SetColor( src_color.GetARGB32() );

	frame_rect.SetBorderWidth( m_RectBorderWidth );
	frame_rect.SetColor( 0xE010F010 );

	int tex_index = tex_rect.m_TextureIndex;
	if( 0 <= tex_index )
        rect.Draw( GetTexture(tex_index).GetTexture() );
	else
		rect.Draw();

	frame_rect.Draw();

//	if( 0 < dialog.GetTitle().length() )
		DrawDialogTitle( dialog );
}


/*
void CUIControlRenderManagerFG::RenderCaption( const string& strCaption )
{
}


void CUIControlRenderManagerFG::ChangeScale( float factor )
{
	CGM_ControlRenderMgr_Default::ChangeScale( factor );
}
*/