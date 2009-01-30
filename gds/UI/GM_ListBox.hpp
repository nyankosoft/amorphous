#ifndef __GM_CONTROL_LISTBOX__
#define __GM_CONTROL_LISTBOX__


#include "fwd.hpp"
#include "GM_Control.hpp"

#include <vector>
#include <string>


/** 
  item owned by CGM_ListBox
 */
class CGM_ListBoxItem
{
	CGM_ListBox *m_pOwnerListBox;

	std::string	text;	///< title of the item

	std::string	desc;	///< brief description of the item

public:

	void*	pUserData;

	int UserDataID;

	SRect	rcActive;

	bool	selected;

public:

	const std::string& GetText() const { return text; }

	const std::string& GetDesc() const { return desc; }

	/// notify the control renderer that item has been modified
	void NotifyItemUpdate();

	inline void SetText( const std::string& new_text );

	inline void SetDesc( const std::string& new_desc );

	CGM_ListBoxItem() : m_pOwnerListBox(NULL), pUserData(NULL), UserDataID(0), selected(false) {}

	CGM_ListBoxItem( const std::string& _text ) : m_pOwnerListBox(NULL), text(_text), pUserData(NULL), UserDataID(0), selected(false) {}

	friend class CGM_ListBox;
};



class CGM_ListBox : public CGM_Control
{
protected:

	/// Send notification to event handler and control renderer
	void OnItemSelectionChanged();

public:

	CGM_ListBox( CGM_Dialog *pDialog, CGM_ListBoxDesc *pDesc );

	virtual ~CGM_ListBox();

	virtual unsigned int GetType() const { return LISTBOX; }

//	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

	virtual bool CanHaveFocus() const { return (IsVisible() && IsEnabled()); }

	virtual bool HandleMouseInput( CGM_InputData& input );

	virtual bool HandleKeyboardInput( CGM_InputData& input );

	virtual void UpdateRects();


	int GetNumItems() const { return (int)m_vecItem.size(); }

	int GetNumItemsToDisplay() const;

	int GetIndexOfFirstItemToDisplay() const;

	void SetStyle( int style ) { m_Style = style; }

	void SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }

	void SetBorder( int nBorder, int nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }

	bool AddItem( const std::string& text, void *pUserData = NULL, int user_data_id = 0, const std::string& desc = "" );
	bool InsertItem( int nIndex, const std::string& text, void *pUserData );

	void RemoveItem( int nIndex );
	void RemoveItemByText( const std::string& text );
	void RemoveItemByData( void *pUserData );
	void RemoveAllItems();

	CGM_ListBoxItem *GetItem( int nIndex );

	/// returns index to the currently focused item
	/// TODO: should change function name to GetFocusedItemIndex() ?
	int GetSelectedIndex( int nPreviousSelected = -1 );

	/// How is this different from the above
	int GetSelectedItemIndex() const { return m_nSelected; }

	/// returns the index of the item last selected.
	/// - Returns -1 if no item has been selected so far.
	/// - Only valid with the single selection listbox
	int GetLastSelectedItemIndex() const { return m_nLastSelected; }

	/// returns index to the currently focusd index
	/// - used by the renderer
	/// \return the local index of the focused item in the currently displayed page.
	int GetFocusedItemLocalIndexInCurrentPage();

	/// returns borrowed pointer to the currently focused item
	/// TODO: should change function name to GetFocusedItem() ?
	CGM_ListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }

	void SetItemSelectionFocus( int nNewIndex );

	int GetStyle() const { return m_Style; }
	const SRect& GetTextRect() const { return m_rcText; }
	const SRect& GetSelectionRect() const { return m_rcSelection; }
	const CGM_ScrollBar *GetScrollbar() const { return m_pScrollBar; }
	int GetScrollBarWidth() const { return m_nSBWidth; }
	int GetTextHeight() const { return m_nTextHeight; }
	int GetBorderWidth() const { return m_nBorder; }
	int GetMargin() const { return m_nMargin; }

	int GetPageSize();

	void SetEventHandler( CGM_ListBoxEventHandlerSharedPtr pEventHandler ) { m_pEventHandler = pEventHandler; }

	void OnItemTextChanged( CGM_ListBoxItem& item );

	void OnItemDescChanged( CGM_ListBoxItem& item );

	void OnItemUpdated( CGM_ListBoxItem& item );

	void SetSoundPlayer( CGM_ListBoxSoundPlayerSharedPtr pSoundPlayer ) { m_pSoundPlayer = pSoundPlayer; }

	virtual bool HasSoundPlayer() const { return ( m_pSoundPlayer.get() != NULL ); }

	enum STYLE
	{
		MULTISELECTION					= (1 << 0), ///< NOT IMPLEMENTED YET.
		CLOSE_DIALOG_ON_ITEM_SELECTION	= (1 << 1), ///< close the owner dialog right after an item is selected
	};

protected:

	/// Text rendering bound
	SRect m_rcText;

	/// Selection box bound
	SRect m_rcSelection;

	CGM_ScrollBar *m_pScrollBar;	///< scroll bar control (borrowed reference)

	int m_Style;		///< List box style
	int m_nSBWidth;
	int m_nTextHeight;	///< Height of a single lineof text
	int m_nBorder;
	int m_nMargin;

	int m_nSelected;	///< Index of the selected item for single selection list box
	int m_nSelStart;	///< Index of the item where selection starts (for handling multi-selection)
	bool m_bDrag;		///< Whether the user is dragging the mouse to select

	int m_nLastSelected; ///< item which was selected most recently.

	std::vector<CGM_ListBoxItem *> m_vecItem;

	CGM_ListBoxEventHandlerSharedPtr m_pEventHandler;

	CGM_ListBoxSoundPlayerSharedPtr m_pSoundPlayer;
};


// ================================== inline implementations ==================================


inline void CGM_ListBoxItem::SetText( const std::string& new_text )
{
	text = new_text;

	if( m_pOwnerListBox )
		m_pOwnerListBox->OnItemTextChanged( *this );
}


inline void CGM_ListBoxItem::SetDesc( const std::string& new_desc )
{
	desc = new_desc;

	if( m_pOwnerListBox )
		m_pOwnerListBox->OnItemDescChanged( *this );
}



/**
  base class of event handler for listbox

 */
class CGM_ListBoxEventHandler
{
public:
	CGM_ListBoxEventHandler() {}
	virtual ~CGM_ListBoxEventHandler() {}
	virtual void OnItemSelected( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemAdded( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemInserted( CGM_ListBoxItem& item, int item_index ) {}
};


// ================================== template ==================================


/* --- template for user defined event handler
class UserListBoxEventHandler : public CGM_ListBoxEventHandler
{

public:
	UserListBoxEventHandler() {}
	virtual ~UserListBoxEventHandler() {}
	virtual void OnItemSelected( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemSelectionChanged( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemAdded( CGM_ListBoxItem& item, int item_index ) {}
	virtual void OnItemInserted( CGM_ListBoxItem& item, int item_index ) {}
};
*/


#endif /*  __GM_CONTROL_LISTBOX__  */
