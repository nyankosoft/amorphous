#ifndef __amorphous_AndroidKeyboard_HPP__
#define __amorphous_AndroidKeyboard_HPP__


#include "amorphous/Input/InputHub.hpp"
#include "amorphous/Input/KeyboardInputDevice.hpp"


namespace amorphous
{
	
class AndroidKeyboard : public KeyboardInputDevice
{
public:
	AndroidKeyboard();
	~AndroidKeyboard() {}

	void OnKeyDown(int android_keycode);

	void OnKeyUp(int android_keycode);
};

} // namespace amorphous


#endif /* __amorphous_AndroidKeyboard_HPP__ */
