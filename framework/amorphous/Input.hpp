#include "Input/InputHub.hpp"
#include "Input/InputHandler.hpp"

#ifdef _MSC_VER
// for Windows platform
#include "Input/Win32/StdKeyboard.hpp"
#include "Input/Win32/DirectInput.hpp"
#include "Input/Win32/DIKeyboard.hpp"
#include "Input/Win32/DirectInputMouse.hpp"
#include "Input/Win32/DirectInputGamepad.hpp"
#include "Input/Win32/DIInputDeviceMonitor.hpp"
#endif
