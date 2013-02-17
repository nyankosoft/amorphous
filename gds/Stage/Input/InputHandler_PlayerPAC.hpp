#ifndef  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__
#define  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__

#include "InputHandler_PlayerBase.hpp"


namespace amorphous
{


class InputHandler_PlayerPAC : public InputHandler_PlayerBase
{

public:
	InputHandler_PlayerPAC();
	~InputHandler_PlayerPAC();

	void ProcessInput(InputData& input);
	void SetDefaultKeyBind();

	// TODO: how to notidy when the keybinds have been changed
	void UpdateKeyBind();

	static int ms_TaskOnPause;
};

} // namespace amorphous



#endif		/*  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__  */
