#include "AndroidKeyboard.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include <map>
#include <android/keycodes.h>


namespace amorphous
{

static std::map<int,int> key_maps;

static void InitKeyMaps() {
    key_maps[AKEYCODE_A] = 'A';
    key_maps[AKEYCODE_B] = 'B';
    key_maps[AKEYCODE_C] = 'C';
    key_maps[AKEYCODE_D] = 'D';
    key_maps[AKEYCODE_E] = 'E';
    key_maps[AKEYCODE_F] = 'F';
    key_maps[AKEYCODE_G] = 'G';
    key_maps[AKEYCODE_H] = 'H';
    key_maps[AKEYCODE_I] = 'I';
    key_maps[AKEYCODE_J] = 'J';
    key_maps[AKEYCODE_K] = 'K';
    key_maps[AKEYCODE_L] = 'L';
    key_maps[AKEYCODE_M] = 'M';
    key_maps[AKEYCODE_N] = 'N';
    key_maps[AKEYCODE_O] = 'O';
    key_maps[AKEYCODE_P] = 'P';
    key_maps[AKEYCODE_Q] = 'Q';
    key_maps[AKEYCODE_R] = 'R';
    key_maps[AKEYCODE_S] = 'S';
    key_maps[AKEYCODE_T] = 'T';
    key_maps[AKEYCODE_U] = 'U';
    key_maps[AKEYCODE_V] = 'V';
    key_maps[AKEYCODE_W] = 'W';
    key_maps[AKEYCODE_X] = 'X';
    key_maps[AKEYCODE_Y] = 'Y';
    key_maps[AKEYCODE_Z] = 'Z';
    key_maps[AKEYCODE_DPAD_UP]    = GIC_UP;
    key_maps[AKEYCODE_DPAD_DOWN]  = GIC_DOWN;
    key_maps[AKEYCODE_DPAD_LEFT]  = GIC_LEFT;
    key_maps[AKEYCODE_DPAD_RIGHT] = GIC_RIGHT;
}


AndroidKeyboard::AndroidKeyboard()
{
    static int s = 0;

    ONCE( InitKeyMaps() );
    
    LOG_PRINTF(("Registering the device to a group. %d",s));
    s+=1;
    
    GetInputDeviceHub().RegisterInputDeviceToGroup( this );

    LOG_PRINTF(("Leaving. %d",s));
    s+=1;
}


void AndroidKeyboard::OnKeyDown(int android_keycode)
{
    if( key_maps.find(android_keycode) == key_maps.end() ) {
        // Key was not found.
        LOG_PRINTF_WARNING(("An unrecognized key: %d",android_keycode));
        return;
    }

    int input_code = key_maps[android_keycode];

    LOG_PRINTF(("Key pressed: %d",android_keycode));

    InputData input;
    input.iGICode = input_code;
    input.iType = ITYPE_KEY_PRESSED;

    GetInputHub().UpdateInput(input);

    UpdateInputState(input);
}


void AndroidKeyboard::OnKeyUp(int android_keycode)
{
    if( key_maps.find(android_keycode) == key_maps.end() ) {
        // Key was not found.
        LOG_PRINTF_WARNING(("An unrecognized key: %d",android_keycode));
        return;
    }

    int input_code = key_maps[android_keycode];
    
    LOG_PRINTF(("Key released: %d",android_keycode));

    InputData input;
    input.iGICode = input_code;
    input.iType = ITYPE_KEY_RELEASED;

    GetInputHub().UpdateInput(input);

    UpdateInputState(input);
}

} // namespace amorphous
