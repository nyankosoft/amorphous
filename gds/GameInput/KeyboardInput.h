#ifndef  __KEBOARDINPUT_H__
#define  __KEBOARDINPUT_H__

#include "../GameInput/3DActionInput.h"

#define VK_DOTKEY 0xBE
#define VK_ALTKEY 0x38

//#define VK_NOCONVERT	0x7B
//#define VK_CONVERT		0x79

#define NUM_VKCODES			512

//define access to the singleton instance
#define KeyboardInput ( CKeyboardInput::ms_KeyboardInput_ )

class CD3DXMeshObject;

class CKeyboardInput : public CInputDevice
{
	int m_aiActionForVKCode[NUM_VKCODES];	//specify action from the virtual key code

	//used in config menu
	D3DXVECTOR3 m_avKeyPosition[NUM_VKCODES];
	CD3DXMeshObject* m_p3DKeyboard;

protected:
	CKeyboardInput();		//singleton

public:
	static CKeyboardInput ms_KeyboardInput_;	//single instance of 'CKeyboardInput'

	~CKeyboardInput();
	void SetDafaultKeyBind();	//used when 'keyconfig' file is not found

	//iKeyState = 1 or 0: key is down(pushed) or up(released)
	inline void UpdateKeyState(unsigned int uiVKCode, int iKeyState)
	{
		if( NUM_VKCODES < uiVKCode )
			return;
		int iAction = m_aiActionForVKCode[uiVKCode];

		m_pActionInput->Update(iAction, (float)iKeyState);
	}
	bool TryToAssignAction(int iAction);
	void RenderConfigMenu();

	void SaveCurrentConfiguration(FILE *fp);
	void LoadConfiguration(FILE *fp);

};

#endif		/*  __KEBOARDINPUT_H__  */
