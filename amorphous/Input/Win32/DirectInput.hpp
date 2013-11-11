#ifndef  __DIRECTINPUT_H__
#define  __DIRECTINPUT_H__


// link "dinput8.lib"

//#define STRICT
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>


namespace amorphous
{

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


#define DIRECTINPUT ( DirectInput::ms_SingletonInstance_ )

class DirectInput
{
private:
	LPDIRECTINPUT8 m_pDI;

	HRESULT InitDirectInput();

protected:
	DirectInput();		//singleton

public:
	static DirectInput ms_SingletonInstance_;	//single instance of 'DirectInput'

	~DirectInput();

	LPDIRECTINPUT8 GetDirectInputObject();

};

} // namespace amorphous



#endif		/*  __DIRECTINPUT_H__  */
