#ifndef  __INPUTHANDLER_CUTSCENE_H__
#define  __INPUTHANDLER_CUTSCENE_H__

#include "GameInput/InputHandler.h"


class CSystemInputHandler_Debug;
class CCopyEntity;
class CBE_CameraController;

class CInputHandler_Cutscene : public CInputHandler
{
	CSystemInputHandler_Debug *m_pInputHandler_Debug;

	CCopyEntity *m_pEntity;
	CBE_CameraController *m_pCameraController;

	void NormalizeAnalogInput( SInputData& input );

public:

//	CInputHandler_Cutscene();
	CInputHandler_Cutscene( /*CCopyEntity *pEntity,*/ CBE_CameraController *pCameraController );

	~CInputHandler_Cutscene();

	void ProcessInput(SInputData& input);

	void SetEntity( CCopyEntity *pCameraControllerEntity ) { m_pEntity = pCameraControllerEntity; }

//	void SetDefaultKeyBind();

	// TODO: how to notidy when the keybinds have been changed
//	void UpdateKeyBind();

};


#endif		/*  __INPUTHANDLER_CUTSCENE_H__  */