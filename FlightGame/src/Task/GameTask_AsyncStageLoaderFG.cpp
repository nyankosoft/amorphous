
#include "GameTask_AsyncStageLoaderFG.h"
#include "Task/GameTask_Stage.h"
#include "Stage/Stage.h"
#include "GameEvent/ScriptManager.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/Font.h"

#include "GameInput/InputHub.h"

//#include "Sound/GameSoundManager.h"

#include "Support/memory_helpers.h"
#include "Support/Log/StateLog.h"
#include "Support/Log/DefaultLog.h"

#include "UI/ui.h"
#include "UI/InputHandler_Dialog.h"

#include "../Stage/FG_EntityFactory.h"
#include "../Stage/PyModule_FlightGame.h"
#include "../EntityGroups.h"
#include "../FlightGameGlobalParams.h"

using namespace std;


static bool gs_LoadStageAsyncronously = false;


static void InitStageForFlightGame( CStageSharedPtr pStage )
{
	InitEntityGroup( pStage );

	pStage->GetScriptManager()->AddModule( "fg", g_PyModuleFlightGameMethod );

	pStage->GetEntitySet()->SetEntityFactory( new CFG_EntityFactory() );
}


CGameTask_AsyncStageLoaderFG::CGameTask_AsyncStageLoaderFG()
{
	// create an empty stage and do the entity group settings first
//	m_AsyncStageLoader.CreateStageInstance();

//	CStageSharedPtr pStage = m_AsyncStageLoader.GetStage();

//	InitEntityGroup( pStage );

	CStageLoader loader;
	m_pStage = loader.CreateStage();

	InitStageForFlightGame( m_pStage );

	ms_strNextTaskName = "StageFG";

	if( 0 < g_FlightGameParams.StageScriptFilename.length() )
		ms_strStageScripName = g_FlightGameParams.StageScriptFilename;

	if( !GetAnimatedGraphicsManager() )
		InitAnimatedGraphicsManager();

	// set animated graphics manager here
	// since init routines of the scripts use graphics manager
	SetAnimatedGraphicsManagerForScript();
}


CGameTask_AsyncStageLoaderFG::~CGameTask_AsyncStageLoaderFG()
{
	if( GetState() == STATE_LOADED_STAGE
	 && !g_pStage.get() )
	{
		g_pStage = m_pStage;
	}
}


int CGameTask_AsyncStageLoaderFG::FrameMove( float dt )
{
	int ret = CGameTask_AsyncStageLoader::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	switch( GetState() )
	{
	case STATE_STAGE_NOT_LOADED:
		if( gs_LoadStageAsyncronously )
		{
			m_pStageLoader = new boost::thread( CAsyncLoadThreadStarter( this ) );
			while( GetState() == STATE_STAGE_NOT_LOADED )
				Sleep(100); // state will be changed to either STATE_LOADING_STAGE or STATE_FAILED_TO_LOAD_STAGE in AsyncLoadThreadMain()
		}
		else
		{
			// load stage sequentially
			if( !m_pStage.get() )
			{
				CStageLoader stg_loader;
				m_pStage = stg_loader.CreateStage();

				InitStageForFlightGame( m_pStage );
			}

			bool loaded = m_pStage->Initialize( ms_strStageScripName );
			if( loaded )
				m_State = STATE_LOADED_STAGE;
			else
				m_State = STATE_FAILED_TO_LOAD_STAGE;

		}
		return ID_INVALID;

	case STATE_LOADED_STAGE:
		g_pStage = m_pStage;
		return ID_INVALID;

	default:
		break;
	}

	return CGameTask::ID_INVALID;
}


void CGameTask_AsyncStageLoaderFG::Render()
{
	// do the render routine of the base class
	CGameTask::Render();

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(64,64,64), 1.0f, 0 );

	// Begin the scene
	pd3dDevice->BeginScene();

	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// render stage select dialog
//	m_pDialogManager->Render( dt );

	if( m_pFont )
	{
		C2DRect rect( 0,0,0,0, 0xFF000000 );
		rect.Draw();

		const string& script_name = ms_strStageScripName;
		int font_width = m_pFont->GetFontWidth();
		int font_height = m_pFont->GetFontHeight();
		int x = GetScreenWidth() / 2 - (int)script_name.length() * font_width / 2;
		int y = GetScreenHeight() / 2 - font_height / 2;
		m_pFont->SetFontColor( 0xFFFFFFFF );
		m_pFont->DrawText( script_name, x, y );
	}

	// draw cursor
//	DrawMouseCursor();

	RenderFadeEffect();

	// End the scene
    pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    pd3dDevice->Present( NULL, NULL, NULL, NULL );

	m_bRendered = true;
}


void CGameTask_AsyncStageLoaderFG::ReleaseGraphicsResources()
{
}


void CGameTask_AsyncStageLoaderFG::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}

