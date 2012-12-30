#ifndef  __GlobalInputHandler_H__
#define  __GlobalInputHandler_H__

#include "Input/InputHandler.hpp"


namespace amorphous
{

//extern bool g_bAppExitRequested;


class CGlobalInputHandler : public CInputHandler
{
public:
	CGlobalInputHandler();
	virtual ~CGlobalInputHandler();

	void ProcessInput(SInputData& input);

};

} // namespace amorphous



#endif		/*  __GlobalInputHandler_H__  */
