
#include "GM_StdControlRenderer.h"
#include "GM_Control.h"
#include "GM_Dialog.h"
#include "GM_DialogManager.h"
#include "GM_ControlRendererManager.h"

#include "3DCommon/GraphicsEffectManager.h"

#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"

using namespace std;


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


void CGM_StdControlRenderer::OnPressed()
{
	ChangeColorToCurrentState();
}

void CGM_StdControlRenderer::OnReleased()
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnChecked()
{
}

void CGM_StdControlRenderer::OnCheckCleared()
{
}


void CGM_StdControlRenderer::OnFocused()
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnFocusCleared()
{
	g_Log.Print( "%s", __FUNCTION__ );

	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnMouseCursorEntered()
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnMouseCursorLeft()
{
	ChangeColorToCurrentState();
}


void CGM_StdControlRenderer::OnItemSelectionFocusCreated()
{
}

void CGM_StdControlRenderer::OnItemSelectionChanged()
{
}

void CGM_StdControlRenderer::OnItemSelected()
{
}


void CGM_StdControlRenderer::OnSliderValueChanged()
{
}


void CGM_StdControlRenderer::OnDialogOpened()
{
}

void CGM_StdControlRenderer::OnDialogClosed()
{
}

void CGM_StdControlRenderer::OnOpenDialogAttemptedToClose()
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


void CGM_StdStaticRenderer::Init()
{
	CGM_Static *pStatic = GetStatic();
	if( !pStatic )
		return;

	int font_id = 0;
	int x = pStatic->GetBoundingBox().left;
	int y = pStatic->GetBoundingBox().top;
	int w = 16;//m_pGraphicsElementManager->GetFont(font_id)->GetFontWidth();// 8;
	int h = 32;//m_pGraphicsElementManager->GetFont(font_id)->GetFontHeight();// 16;
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];

//	m_pText = m_pGraphicsElementManager->CreateText( 0, pStatic->GetText(), x, y, normal_color, w, h );
	m_pText = m_pGraphicsElementManager->CreateTextBox( 0, pStatic->GetText(), pStatic->GetLocalRect(),
		CGE_Text::TAL_CENTER, CGE_Text::TAL_CENTER, normal_color, w, h );

	// render the text on top
	RegisterGraphicsElement( 0, m_pText );

	RegisterColoredElement( m_pText );
}


void CGM_StdButtonRenderer::Init()
{
	CGM_StdStaticRenderer::Init();

	CGM_Button *pButton = GetButton();

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect      = m_pGraphicsElementManager->CreateRect(      pButton->GetLocalRect(), bg_color );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( pButton->GetLocalRect(), normal_color, 2 );

	// register elements
	// - set local layer offset to determine rendering order
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );

	// register elements that chages colors depending on states
	RegisterColoredElement( m_pFrameRect );
}


void CGM_StdCheckBoxRenderer::Init()
{
	CGM_StdButtonRenderer::Init();

	m_BaseTitle = m_pText->GetText();

	m_pText->SetText( ( GetCheckBox()->IsChecked() ? " [x] " : " [ ] " ) + m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CGM_StdCheckBoxRenderer::OnChecked()
{
	m_pText->SetText( " [x] " + m_BaseTitle );
}

void CGM_StdCheckBoxRenderer::OnCheckCleared()
{
	m_pText->SetText( " [ ] " + m_BaseTitle );
}


void CGM_StdRadioButtonRenderer::Init()
{
	CGM_StdButtonRenderer::Init();

	m_BaseTitle = m_pText->GetText();

	m_pText->SetText( ( GetCheckBox()->IsChecked() ? " (x) " : " ( ) " ) + m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CGM_StdRadioButtonRenderer::OnChecked()
{
	m_pText->SetText( " (x) " + m_BaseTitle );
}

void CGM_StdRadioButtonRenderer::OnCheckCleared()
{
	m_pText->SetText( " ( ) " + m_BaseTitle );
}


void CGM_StdListBoxRenderer::Init()
{
//	CGM_ControlRenderer::Init();

	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);

	m_pRect      = m_pGraphicsElementManager->CreateRect(      pListBox->GetLocalRect(), bg_color );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( pListBox->GetLocalRect(), normal_color, 2 );

	// create empty text elements
	int x = pListBox->GetLocalRect().left;
	int y = pListBox->GetLocalRect().top;
	int text_x = x;
	int text_y = y;
//	int w = 8;
//	int h = 16;
	int i, num_items_in_page = pListBox->GetScrollbar()->GetPageSize();
	m_vecpText.resize( num_items_in_page );
	for( i=0; i<num_items_in_page; i++ )
	{
		text_x = x + 5;
		text_y = y + i * pListBox->GetTextHeight();
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
void CGM_StdListBoxRenderer::UpdateItems( bool update_text )
{
	update_text = true; // For now, just always update the texts//The user may have scrolled the page

	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	const int focused_item_index = pListBox->GetSelectedIndex();

	int i;
	const int num_items_to_display = pListBox->GetNumItemsToDisplay();
	const int first_item_index = pListBox->GetIndexOfFirstItemToDisplay();

	if( update_text )
	{
		for( i=0; i<num_items_to_display; i++ )
		{
			CGM_ListBoxItem *pItem = pListBox->GetItem( first_item_index + i );
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
void CGM_StdListBoxRenderer::OnItemSelectionChanged()
{
	bool update_text = false;
	UpdateItems( update_text );
}


void CGM_StdListBoxRenderer::OnItemSelected()
{
	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	int selected_item_index = pListBox->GetSelectedIndex();
	if( selected_item_index < 0 )
		return;

	int first_item_index = 0;
	if( pListBox->GetScrollbar() )
		first_item_index = pListBox->GetScrollbar()->GetTrackPos();

	int text_index = selected_item_index - first_item_index;

	int color_index = 0;
	m_vecpText[text_index]->SetColor( color_index, m_aColor[CGM_Control::STATE_PRESSED] );
//	m_pGraphicsEffectManager->SetTimeOffset();
//	m_pGraphicsEffectManager->ChangeColorTo( m_vecpText[text_index], 0.0, 0.2, m_aColor[CGM_Control::STATE_FOCUS], 0 );
}


void CGM_StdListBoxRenderer::OnItemAdded( int index )
{
	UpdateItems();
}


void CGM_StdListBoxRenderer::OnItemInserted( int index )
{
	UpdateItems();
}


void CGM_StdListBoxRenderer::OnItemRemoved( int index )
{
	UpdateItems();
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


void CGM_StdScrollBarRenderer::Init()
{
	CGM_ScrollBar *pScrollbar = GetScrollBar();
	if( !pScrollbar )
		return;

	CGraphicsElementManager *pElementMgr = m_pGraphicsElementManager;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);

	SRect thumb_rect = pScrollbar->GetThumbButtonRect();
	const SRect src_rect[NUM_RECT_ELEMENTS] =
	{
		pScrollbar->GetLocalUpButtonRectInOwnerDialogCoord(),
		pScrollbar->GetLocalDownButtonRectInOwnerDialogCoord(),
		pScrollbar->GetLocalTrackRectInOwnerDialogCoord(),
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
	SRect thumb_rect_in_owner_dlg_coord = pScrollbar->GetLocalThumbButtonRectInOwnerDialogCoord();
	m_pThumbGroup = pElementMgr->CreateGroup( apElement, numof(apElement), thumb_rect_in_owner_dlg_coord.GetTopLeftCorner() );

	RegisterGraphicsElementToParentDialog( m_pThumbGroup );

	SetLocalLayerOffset( 0, m_apFrameRect[RE_THUMB] );
	SetLocalLayerOffset( 1, m_apRect[RE_THUMB] );
}


void CGM_StdScrollBarRenderer::OnThumbUpdated( CGM_ScrollBar *pScrollbar )
{
	SRect thumb_rect = pScrollbar->GetLocalThumbButtonRectInOwnerDialogCoord();
	if( m_pThumbGroup )
	{
		m_pThumbGroup->SetLocalTopLeftPos( thumb_rect.GetTopLeftCorner() );

		SRect local_thumb_rect = RectLTWH( 0, 0, thumb_rect.GetWidth(), thumb_rect.GetHeight() );
		for( size_t i=0; i<m_pThumbGroup->GetElementBuffer().size(); i++ )
			m_pThumbGroup->GetElementBuffer()[i]->SetLocalRect( local_thumb_rect );
	}
}


void CGM_StdSliderRenderer::Init()
{
//	CGM_StdStaticRenderer::Init();

	CGM_Slider *pSlider = GetSlider();
	if( !pSlider )
		return;

	// slider frame
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect                  = m_pGraphicsElementManager->CreateRect(      pSlider->GetLocalRect(), bg_color );
	m_pFrameRect             = m_pGraphicsElementManager->CreateFrameRect( pSlider->GetLocalRect(), normal_color, 2 );

	// slider button
	SRect btn_rect = pSlider->GetButtonRect();
	SRect local_btn_rect = RectLTWH( 0, 0, btn_rect.GetWidth(), btn_rect.GetHeight() );
	m_pSliderButtonRect      = m_pGraphicsElementManager->CreateRect(      local_btn_rect,  bg_color );
	m_pSliderButtonFrameRect = m_pGraphicsElementManager->CreateFrameRect( local_btn_rect,  normal_color, 2 );

	SRect dot_rect = local_btn_rect;
	dot_rect.Inflate( -dot_rect.GetWidth() / 4, -dot_rect.GetHeight() / 4 );
	m_pSliderButtonDot       = m_pGraphicsElementManager->CreateRect( dot_rect, normal_color, 2 );

	const SRect slider_rect = pSlider->GetBoundingBox();
	const SPoint slider_topleft = slider_rect.GetTopLeftCorner(); // global
	const SPoint btn_local_topleft = pSlider->GetLocalButtonRectInOwnerDialogCoord().GetTopLeftCorner(); // local coord of owner dialog

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
	// - these guy don't belong to any subgroup of the renderer so each of them can be
	//   registered with a single call of RegisterGraphicsElement()
	RegisterGraphicsElement( 2, m_pFrameRect );
	RegisterGraphicsElement( 3, m_pRect );

	// register elements that chages colors depending on states
	RegisterColoredElement( m_pFrameRect );
	RegisterColoredElement( m_pSliderButtonFrameRect );

}


void CGM_StdSliderRenderer::OnSliderValueChanged()
{
	CGM_Slider *pSlider = GetSlider();
	if( !pSlider )
		return;
/*
	const SRect& btn_rect = pSlider->GetButtonRect();
	Vector2 vDestPos = Vector2( (float)btn_rect.left, (float)btn_rect.top );
//	m_pGraphicsEffectManager->SetTimeOffset();
//	m_pGraphicsEffectManager->TranslateCDV( m_pSliderButton, 0.0f, vDestPos, Vector2( 50.0f, 0.0f ), 0.15f, 0 );

	m_pSliderButton->SetLocalTopLeftPos( vDestPos );
*/

	m_pSliderButton->SetLocalOrigin( pSlider->GetLocalButtonRectInOwnerDialogCoord().GetTopLeftCorner() );
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


void CGM_StdDialogRenderer::Init()
{
	CGM_Dialog *pDialog = GetDialog();
	if( !pDialog )
		return;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor& bg_color     = SFloatRGBAColor(0.0f,0.0f,0.0f,0.6f);
	m_pRect      = m_pGraphicsElementManager->CreateRect(      pDialog->GetLocalRect(), bg_color );
//	m_pRect      = m_pGraphicsElementManager->CreateRoundRect( pDialog->GetLocalRect(), bg_color, 6 );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( pDialog->GetLocalRect(), normal_color, 2 );
//	m_pFrameRect = m_pGraphicsElementManager->CreateRoundFrameRect( pDialog->GetLocalRect(), normal_color, 6, 6 );

	// render the frame rect on the background rect
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );
}


void CGM_StdDialogRenderer::OnDialogOpened()
{
	if( !m_pGroupElement )
		return;

	// cancel the previous slide in/out effect
	m_pGraphicsEffectManager->CancelEffect( m_PrevSlideEffect );

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide in
	if( m_bUseSlideEffects )
	{
		const SRect& rect = GetDialog()->GetBoundingBox();
		Vector2 vDestPos = Vector2( (float)rect.left, (float)rect.top );
		m_pGroupElement->SetTopLeftPos( vDestPos + Vector2( -50, 0 ) );
//		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vDestPos, 0, 0 );
		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateCDV( m_pGroupElement, 0.0f, vDestPos, Vector2( 50.0f, 0.0f ), 0.15f, 0 );
	}

	// fade in (change alpha form 0 to 1)
	m_pGroupElement->SetAlpha( m_DialogFadeColorIndex, 0.0f );
	m_pGraphicsEffectManager->ChangeAlphaTo( m_pGroupElement, 0.0f, 0.15f, m_DialogFadeColorIndex, 1.0f, 0 );
}


void CGM_StdDialogRenderer::OnDialogClosed()
{
	if( !m_pGroupElement )
		return;

	// cancel the previous slide in/out effect
	m_pGraphicsEffectManager->CancelEffect( m_PrevSlideEffect );

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide out
	if( m_bUseSlideEffects )
	{
		const SRect& rect = GetDialog()->GetBoundingBox();
		Vector2 vStartPos = Vector2( (float)rect.left, (float)rect.top );
		m_pGroupElement->SetTopLeftPos( vStartPos );
//		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vStartPos + Vector2( -50, 0 ), 0, 0 );
		m_PrevSlideEffect = m_pGraphicsEffectManager->TranslateCDV( m_pGroupElement, 0.0f, vStartPos + Vector2( -50, 0 ), -Vector2( 50.0f, 0.0f ), 0.15f, 0 );
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


void CGM_StdCaptionRenderer::Init()
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


void CGM_StdCaptionRenderer::OnControlFocused( CGM_Control *pControl )
{
	m_pCaptionText->SetText( pControl->GetCaptionText() );
}


void CGM_StdCaptionRenderer::OnMouseOverControlChanged( CGM_Control *pControlUnderMouse )
{
	m_pCaptionText->SetText( pControlUnderMouse->GetCaptionText() );
}
