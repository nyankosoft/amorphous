#include "GM_Keybind.hpp"


void CGM_CompleteKeybind::SetDefault()
{
	m_aGICodeToGMInputCode[GIC_ENTER]            = CGM_Input::OK;
	m_aGICodeToGMInputCode[GIC_SPACE]            = CGM_Input::OK;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_00]    = CGM_Input::OK;

	m_aGICodeToGMInputCode[GIC_MOUSE_BUTTON_L]   = CGM_Input::MOUSE_BUTTON_L;
	m_aGICodeToGMInputCode[GIC_MOUSE_AXIS_X]     = CGM_Input::MOUSE_AXIS_X;
	m_aGICodeToGMInputCode[GIC_MOUSE_AXIS_Y]     = CGM_Input::MOUSE_AXIS_Y;

	m_aGICodeToGMInputCode[GIC_BACK]             = CGM_Input::CANCEL;
//	m_aGICodeToGMInputCode[GIC_ESC]              = CGM_Input::CANCEL;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_01]    = CGM_Input::CANCEL;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_02]    = CGM_Input::CANCEL;

	m_aGICodeToGMInputCode[GIC_DOWN]             = CGM_Input::SHIFT_FOCUS_DOWN;
	m_aGICodeToGMInputCode[GIC_UP]               = CGM_Input::SHIFT_FOCUS_UP;
	m_aGICodeToGMInputCode[GIC_RIGHT]            = CGM_Input::SHIFT_FOCUS_RIGHT;
	m_aGICodeToGMInputCode[GIC_LEFT]             = CGM_Input::SHIFT_FOCUS_LEFT;

	m_aGICodeToGMInputCode[GIC_GPD_DOWN]         = CGM_Input::SHIFT_FOCUS_DOWN;
	m_aGICodeToGMInputCode[GIC_GPD_UP]           = CGM_Input::SHIFT_FOCUS_UP;
	m_aGICodeToGMInputCode[GIC_GPD_RIGHT]        = CGM_Input::SHIFT_FOCUS_RIGHT;
	m_aGICodeToGMInputCode[GIC_GPD_LEFT]         = CGM_Input::SHIFT_FOCUS_LEFT;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_00]    = CGM_Input::CANCEL;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_01]    = CGM_Input::OK;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_02]    = CGM_Input::CANCEL;
	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_03]    = CGM_Input::OK;
//	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_04]    = CGM_Input::SHIFT_FOCUS_LEFT;
//	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_05]    = CGM_Input::SHIFT_FOCUS_LEFT;
//	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_06]    = CGM_Input::SHIFT_FOCUS_LEFT;
//	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_07]    = CGM_Input::SHIFT_FOCUS_LEFT;
//	m_aGICodeToGMInputCode[GIC_GPD_BUTTON_08]    = CGM_Input::SHIFT_FOCUS_LEFT;

//	m_aGICodeToGMInputCode[GIC_MOUSE_WHEEL_DOWN] = CGM_Input::SHIFT_FOCUS_DOWN;
//	m_aGICodeToGMInputCode[GIC_MOUSE_WHEEL_UP]   = CGM_Input::SHIFT_FOCUS_UP;

	m_aGICodeToGMInputCode[GIC_MOUSE_WHEEL_DOWN] = CGM_Input::SCROLL_DOWN;
	m_aGICodeToGMInputCode[GIC_MOUSE_WHEEL_UP]   = CGM_Input::SCROLL_UP;

//	m_aGICodeToGMInputCode[GIC_TAB]              = CGM_Input::NEXT_DIALOG_DOWN;
//	m_aGICodeToGMInputCode[GIC_TAB]              = CGM_Input::NEXT_DIALOG_UP;
	m_aGICodeToGMInputCode[GIC_TAB]              = CGM_Input::NEXT_DIALOG_RIGHT;
//	m_aGICodeToGMInputCode[GIC_TAB]              = CGM_Input::NEXT_DIALOG_LEFT;
}
