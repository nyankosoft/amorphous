#ifndef  __GM_LISTBOXDESC_H__
#define  __GM_LISTBOXDESC_H__


#include "GM_ControlDescBase.hpp"


class CGM_ListBoxDesc : public CGM_ControlDesc
{
public:

	CGM_ListBoxDesc() { SetDefault(); }

	virtual void SetDefault()
	{
		CGM_ControlDesc::SetDefault();

		Style = 0;
		PageSize = 10;
		nSBWidth = 32;
		nTextHeight = 16;
		nBorder = 6;
		nMargin = 5;
		TextRect = SRect(0,0,0,0);
		SelectionRect = SRect(0,0,0,0);
	}

	virtual unsigned int GetType() { return CGM_Control::LISTBOX; }

public:

	/// Text rendering bound
	SRect TextRect;

	/// Selection box bound
	SRect SelectionRect;

//	CGM_ScrollBarDesc ScrollbarDesc;

	int PageSize;

	int nSBWidth;	///< width of the scroll bar

	int nBorder;

	int nMargin;

	int nTextHeight;///< Height of a single line of text

	int Style;		///< List box style flag

//	int nSelected;	// Index of the selected item for single selection list box

	int nSelStart;	// Index of the item where selection starts (for handling multi-selection)

	bool bDrag;	   // Whether the user is dragging the mouse to select

};



#endif  /*  __GM_LISTBOXDESC_H__  */
