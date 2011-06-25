#ifndef  __INPUTHANDLER_PLAYERBASE_H__
#define  __INPUTHANDLER_PLAYERBASE_H__

#include "gds/Input/InputHandler.hpp"
#include "gds/GameCommon/3DActionCode.hpp"


class CSystemInputHandler_Debug;

class CInputHandler_PlayerBase : public CInputHandler
{
protected:

	/// holds player actions for each general input code
	/// - mapping of GI codes -> 3D action codes
	/// - need to be updated when the keybinds are changed by the player
	///   - should be saved with the game save data
	/// - CInputHandler_PlayerPAC does not use this mapping
	///   - uses keybinds stored in SinglePlayerInfo()::m_KeyBind
	int m_aiActionForGICode[NUM_GENERAL_INPUT_CODES];

	/// holds the time of last inputs
	unsigned int m_adwLastInputTimeOfGICode[NUM_GENERAL_INPUT_CODES];

	float m_afActionState[NUM_ACTION_CODES];


	CSystemInputHandler_Debug *m_pInputHandler_Debug;

public:

	inline CInputHandler_PlayerBase();
	virtual ~CInputHandler_PlayerBase() {}

//	virtual void ProcessInput(SInputData& input);
//	void SetDefaultKeyBind();

	inline void ResetActionState();	// clear all the actions
	inline float GetActionState( int iActionCode );

};


//================================================================================================
// inline implementations
//================================================================================================


inline CInputHandler_PlayerBase::CInputHandler_PlayerBase()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
	{
		m_aiActionForGICode[i] = ACTION_NOT_ASSIGNED;
		m_adwLastInputTimeOfGICode[i] = 0;
	}
}


inline void CInputHandler_PlayerBase::ResetActionState()
{
	int i;
	for(i=0; i<NUM_ACTION_CODES; i++)
	{
		m_afActionState[i] = 0.0f;
	}
}


inline float CInputHandler_PlayerBase::GetActionState( int iActionCode )
{
	return m_afActionState[iActionCode];
}


#endif		/*  __INPUTHANDLER_PLAYER_H__  */