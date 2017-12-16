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
    key_maps[AKEYCODE_0] = '0';
    key_maps[AKEYCODE_1] = '1';
    key_maps[AKEYCODE_2] = '2';
    key_maps[AKEYCODE_3] = '3';
    key_maps[AKEYCODE_4] = '4';
    key_maps[AKEYCODE_5] = '5';
    key_maps[AKEYCODE_6] = '6';
    key_maps[AKEYCODE_7] = '7';
    key_maps[AKEYCODE_8] = '8';
    key_maps[AKEYCODE_9] = '9';
    key_maps[AKEYCODE_F1] = GIC_F1;
    key_maps[AKEYCODE_F2] = GIC_F2;
    key_maps[AKEYCODE_F3] = GIC_F3;
    key_maps[AKEYCODE_F4] = GIC_F4;
    key_maps[AKEYCODE_F5] = GIC_F5;
    key_maps[AKEYCODE_F6] = GIC_F6;
    key_maps[AKEYCODE_F7] = GIC_F7;
    key_maps[AKEYCODE_F8] = GIC_F8;
    key_maps[AKEYCODE_F9] = GIC_F9;
    key_maps[AKEYCODE_F10] = GIC_F10;
    key_maps[AKEYCODE_F11] = GIC_F11;
    key_maps[AKEYCODE_F12] = GIC_F12;
    key_maps[AKEYCODE_ENTER] = GIC_ENTER;
    key_maps[AKEYCODE_DEL]   = GIC_DELETE;
    key_maps[AKEYCODE_DPAD_UP]    = GIC_UP;
    key_maps[AKEYCODE_DPAD_DOWN]  = GIC_DOWN;
    key_maps[AKEYCODE_DPAD_LEFT]  = GIC_LEFT;
    key_maps[AKEYCODE_DPAD_RIGHT] = GIC_RIGHT;

    key_maps[AKEYCODE_BUTTON_A]  = GIC_GPD_BUTTON_00;
    key_maps[AKEYCODE_BUTTON_B]  = GIC_GPD_BUTTON_01;
    key_maps[AKEYCODE_BUTTON_C]  = GIC_GPD_BUTTON_02;
    key_maps[AKEYCODE_BUTTON_X]  = GIC_GPD_BUTTON_03;
    key_maps[AKEYCODE_BUTTON_Y]  = GIC_GPD_BUTTON_04;
    key_maps[AKEYCODE_BUTTON_Z]  = GIC_GPD_BUTTON_05;
    key_maps[AKEYCODE_BUTTON_L1] = GIC_GPD_BUTTON_06;
    key_maps[AKEYCODE_BUTTON_R1] = GIC_GPD_BUTTON_07;
    key_maps[AKEYCODE_BUTTON_L2] = GIC_GPD_BUTTON_08;
    key_maps[AKEYCODE_BUTTON_R2] = GIC_GPD_BUTTON_09;
    key_maps[AKEYCODE_BUTTON_THUMBL] = GIC_GPD_BUTTON_10;
    key_maps[AKEYCODE_BUTTON_THUMBR] = GIC_GPD_BUTTON_11;
}


AndroidKeyboard::AndroidKeyboard()
{
    static int s = 0;

    ONCE( InitKeyMaps() );

    LOG_PRINTF(("key map size: %d",(int)key_maps.size()));

    LOG_PRINTF(("Registering the device to a group. %d",s));
    s+=1;
    
    GetInputDeviceHub().RegisterInputDeviceToGroup( this );

    LOG_PRINTF(("Leaving. %d",s));
    s+=1;
}


void AndroidKeyboard::OnKeyDown(int android_keycode)
{
    //LOG_PRINTF(("map size: %d",(int)key_maps.size()));

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
    //LOG_PRINTF(("map size: %d",(int)key_maps.size()));

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
