#ifndef  __GlobalInputHandler_H__
#define  __GlobalInputHandler_H__

#include "GameInput/InputHandler.h"

extern bool g_bAppExitRequested;


class CGlobalInputHandler : public CInputHandler
{
public:
	CGlobalInputHandler();
	virtual ~CGlobalInputHandler();

	void ProcessInput(SInputData& input);

};


#endif		/*  __GlobalInputHandler_H__  */
