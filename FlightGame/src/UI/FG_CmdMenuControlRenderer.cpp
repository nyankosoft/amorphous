
#include "FG_CmdMenuControlRenderer.h"
#include "FG_CmdMenuControlRendererManager.h"
#include "UI/ui.h"

#include "3DCommon/GraphicsEffectManager.h"

using namespace std;


CFG_CmdMenuControlRenderer::CFG_CmdMenuControlRenderer()
{
	m_aColor[CGM_Control::STATE_NORMAL]			= SFloatRGBAColor( 0.8f, 0.8f, 0.8f, 1.0f );
    m_aColor[CGM_Control::STATE_DISABLED]		= SFloatRGBAColor( 0.3f, 0.3f, 0.3f, 1.0f );
    m_aColor[CGM_Control::STATE_HIDDEN]			= SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 1.0f );
    m_aColor[CGM_Control::STATE_FOCUS]			= SFloatRGBAColor( 0.2f, 1.0f, 0.2f, 1.0f );
    m_aColor[CGM_Control::STATE_MOUSEOVER]		= SFloatRGBAColor( 0.2f, 0.7f, 0.2f, 1.0f );
    m_aColor[CGM_Control::STATE_PRESSED]		= SFloatRGBAColor( 0.6f, 1.0f, 0.6f, 1.0f );
    m_aColor[CGM_Control::STATE_SUBDIALOGOPEN]	= SFloatRGBAColor( 0.5f, 0.9f, 0.5f, 1.0f );
}

/*
void CFG_CmdMenuControlRenderer::OnFocused()
{
}
void CFG_CmdMenuControlRenderer::OnFocusCleared()
{
}
void CFG_CmdMenuControlRenderer::OnMouseCursorEntered()
{
}
void CFG_CmdMenuControlRenderer::OnMouseCursorLeft()
{
}
*/

void CFG_CmdMenuControlRenderer::OnPressed()
{
}

void CFG_CmdMenuControlRenderer::OnReleased()
{
}


void CFG_CmdMenuControlRenderer::OnChecked()
{
}

void CFG_CmdMenuControlRenderer::OnCheckCleared()
{
}


void CFG_CmdMenuControlRenderer::OnFocused()
{
	ChangeColorToCurrentState();
}


void CFG_CmdMenuControlRenderer::OnFocusCleared()
{
	ChangeColorToCurrentState();
}


void CFG_CmdMenuControlRenderer::OnMouseCursorEntered()
{
	ChangeColorToCurrentState();
}


void CFG_CmdMenuControlRenderer::OnMouseCursorLeft()
{
	ChangeColorToCurrentState();
}


void CFG_CmdMenuControlRenderer::OnItemSelectionFocusCreated()
{
}

void CFG_CmdMenuControlRenderer::OnItemSelectionChanged()
{
}

void CFG_CmdMenuControlRenderer::OnItemSelected()
{
}


void CFG_CmdMenuControlRenderer::OnSliderValueChanged()
{
}


void CFG_CmdMenuControlRenderer::OnDialogOpened()
{
}

void CFG_CmdMenuControlRenderer::OnDialogClosed()
{
}

void CFG_CmdMenuControlRenderer::OnOpenDialogAttemptedToClose()
{
}


void CFG_CmdMenuControlRenderer::OnParentSubDialogButtonFocused()
{
}

void CFG_CmdMenuControlRenderer::OnParentSubDialogButtonFocusCleared()
{
}


void CFG_CmdMenuStaticRenderer::Init()
{
	CGM_Static *pStatic = GetStatic();
	if( !pStatic )
		return;

	int font_id = 0;
	int x = pStatic->GetBoundingBox().left;
	int y = pStatic->GetBoundingBox().top;
	int w = m_pGraphicsElementManager->GetFont(font_id)->GetFontWidth();// 8;
	int h = m_pGraphicsElementManager->GetFont(font_id)->GetFontHeight();// 16;
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];

	SRect text_rect = pStatic->GetBoundingBox();
	text_rect.left  += 6;
	text_rect.right -= 6;
	m_pText = m_pGraphicsElementManager->CreateTextBox( 0, pStatic->GetText(), text_rect,
		CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER, normal_color, w, h );

//	m_pText->SetMargin( LEFT,  6 );
//	m_pText->SetMargin( RIGHT, 6 );

	// not visible by default
	// - visibility is controled by the owner dialog
	int dlg_color_index = 1;
	m_pText->SetAlpha( dlg_color_index, 0 );

	// render the text on top
	RegisterGraphicsElement( 0, m_pText );

	RegisterColoredElement( m_pText );
}


void CFG_CmdMenuButtonRenderer::Init()
{
	CFG_CmdMenuStaticRenderer::Init();

	CGM_Button *pButton = GetButton();
/*
	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	m_pRect      = m_pGraphicsElementManager->CreateRect( pButton->GetBoundingBox(),      SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f) );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( pButton->GetBoundingBox(), normal_color, 2 );

	// not visible by default
	// - visibility is controled by the owner dialog
	int dlg_color_index = 1;
	m_pRect->SetAlpha( dlg_color_index, 0.0f );
	m_pFrameRect->SetAlpha( dlg_color_index, 0.0f );

	// register elements
	// - set local layer offset to determine rendering order
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );

	// register elements that chages colors depending on states
	RegisterColoredElement( m_pFrameRect );
*/
}


void CFG_CmdMenuCheckBoxRenderer::Init()
{
	CFG_CmdMenuButtonRenderer::Init();

	m_BaseTitle = m_pText->GetText();
	m_pText->SetText( ( GetCheckBox()->IsChecked() ? " [x] " : " [ ] " ) + m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CFG_CmdMenuCheckBoxRenderer::OnChecked()
{
	m_pText->SetText( " [x] " + m_BaseTitle );
}

void CFG_CmdMenuCheckBoxRenderer::OnCheckCleared()
{
	m_pText->SetText( " [ ] " + m_BaseTitle );
}


void CFG_CmdMenuRadioButtonRenderer::Init()
{
	CFG_CmdMenuButtonRenderer::Init();

//	m_BaseTitle = m_pText->GetText();
//	m_pText->SetText( ( GetCheckBox()->IsChecked() ? " (x) " : " ( ) " ) + m_BaseTitle );

	m_BaseTitle    = " " + m_pText->GetText() + " ";
	m_CheckedTitle = "[" + m_pText->GetText() + "]";

	if( GetCheckBox()->IsChecked() )
		m_pText->SetText( m_CheckedTitle );
	else
		m_pText->SetText( m_BaseTitle );

	m_pText->SetTextAlignment( CGE_Text::TAL_LEFT, CGE_Text::TAL_CENTER );
}


void CFG_CmdMenuRadioButtonRenderer::OnChecked()
{
//	m_pText->SetText( " (x) " + m_BaseTitle );
	m_pText->SetText( m_CheckedTitle );
}

void CFG_CmdMenuRadioButtonRenderer::OnCheckCleared()
{
//	m_pText->SetText( " ( ) " + m_BaseTitle );
	m_pText->SetText( m_BaseTitle );
}


void CFG_CmdMenuListBoxRenderer::Init()
{
//	CGM_ControlRenderer::Init();

	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	m_pRect      = m_pGraphicsElementManager->CreateRect( pListBox->GetBoundingBox(),      SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f) );
//	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( pListBox->GetBoundingBox(), normal_color, 2 );

	// create empty text elements
	int x = GetBaseControl()->GetBoundingBox().left;
	int y = GetBaseControl()->GetBoundingBox().top;
	int text_x = x;
	int text_y = y;
//	int w = 8;
//	int h = 16;
	int i, num_items_in_page = 10;//pListBox->GetScrollBar();
	m_vecpText.resize( num_items_in_page );
	for( i=0; i<num_items_in_page; i++ )
	{
		text_x = x + 5;
		text_y = y + i * pListBox->GetTextHeight();
		m_vecpText[i] = m_pGraphicsElementManager->CreateText( 0, "-", (float)text_x, (float)text_y, m_aColor[CGM_Control::STATE_NORMAL] );
	}

	// set local layer offset to determine rendering order
	int num_text_elements = (int)m_vecpText.size();
	for( i=0; i<num_text_elements; i++ )
		RegisterGraphicsElement( 0, m_vecpText[i] );
//	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 1, m_pRect );

	// register elements that chages colors depending on states
//	RegisterColoredElement( m_pFrameRect );
//	for( i=0; i<num_text_elements; i++ )
//		RegisterColoredElement( m_vecpText[i] );

	// not visible by default
	int dlg_color_index = 1;
//	m_pFrameRect->SetAlpha( dlg_color_index, 0 );
	m_pRect->SetAlpha( dlg_color_index, 0 );
	for( i=0; i<num_text_elements; i++ )
		m_vecpText[i]->SetAlpha( dlg_color_index, 0 );

}

/*
void CFG_CmdMenuListBoxRenderer::SetColorToTextElement( CGE_Text& rTextElement, int state )
{
}
*/


// update all the text of all the text elements
void CFG_CmdMenuListBoxRenderer::UpdateItems( bool update_text )
{
	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	int focused_item_index = pListBox->GetSelectedIndex();

	int num_items = pListBox->GetNumItems();
	int num_items_in_page = 10;

	int i, num_items_to_display = num_items < num_items_in_page ? num_items : num_items_in_page;

	if( update_text )
	{
		for( i=0; i<num_items_to_display; i++ )
		{
			CGM_ListBoxItem *pItem = pListBox->GetItem( i );
			m_vecpText[i]->SetText( pItem->GetText() );
		}
	}

	int color_index = 0;
//	if( pListBox->GetOwnerDialog()->IsOpen() )
//	{
		for( i=0; i<num_items_to_display; i++ )
		{
			if( i == focused_item_index )
				m_vecpText[i]->SetColor( color_index, m_aColor[CGM_Control::STATE_FOCUS] );
			else
				m_vecpText[i]->SetColor( color_index, m_aColor[CGM_Control::STATE_NORMAL] );
		}
//	}
}


// update all the text of all the text elements
void CFG_CmdMenuListBoxRenderer::OnItemSelectionChanged()
{
	bool update_text = false;
	UpdateItems( update_text );

/*
	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	int focused_item_index = pListBox->GetSelectedIndex();

	int num_items = pListBox->GetNumItems();
	int num_items_in_page = 10;

	int i, num_items_to_display = num_items < num_items_in_page ? num_items : num_items_in_page;
	for( i=0; i<num_items_to_display; i++ )
	{
		if( i == focused_item_index )
			m_vecpText[i]->SetColor( m_aColor[CGM_Control::STATE_FOCUS] );
		else
			m_vecpText[i]->SetColor( m_aColor[CGM_Control::STATE_NORMAL] );
	}
*/
}


void CFG_CmdMenuListBoxRenderer::OnItemSelected()
{
	CGM_ListBox *pListBox = GetListBox();
	if( !pListBox )
		return;

	int selected_item_index = pListBox->GetSelectedIndex();
	if( selected_item_index < 0 )
		return;

	// TODO: support scrolling
	int text_index = selected_item_index;

	int color_index = 0;
	m_vecpText[text_index]->SetColor( color_index, m_aColor[CGM_Control::STATE_PRESSED] );
//	m_pGraphicsEffectManager->SetTimeOffset();
//	m_pGraphicsEffectManager->ChangeColorTo( m_vecpText[text_index], 0.0, 0.2, m_aColor[CGM_Control::STATE_FOCUS], 0 );
}


void CFG_CmdMenuListBoxRenderer::OnItemAdded( int index )
{
	UpdateItems();
}


void CFG_CmdMenuListBoxRenderer::OnItemInserted( int index )
{
	UpdateItems();
}


void CFG_CmdMenuListBoxRenderer::OnItemRemoved( int index )
{
	UpdateItems();
}


CFG_CmdMenuDialogRenderer::CFG_CmdMenuDialogRenderer()
:
m_pRect(NULL),
m_pFrameRect(NULL),
m_pText(NULL),
m_pSeparatorRect(NULL)
{
	m_vSlideIn  = Vector2( 50, 0 );
	m_vSlideOut = Vector2(-50, 0 );
}

void CFG_CmdMenuDialogRenderer::Init()
{
	CGM_Dialog *pDialog = GetDialog();
	if( !pDialog )
		return;

	const SFloatRGBAColor& normal_color = m_aColor[CGM_Control::STATE_NORMAL];
	const SFloatRGBAColor bg_color = SFloatRGBAColor(0.0f,0.0f,0.0f,0.5f);
	SRect dlg_rect = pDialog->GetBoundingBox();
	SRect frame_rect = dlg_rect;
//	frame_rect.Inflate( 3, 3 );
	m_pRect      = m_pGraphicsElementManager->CreateRect( dlg_rect,           bg_color );
//	m_pRect      = m_pGraphicsElementManager->CreateRoundRect( dlg_rect,      bg_color, 4 );
	m_pFrameRect = m_pGraphicsElementManager->CreateFrameRect( frame_rect,    normal_color, 3 );
//	m_pFrameRect = m_pGraphicsElementManager->CreateRoundFrameRect( dlg_rect, SFloatRGBAColor(1,1,1,1), 6, 4 );

	m_pFrameRect->SetTexture( CFG_CmdMenuControlRendererManager::ID_TEX_ROUNDFRAME );

	// title
	SRect text_rect  = dlg_rect;
	text_rect.bottom = text_rect.top + 24;
	text_rect.left   += 3;
	m_pText = m_pGraphicsElementManager->CreateTextBox(
		CFG_CmdMenuControlRendererManager::ID_FONT_MAIN,
		pDialog->GetTitle(),
		text_rect,
		CGE_Text::TAL_RIGHT,
		CGE_Text::TAL_CENTER,
		normal_color );

	// separator
	SRect separator;
	separator.SetPositionLTRB( dlg_rect.left + 3, dlg_rect.top + 24, dlg_rect.right - 3, dlg_rect.top + 26 );
	m_pSeparatorRect = m_pGraphicsElementManager->CreateRect( separator, SFloatRGBAColor(0.9f,0.9f,0.9f,1.0f) );

	int dlg_color_index = 1;
	m_pRect->SetAlpha(          dlg_color_index, 0.0f );
	m_pText->SetAlpha(          dlg_color_index, 0.0f );
	m_pFrameRect->SetAlpha(     dlg_color_index, 0.0f );
	m_pSeparatorRect->SetAlpha( dlg_color_index, 0.0f );

	// render the frame rect on the background rect
	RegisterGraphicsElement( 0, m_pFrameRect );
	RegisterGraphicsElement( 0, m_pText );
	RegisterGraphicsElement( 0, m_pSeparatorRect );
	RegisterGraphicsElement( 1, m_pRect );
}


void CFG_CmdMenuDialogRenderer::OnDialogOpened()
{
	if( !m_pGroupElement )
		return;

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide in
	const SRect& rect = GetDialog()->GetBoundingBox();
	Vector2 vDestPos = Vector2( (float)rect.left, (float)rect.top );
	m_pGroupElement->SetTopLeftPos( vDestPos - m_vSlideIn );
//	m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vDestPos, 0, 0 );
	m_pGraphicsEffectManager->TranslateCDV( m_pGroupElement, 0.0f, vDestPos, m_vSlideIn, 0.15f, 0 );

	// fade in (change alpha form 0 to 1)
	int dlg_color_index = 1;
	m_pGroupElement->SetAlpha( dlg_color_index, 0.0f );
	m_pGraphicsEffectManager->ChangeAlphaTo( m_pGroupElement, 0.0f, 0.15f, dlg_color_index, 1.0f, 0 );
}


void CFG_CmdMenuDialogRenderer::OnDialogClosed()
{
	if( !m_pGroupElement )
		return;

	m_pGraphicsEffectManager->SetTimeOffset();

	// slide out
	const SRect& rect = GetDialog()->GetBoundingBox();
	Vector2 vStartPos = Vector2( (float)rect.left, (float)rect.top );
	m_pGroupElement->SetTopLeftPos( vStartPos );
//	m_pGraphicsEffectManager->TranslateTo( m_pGroupElement, 0.0f, 0.2f, vStartPos + m_vSlideOut, 0, 0 );
	m_pGraphicsEffectManager->TranslateCDV( m_pGroupElement, 0.0f, vStartPos + m_vSlideOut, m_vSlideOut, 0.15f, 0 );

	// fade out (change alpha form 1 to 0)
	int dlg_color_index = 1;
	m_pGraphicsEffectManager->ChangeAlphaTo( m_pGroupElement, 0.0f, 0.15f, dlg_color_index, 0.0f, 0 );
}
