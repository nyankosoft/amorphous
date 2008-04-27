#include <stdio.h>
#include <windows.h>
#include <d3dx9.h>
#include "../3DCommon/D3DXMeshObject.h"

#include "../GameInput/KeyboardInput.h"

CKeyboardInput CKeyboardInput::ms_KeyboardInput_;

CKeyboardInput::CKeyboardInput()
{
	this->m_pActionInput = NULL;

	for(int i=0; i<NUM_VKCODES; i++)
		this->m_aiActionForVKCode[i] = ACTION_NOT_ASSIGNED;

	//load key configration from file
	FILE* fp = fopen("keyconfig", "r");

	if(fp)
	{
		//Read Key-binds from the file
		fclose(fp);
	}
	else
		SetDafaultKeyBind();


	//Load icon positions for each key from "IconPosForKeys.dat" file
	fp = fopen("..\\GameMain\\System\\IconPosFor3DKeyboard.dat", "rb");
	if(fp)
	{
		fread(m_avKeyPosition, NUM_VKCODES, sizeof(D3DXVECTOR3), fp);
		fclose(fp);
	}

	m_p3DKeyboard = NULL;
}

void CKeyboardInput::SetDafaultKeyBind()
{
	int *piActionForVKCode = this->m_aiActionForVKCode;

	piActionForVKCode['E']			= ACTION_MOV_FORWARD;
	piActionForVKCode['S']			= ACTION_MOV_SLIDE_L;
	piActionForVKCode['D']			= ACTION_MOV_BACKWARD;
	piActionForVKCode['F']			= ACTION_MOV_SLIDE_R;
//	piActionForVKCode[VK_SPACE]		= ACTION_MOV_JUMP;
	piActionForVKCode[VK_SPACE]		= ACTION_MOV_SLIDE_UP;
	piActionForVKCode[0x7B]			= ACTION_MOV_SLIDE_DOWN;
	piActionForVKCode[VK_NONCONVERT]= ACTION_MOV_SLIDE_DOWN;
	piActionForVKCode[VK_BACK]		= ACTION_MOV_CROUCH;
	piActionForVKCode[VK_SHIFT]		= ACTION_MOV_RUN;

	piActionForVKCode[VK_UP]		= ACTION_MOV_FORWARD;
	piActionForVKCode[VK_LEFT]		= ACTION_MOV_TURN_L;
	piActionForVKCode[VK_DOWN]		= ACTION_MOV_BACKWARD;
	piActionForVKCode[VK_RIGHT]		= ACTION_MOV_TURN_R;
	piActionForVKCode[VK_NUMPAD1]	= ACTION_MOV_SLIDE_L;
	piActionForVKCode[VK_NUMPAD3]	= ACTION_MOV_SLIDE_R;

	piActionForVKCode[VK_NUMPAD0]	= ACTION_ATK_FIRE;
	
	piActionForVKCode['A']			= ACTION_ARMS_SELECTWEAPON1;
	piActionForVKCode['Q']			= ACTION_ARMS_SELECTWEAPON2;
	piActionForVKCode['T']			= ACTION_ARMS_SELECTWEAPON3;
	piActionForVKCode['G']			= ACTION_ARMS_SELECTWEAPON4;
	piActionForVKCode['V']			= ACTION_ARMS_SELECTWEAPON5;

	//piActionForVKCode[VK_PERIOD]	= ACTION_MOV_FORWARD;
	//piActionForVKCode[0xBD]		= ACTION_MOV_TURN_L;
}

CKeyboardInput::~CKeyboardInput()
{
	if( m_p3DKeyboard )
	{
		m_p3DKeyboard->Release();
		delete m_p3DKeyboard;
	}
}

void CKeyboardInput::RenderConfigMenu()
{

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	D3DXMATRIX mat, matView, matProj;

	D3DXMatrixIdentity(&mat);
	pd3dDevice->SetTransform(D3DTS_WORLD, &mat);

	D3DXVECTOR3 vEyePosition =  D3DXVECTOR3(0, 0.4f, 0);
	D3DXVECTOR3 vLookAtPoint =	D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 vUp =			D3DXVECTOR3(0, 0, 1);
	D3DXMatrixLookAtLH( &matView, &vEyePosition, &vLookAtPoint, &vUp);
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI  / 3.0f, 1.0f, 0.01f, 50.0f );
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	//lighting configuration
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );

	light.Type        = D3DLIGHT_POINT;
	light.Diffuse.r   = 0.3f;
	light.Diffuse.g   = 0.3f;
	light.Diffuse.b   = 0.3f;
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vLookAtPoint );
	light.Position	   = (D3DVECTOR)vEyePosition;
	light.Range        = 10.0f;

	pd3dDevice->SetLight(0, &light );
	pd3dDevice->LightEnable(0, TRUE );

	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff505050 );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	if( !m_p3DKeyboard )
	{
		m_p3DKeyboard = new CD3DXMeshObject;
		if(m_p3DKeyboard)
			m_p3DKeyboard->LoadFromFile("Model\\Keyboard.x");
	}
	else
		m_p3DKeyboard->Draw();	//Draw 3D Keyboard

	int iAction;
	D3DXVECTOR3 avIconRect[4];
	//put textured icons which shows the type of action assigned to each key
	for(int i=0; i<NUM_VKCODES; i++)
	{
		iAction = m_aiActionForVKCode[i];
		if( iAction == ACTION_NOT_ASSIGNED )
			continue;

		D3DXVECTOR3& rvIconPos = m_avKeyPosition[i];
		if( rvIconPos.y == -1.0f )
			continue;

		for(int j=0; j<4; j++)
			avIconRect[j] = m_avKeyPosition[i];
		avIconRect[1].x += 0.012f; 
		avIconRect[2].x += 0.012f; avIconRect[2].z -= 0.012f;
		avIconRect[3].z -= 0.012f;
		ActionInput.DrawActionIcon( iAction, avIconRect, &matView, &matProj );
	}

}

bool CKeyboardInput::TryToAssignAction(int iAction)
{
//	unsigned char acKeyState[256];

	//Get the states of all the keys on the keyboard
	//GetKeyboardState( acKeyState );

	int iVKCode;
	for(iVKCode=0; iVKCode<256; iVKCode++)
	{
//		if( acKeyState[iVKCode] & 0x80 )
		if( GetAsyncKeyState(iVKCode) & 0x8000 )
		{	//This key is currently down.
			m_aiActionForVKCode[iVKCode] = iAction;
			return true;
		}
	}

	//All the keys are up.
	return false;
}

void CKeyboardInput::SaveCurrentConfiguration(FILE *fp)
{
	fwrite( m_aiActionForVKCode, sizeof(int), NUM_VKCODES, fp );
}

void CKeyboardInput::LoadConfiguration(FILE *fp)
{
	fread( m_aiActionForVKCode, sizeof(int), NUM_VKCODES, fp );
}

/*
void InitForConfigMenu()
{

	FILE* fp = fopen("configmenu.dat", "rb");

	if( !fp )
		return;

	fread(buffer, NUM_VKCODES, sizeof(D3DXVECTOR2), fp);
	fclose(fp);

}
*/