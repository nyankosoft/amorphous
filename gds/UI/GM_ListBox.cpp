#include "GM_ListBox.h"

#include "GM_ListBoxDesc.h"
#include "GM_ScrollBarDesc.h"
#include "GM_ScrollBar.h"
#include "GM_Dialog.h"
#include "GM_ControlRenderer.h"
#include "GM_Input.h"
#include "GM_SoundPlayer.h"

#include "Support/SafeDeleteVector.h"

using namespace std;


//--------------------------------------------------------------------------------------
// CGM_ListBox class
//--------------------------------------------------------------------------------------
CGM_ListBox::CGM_ListBox( CGM_Dialog *pDialog, CGM_ListBoxDesc *pDesc )
:
CGM_Control( pDialog, pDesc ),
m_pScrollBar(NULL)
{
	m_Style		= pDesc->Style;

	// TODO: support multi-selection
	m_Style &= (~MULTISELECTION);

	m_nSBWidth    = pDesc->nSBWidth;
	m_nTextHeight = pDesc->nTextHeight;
	m_nBorder     = pDesc->nBorder;
	m_nMargin     = pDesc->nMargin;

	m_rcText = pDesc->TextRect;
	m_rcSelection = pDesc->SelectionRect;
	m_nSelected = -1;	// original DXUT
//	m_nSelected = 0;
	m_nSelStart = 0;
	m_bDrag = false;

	// create scroll bar
	CGM_ScrollBarDesc bar_desc;
	bar_desc.Rect = GetBoundingBox();
	bar_desc.Rect.left = GetBoundingBox().right - m_nSBWidth;
	bar_desc.iPageSize = pDesc->PageSize;

	// the rect has been already transformed to global coord in ctor of CGM_Control
//	SRect dlg_rect = GetOwnerDialog()->GetBoundingBox();
//	bar_desc.Rect.Offset( -dlg_rect.left, -dlg_rect.top );
	bar_desc.coord_type = CGM_ControlDesc::COORD_GLOBAL;

	m_pScrollBar = m_pDialog->AddControl( &bar_desc );

	UpdateRects();
}


//--------------------------------------------------------------------------------------
CGM_ListBox::~CGM_ListBox()
{
//	MsgBox( "destroying list box" );

	// RemoveAllItems() internally accesses the scroll bar, which may have been
	// already destroyed by the dialog at this point
//	RemoveAllItems();

	SafeDeleteVector( m_vecItem );

//	MsgBox( "list box destroyed" );

}


//--------------------------------------------------------------------------------------
void CGM_ListBox::UpdateRects()
{
//	CGM_Control::UpdateRects();

	m_rcSelection = m_BoundingBox;
	m_rcSelection.right -= m_nSBWidth;
	m_rcSelection.Inflate( -m_nBorder, -m_nBorder );

	m_rcText = m_rcSelection;
	m_rcText.Inflate( -m_nMargin, 0 );

	// Update the scrollbar's rects
//	m_pScrollBar->SetLocation( m_BoundingBox.right - m_nSBWidth, m_BoundingBox.top );
//	m_pScrollBar->SetSize( m_nSBWidth, m_height );
	m_pScrollBar->m_BoundingBox.SetPositionLTWH(
		m_BoundingBox.right - m_nSBWidth,
		m_BoundingBox.top,
		m_nSBWidth,
		m_BoundingBox.GetHeight() );

/*	DXUTFontNode* pFontNode = DXUTGetGlobalDialogResourceManager()->GetFontNode( m_Elements.GetAt( 0 )->iFont );
	if( pFontNode && pFontNode->nHeight )
	{
		m_pScrollBar->SetPageSize( RectHeight( m_rcText ) / pFontNode->nHeight );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_pScrollBar->ShowItem( m_nSelected );
	}*/

/**	CFont* pFont = m_pDialog->GetFont( m_Elements.GetAt( 0 )->iFont );
	if( pFont && pFont->GetLetterHeight() )
	{
		m_pScrollBar->SetPageSize( m_rcText.GetHeight() / pFont->GetLetterHeight() );

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_pScrollBar->ShowItem( m_nSelected );
	}**/
}


int CGM_ListBox::GetNumItemsToDisplay() const
{
	const int num_items = GetNumItems();

	if( m_pScrollBar )
	{
		const int num_items_in_page = m_pScrollBar->GetPageSize();
		return num_items < num_items_in_page ? num_items : num_items_in_page;
	}
	else
		return num_items;
}


int CGM_ListBox::GetIndexOfFirstItemToDisplay() const
{
	if( m_pScrollBar )
		return m_pScrollBar->GetTrackPos();
	else
		return 0;
}


void CGM_ListBox::OnItemSelectionChanged()
{
	CGM_ListBoxItem *pItem = GetSelectedItem();
	CGM_ListBoxItem empty_item;

	if( !pItem )
		pItem = &empty_item;

	// send notification to event handler
	if( m_pEventHandler )
	{
		m_pEventHandler->OnItemSelectionChanged( *pItem );
	}

	// also notify the event handler of the dialog
	m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );

	// send notification to control renderer
	if( m_pRenderer.get() )
		m_pRenderer->OnItemSelectionChanged( *this );

	// play sound
	if( m_pSoundPlayer.get() )
		m_pSoundPlayer->OnItemSelectionChanged( *pItem );
}


bool CGM_ListBox::AddItem( const string& text, void *pUserData, int user_data_id, const string& desc )
{
	CGM_ListBoxItem *pNewItem = new CGM_ListBoxItem;

	if( !pNewItem )
		return false;

	pNewItem->text       = text;
	pNewItem->pUserData  = pUserData;
	pNewItem->UserDataID = user_data_id;
	pNewItem->desc       = desc;
//	SetRect( &pNewItem->rcActive, 0, 0, 0, 0 );
	pNewItem->rcActive.SetValues( 0, 0, 0, 0 );
	pNewItem->selected = false;

//	HRESULT hr = m_vecItem.Add( pNewItem );
//	if( SUCCEEDED( hr ) )
//		m_pScrollBar->SetTrackRange( 0, m_vecItem.size() );

	int added_item_index = (int)m_vecItem.size();

	pNewItem->m_pOwnerListBox = this;

	// add item to the list
	m_vecItem.push_back( pNewItem );

	m_pScrollBar->SetTrackRange( 0, (int)m_vecItem.size() );

	// set initial selection focus when the first item is given
	if( m_vecItem.size() == 1 )
		SetItemSelectionFocus( 0 );

	// send notification to control renderer
	if( m_pRenderer.get() )
		m_pRenderer->OnItemAdded( *this, added_item_index );

	return true;
}


bool CGM_ListBox::InsertItem( int nIndex, const string& text, void *pUserData )
{
	CGM_ListBoxItem *pNewItem = new CGM_ListBoxItem;
	if( !pNewItem )
		return false;//E_OUTOFMEMORY;

	pNewItem->text = text;
	pNewItem->pUserData = pUserData;
	pNewItem->rcActive.SetValues( 0, 0, 0, 0 );
	pNewItem->selected = false;

//	HRESULT hr = m_vecItem.Insert( nIndex, pNewItem );
//	if( SUCCEEDED( hr ) )
//		m_pScrollBar->SetTrackRange( 0, m_vecItem.size() );

	pNewItem->m_pOwnerListBox = this;

	m_vecItem.insert( m_vecItem.begin() + nIndex, pNewItem );
	m_pScrollBar->SetTrackRange( 0, (int)m_vecItem.size() );

	// send notification to control renderer
	if( m_pRenderer.get() )
		m_pRenderer->OnItemInserted( *this, nIndex );

	return true;//hr;
}


//--------------------------------------------------------------------------------------
void CGM_ListBox::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_vecItem.size() )
		return;

	SafeDelete( m_vecItem.at(nIndex) );

	m_vecItem.erase( m_vecItem.begin() + nIndex );

	m_pScrollBar->SetTrackRange( 0, (int)m_vecItem.size() );

	if( m_nSelected >= (int)m_vecItem.size() )
	{
		// The selection focus was on the last item and the last item was removed
		// - need to update selection focus
		m_nSelected = (int)m_vecItem.size() - 1;

		SetItemSelectionFocus( m_nSelected );
	}
}


//--------------------------------------------------------------------------------------
//void CGM_ListBox::RemoveItemByText( char *pText )
void CGM_ListBox::RemoveItemByText( const string& text )
{
}


//--------------------------------------------------------------------------------------
void CGM_ListBox::RemoveItemByData( void *pUserData )
{
}


//--------------------------------------------------------------------------------------
void CGM_ListBox::RemoveAllItems()
{
	SafeDeleteVector( m_vecItem );

	m_vecItem.clear();

	m_pScrollBar->SetTrackRange( 0, 1 );

//	m_nSelected = -1;
}


//--------------------------------------------------------------------------------------
CGM_ListBoxItem *CGM_ListBox::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_vecItem.size() )
		return NULL;

	return m_vecItem[nIndex];
}


/**
 * For single-selection listbox, returns the index of the selected item.
 * For multi-selection, returns the first selected item after the nPreviousSelected position.
 * To search for the first selected item, the app passes -1 for nPreviousSelected.  For
 * subsequent searches, the app passes the returned index back to GetSelectedIndex as.
 * nPreviousSelected.
 * Returns -1 on error or if no item is selected.
 */
int CGM_ListBox::GetSelectedIndex( int nPreviousSelected )
{
	if( nPreviousSelected < -1 )
		return -1;

	if( m_Style & MULTISELECTION )
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for( int i = nPreviousSelected + 1; i < (int)m_vecItem.size(); ++i )
		{
			CGM_ListBoxItem *pItem = m_vecItem.at( i );

			if( pItem->selected )
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}


int CGM_ListBox::GetFocusedItemLocalIndexInCurrentPage()
{
	int selected_item_index = GetSelectedIndex();
	if( selected_item_index < 0 )
		return selected_item_index;

	int first_item_index = 0;
	if( GetScrollbar() )
		first_item_index = GetScrollbar()->GetTrackPos();

	return selected_item_index - first_item_index;
}


void CGM_ListBox::SetItemSelectionFocus( int nNewIndex )
{
	// If no item exists, do nothing.
	if( m_vecItem.size() == 0 )
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 )
		m_nSelected = 0;
	if( m_nSelected >= (int)m_vecItem.size() )
		m_nSelected = (int)m_vecItem.size() - 1;

	if( nOldSelected != m_nSelected )
	{
		if( m_Style & MULTISELECTION )
		{
			m_vecItem[m_nSelected]->selected = true;
		}

		// Update selection start
		m_nSelStart = m_nSelected;

		// Adjust scroll bar
		m_pScrollBar->ShowItem( m_nSelected );
	}

//	CGM_Event event( CGM_Event::LISTBOX_SELECTION, this );
//	m_pDialog->SendEvent( event );

	m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );

	if( m_vecItem.size() == 1 && nNewIndex == 0 )
	{
		if( m_pRenderer )
			m_pRenderer->OnItemSelectionFocusCreated( *this );
	}
}


int CGM_ListBox::GetPageSize()
{
	return m_pScrollBar->GetPageSize();
}


bool CGM_ListBox::HandleMouseInput( CGM_InputData& input )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( input.code == CGM_Input::MOUSE_BUTTON_L && input.type == CGM_InputData::TYPE_PRESSED )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	// Let the scroll bar handle it first.
	if( m_pScrollBar->HandleMouseInput( input ) )
		return true;

	SPoint pt = input.pos;

	switch( input.code )
	{
		case CGM_Input::MOUSE_BUTTON_L:
//		case WM_LBUTTONDBLCLK:	TODO: support double click
        if( input.type == CGM_InputData::TYPE_PRESSED )
		{
			// Check for clicks in the text area
			if( m_vecItem.size() > 0 && m_rcSelection.ContainsPoint( pt ) )
			{
				// Compute the index of the clicked item

				int nClicked;	// index of the clicked item
				if( m_nTextHeight )
					nClicked = m_pScrollBar->GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
				else
					nClicked = -1;

				// Only proceed if the click falls on top of an item.

				if( nClicked >= m_pScrollBar->GetTrackPos() &&
					nClicked < (int)m_vecItem.size() &&
					nClicked < m_pScrollBar->GetTrackPos() + m_pScrollBar->GetPageSize() )
				{
//					SetCapture( DXUTGetHWND() );
					m_bDrag = true;

					// If this is a double click, fire off an event and exit
					// since the first click would have taken care of the selection
					// updating.
/*					if( uMsg == WM_LBUTTONDBLCLK )
					{
						m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
						return true;
					}*/

					m_nSelected = nClicked;
//					if( !( wParam & MK_SHIFT ) )
//						m_nSelStart = m_nSelected;

					// If this is a multi-selection listbox, update per-item
					// selection data.

/*					if( m_Style & MULTISELECTION )
					{
						// Determine behavior based on the state of Shift and Ctrl

						CGM_ListBoxItem *pSelItem = m_vecItem.at( m_nSelected );
						if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
						{
							// Control click. Reverse the selection of this item.

							pSelItem->selected = !pSelItem->selected;
						} else
						if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
						{
							// Shift click. Set the selection for all items
							// from last selected item to the current item.
							// Clear everything else.

							int nBegin = min( m_nSelStart, m_nSelected );
							int nEnd = max( m_nSelStart, m_nSelected );

							for( int i = 0; i < nBegin; ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem.at( i );
								pItem->selected = false;
							}

							for( int i = nEnd + 1; i < (int)m_vecItem.size(); ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem.at( i );
								pItem->selected = false;
							}

							for( int i = nBegin; i <= nEnd; ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem.at( i );
								pItem->selected = true;
							}
						} else
						if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
						{
							// Control-Shift-click.

							// The behavior is:
							//   Set all items from m_nSelStart to m_nSelected to
							//	 the same state as m_nSelStart, not including m_nSelected.
							//   Set m_nSelected to selected.

							int nBegin = min( m_nSelStart, m_nSelected );
							int nEnd = max( m_nSelStart, m_nSelected );

							// The two ends do not need to be set here.

							bool bLastSelected = m_vecItem.at( m_nSelStart )->bSelected;
							for( int i = nBegin + 1; i < nEnd; ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem.at( i );
								pItem->selected = bLastSelected;
							}

							pSelItem->selected = true;

							// Restore m_nSelected to the previous value
							// This matches the Windows behavior

							m_nSelected = m_nSelStart;
						} else
						{
							// Simple click.  Clear all items and select the clicked
							// item.


							for( int i = 0; i < (int)m_vecItem.size(); ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem.at( i );
								pItem->selected = false;
							}

							pSelItem->selected = true;
						}
					}  // End of multi-selection case
*/
					m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
				}

				return true;
			}
		}
//		break;
//		case WM_LBUTTONUP:
		else if( input.type == CGM_InputData::TYPE_RELEASED )
		{
//			ReleaseCapture();
			m_bDrag = false;

			if( m_nSelected != -1 )
			{
				// Set all items between m_nSelStart and m_nSelected to
				// the same state as m_nSelStart
//				int nEnd = std::max( m_nSelStart, m_nSelected );
				int nEnd = m_nSelStart > m_nSelected ? m_nSelStart : m_nSelected;

//				for( int n = std::min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
				int min_select = ( m_nSelStart < m_nSelected ? m_nSelStart : m_nSelected ) + 1;
				for( int n = min_select; n < nEnd; ++n )
					m_vecItem[n]->selected = m_vecItem[m_nSelStart]->selected;
				m_vecItem[m_nSelected]->selected = m_vecItem[m_nSelStart]->selected;

				// If m_nSelStart and m_nSelected are not the same,
				// the user has dragged the mouse to make a selection.
				// Notify the application of this.
				if( m_nSelStart != m_nSelected )
					m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
			}
			return false;
		}
		break;

//		case WM_MOUSEMOVE:
		case CGM_Input::MOUSE_AXIS_X:
		case CGM_Input::MOUSE_AXIS_Y:
			if( m_bDrag )
			{
				// Compute the index of the item below cursor

				int nItem;
				if( m_nTextHeight )
					nItem = m_pScrollBar->GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
				else
					nItem = -1;

				// Only proceed if the cursor is on top of an item.

				if( nItem >= (int)m_pScrollBar->GetTrackPos() &&
					nItem < (int)m_vecItem.size() &&
					nItem < m_pScrollBar->GetTrackPos() + m_pScrollBar->GetPageSize() )
				{
					m_nSelected = nItem;
					m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
				}
				else if( nItem < (int)m_pScrollBar->GetTrackPos() )
				{
					// User drags the mouse above window top
					m_pScrollBar->Scroll( -1 );
					m_nSelected = m_pScrollBar->GetTrackPos();
					m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
				} 
				else if( nItem >= m_pScrollBar->GetTrackPos() + m_pScrollBar->GetPageSize() )
				{
					// User drags the mouse below window bottom
					m_pScrollBar->Scroll( 1 );
//					m_nSelected = std::min( (int)m_vecItem.size(), m_pScrollBar->GetTrackPos() + m_pScrollBar->GetPageSize() ) - 1;
					int offset = ( m_pScrollBar->GetTrackPos() + m_pScrollBar->GetPageSize() ) - 1;
					m_nSelected = GetNumItems() < offset ? GetNumItems() : offset;
					m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
				}
			}
			break;

//		case WM_MOUSEWHEEL:
//		case GIC_MOUSE_WHEEL_UP:
//		case GIC_MOUSE_WHEEL_DOWN:
		case CGM_Input::SCROLL_UP:
		case CGM_Input::SCROLL_DOWN:
		{
/*			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			m_pScrollBar->Scroll( -nScrollAmount );*/
			int nScrollAmount = 1;
			m_pScrollBar->Scroll( -nScrollAmount );
			return true;
		}
	}

	return false;
}


bool CGM_ListBox::HandleKeyboardInput( CGM_InputData& input )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
//	if( m_pScrollBar->HandleKeyboard( uMsg, wParam, lParam ) )
	if( m_pScrollBar->HandleKeyboardInput( input ) )
		return true;

	switch( input.code )
	{
	case CGM_Input::OK:
		if( input.type == CGM_InputData::TYPE_PRESSED )
		{
			if( GetSelectedItem() )
			{
				if( m_pEventHandler )
					m_pEventHandler->OnItemSelected( *GetSelectedItem() );

				m_pDialog->SendEvent( CGM_Event::LISTBOX_ITEM_SELECTED, true, this );

				if( m_pRenderer )
					m_pRenderer->OnItemSelected( *this );

				if( m_pSoundPlayer.get() )
					m_pSoundPlayer->OnItemSelected( *GetSelectedItem() );

				if( m_Style & CLOSE_DIALOG_ON_ITEM_SELECTION )
					m_pDialog->Close();
			}
			return true;
		}
		break;

	case CGM_Input::SHIFT_FOCUS_UP:
	case CGM_Input::SHIFT_FOCUS_DOWN:
	case CGM_Input::SHIFT_FOCUS_RIGHT:
	case CGM_Input::SHIFT_FOCUS_LEFT:
		if( input.type == CGM_InputData::TYPE_PRESSED )
		{
			// If no item exists, do nothing.
			if( m_vecItem.size() == 0 )
				return true;

			int nOldSelected = m_nSelected;

			// Adjust m_nSelected
			switch( input.code )
			{
				case CGM_Input::SHIFT_FOCUS_UP:   --m_nSelected; break;
				case CGM_Input::SHIFT_FOCUS_DOWN: ++m_nSelected; break;
//				case VK_NEXT: m_nSelected += m_pScrollBar->GetPageSize() - 1; break;
//				case VK_PRIOR: m_nSelected -= m_pScrollBar->GetPageSize() - 1; break;
//				case CGM_Input::SET_FOCUS_FIRST: m_nSelected = 0; break;
//				case CGM_Input::SET_FOCUS_LAST:   m_nSelected = m_vecItem.size() - 1; break;
			}

			// Perform capping
			if( m_nSelected < 0 )
				m_nSelected = 0;
			if( m_nSelected >= (int)m_vecItem.size() )
				m_nSelected = (int)m_vecItem.size() - 1;

			if( nOldSelected != m_nSelected )
			{
				if( m_Style & MULTISELECTION )
				{
					// Multiple selection

					// Clear all selection
					for( int i = 0; i < (int)m_vecItem.size(); ++i )
					{
						CGM_ListBoxItem *pItem = m_vecItem[i];
						pItem->selected = false;
					}

/*					if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Select all items from m_nSelStart to
						// m_nSelected
						int nEnd = max( m_nSelStart, m_nSelected );

						for( int n = min( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
							m_vecItem[n]->bSelected = true;
					}
					else
					{*/
						m_vecItem[m_nSelected]->selected = true;

						// Update selection start
						m_nSelStart = m_nSelected;
/*					}*/
				}
				else
					m_nSelStart = m_nSelected;

				// Adjust scroll bar
				m_pScrollBar->ShowItem( m_nSelected );

				// Send notification to event handler and control renderer
				OnItemSelectionChanged();
			}
			return true;
		}
		break;
	}

	return false;
}


void CGM_ListBox::OnItemTextChanged( CGM_ListBoxItem& item )
{
	if( m_pRenderer.get() )
		m_pRenderer->OnItemTextChanged( *this, item );
}


void CGM_ListBox::OnItemDescChanged( CGM_ListBoxItem& item )
{
	if( m_pRenderer.get() )
		m_pRenderer->OnItemDescChanged( *this, item );
}


void CGM_ListBox::OnItemUpdated( CGM_ListBoxItem& item )
{
	if( m_pRenderer.get() )
		m_pRenderer->OnItemUpdated( *this, item );
}



/*	switch( uMsg )
	{
		case WM_KEYDOWN:
			switch( wParam )
			{
				case VK_UP:
				case VK_DOWN:
				case VK_NEXT:
				case VK_PRIOR:
				case VK_HOME:
				case VK_END:

					// If no item exists, do nothing.
					if( m_vecItem.size() == 0 )
						return true;

					int nOldSelected = m_nSelected;

					// Adjust m_nSelected
					switch( wParam )
					{
						case VK_UP: --m_nSelected; break;
						case VK_DOWN: ++m_nSelected; break;
						case VK_NEXT: m_nSelected += m_pScrollBar->GetPageSize() - 1; break;
						case VK_PRIOR: m_nSelected -= m_pScrollBar->GetPageSize() - 1; break;
						case VK_HOME: m_nSelected = 0; break;
						case VK_END: m_nSelected = m_vecItem.size() - 1; break;
					}

					// Perform capping
					if( m_nSelected < 0 )
						m_nSelected = 0;
					if( m_nSelected >= (int)m_vecItem.size() )
						m_nSelected = m_vecItem.size() - 1;

					if( nOldSelected != m_nSelected )
					{
						if( m_Style & MULTISELECTION )
						{
							// Multiple selection

							// Clear all selection
							for( int i = 0; i < (int)m_vecItem.size(); ++i )
							{
								CGM_ListBoxItem *pItem = m_vecItem[i];
								pItem->selected = false;
							}

							if( GetKeyState( VK_SHIFT ) < 0 )
							{
								// Select all items from m_nSelStart to
								// m_nSelected
								int nEnd = max( m_nSelStart, m_nSelected );

								for( int n = min( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
									m_vecItem[n]->bSelected = true;
							}
							else
							{
								m_vecItem[m_nSelected]->bSelected = true;

								// Update selection start
								m_nSelStart = m_nSelected;
							}
						} else
							m_nSelStart = m_nSelected;

						// Adjust scroll bar

						m_pScrollBar->ShowItem( m_nSelected );

						// Send notification

						m_pDialog->SendEvent( CGM_Event::LISTBOX_SELECTION, true, this );
					}
					return true;
			}
			break;
	}

	return false;
}*/



/*
//--------------------------------------------------------------------------------------
void CGM_ListBox::Render()
{
	if( m_bVisible == false )
		return;

	CDXUTElement* pElement = m_Elements.GetAt( 0 );
	pElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
	pElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

	CDXUTElement* pSelElement = m_Elements.GetAt( 1 );
	pSelElement->TextureColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );
	pSelElement->FontColor.Blend( DXUT_STATE_NORMAL, fElapsedTime );

	m_pDialog->DrawSprite( pElement, &m_BoundingBox );

	// Render the text
	if( m_vecItem.size() > 0 )
	{
		// Find out the height of a single line of text
		RECT rc = m_rcText;
		RECT rcSel = m_rcSelection;
		rc.bottom = rc.top + DXUTGetGlobalDialogResourceManager()->GetFontNode( pElement->iFont )->nHeight;

		// Update the line height formation
		m_nTextHeight = rc.bottom - rc.top;

		static bool bSBInit;
		if( !bSBInit )
		{
			// Update the page size of the scroll bar
			if( m_nTextHeight )
				m_pScrollBar->SetPageSize( RectHeight( m_rcText ) / m_nTextHeight );
			else
				m_pScrollBar->SetPageSize( RectHeight( m_rcText ) );
			bSBInit = true;
		}

		rc.right = m_rcText.right;
		for( int i = m_pScrollBar->GetTrackPos(); i < (int)m_vecItem.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			CGM_ListBoxItem *pItem = m_vecItem.at( i );

			// Determine if we need to render this item with the
			// selected element.
			bool bSelectedStyle = false;

			if( !( m_Style & MULTISELECTION ) && i == m_nSelected )
				bSelectedStyle = true;
			else
			if( m_Style & MULTISELECTION )
			{
				if( m_bDrag &&
					( ( i >= m_nSelected && i < m_nSelStart ) ||
					  ( i <= m_nSelected && i > m_nSelStart ) ) )
					bSelectedStyle = m_vecItem[m_nSelStart]->bSelected;
				else
				if( pItem->selected )
					bSelectedStyle = true;
			}

			if( bSelectedStyle )
			{
				rcSel.top = rc.top; rcSel.bottom = rc.bottom;
				m_pDialog->DrawSprite( pSelElement, &rcSel );
				m_pDialog->DrawText( pItem->text, pSelElement, &rc );
			}
			else
				m_pDialog->DrawText( pItem->text, pElement, &rc );

			OffsetRect( &rc, 0, m_nTextHeight );
		}
	}

	// Render the scroll bar

	m_pScrollBar->Render();
}*/



