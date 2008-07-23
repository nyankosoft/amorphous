#ifndef  __DIRECTINPUT_H__
#define  __DIRECTINPUT_H__


// link "dinput8.lib"

//#define STRICT
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )


#define DIRECTINPUT ( CDirectInput::ms_SingletonInstance_ )

class CDirectInput
{
private:
	LPDIRECTINPUT8 m_pDI;

	HRESULT InitDirectInput();

protected:
	CDirectInput();		//singleton

public:
	static CDirectInput ms_SingletonInstance_;	//single instance of 'CDirectInput'

	~CDirectInput();

	LPDIRECTINPUT8 GetDirectInputObject();

};


#endif		/*  __DIRECTINPUT_H__  */
