#include "InputHandler_PlayerPAC.hpp"

#include "amorphous/Stage/PlayerInfo.hpp"
#include "amorphous/Stage/BE_Player.hpp"
#include "amorphous/Item/WeaponSystem.hpp"
#include "amorphous/Support/StringAux.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Task/GameTask.hpp"


namespace amorphous
{


int InputHandler_PlayerPAC::ms_TaskOnPause = GameTask::ID_INVALID;


InputHandler_PlayerPAC::InputHandler_PlayerPAC()
{
	// 13:22 2007/07/14 - keybind init moved to SetDefaultKeybindFG()
//	SetDefaultKeyBind();

	// copy keybind stored in PlayerInfo
//	UpdateKeyBind();

	ResetActionState();
}


InputHandler_PlayerPAC::~InputHandler_PlayerPAC()
{
}

/*
void InputHandler_PlayerPAC::UpdateKeyBind()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
		m_aiActionForGICode[i] = SinglePlayerInfo().KeyBind().GetActionCode(i);
}


void InputHandler_PlayerPAC::SetDefaultKeyBind()
{
	m_aiActionForGICode[GIC_UP]			= ACTION_MOV_PITCH_ACCEL_POS;
	m_aiActionForGICode[GIC_DOWN]		= ACTION_MOV_PITCH_ACCEL_NEG;
	m_aiActionForGICode[GIC_LEFT]		= ACTION_MOV_ROLL_ACCEL_POS;
	m_aiActionForGICode[GIC_RIGHT]		= ACTION_MOV_ROLL_ACCEL_NEG;

	m_aiActionForGICode[GIC_NUMPAD8]	= ACTION_MOV_PITCH_ACCEL_NEG;
	m_aiActionForGICode[GIC_NUMPAD5]	= ACTION_MOV_PITCH_ACCEL_POS;
	m_aiActionForGICode[GIC_NUMPAD4]	= ACTION_MOV_ROLL_ACCEL_POS;
	m_aiActionForGICode[GIC_NUMPAD6]	= ACTION_MOV_ROLL_ACCEL_NEG;

	m_aiActionForGICode[GIC_NUMPAD0]		= ACTION_MOV_BRAKE;
	m_aiActionForGICode[GIC_NUMPAD_ENTER]	= ACTION_MOV_BOOST;

	m_aiActionForGICode[GIC_TAB]		= ACTION_MISC_CYCLE_VIEWPOINTS;

	m_aiActionForGICode[GIC_ENTER]		= ACTION_MISC_CYCLE_TARGET_FOCUS;

	m_aiActionForGICode['Z']			= ACTION_ATK_FIRE;
//	m_aiActionForGICode['X']			= ;

	m_aiActionForGICode[GIC_SPACE]		= ACTION_ATK_FIRE;
//	m_aiActionForGICode[GIC_NOCONVERT]	= ;
//	m_aiActionForGICode[GIC_BACK]		= ;
//	m_aiActionForGICode[GIC_LSHIFT]		= ;
	m_aiActionForGICode['N']			= ACTION_MISC_TOGGLE_NIGHTVISION;
//	m_aiActionForGICode['C']			= ACTION_MISC_TOGGLE_HEADLIGHT;
	m_aiActionForGICode['C']			= ACTION_MISC_CYCLE_WEAPON;

	m_aiActionForGICode['A']			= ACTION_ARMS_SELECTWEAPON1;
	m_aiActionForGICode['Q']			= ACTION_ARMS_SELECTWEAPON2;
	m_aiActionForGICode['T']			= ACTION_ARMS_SELECTWEAPON3;
	m_aiActionForGICode['G']			= ACTION_ARMS_SELECTWEAPON4;
	m_aiActionForGICode['V']			= ACTION_ARMS_SELECTWEAPON5;

	m_aiActionForGICode[GIC_MOUSE_BUTTON_L] = ACTION_ATK_FIRE;
//	m_aiActionForGICode[GIC_MOUSE_BUTTON_R] = ACTION_ATK_RAISEWEAPON;
	m_aiActionForGICode[GIC_MOUSE_BUTTON_R] = ACTION_ATK_UNLOCK_TRIGGER_SAFETY;
	m_aiActionForGICode[GIC_MOUSE_BUTTON_M] = ACTION_QMENU_OPEN;

//	m_aiActionForGICode[GIC_MOUSE_AXIS_X]   = ACTION_MOV_ROLL_ACCEL_NEG;
//	m_aiActionForGICode[GIC_MOUSE_AXIS_Y]   = ACTION_MOV_PITCH_ACCEL_NEG;

	m_aiActionForGICode[GIC_GPD_AXIS_X]		= ACTION_MOV_ROLL_ACCEL_NEG;
	m_aiActionForGICode[GIC_GPD_AXIS_Y]		= ACTION_MOV_PITCH_ACCEL_NEG;

	m_aiActionForGICode[GIC_GPD_AXIS_Z]		= ACTION_MOV_LOOK_RIGHT;
	m_aiActionForGICode[GIC_GPD_ROTATION_Z]	= ACTION_MOV_LOOK_UP;

	m_aiActionForGICode[GIC_GPD_BUTTON_00]	= ACTION_MISC_CYCLE_TARGET_FOCUS;
	m_aiActionForGICode[GIC_GPD_BUTTON_09]	= ACTION_MISC_CYCLE_WEAPON;
	m_aiActionForGICode[GIC_GPD_BUTTON_10]	= ACTION_MISC_CYCLE_VIEWPOINTS;
	m_aiActionForGICode[GIC_GPD_BUTTON_11]	= ACTION_MISC_CYCLE_VIEWPOINTS;

	m_aiActionForGICode[GIC_GPD_BUTTON_01]	= ACTION_ATK_FIRE;
	m_aiActionForGICode[GIC_GPD_BUTTON_03]	= ACTION_MISC_HOLD_RADAR;
	m_aiActionForGICode[GIC_GPD_BUTTON_04]	= ACTION_MOV_YAW_ACCEL_NEG;
	m_aiActionForGICode[GIC_GPD_BUTTON_05]	= ACTION_MOV_YAW_ACCEL_POS;

	m_aiActionForGICode[GIC_GPD_BUTTON_06]	= ACTION_MOV_BRAKE;
	m_aiActionForGICode[GIC_GPD_BUTTON_07]	= ACTION_MOV_BOOST;

	m_aiActionForGICode[GIC_MOUSE_WHEEL_UP]   = ACTION_QMENU_UP;
	m_aiActionForGICode[GIC_MOUSE_WHEEL_DOWN] = ACTION_QMENU_DOWN;

}
*/

void InputHandler_PlayerPAC::ProcessInput( InputData& input )
{
	CBE_Player* pPlayerBaseEntity = SinglePlayerInfo().GetCurrentPlayerBaseEntity();

	if( !pPlayerBaseEntity )
		return;

	if( pPlayerBaseEntity->GetPlayerCopyEntity()->fLife <= 0 )
		return;

	// get the action code for the GI code
//	int action_code = m_aiActionForGICode[input.iGICode];
	int action_code           = SinglePlayerInfo().KeyBind().GetActionCode( input.iGICode, KeyBind::ACTION_TYPE_PRIMARY );
	int secondary_action_code = SinglePlayerInfo().KeyBind().GetActionCode( input.iGICode, KeyBind::ACTION_TYPE_SECONDARY );

	float fParam = input.fParam1;

	// process the fixed-keybind(non-configurable) controls
	const float gpd_margin = 0.2f;
	if( GIC_GPD_AXIS_X <= input.iGICode && input.iGICode <= GIC_GPD_AXIS_Z ||
		GIC_GPD_ROTATION_X <= input.iGICode && input.iGICode <= GIC_GPD_ROTATION_Z )
	{
		// set fParam to 0 if the absolute value of fParam is less that the margin
		if( fabsf(fParam) < gpd_margin )
			fParam = 0;
		else
		{
			float param_sign = fParam / fabsf(fParam);
			fParam	= ( fParam - gpd_margin * param_sign ) / (1.0f - gpd_margin) * 1.0f;
		}

	}
	else if( input.iGICode == GIC_MOUSE_AXIS_X || input.iGICode == GIC_MOUSE_AXIS_Y )
	{
		fParam *= 0.2f;

//		static count = 0;
//		count = (count+1) % 100;
//		if( count == 0 ) PrintLog( "axis(" + to_string(input.iGICode) + ") : " + to_string(fParam) );
	}

	switch( action_code )
	{
	case ACTION_MOV_PITCH_ACCEL_POS: action_code = ACTION_MOV_PITCH_ACCEL; break;
	case ACTION_MOV_PITCH_ACCEL_NEG: action_code = ACTION_MOV_PITCH_ACCEL; fParam *= -1.0f; break;
	case ACTION_MOV_ROLL_ACCEL_POS:  action_code = ACTION_MOV_ROLL_ACCEL;  break;
	case ACTION_MOV_ROLL_ACCEL_NEG:  action_code = ACTION_MOV_ROLL_ACCEL;  fParam *= -1.0f; break;
	case ACTION_MOV_YAW_ACCEL_POS:   action_code = ACTION_MOV_YAW_ACCEL;   break;
	case ACTION_MOV_YAW_ACCEL_NEG:   action_code = ACTION_MOV_YAW_ACCEL;   fParam *= -1.0f; break;
	}

	// create the player action, which is close to the original input data,
	// yet made more specific
	SPlayerEntityAction action_input;
	action_input.ActionCode          = action_code;
	action_input.SecondaryActionCode = secondary_action_code;
	action_input.fParam              = fParam;
	action_input.type                = input.iType;

	if( input.iType == ITYPE_KEY_PRESSED
	 || input.iType == ITYPE_VALUE_CHANGED )
	{
		m_afActionState[action_code] = fParam;
	}
	else if( input.iType == ITYPE_KEY_RELEASED )
	{
		m_afActionState[action_code] = 0;
	}

	// check if there is an item the player is using right now
/*	if( SinglePlayerInfo().GetFocusedItem() )
	{
		if( SinglePlayerInfo().GetFocusedItem()->HandleInput( action_code, input.iType, input.fParam1 ) )
			return;
	}*/

	// let the player entity handle the input
	if( pPlayerBaseEntity->HandleInput( action_input ) )
		return;

	// input has not been handled yet

	CWeaponSystem *pWeaponSystem = SinglePlayerInfo().GetWeaponSystem();

	// let the currently selected weapon handle the input
	if( pWeaponSystem && pWeaponSystem->HandleInput( action_code, input.iType, input.fParam1 ) )
		return;

	switch( action_code )
	{
/*
	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input.iType == ITYPE_KEY_PRESSED )
			WEAPONSYSTEM.SetTriggerState( 1, 1 );
		else if( input.iType == ITYPE_KEY_RELEASED )
			WEAPONSYSTEM.SetTriggerState( 1, 0 );
		break;
*/
	case ACTION_ARMS_SELECTWEAPON1:
		if( input.iType == ITYPE_KEY_PRESSED && pWeaponSystem )
			pWeaponSystem->SelectPrimaryWeapon( 0 );
		break;
	case ACTION_ARMS_SELECTWEAPON2:
		if( input.iType == ITYPE_KEY_PRESSED && pWeaponSystem )
			pWeaponSystem->SelectPrimaryWeapon( 1 );
		break;
	case ACTION_ARMS_SELECTWEAPON3:
		if( input.iType == ITYPE_KEY_PRESSED && pWeaponSystem )
			pWeaponSystem->SelectPrimaryWeapon( 2 );
		break;
	case ACTION_ARMS_SELECTWEAPON4:
		if( input.iType == ITYPE_KEY_PRESSED && pWeaponSystem )
			pWeaponSystem->SelectPrimaryWeapon( 3 );
		break;
	case ACTION_ARMS_SELECTWEAPON5:
		if( input.iType == ITYPE_KEY_PRESSED && pWeaponSystem )
			pWeaponSystem->SelectPrimaryWeapon( 4 );
		break;

	case ACTION_NOT_ASSIGNED:
		break;	// no action is assigned to this input
	}

	switch( input.iGICode )
	{
	case GIC_ESC:
	case GIC_GPD_BUTTON_08:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			SinglePlayerInfo().RequestTaskChange( GameTask::ID_MAIN_MENU );
//			SinglePlayerInfo().RequestTaskChange( GameTaskFG::ID_INSTAGEMENU_FG );
			SinglePlayerInfo().RequestTaskChange( ms_TaskOnPause );
		}
		break;
	}
}


} // namespace amorphous
