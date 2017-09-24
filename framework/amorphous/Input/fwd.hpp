#ifndef  __GAMEINPUT_FWD_H__
#define  __GAMEINPUT_FWD_H__


#include <memory>


namespace amorphous
{

struct InputData;
class InputHandler;

class InputDevice;
class MouseInputDevice;
class KeyboardInputDevice;
class InputDeviceStateCallback;
class Win32StdKeyboard;        /// for Windows platform
class Win32StdMouseInput;      /// for Windows platform
class DIKeyboard;         /// for Windows platform (DirectInput)
class DirectInputMouse;   /// for Windows platform (DirectInput)
class DirectInputGamepad; /// for Windows platform (DirectInput)

typedef std::shared_ptr<InputHandler> InputHandlerSharedPtr;


class InputDeviceParam
{
public:

	enum Params
	{
		NUM_MAX_SIMULTANEOUS_PRESSES = 4,
	};
};

} // namespace amorphous


#endif /* __GAMEINPUT_FWD_H__ */
