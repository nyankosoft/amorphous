#ifndef  __GlobalInputHandler_H__
#define  __GlobalInputHandler_H__

#include "amorphous/Input/InputHandler.hpp"


namespace amorphous
{

//extern bool g_bAppExitRequested;


class GlobalInputHandler : public InputHandler
{
public:
	GlobalInputHandler();
	virtual ~GlobalInputHandler();

	void ProcessInput(InputData& input);

};

} // namespace amorphous



#endif		/*  __GlobalInputHandler_H__  */
