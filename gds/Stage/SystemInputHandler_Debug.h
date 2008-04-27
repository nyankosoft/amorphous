#ifndef  __SYSTEMINPUTHANDLER_DEBUG_H__
#define  __SYSTEMINPUTHANDLER_DEBUG_H__

#include "GameInput/InputHandler.h"

extern bool g_bAppExitRequested;

class CStage;

class CSystemInputHandler_Debug : public CInputHandler
{


	// screen size management variables
	int m_aiScreenWidth[5];
	int m_aiScreenHeight[5];
	bool m_abFullscreen[5];
	int m_iScreenSize;


public:
	CSystemInputHandler_Debug();
	~CSystemInputHandler_Debug();

	void ProcessInput(SInputData& input);


	void ChangeScreenSize();
	void WriteEntityTreeToFile(CStage* pStage);

};



#endif		/*  __SystemInputHandler_Debug_H__  */