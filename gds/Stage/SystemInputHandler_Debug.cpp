
#include "SystemInputHandler_Debug.h"
//#include <mmsystem.h>
#include <direct.h>

#include "App/GameWindowManager_Win32.h"
#include "App/ApplicationBase.h"
#include "Task/GameTaskManager.h"

#include "Stage/DebugWindow.h"

#include "3DCommon/font.h"
#include "3DCommon/TextureFont.h"
#include "Stage/PlayerInfo.h"
#include "Stage/EntitySet.h"
#include "Stage/BE_PlayerShip.h"
#include "Stage/Stage.h"
#include "Stage/ScreenEffectManager.h"
#include "GameCommon/ImageCapture.h"
#include "GameCommon/ScreenShotManager.h"
#include "Item/WeaponSystem.h"

#include "Support/FileOpenDialog_Win32.h"

#include "Task/GameTask_Stage.h"

bool g_bAppExitRequested = false;


CSystemInputHandler_Debug::CSystemInputHandler_Debug()
{
	int aiScreenWidth[5]  = {640,   800,   1024,  800,  1024};
	int aiScreenHeight[5] = {480,   600,   768,   600,  768};
	bool abFullscreen[5] =  {false, false, false, true, true };

	int i;
	for( i=0; i<5; i++ )
	{
		m_aiScreenWidth[i]  = aiScreenWidth[i];
		m_aiScreenHeight[i] = aiScreenHeight[i];
		m_abFullscreen[i]   = abFullscreen[i];
	}
	
	m_iScreenSize = 1;
}


CSystemInputHandler_Debug::~CSystemInputHandler_Debug()
{
}


void CSystemInputHandler_Debug::ProcessInput(SInputData& input)
{
//	static bool s_bMouseAcquired = false;

//	CImageCapture image_capture;
//	DWORD *pdwImageData;
//	CScreenShotManager screenshot;
//	char acCurrentDirectory[512], acStageFilename[512];

	switch( input.iGICode )
	{
	case GIC_ESC:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			g_bAppExitRequested = true;
		}
//		else if( input.iType == ITYPE_KEY_RELEASED ) {}
		break;

	case GIC_F1:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			g_iShowDebugInfo = (g_iShowDebugInfo + 1) % NUM_DEBUG_DISPLAYS;
			break;
		}*/
		break;

	case GIC_F4:
		if( input.iType == ITYPE_KEY_PRESSED )
			if( g_pStage.get() )
                WriteEntityTreeToFile( g_pStage.get() );
		break;

	case GIC_F5:	// quick save
//		if( input.iType == ITYPE_KEY_PRESSED )
//			SAVEDATAMANAGER.SaveCurrentState( 0, g_pStage );
		break;

	case GIC_F7:	// quick load
//		if( input.iType == ITYPE_KEY_PRESSED )
//			SAVEDATAMANAGER.LoadSavedData( 0, g_pStage );
		break;

	case GIC_F9:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			_getcwd( acCurrentDirectory, 511 );
			GetFilename( acStageFilename, "bspx" );
			_chdir( acCurrentDirectory );
			delete g_pStage;
			g_pStage = new CStage;
			g_pStage->LoadStageFromFile( acStageFilename );
			PlayerShip-.GetPlayerCopyEntity()->Position() = Vector3(0,0,0);
			PlayerShip-.GetPlayerCopyEntity()->Right()    = Vector3(1,0,0);
			PlayerShip-.GetPlayerCopyEntity()->Up()       = Vector3(0,1,0);
			PlayerShip-.GetPlayerCopyEntity()->Direction()= Vector3(0,0,1);
			PlayerShip-.Init( g_pStage );
			g_pStage->GetEntitySet()->SetCameraEntity( PlayerShip.GetPlayerCopyEntity() );
			WEAPONSYSTEM.Init( g_pStage );
		}*/
		break;

	case GIC_F10:	// change screen size (to a smaller one)
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_iScreenSize--;
			if( m_iScreenSize < 0)
				m_iScreenSize = 4;
//			ChangeScreenSize();
		}
		break;

	case GIC_F11:	// change screen size (to a larger one)
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_iScreenSize++;
			if( 5 <= m_iScreenSize )
				m_iScreenSize = 0;
//			ChangeScreenSize();
		}
		break;

	case GIC_F12:
		break;

	case GIC_MULTIPLY:
		if( input.iType == ITYPE_KEY_PRESSED )
			g_pStage->GetScreenEffectManager()->RaiseEffectFlag( ScreenEffect::PseudoNightVision );
		break;

	case GIC_DIVIDE:
		if( input.iType == ITYPE_KEY_PRESSED )
			g_pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PseudoNightVision );
		break;

	default:
		break;	// no action is assigned to this input
	}
}


void CSystemInputHandler_Debug::ChangeScreenSize()
{
	GAMEWINDOWMANAGER.ChangeScreenSize( m_aiScreenWidth[m_iScreenSize], m_aiScreenHeight[m_iScreenSize], m_abFullscreen[m_iScreenSize] );
}


void CSystemInputHandler_Debug::WriteEntityTreeToFile(CStage* pStage)
{
	char acFilename[128], acTime[64];
	static DWORD dwLastOutputTime = 0;

	if( 1000 < (timeGetTime() - dwLastOutputTime) )	// don't output more than once in a second
	{
		CEntitySet* pEntSet = pStage->GetEntitySet();
		sprintf( acTime, "%.3f", ((float)(timeGetTime() / 1000.0f)) );
//		ftos((float)(timeGetTime() / 1000.0f), 3, acTime);

		strcpy( acFilename, "entity_tree[" );
		strcat( acFilename, acTime );
		strcat( acFilename, "].txt" );

		pEntSet->WriteEntityTreeToFile(acFilename);
		dwLastOutputTime = timeGetTime();
	}
}
