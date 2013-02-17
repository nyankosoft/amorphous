#ifndef  __INPUTHANDLER_CUTSCENE_H__
#define  __INPUTHANDLER_CUTSCENE_H__

#include "Input/InputHandler.hpp"


namespace amorphous
{


class CCopyEntity;
class CBE_CameraController;

class InputHandler_Cutscene : public InputHandler
{
	CCopyEntity *m_pEntity;
	CBE_CameraController *m_pCameraController;

	void NormalizeAnalogInput( InputData& input );

public:

//	InputHandler_Cutscene();
	InputHandler_Cutscene( /*CCopyEntity *pEntity,*/ CBE_CameraController *pCameraController );

	~InputHandler_Cutscene();

	void ProcessInput(InputData& input);

	void SetEntity( CCopyEntity *pCameraControllerEntity ) { m_pEntity = pCameraControllerEntity; }

//	void SetDefaultKeyBind();

	// TODO: how to notidy when the keybinds have been changed
//	void UpdateKeyBind();

};

} // namespace amorphous



#endif		/*  __INPUTHANDLER_CUTSCENE_H__  */