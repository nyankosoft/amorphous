#ifndef  __INPUTHANDLER_PLAYERBASE_H__
#define  __INPUTHANDLER_PLAYERBASE_H__

#include "gds/Input/InputHandler.hpp"
#include "gds/GameCommon/3DActionCode.hpp"


namespace amorphous
{


class InputHandler_PlayerBase : public InputHandler
{
protected:

	/// holds player actions for each general input code
	/// - mapping of GI codes -> 3D action codes
	/// - need to be updated when the keybinds are changed by the player
	///   - should be saved with the game save data
	/// - InputHandler_PlayerPAC does not use this mapping
	///   - uses keybinds stored in SinglePlayerInfo()::m_KeyBind
	int m_aiActionForGICode[NUM_GENERAL_INPUT_CODES];

	/// holds the time of last inputs
	unsigned int m_adwLastInputTimeOfGICode[NUM_GENERAL_INPUT_CODES];

	float m_afActionState[NUM_ACTION_CODES];

public:

	inline InputHandler_PlayerBase();
	virtual ~InputHandler_PlayerBase() {}

//	virtual void ProcessInput(InputData& input);
//	void SetDefaultKeyBind();

	inline void ResetActionState();	// clear all the actions
	inline float GetActionState( int iActionCode );

};


//================================================================================================
// inline implementations
//================================================================================================


inline InputHandler_PlayerBase::InputHandler_PlayerBase()
{
	for( int i=0; i<NUM_GENERAL_INPUT_CODES; i++ )
	{
		m_aiActionForGICode[i] = ACTION_NOT_ASSIGNED;
		m_adwLastInputTimeOfGICode[i] = 0;
	}
}


inline void InputHandler_PlayerBase::ResetActionState()
{
	int i;
	for(i=0; i<NUM_ACTION_CODES; i++)
	{
		m_afActionState[i] = 0.0f;
	}
}


inline float InputHandler_PlayerBase::GetActionState( int iActionCode )
{
	return m_afActionState[iActionCode];
}

} // namespace amorphous



#endif		/*  __INPUTHANDLER_PLAYER_H__  */