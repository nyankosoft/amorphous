#include "GM_StdControlRenderer.h"
#include "GM_Control.h"
#include "GM_Dialog.h"
#include "GM_DialogManager.h"
#include "GM_ControlRendererManager.h"

#include "3DCommon/GraphicsEffectManager.h"

#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"

using namespace std;

/*
class CGraphicsElementHandle
{
public:
};


class CRectElementHandle
{
	CGE_Rect *m_pRect;

public:
};


class CTextElementHandle
{
	CGE_Text *m_pText;

public:

	CGE_Text *operator ->() { return m_pText; };
};
*/

CGM_StdControlRenderer::CGM_StdControlRenderer()
:
m_DialogFadeColorIndex(STD_FADE_COLOR_INDEX)
{
	m_aColor[CGM_Control::STATE_NORMAL]			= SFloatRGBAColor( 0.8f, 0.8f, 0.8f, 1.0f );
    m_aColor[CGM_Control::STATE_DISABLED]		= SFloatRGBAColor( 0.3f, 0.3f, 0.3f, 1.0f );
    m_aColor[CGM_Control::STATE_HIDDEN]			= SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 1.0f );
    m_aColor[CGM_Control::STATE_FOCUS]			= SFloatRGBAColor( 0.2f, 1.0f, 0.2f, 1.0f );
    m_aColor[CGM_Control::STATE_MOUSEOVER]		= SFloatRGBAColor( 0.2f, 0.7f, 0.2f, 1.0f );
    m_aColor[CGM_Control::STATE_PRESSED]		= SFloatRGBAColor( 0.6f, 1.0f, 0.6f, 1.0f );
    m_aColor[CGM_Control::STATE_SUBDIALOGOPEN]	= SFloatRGBAColor( 0.5f, 0.9f, 0.5f, 1.0f );
}


void CGM_StdControlRenderer::OnGroupElementCreated()
{
	m_pGroupElement->SetAlpha( STD_FADE_COLOR_INDEX, 0.0f );
}


void CGM_StdControlRenderer::OnPressed( CGM_Button& button )
{
	ChangeColorToCurrentState();
}

void CGM_StdControlRenderer::OnReleased( CGM_Button& button )
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnChecked( CGM_CheckBox& checkbox )
{
}

void CGM_StdControlRenderer::OnCheckCleared( CGM_CheckBox& checkbox )
{
}


void CGM_StdControlRenderer::OnFocused( CGM_Control& control )
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnFocusCleared( CGM_Control& control )
{
	g_Log.Print( "%s", __FUNCTION__ );

	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnMouseCursorEntered( CGM_Control& control )
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnMouseCursorLeft( CGM_Control& control )
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnItemSelectionFocusCreated( CGM_ListBox& listbox )
{
}

void CGM_StdControlRenderer::OnItemSelectionChanged( CGM_ListBox& listbox )
{
}

void CGM_StdControlRenderer::OnItemSelected( CGM_ListBox& listbox )
{
}


void CGM_StdControlRenderer::OnSliderValueChanged( CGM_Slider& slider )
{
}


void CGM_StdControlRenderer::OnDialogOpened( CGM_Dialog& dialog )
{
}

void CGM_StdControlRenderer::OnDialogClosed( CGM_Dialog& dialog )
{
}

void CGM_StdControlRenderer::OnOpenDialogAttemptedToClose( CGM_Dialog& dialog )
{
}


void CGM_StdControlRenderer::OnParentSubDialogButtonFocused()
{
}

void CGM_StdControlRenderer::OnParentSubDialogButtonFocusCleared()
{
}


void CGM_StdControlRenderer::ChangeColorToCurrentState()
{
	CGM_Control *pControl = GetControl();

	if( !pControl )
		return;

	if( !pControl->GetOwnerDialog()->IsOpen() )
		return;

	int color_index = 0;
	int state = pControl->GetState();
	const SFloatRGBAColor& dest_color = m_aColor[ state ];

	string title = "";
	if( GetStatic() )
		title = "of '" + GetStatic()->GetText() + "'";

	LOG_PRINT( fmt_string("changing color %s - state: %d", title.c_str(), state ) );

	double shift_time;
	if( state == CGM_Control::STATE_FOCUS
	 || state == CGM_Control::STATE_MOUSEOVER )
	{
		shift_time = 0.1; // change color immediately
	}
	else
	{
		shift_time = 0.5;
	}

	m_pGraphicsEffectManager->SetTimeOffset();
	size_t i, num_colored_elements = m_vecColoredElement.size();
	for( i=0; i<num_colored_elements; i++)
	{
		// cancel previous color changer effect
		// - prevent new effect from getting overridden by previous effects
		m_pGraphicsEffectManager->CancelEffect( m_vecColoredElement[i].m_PrevEffect );

		// do not change alpha
		// - avoid conflicts with fade in/out effect of dialog
		m_vecColoredElement[i].m_PrevEffect
			= m_pGraphicsEffectManager->ChangeColorTo( m_vecColoredElement[i].m_pElement, 0.0, shift_time, color_index, dest_color.GetRGBColor(), 0 );
	}
}


void CGM_StdStaticRenderer::Init( CGM_Static& static_control )
{
	int font_id = 0;
	int x = static_control.GetBoundingBox().left;
	int y = static_control.GetBoundingBox().top;
	int w = 16;//m_pGraphicsElementManager->GetFont(font_id)->GetFontWidth();// 8;
	int h = 32;//m_pGraphicsElementManager->GetFont(font_id)->GetFontHeight();// 16;
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];

//	m_pText = m_pGraphicsElementManager->CreateText( 0, static_control.GetText(), x, y, normal_color, w, h );
	m_pText = m_pGraphicsElementManager->CreateTextBox( 0, static_control.GetText(), static_control.GetLocalRect(),
		CGE_Text::TAL_CENTER, CGE_Text::TAL_CENTER, normal_color, w, h );

	// render the text on top
	RegisterGraphicsElement( 0, m_pText );

	RegisterColoredElement( m_pText );
}


void CGM_StdButtonRenderer::Init( CGM_Button& button )
{
	CGM_StdStaticRenderer::Init( button );

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect      = m_pGraphicsElementManager->CreateRect(      button.GetLocalRect(), bg_color );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( button.GetLocalRect(), normal_color, 2 );

	// register elements
	// - set local layer offset to determine rendering order
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );

	// register elements that chages colors depending on states
	RegisterColoredElement( m_pFrameRect );
}


void CGM_StdCheckBoxRenderer::Init( CGM_CheckBox& checkbox )
{
	CGM_StdButtonRenderer::Init( checkbox );

	m_BaseTitle = m_pText->GetText();

	m_pText->SetText( ( checkbox.IsChecked() ? " [x] " : " [ ] " ) + m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CGM_StdCheckBoxRenderer::OnChecked( CGM_CheckBox& checkbox )
{
	m_pText->SetText( " [x] " + m_BaseTitle );
}

void CGM_StdCheckBoxRenderer::OnCheckCleared( CGM_CheckBox& checkbox )
{
	m_pText->SetText( " [ ] " + m_BaseTitle );
}


void CGM_StdRadioButtonRenderer::Init( CGM_RadioButton& radiobutton )
{
	CGM_StdButtonRenderer::Init( radiobutton );

	m_BaseTitle = m_pText->GetText();

	m_pText->SetText( ( radiobutton.IsChecked() ? " (x) " : " ( ) " ) + m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CGM_StdRadioButtonRenderer::OnChecked( CGM_RadioButton& radiobutton )
{
	m_pText->SetText( " (x) " + m_BaseTitle );
}

void CGM_StdRadioButtonRenderer::OnCheckCleared( CGM_RadioButton& radiobutton )
{
	m_pText->SetText( " ( ) " + m_BaseTitle );
}


void CGM_StdListBoxRenderer::Init( CGM_ListBox& listbox )
{
//	CGM_ControlRenderer::Init();

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);

	m_pRect      = m_pGraphicsElementManager->CreateRect(      listbox.GetLocalRect(), bg_color );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( listbox.GetLocalRect(), normal_color, 2 );

	// create empty text elements
	int x = listbox.GetLocalRect().left;
	int y = listbox.GetLocalRect().top;
	int text_x = x;
	int text_y = y;
//	int w = 8;
//	int h = 16;
	int i, num_items_in_page = listbox.GetScrollbar()->GetPageSize();
	m_vecpText.resize( num_items_in_page );
	for( i=0; i<num_items_in_page; i++ )
	{
		text_x = x + 5;
		text_y = y + i * listbox.GetTextHeight();
		m_vecpText[i] = m_pGraphicsElementManager->CreateText( 0, "-", (float)text_x, (float)text_y, normal_color );
	}

	// start with 4 to render the scrollbar on top
	int local_layer_offset = 4;

	// set local layer offset to determine rendering order
	int num_text_elements = (int)m_vecpText.size();
	for( i=0; i<num_text_elements; i++ )
		RegisterGraphicsElement( local_layer_offset, m_vecpText[i] );
	RegisterGraphicsElement( local_layer_offset,   m_pFrameRect );
	RegisterGraphicsElement( local_layer_offset+1, m_pRect );

	// register elements that changes colors depending on states
	RegisterColoredElement( m_pFrameRect );
//	for( i=0; i<num_text_elements; i++ )
//		RegisterColoredElement( m_vecpText[i] );

	// not visible by default
	int dlg_color_index = m_DialogFadeColorIndex;
	m_pFrameRect->SetAlpha( dlg_color_index, 0 );
	m_pRect->SetAlpha( dlg_color_index, 0 );
	for( i=0; i<num_text_elements; i++ )
		m_vecpText[i]->SetAlpha( dlg_color_index, 0 );

}

/*
void CGM_StdListBoxRenderer::SetColorToTextElement( CGE_Text& rTextElement, int state )
{
}
*/


// update all the text of all the text elements
void CGM_StdListBoxRenderer::UpdateItems( CGM_ListBox& listbox, bool update_text )
{
	update_text = true; // For now, just always update the texts//The user may have scrolled the page

	const int focused_item_index = listbox.GetSelectedIndex();

	int i;
	const int num_items_to_display = listbox.GetNumItemsToDisplay();
	const int first_item_index = listbox.GetIndexOfFirstItemToDisplay();

	if( update_text )
	{
		for( i=0; i<num_items_to_display; i++ )
		{
			CGM_ListBoxItem *pItem = listbox.GetItem( first_item_index + i );
			if( pItem )
				m_vecpText[i]->SetText( pItem->GetText() );
		}
	}

	int color_index = 0;
	for( i=0; i<num_items_to_display; i++ )
	{
		if( first_item_index + i == focused_item_index )
			m_vecpText[i]->SetColor( color_index, m_aColor[CGM_Control::STATE_FOCUS] );
		else
			m_vecpText[i]->SetColor( color_index, m_aColor[CGM_Control::STATE_NORMAL] );
	}
}


// update all the text of all the text elements
void CGM_StdListBoxRenderer::OnItemSelectionChanged( CGM_ListBox& listbox )
{
	bool update_text = false;
	UpdateItems( listbox, update_text );
}


void CGM_StdListBoxRenderer::OnItemSelected( CGM_ListBox& listbox )
{
	int text_index = listbox.GetFocusedItemLocalIndexInCurrentPage();
	if( text_index < 0 || (int)m_vecpText.size() <= text_index )
		return;

	int color_index = 0;
	m_vecpText[text_index]->SetColor( color_index, m_aColor[CGM_Control::STATE_PRESSED] );
//	m_pGraphicsEffectManager->SetTimeOffset();
//	m_pGraphicsEffectManager->ChangeColorTo( m_vecpText[text_index], 0.0, 0.2, m_aColor[CGM_Control::STATE_FOCUS], 0 );
}


void CGM_StdListBoxRenderer::OnItemAdded( CGM_ListBox& listbox, int index )
{
	UpdateItems( listbox );
}


void CGM_StdListBoxRenderer::OnItemInserted( CGM_ListBox& listbox, int index )
{
	UpdateItems( listbox );
}


void CGM_StdListBoxRenderer::OnItemRemoved( CGM_ListBox& listbox, int index )
{
	UpdateItems( listbox );
}



CGM_StdScrollBarRenderer::CGM_StdScrollBarRenderer()
:
m_pThumbGroup(NULL)
{
	for( int i=0; i<NUM_RECT_ELEMENTS; i++ )
	{
		m_apRect[i] = NULL;
		m_apFrameRect[i] = NULL;
	}

	for( int i=0; i<NUM_TRIANGLE_ELEMENTS; i++ )
		m_apTriangle[i] = NULL;
}


void CGM_StdScrollBarRenderer::Init( CGM_ScrollBar& scrollbar )
{
	CGraphicsElementManager *pElementMgr = m_pGraphicsElementManager;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);

	SRect thumb_rect = scrollbar.GetThumbButtonRect();
	const SRect src_rect[NUM_RECT_ELEMENTS] =
	{
		scrollbar.GetLocalUpButtonRectInOwnerDialogCoord(),
		scrollbar.GetLocalDownButtonRectInOwnerDialogCoord(),
		scrollbar.GetLocalTrackRectInOwnerDialogCoord(),
		RectLTWH( 0, 0, thumb_rect.GetWidth(), thumb_rect.GetHeight() ) // thumb rect elements are grouped by a group element
	};

	for( int i=0; i<NUM_RECT_ELEMENTS; i++ )
	{
		m_apFrameRect[i] = pElementMgr->CreateFrameRect( src_rect[i], normal_color, 2 );
		m_apRect[i]      = pElementMgr->CreateRect(      src_rect[i], bg_color );

		RegisterColoredElement( m_apFrameRect[i] );
	}

	// make the thumb rect brighter than the other so that it can be distinguished
	m_apRect[RE_THUMB]->SetColor( 0, SFloatRGBAColor(0.5f,0.5f,0.5f,0.7f) );

	// register all except for the thumb rects
	for( int i=0; i<NUM_RECT_ELEMENTS - 1; i++ )
	{
		RegisterGraphicsElement( 2, m_apFrameRect[i] );
		RegisterGraphicsElement( 3, m_apRect[i] );
	}

	// craete triangles for arrows on the up/down buttons
	SRect arrow_src_rect[2] = { src_rect[RE_UP_BUTTON], src_rect[RE_DOWN_BUTTON] };
	C2DTriangle::Direction arrow_dir[2] = { C2DTriangle::DIR_UP, C2DTriangle::DIR_DOWN };
	for( int i=0; i<NUM_TRIANGLE_ELEMENTS; i++ )
	{
		SRect arrow_rect = arrow_src_rect[i];
		arrow_rect.Inflate( -6, -6 );
		m_apTriangle[i] = pElementMgr->CreateTriangle( arrow_dir[i], arrow_rect, normal_color );

		RegisterColoredElement( m_apTriangle[i] );
		RegisterGraphicsElement( 2, m_apTriangle[i] );

/*		CGE_Rect *pRect = pElementMgr->CreateRect( arrow_rect, normal_color );
		RegisterColoredElement( pRect );
		RegisterGraphicsElement( 2, pRect );*/
	}

	// register thumb rects
	// - rendered on top of other elements
	// - create a subgroup
	// - local origin: top-left corner of the rect in local coord of the owner dialog (owner dialog coord)
	CGraphicsElement *apElement[] = { m_apFrameRect[RE_THUMB], m_apRect[RE_THUMB] };
	SRect thumb_rect_in_owner_dlg_coord = scrollbar.GetLocalThumbButtonRectInOwnerDialogCoord();
	m_pThumbGroup = pElementMgr->CreateGroup( apElement, numof(apElement), thumb_rect_in_owner_dlg_coord.GetTopLeftCorner() );

	RegisterGraphicsElementToParentDialog( m_pThumbGroup );

	SetLocalLayerOffset( 0, m_apFrameRect[RE_THUMB] );
	SetLocalLayerOffset( 1, m_apRect[RE_THUMB] );
}


void CGM_StdScrollBarRenderer::OnThumbUpdated( CGM_ScrollBar& scrollbar )
{
	SRect thumb_rect = scrollbar.GetLocalThumbButtonRectInOwnerDialogCoord();
	if( m_pThumbGroup )
	{
		m_pThumbGroup->SetLocalTopLeftPos( thumb_rect.GetTopLeftCorner() );

		SRect local_thumb_rect = RectLTWH( 0, 0, thumb_rect.GetWidth(), thumb_rect.GetHeight() );
		for( size_t i=0; i<m_pThumbGroup->GetElementBuffer().size(); i++ )
			m_pThumbGroup->GetElementBuffer()[i]->SetLocalRect( local_thumb_rect );
	}
}


void CGM_StdSliderRenderer::Init( CGM_Slider& slider )
{
//	CGM_StdStaticRenderer::Init( CGM_Static *pStatic );

	// slider frame
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect                  = m_pGraphicsElementManager->CreateRect(      slider.GetLocalRect(), bg_color );
	m_pFrameRect             = m_pGraphicsElementManager->CreateFrameRect( slider.GetLocalRect(), normal_color, 2 );

	// slider button
	SRect btn_rect = slider.GetButtonRect();
	SRect local_btn_rect = RectLTWH( 0, 0, btn_rect.GetWidth(), btn_rect.GetHeight() );
	m_pSliderButtonRect      = m_pGraphicsElementManager->CreateRect(      local_btn_rect,  bg_color );
	m_pSliderButtonFrameRect = m_pGraphicsElementManager->CreateFrameRect( local_btn_rect,  normal_color, 2 );

	SRect dot_rect = local_btn_rect;
	dot_rect.Inflate( -dot_rect.GetWidth() / 4, -dot_rect.GetHeight() / 4 );
	m_pSliderButtonDot       = m_pGraphicsElementManager->CreateRect( dot_rect, normal_color, 2 );

	const SRect slider_rect = slider.GetBoundingBox();
	const SPoint slider_topleft = slider_rect.GetTopLeftCorner(); // global
	const SPoint btn_local_topleft = slider.GetLocalButtonRectInOwnerDialogCoord().GetTopLeftCorner(); // local coord of owner dialog

	// subgroup for slider button
	vector<CGraphicsElement *> vecpButtonElement;
	vecpButtonElement.push_back( m_pSliderButtonRect );
	vecpButtonElement.push_back( m_pSliderButtonFrameRect );
	vecpButtonElement.push_back( m_pSliderButtonDot );
	m_pSliderButtonRect->SetLocalTopLeftPos( SPoint(0,0) );
	m_pSliderButtonFrameRect->SetLocalTopLeftPos( SPoint(0,0) );
	m_pSliderButtonDot->SetLocalTopLeftPos( SPoint(0,0) + dot_rect.GetTopLeftCorner() );
	m_pSliderButton = m_pGraphicsElementManager->CreateGroup( vecpButtonElement, btn_local_topleft );


	// For slider button elements, the following 2 things need to be done separately
	// 1. register to parent dialog renderer
	//    - register the subgroup of slider button elements
	// 2. set local layer offsets
	//    - set for each element

	// register the subgroup to the parent dialog renderer
	RegisterGraphicsElementToParentDialog( m_pSliderButton );

	// set different local layer offset for each grouped element
	SetLocalLayerOffset( 0, m_pSliderButtonDot );
	SetLocalLayerOffset( 0, m_pSliderButtonFrameRect );
	SetLocalLayerOffset( 1, m_pSliderButtonRect );

	// register elements and set local layer offset to determine rendering order
	// - these guys don't belong to any subgroup of the renderer so each of them can be
	//   registered with a single call of RegisterGraphicsElement()
	RegisterGraphicsElement( 2, m_pFrameRect );
	RegisterGraphicsElement( 3, m_pRect );

	// register elements that chages colors depending on states
	RegisterColoredElement( m_pFrameRect );
	RegisterColoredElement( m_pSliderButtonFrameRect );

}


void CGM_StdSliderRenderer::OnSliderValueChanged( CGM_Slider& slider )
{
/*
	const SRect& btn_rect = slider.GetButtonRect();
	Vector2 vDestPos = Vector2( (float)btn_rect.left, (float)btn_rect.top );
//	m_pGraphicsEffectManager->SetTimeOffset();
//	m_pGraphicsEffectManager->TranslateNonLinear( m_pSliderButton, 0.0f, vDestPos, Vector2( 50.0f, 0.0f ), 0.15f, 0 );

	m_pSliderButton->SetLocalTopLeftPos( vDestPos );
*/

	m_pSliderButton->SetLocalOrigin( slider.GetLocalButtonRectInOwnerDialogCoord().GetTopLeftCorner() );
}


//========================================================================
// CGM_StdDialogRenderer
//========================================================================

CGM_StdDialogRenderer::CGM_StdDialogRenderer()
:
m_pRect(NULL),
m_pFrameRect(NULL)
{
	m_bUseSlideEffects = false;
}


void CGM_StdDialogRenderer::Init( CGM_Dialog& dialog )
{
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect      = m_pGraphicsElementManager->CreateRect(      dialog.GetLocalRect(), bg_color );
//	m_pRect      = m_pGraphicsElementManager->CreateRoundRect( dialog.GetLocalRect(), bg_color, 6 );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( dialog.GetLocalRect(), normal_color, 2 );
//	m_pFrameRect = m_pGraphicsElementManager->CreateRoundFrameRect( dialog.GetLocalRect(), normal_color, 6, 6 );

	// render the frame rect on the background rect
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );
}


void CGM_StdDialogRenderer::OnDialogOpened( CGM_Dialog& dialog )
{
	if( !m_pGroupElement )
		return;

	// cancel the previous slide in/out effect
	m_pGraphicsEffectManager->CancelEffect( m_PrevSlideEffect );

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide in
	if( m_bUseSlideEffects )
	{
		const SRect& rect = dialog.GetBoundingBox();
		Vector2 vDestPos = Vector2( (float)rect.left, (float)rect.top );
		m_pGroupElement->SetTopLeftPos( vDestPos + Vector2( -50, 0 ) );
//		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vDestPos, 0, 0 );
		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateNonLinear( m_pGroupElement, 0.0f, vDestPos, Vector2( 50.0f, 0.0f ), 0.15f, 0 );
	}

	// fade in (change alpha form 0 to 1)
	m_pGroupElement->SetAlpha( m_DialogFadeColorIndex, 0.0f );
	m_pGraphicsEffectManager->ChangeAlphaTo( m_pGroupElement, 0.0f, 0.15f, m_DialogFadeColorIndex, 1.0f, 0 );
}


void CGM_StdDialogRenderer::OnDialogClosed( CGM_Dialog& dialog )
{
	if( !m_pGroupElement )
		return;

	// cancel the previous slide in/out effect
	m_pGraphicsEffectManager->CancelEffect( m_PrevSlideEffect );

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide out
	if( m_bUseSlideEffects )
	{
		const SRect& rect = dialog.GetBoundingBox();
		Vector2 vStartPos = Vector2( (float)rect.left, (float)rect.top );
		m_pGroupElement->SetTopLeftPos( vStartPos );
//		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vStartPos + Vector2( -50, 0 ), 0, 0 );
		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateNonLinear( m_pGroupElement, 0.0f, vStartPos + Vector2( -50, 0 ), -Vector2( 50.0f, 0.0f ), 0.15f, 0 );
	}

	// fade out (change alpha form 1 to 0)
	m_pGraphicsEffectManager->ChangeAlphaTo( m_pGroupElement, 0.0f, 0.15f, m_DialogFadeColorIndex, 0.0f, 0 );
}


/*
void CGM_DialogManager::RegisterCaptionRenderer( pCaptionRenderer )
{
	pCaptionRenderer(  );
	pCaptionRenderer->Init();
}
*/


void CGM_StdCaptionRenderer::InitCaptionRenderer()
{
	int font_id = 0;
	int w = 24;
	int h = 48;
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);

	SRect caption_rect = m_pGraphicsElementManager->RectAtCenterBottom( 960, 60, 90 );

	CGE_Rect *pRect = m_pGraphicsElementManager->CreateRect(      caption_rect, bg_color );

	m_pCaptionText = m_pGraphicsElementManager->CreateTextBox(
		font_id, // font id
		"",// text - set this null. update the text in OnFocusedControlChanged()
		caption_rect,
		CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER,
		SFloatRGBAColor(0.9f,0.9f,0.9f,1.0f), w, h );

	// render the text on top
	RegisterGraphicsElement( 0, m_pCaptionText );
	RegisterGraphicsElement( 1, pRect );

//	RegisterColoredElement( m_pCaptionText );
}


void CGM_StdCaptionRenderer::UpdateCaptionText( CGM_Control& control )
{
	// cancel the previous text draw effect
	m_pGraphicsEffectManager->CancelEffect( m_PrevTextDrawEffect );

	m_pGraphicsEffectManager->SetTimeOffset();

	m_pCaptionText->SetText( control.GetCaptionText() );

	int num_chars_per_sec = 50;
	m_PrevTextDrawEffect = m_pGraphicsEffectManager->DrawText( m_pCaptionText, 0.0f, num_chars_per_sec );
}


void CGM_StdCaptionRenderer::OnControlFocused( CGM_Control& control )
{
	UpdateCaptionText( control );
}


void CGM_StdCaptionRenderer::OnMouseOverControlChanged( CGM_Control& control_under_mouse )
{
	UpdateCaptionText( control_under_mouse );
}
