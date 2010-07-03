#include "SystemInputHandler_Debug.hpp"

#include "App/GameWindowManager.hpp"
#include "App/GameApplicationBase.hpp"
#include "Task/GameTaskManager.hpp"

//#include "Stage/PlayerInfo.hpp"
//#include "Stage/EntitySet.hpp"
//#include "Stage/BE_PlayerShip.hpp"
#include "Stage/Stage.hpp"
#include "Stage/ScreenEffectManager.hpp"
//#include "GameCommon/ImageCapture.hpp"
//#include "GameCommon/ScreenShotManager.hpp"
//#include "Item/WeaponSystem.hpp"

//#include "Task/GameTask_Stage.hpp"

//bool g_bAppExitRequested = false;


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
	switch( input.iGICode )
	{
	case GIC_ESC:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			g_bAppExitRequested = true;
		}
		break;

	case GIC_F5:	// quick save
//		if( input.iType == ITYPE_KEY_PRESSED )
//			SAVEDATAMANAGER.SaveCurrentState( 0, g_pStage );
		break;

	case GIC_F7:	// quick load
//		if( input.iType == ITYPE_KEY_PRESSED )
//			SAVEDATAMANAGER.LoadSavedData( 0, g_pStage );
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

/*	case GIC_MULTIPLY:
		if( input.iType == ITYPE_KEY_PRESSED )
			g_pStage->GetScreenEffectManager()->RaiseEffectFlag( ScreenEffect::PseudoNightVision );
		break;

	case GIC_DIVIDE:
		if( input.iType == ITYPE_KEY_PRESSED )
			g_pStage->GetScreenEffectManager()->ClearEffectFlag( ScreenEffect::PseudoNightVision );
		break;
*/
	default:
		break;	// no action is assigned to this input
	}
}


void CSystemInputHandler_Debug::ChangeScreenSize()
{
	GameWindowManager().ChangeScreenSize( m_aiScreenWidth[m_iScreenSize], m_aiScreenHeight[m_iScreenSize], m_abFullscreen[m_iScreenSize] );
}
