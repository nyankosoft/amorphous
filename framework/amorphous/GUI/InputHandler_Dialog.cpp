
#include "InputHandler_Dialog.hpp"

#include "GM_DialogManager.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{


CInputHandler_Dialog::CInputHandler_Dialog( CGM_DialogManager *pDialogManager )
{
	m_pDialogManager = pDialogManager;

	m_bPrevInputProcessed = false;

	SetDefaultKeyBindsForGeneralInputCodes();

	EnableAutoRepeat( true );
}


CInputHandler_Dialog::CInputHandler_Dialog( CGM_DialogManagerSharedPtr pDialogManager )
{
	m_pDialogManager = pDialogManager.get();

	m_bPrevInputProcessed = false;

	SetDefaultKeyBindsForGeneralInputCodes();

	EnableAutoRepeat( true );
}


CInputHandler_Dialog::~CInputHandler_Dialog()
{
}


void CInputHandler_Dialog::ProcessInput(InputData& input)
{
	if( input.IsGamepadInput() )
		input = m_GPD2KBDInputConterter.Convert( input );

	// convert input data to CGM_InputData

	CGM_InputData gm_input;

	gm_input.code = m_aGICodeToGMInputCode[input.iGICode];

	switch(input.iType)
	{
	case ITYPE_KEY_PRESSED:  gm_input.type = CGM_InputData::TYPE_PRESSED;  break;
	case ITYPE_KEY_RELEASED: gm_input.type = CGM_InputData::TYPE_RELEASED; break;
	case ITYPE_VALUE_CHANGED:
		return;
	default:
		LOG_PRINT_ERROR( fmt_string( " An invalid input type (input.iType = %d)", input.iType ) );
		break;
	}

	gm_input.fParam = input.fParam1;

//	gm_input.uiParam = input.uiParam;

	if( input.IsMouseInput() )
	{
		// set position only if it is a mouse input
        gm_input.pos = SPoint( input.GetParamH16(), input.GetParamL16() );
	}

	m_bPrevInputProcessed = m_pDialogManager->HandleInput( gm_input );
}


void CInputHandler_Dialog::SetDefaultKeyBindsForGeneralInputCodes()
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


void CInputHandler_Dialog::ClearKeyBinds()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
	{
		m_aGICodeToGMInputCode[i] = CGM_Input::INVALID;
	}
}


} // namespace amorphous
