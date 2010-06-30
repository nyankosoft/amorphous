#ifndef  __GM_INPUT_H__
#define  __GM_INPUT_H__


#include "Graphics/Point.hpp"
using namespace Graphics;


class CGM_Input
{
public:

	enum InputCode
	{
		INVALID,
		OK,
		CANCEL,
//		CLICK,
//		DOUBLE_CLICK, // error - already taken
//		CONTROL_SELECTED,
		CLOSE_CURRENT_DIALOG,
		CLOSE_ROOT_DIALOG,
		OPEN_ROOT_DIALOG,
		OPEN_NEXT_DIALOG, ///< close the current dialog and open the next one
		OPEN_PREV_DIALOG, ///< close the current dialog and open the previous one

		// general codes for focus shifting
		// - used for both controls and list box items
		SHIFT_FOCUS_RIGHT, ///< shift the focus to the next control on the right
		SHIFT_FOCUS_LEFT,  ///< shift the focus to the next control on the left
		SHIFT_FOCUS_DOWN,
		SHIFT_FOCUS_UP,

		// for controls only
		// - used to quickly change focuses between multiple list boxes
		NEXT_CONTROL_RIGHT, ///< shift the focus to the next control on the right
		NEXT_CONTROL_LEFT,  ///< shift the focus to the next control on the left
		NEXT_CONTROL_DOWN,
		NEXT_CONTROL_UP,

		NEXT_DIALOG_RIGHT,
		NEXT_DIALOG_LEFT,
		NEXT_DIALOG_DOWN,
		NEXT_DIALOG_UP,

		INCREASE_SLIDER_VALUE,
		DECREASE_SLIDER_VALUE,

//		NEXT_LISTBOX_ITEM_RIGHT,
//		NEXT_LISTBOX_ITEM_LEFT,
//		NEXT_LISTBOX_ITEM_DOWN,
//		NEXT_LISTBOX_ITEM_UP,

		MOUSE_AXIS_X,
		MOUSE_AXIS_Y,
		MOUSE_BUTTON_L,
		MOUSE_BUTTON_R,
		MOUSE_BUTTON_M,

		SCROLL_DOWN,
		SCROLL_UP,

		NUM_INPUT_CODES
	};
};


class CGM_InputData
{
public:

	int code;
	int type; // pressed / released
	float fParam;
//	int x,y;
	SPoint pos;

	/// Retain the original general input code so that each control
	/// can interpret the input data for its own purpose.
	int GeneralInputCode;

	enum eType
	{
		TYPE_PRESSED,
		TYPE_RELEASED,
		NUM_TYPES
	};

	enum eParams
	{
		MIN_VALID_MOUSE_COORD = -1000
	};

public:

	CGM_InputData()
		:
	code(0),
	type(0),
	fParam(0),
	pos( SPoint( MIN_VALID_MOUSE_COORD, MIN_VALID_MOUSE_COORD ) ),
	GeneralInputCode(-1)
	{}

	bool IsMouseInput() { return ( MIN_VALID_MOUSE_COORD < pos.x && MIN_VALID_MOUSE_COORD < pos.y ); }
};


#endif		/*  __GM_INPUT_H__  */
