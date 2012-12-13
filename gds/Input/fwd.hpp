#ifndef  __GAMEINPUT_FWD_H__
#define  __GAMEINPUT_FWD_H__


struct SInputData;
class CInputHandler;

class CInputDevice;
class MouseInputDevice;
class CInputDeviceStateCallback;
class CStdKeyboard;        /// for Windows platform
class CStdMouseInput;      /// for Windows platform
class CDIKeyboard;         /// for Windows platform (DirectInput)
class CDirectInputMouse;   /// for Windows platform (DirectInput)
class CDirectInputGamepad; /// for Windows platform (DirectInput)

#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CInputHandler> CInputHandlerSharedPtr;


class CInputDeviceParam
{
public:

	enum Params
	{
		NUM_MAX_SIMULTANEOUS_PRESSES = 4,
	};
};


#endif /* __GAMEINPUT_FWD_H__ */
