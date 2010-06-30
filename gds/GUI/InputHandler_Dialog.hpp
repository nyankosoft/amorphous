#ifndef  __INPUTHANDLER_DIALOG_H__
#define  __INPUTHANDLER_DIALOG_H__


#include "fwd.hpp"
#include "GM_Input.hpp"
#include "../Input/InputHandler.hpp"

#include <math.h>


/**
 * converts gamepad input into keyboard input
 */
class GPD2KBDInputConverter
{
	enum eAnalogKey
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		ROT_X,
		ROT_Y,
		ROT_Z,
		NUM_ANALOG_KEYS
	};

	bool m_bPrevHold[NUM_ANALOG_KEYS];

	inline bool GetNewAnalogKeyInput( int analog_key, float param, SInputData& dest_input );

public:

	GPD2KBDInputConverter() { for( int i=0; i<NUM_ANALOG_KEYS; i++ ) m_bPrevHold[i] = false; }

	inline SInputData Convert( const SInputData& gpd_input );
};


/// Returns true if change in analog axis input should be treated as pressing or releasing of a key
inline bool GPD2KBDInputConverter::GetNewAnalogKeyInput( int analog_key,
														 float param,
														 SInputData& dest_input )
{
	const float& dirkey_threshold = 0.7f;

	bool prev_hold = m_bPrevHold[analog_key];
	bool hold = dirkey_threshold < fabsf(param) ? true : false;

	m_bPrevHold[analog_key] = hold;

	if( !prev_hold && hold )
	{
		dest_input.iType = ITYPE_KEY_PRESSED;
		dest_input.fParam1 = 1.0f;
		return true;
	}
	else if( prev_hold && !hold )
	{
		dest_input.iType = ITYPE_KEY_RELEASED;
		dest_input.fParam1 = 0.0f;
		return true;
	}
	else
		return false;
}


inline SInputData GPD2KBDInputConverter::Convert( const SInputData& gpd_input )
{
	SInputData kbd_input = gpd_input;

	const float& gpd_param = gpd_input.fParam1;

	switch( gpd_input.iGICode )
	{
/*	case GIC_GPD_UP:
		kbd_input.iGICode = GIC_UP;
		break;
	case GIC_GPD_DOWN:
		kbd_input.iGICode = GIC_DOWN;
		break;
	case GIC_GPD_RIGHT:
		kbd_input.iGICode = GIC_RIGHT;
		break;
	case GIC_GPD_LEFT:
		kbd_input.iGICode = GIC_LEFT;
		break;*/
/*	case GIC_GPD_BUTTON_01:
	case GIC_GPD_BUTTON_03:
		kbd_input.iGICode = GIC_ENTER;
		break;
	case GIC_GPD_BUTTON_00:
	case GIC_GPD_BUTTON_02:
		kbd_input.iGICode = GIC_BACK;
		break;*/
	case GIC_GPD_AXIS_X:
		if( GetNewAnalogKeyInput( AXIS_X, gpd_param, kbd_input ) )
		{
			kbd_input.iGICode = 0.0f < gpd_param ? GIC_RIGHT : GIC_LEFT;
			return kbd_input;
		}
		break;	// TODO: report as no input 
	case GIC_GPD_AXIS_Y:
		if( GetNewAnalogKeyInput( AXIS_Y, gpd_param, kbd_input ) )
		{
			kbd_input.iGICode = 0.0f < gpd_param ? GIC_DOWN : GIC_UP;
			return kbd_input;
		}
		break;	// TODO: report as no input
/*	case GIC_GPD_:
		kbd_input.iGICode = GIC_;
		break;*/
	}

	return kbd_input;
}


/**
 CInputHandler_Dialog
 - Sends input data to dialog manager
 - Auto repeat is enabled by default
 */
class CInputHandler_Dialog : public CInputHandler
{
	/// UI manager that uses inputs sent from this input handler.
	/// - borrowed reference
	CGM_DialogManager *m_pDialogManager;

	GPD2KBDInputConverter m_GPD2KBDInputConterter;

	bool m_bPrevInputProcessed;

	int m_aGICodeToGMInputCode[NUM_GENERAL_INPUT_CODES];

public:

	CInputHandler_Dialog( CGM_DialogManager *pDialogManager );

	CInputHandler_Dialog( CGM_DialogManagerSharedPtr pDialogManager );

	virtual ~CInputHandler_Dialog();

	virtual void ProcessInput(SInputData& input);

	bool PrevInputProcessed() { return m_bPrevInputProcessed; }

	/// change the target ui manager
	void SetDialogManager( CGM_DialogManager *pDialogManager ) { m_pDialogManager = pDialogManager; }

	void SetDialogManager( CGM_DialogManagerSharedPtr pDialogManager ) { m_pDialogManager = pDialogManager.get(); }

	void SetDefaultKeyBindsForGeneralInputCodes();

	void ClearKeyBinds();

	inline void BindGICodeToGMInputCode( int gi_code, int gm_input_code );
};


// ============================= inline implementations =============================

inline void CInputHandler_Dialog::BindGICodeToGMInputCode( int gi_code, int gm_input_code )
{
	if( gi_code < 0 || NUM_GENERAL_INPUT_CODES <= gi_code )
		return;

	m_aGICodeToGMInputCode[gi_code] = gm_input_code;
}



#endif		/*  __INPUTHANDLER_DIALOG_H__  */
