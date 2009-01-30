#ifndef  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__
#define  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__

#include "InputHandler_PlayerBase.hpp"


class CSystemInputHandler_Debug;

class CInputHandler_PlayerPAC : public CInputHandler_PlayerBase
{

public:
	CInputHandler_PlayerPAC();
	~CInputHandler_PlayerPAC();

	void ProcessInput(SInputData& input);
	void SetDefaultKeyBind();

	// TODO: how to notidy when the keybinds have been changed
	void UpdateKeyBind();

};


#endif		/*  __INPUTHANDLER_PLAYERPSEUDOAIRCRAFT_H__  */