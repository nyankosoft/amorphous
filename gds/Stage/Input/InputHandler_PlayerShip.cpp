
#include "InputHandler_PlayerShip.hpp"

#include "Stage/PlayerInfo.hpp"
#include "Stage/HUD_PlayerBase.hpp"
#include "Stage/BE_Player.hpp"
//#include "Stage/BE_PlayerShip.hpp"
#include "Item/WeaponSystem.hpp"

#include "Task/GameTask.hpp"

#include "Stage/SystemInputHandler_Debug.hpp"



CInputHandler_PlayerShip::CInputHandler_PlayerShip()
{
/*	int i;
	for( i=0; i<256; i++ )
	{
		m_aiActionForGICode[i] = ACTION_NOT_ASSIGNED;
		m_adwLastInputTimeOfGICode[i] = 0;
	}
*/
	SetDefaultKeyBind();

	ResetActionState();


	m_pInputHandler_Debug = new CSystemInputHandler_Debug;
}


CInputHandler_PlayerShip::~CInputHandler_PlayerShip()
{
	if( m_pInputHandler_Debug )
		delete m_pInputHandler_Debug;
}


void CInputHandler_PlayerShip::SetDefaultKeyBind()
{
	m_aiActionForGICode['E']			= ACTION_MOV_FORWARD;
	m_aiActionForGICode['S']			= ACTION_MOV_SLIDE_L;
	m_aiActionForGICode['D']			= ACTION_MOV_BACKWARD;
	m_aiActionForGICode['F']			= ACTION_MOV_SLIDE_R;
	m_aiActionForGICode[GIC_SPACE]		= ACTION_MOV_SLIDE_UP;
	m_aiActionForGICode[GIC_NOCONVERT]	= ACTION_MOV_SLIDE_DOWN;
	m_aiActionForGICode[GIC_BACK]		= ACTION_MOV_CROUCH;
	m_aiActionForGICode[GIC_LSHIFT]		= ACTION_MOV_BOOST;

	m_aiActionForGICode[GIC_UP]			= ACTION_MOV_FORWARD;
	m_aiActionForGICode[GIC_LEFT]		= ACTION_MOV_SLIDE_L;
	m_aiActionForGICode[GIC_DOWN]		= ACTION_MOV_BACKWARD;
	m_aiActionForGICode[GIC_RIGHT]		= ACTION_MOV_SLIDE_R;
	m_aiActionForGICode[GIC_NUMPAD1]	= ACTION_MOV_TURN_L;
	m_aiActionForGICode[GIC_NUMPAD3]	= ACTION_MOV_TURN_R;

	m_aiActionForGICode['Z']			= ACTION_MOV_TURN_L;
	m_aiActionForGICode['X']			= ACTION_MOV_TURN_R;

	m_aiActionForGICode[GIC_NUMPAD0]	= ACTION_ATK_FIRE;
	
	m_aiActionForGICode['A']			= ACTION_ARMS_SELECTWEAPON1;
	m_aiActionForGICode['Q']			= ACTION_ARMS_SELECTWEAPON2;
	m_aiActionForGICode['T']			= ACTION_ARMS_SELECTWEAPON3;
	m_aiActionForGICode['G']			= ACTION_ARMS_SELECTWEAPON4;
	m_aiActionForGICode['V']			= ACTION_ARMS_SELECTWEAPON5;

	m_aiActionForGICode[GIC_MOUSE_BUTTON_L] = ACTION_ATK_FIRE;
//	m_aiActionForGICode[GIC_MOUSE_BUTTON_R] = ACTION_ATK_RAISEWEAPON;
	m_aiActionForGICode[GIC_MOUSE_BUTTON_R] = ACTION_ATK_UNLOCK_TRIGGER_SAFETY;
///	m_aiActionForGICode[GIC_MOUSE_BUTTON_M] = ACTION_ATK_FIRE;
	m_aiActionForGICode[GIC_MOUSE_BUTTON_M] = ACTION_QMENU_OPEN;
//	m_aiActionForGICode[GIC_MOUSE_AXIS_X]   = ACTION_MOV_ABS_YAW;
//	m_aiActionForGICode[GIC_MOUSE_AXIS_Y]   = ACTION_MOV_ABS_PITCH;
	m_aiActionForGICode[GIC_MOUSE_AXIS_X]   = ACTION_MOV_YAW;
	m_aiActionForGICode[GIC_MOUSE_AXIS_Y]   = ACTION_MOV_PITCH;

///	m_aiActionForGICode[GIC_MOUSE_WHEEL_UP]   = ACTION_MISC_CAMERA_ZOOMUP;
///	m_aiActionForGICode[GIC_MOUSE_WHEEL_DOWN] = ACTION_MISC_CAMERA_ZOOMOUT;

	m_aiActionForGICode[GIC_MOUSE_WHEEL_UP]   = ACTION_QMENU_UP;
	m_aiActionForGICode[GIC_MOUSE_WHEEL_DOWN] = ACTION_QMENU_DOWN;

	m_aiActionForGICode['C'] = ACTION_MISC_TOGGLE_HEADLIGHT;
}


void CInputHandler_PlayerShip::ProcessInput( SInputData& input )
{
	CBE_Player* pPlayerBaseEntity = SinglePlayerInfo().GetCurrentPlayerBaseEntity();

	int action_code = m_aiActionForGICode[input.iGICode];

	SPlayerEntityAction action_input;
	action_input.ActionCode = action_code;

	if( input.iType == ITYPE_KEY_PRESSED )
	{
		m_afActionState[action_code] = input.fParam1;
		action_input.type = SPlayerEntityAction::KEY_PRESSED;
	}
	else if( input.iType == ITYPE_KEY_RELEASED )
	{
		m_afActionState[action_code] = 0;
		action_input.type = SPlayerEntityAction::KEY_RELEASED;
	}

	action_input.fParam = input.fParam1;

	// check if there is an item the player is using right now
	if( SinglePlayerInfo().GetFocusedItem() )
	{
		if( SinglePlayerInfo().GetFocusedItem()->HandleInput( action_code, input.iType, input.fParam1 ) )
			return;
	}

	// let the quick menu handle the input first
	// if the input is used for menu operation, return
//	if( SinglePlayerInfo().GetHUD()->HandleInput( action_code, input.iType, input.fParam1 ) )
//		return;

	// let the player entity handle the input
	if( SinglePlayerInfo().GetCurrentPlayerBaseEntity()->HandleInput( action_input ) )
		return;

	// input has not been handled yet

	CWeaponSystem *pWeaponSystem = SinglePlayerInfo().GetWeaponSystem();

	// let the currently selected weapon handle the input
	if( pWeaponSystem->HandleInput( action_code, input.iType, input.fParam1 ) )
		return;

	switch( action_code )
	{
/*	case ACTION_ATK_FIRE:
		if( input.iType == ITYPE_KEY_PRESSED )
			WEAPONSYSTEM.SetTriggerState( 0, 1 );
		else if( input.iType == ITYPE_KEY_RELEASED )
			WEAPONSYSTEM.SetTriggerState( 0, 0 );
		break;

	case ACTION_ATK_RAISEWEAPON:
	case ACTION_ATK_UNLOCK_TRIGGER_SAFETY:
		if( input.iType == ITYPE_KEY_PRESSED )
			WEAPONSYSTEM.SetTriggerState( 1, 1 );
		else if( input.iType == ITYPE_KEY_RELEASED )
			WEAPONSYSTEM.SetTriggerState( 1, 0 );
		break;
*/
	case ACTION_ARMS_SELECTWEAPON1:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 0 );
		break;
	case ACTION_ARMS_SELECTWEAPON2:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 1 );
		break;
	case ACTION_ARMS_SELECTWEAPON3:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 2 );
		break;
	case ACTION_ARMS_SELECTWEAPON4:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 3 );
		break;
	case ACTION_ARMS_SELECTWEAPON5:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 4 );
		break;
	case ACTION_ARMS_SELECTWEAPON6:
		if( input.iType == ITYPE_KEY_PRESSED )
			pWeaponSystem->SelectPrimaryWeapon( 5 );
		break;

	case ACTION_MISC_CAMERA_ZOOMOUT:
//		if( input.iType == ITYPE_KEY_PRESSED )
//			WEAPONSYSTEM.SelectWeaponSlot( (WEAPONSYSTEM.GetCurrentWeaponSlotID() + 1) % 6 );
		break;

	case ACTION_MISC_CAMERA_ZOOMUP:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			i = (WEAPONSYSTEM.GetCurrentWeaponSlotID() - 1);
			if( i<0 ) i = 5;
			WEAPONSYSTEM.SelectWeaponSlot( i );
		}*/
		break;

	case ACTION_NOT_ASSIGNED:
		break;	// no action is assigned to this input
	}

	switch( input.iGICode )
	{
	case GIC_ESC:
		SinglePlayerInfo().RequestTaskChange( CGameTask::ID_MAIN_MENU );
		break;
	}

	if( m_pInputHandler_Debug )
		m_pInputHandler_Debug->ProcessInput( input );


}