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

typedef std::shared_ptr<InputHandler> InputHandlerSharedPtr;

class InputDeviceParam
{
public:

	enum Params
	{
		NUM_MAX_SIMULTANEOUS_PRESSES = 4,
	};
};

// for Windows platform
class Win32StdKeyboard;
class Win32StdMouseInput;
class DIKeyboard;         // DirectInput
class DirectInputMouse;   // DirectInput
class DirectInputGamepad; // DirectInput

} // namespace amorphous


#endif /* __GAMEINPUT_FWD_H__ */
