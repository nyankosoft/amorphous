#include "GameTask_GlobalStageLoader.hpp"
#include "GameTask_Stage.hpp"
#include "Stage/Stage.hpp"

#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/Font/Font.hpp"
#include "Input/InputHub.hpp"
#include "GUI.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/StateLog.hpp"
#include "Support/Log/DefaultLog.hpp"


std::string CGameTask_GlobalStageLoader::ms_strStageTask = "Stage";


CGameTask_GlobalStageLoader::CGameTask_GlobalStageLoader()
{
	m_bRendered = false;

	m_bStageLoaded = false;

	int w = 16 * GetScreenWidth() / 800;
	int h = w * 2;
	m_pFont = new CFont( "Arial", w, h );
}


CGameTask_GlobalStageLoader::~CGameTask_GlobalStageLoader()
{
	SafeDelete( m_pFont );
}


int CGameTask_GlobalStageLoader::FrameMove( float dt )
{
	int ret = CGameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( !m_bRendered )
		return CGameTask::ID_INVALID;
	
	if( m_bStageLoaded )
		return CGameTask::ID_INVALID;

	// load the global stage
	if( 0 < GetGlobalStageScriptFilename().length() )
	{
		InitAnimatedGraphicsManager();

		// set animated graphics manager here
		// since init routines of the scripts use graphics manager
		SetAnimatedGraphicsManagerForScript();

		LoadStage( GetGlobalStageScriptFilename() );
	}
	else
	{
		PrintLog( "CGameTask_GlobalStageLoader::CGameTask_Stage() - no global stage has been specified" );
		return CGameTask::ID_PREVTASK;
	}

	m_bStageLoaded = true;


	//
	// stage has been loaded
	//

	// move on to the stage task
	RequestTaskTransition( ms_strStageTask, 0, 0.0f, 0.0f, 0.0f );

	return CGameTask::ID_INVALID;
}


void CGameTask_GlobalStageLoader::Render()
{
	// render stage select dialog
//	m_pDialogManager->Render( dt );

	if( m_pFont )
	{
		C2DRect rect( 0,0,0,0, 0xFF000000 );
		rect.Draw();

		const string& script_name = GetGlobalStageScriptFilename();
		int font_width = m_pFont->GetFontWidth();
		int font_height = m_pFont->GetFontHeight();
		int x = GetScreenWidth() / 2 - (int)script_name.length() * font_width / 2;
		int y = GetScreenHeight() / 2 - font_height / 2;
		m_pFont->SetFontColor( 0xFFFFFFFF );
		m_pFont->DrawText( script_name, x, y );
	}

	m_bRendered = true;
}


void CGameTask_GlobalStageLoader::ReleaseGraphicsResources()
{
}


void CGameTask_GlobalStageLoader::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}

