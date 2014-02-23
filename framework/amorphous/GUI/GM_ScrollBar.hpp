#ifndef __GM_CONTROL_SCROLLBAR__
#define __GM_CONTROL_SCROLLBAR__


#include "GM_Control.hpp"


namespace amorphous
{


/** Scrollbar
 - CGM_ListBox creates the scrollbar control
   - User does not create the scrollbar
 - Holds the sizes and positions of scrollbar components
   - such as scrollbar, up button, down button, and thumb button
 - Also decides which item should be displayed on the owner listbox
   - This may be the job for the libox, though.



*/
class CGM_ScrollBar : public CGM_Control
{

public:

	CGM_ScrollBar( CGM_Dialog *pDialog, CGM_ScrollBarDesc *pDesc );

	virtual ~CGM_ScrollBar();

	virtual unsigned int GetType() const { return SCROLLBAR; }

//	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }

	virtual bool CanHaveFocus() const { return false;/*(IsVisible() && IsEnabled());*/ }

	virtual bool HandleMouseInput( CGM_InputData& input );

	virtual void UpdateRects();

    void SetTrackRange( int nStart, int nEnd );
    int GetTrackPos() const { return m_nPosition; }
    void SetTrackPos( int nPosition ) { m_nPosition = nPosition; Cap(); UpdateThumbRect(); }
    int GetPageSize() const { return m_nPageSize; }
    void SetPageSize( int nPageSize ) { m_nPageSize = nPageSize; Cap(); UpdateThumbRect(); }

	const SRect& GetUpButtonRect() const { return m_rcUpButton; }
    const SRect& GetDownButtonRect() const { return m_rcDownButton; }
    const SRect& GetTrackRect() const { return m_rcTrack; }
    const SRect& GetThumbButtonRect() const { return m_rcThumb; }

	SRect GetLocalUpButtonRectInOwnerDialogCoord() const;
	SRect GetLocalDownButtonRectInOwnerDialogCoord() const;
	SRect GetLocalTrackRectInOwnerDialogCoord() const;
	SRect GetLocalThumbButtonRectInOwnerDialogCoord() const;

    void Scroll( int nDelta );    ///< Scroll by nDelta items (plus or minus)
    void ShowItem( int nIndex );  ///< Ensure that item nIndex is displayed, scroll if necessary

	/// need to access CGM_ScrollBar::m_BoundingBox in CGM_ListBox::UpdateRects()
	friend class CGM_ListBox;

protected:

	/**
     * ARROWSTATE indicates the state of the arrow buttons.
     * - CLEAR            No arrow is down.
     * - CLICKED_UP       Up arrow is clicked.
     * - CLICKED_DOWN     Down arrow is clicked.
     * - HELD_UP          Up arrow is held down for sustained period.
     * - HELD_DOWN        Down arrow is held down for sustained period.
	 */
    enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN };

    void UpdateThumbRect();
    void Cap();  // Clips position at boundaries. Ensures it stays within legal range.

    bool m_bShowThumb;
    bool m_bDrag;
    SRect m_rcUpButton;
    SRect m_rcDownButton;
    SRect m_rcTrack;
    SRect m_rcThumb;
    int m_nPosition;	///< Position of the first displayed item
    int m_nPageSize;	///< How many items are displayable in one page
    int m_nStart;		///< First item
    int m_nEnd;			///< The index after the last item
    SPoint m_LastMouse;	///< Last mouse position
    ARROWSTATE m_Arrow;	///< State of the arrows
    double m_dArrowTS;	///< Timestamp of last arrow event.
};

} // namespace amorphous



#endif /*  __GM_CONTROL_SCROLLBAR__  */
