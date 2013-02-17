#ifndef  __INPUTHANDLER_PLAYERSHIP_H__
#define  __INPUTHANDLER_PLAYERSHIP_H__

#include "InputHandler_PlayerBase.hpp"


namespace amorphous
{


//#define NUM_PLAYERSHIP_ACTIONS 256


class InputHandler_PlayerShip : public InputHandler_PlayerBase
{
/*	/// holds player actions for each general input code 
	int m_aiActionForGICode[NUM_GENERAL_INPUT_CODES];

	/// holds the time of last inputs
	unsigned int m_adwLastInputTimeOfGICode[NUM_GENERAL_INPUT_CODES];

	float m_afActionState[NUM_PLAYERSHIP_ACTIONS];
*/

public:
	InputHandler_PlayerShip();
	~InputHandler_PlayerShip();

	void ProcessInput(InputData& input);
	void SetDefaultKeyBind();

//	inline void ResetActionState();	// clear all the actions
//	inline float GetActionState( int iActionCode );

};


//================================================================================================
// inline implementations
//================================================================================================
/*

inline void InputHandler_PlayerShip::ResetActionState()
{
	int i;
	for(i=0; i<NUM_PLAYERSHIP_ACTIONS; i++)
	{
		m_afActionState[i] = 0.0f;
	}
}

inline float InputHandler_PlayerShip::GetActionState( int iActionCode )
{
	return m_afActionState[iActionCode];
}*/

} // namespace amorphous



#endif		/*  __INPUTHANDLER_PLAYERSHIP_H__  */