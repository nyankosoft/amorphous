#ifndef  __INPUTHANDLER_PLAYERSHIP_H__
#define  __INPUTHANDLER_PLAYERSHIP_H__

#include "InputHandler_PlayerBase.h"


//#define NUM_PLAYERSHIP_ACTIONS 256

class CSystemInputHandler_Debug;

class CInputHandler_PlayerShip : public CInputHandler_PlayerBase
{
/*	/// holds player actions for each general input code 
	int m_aiActionForGICode[NUM_GENERAL_INPUT_CODES];

	/// holds the time of last inputs
	unsigned int m_adwLastInputTimeOfGICode[NUM_GENERAL_INPUT_CODES];

	float m_afActionState[NUM_PLAYERSHIP_ACTIONS];

	CSystemInputHandler_Debug *m_pInputHandler_Debug;
*/

public:
	CInputHandler_PlayerShip();
	~CInputHandler_PlayerShip();

	void ProcessInput(SInputData& input);
	void SetDefaultKeyBind();

//	inline void ResetActionState();	// clear all the actions
//	inline float GetActionState( int iActionCode );

};


//================================================================================================
// inline implementations
//================================================================================================
/*

inline void CInputHandler_PlayerShip::ResetActionState()
{
	int i;
	for(i=0; i<NUM_PLAYERSHIP_ACTIONS; i++)
	{
		m_afActionState[i] = 0.0f;
	}
}

inline float CInputHandler_PlayerShip::GetActionState( int iActionCode )
{
	return m_afActionState[iActionCode];
}*/


#endif		/*  __INPUTHANDLER_PLAYERSHIP_H__  */