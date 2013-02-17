#ifndef  __GlobalInputHandler_H__
#define  __GlobalInputHandler_H__

#include "Input/InputHandler.hpp"


namespace amorphous
{

//extern bool g_bAppExitRequested;


class CGlobalInputHandler : public InputHandler
{
public:
	CGlobalInputHandler();
	virtual ~CGlobalInputHandler();

	void ProcessInput(InputData& input);

};

} // namespace amorphous



#endif		/*  __GlobalInputHandler_H__  */
