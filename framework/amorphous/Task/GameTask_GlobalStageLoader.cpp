#include "GameTask_GlobalStageLoader.hpp"
#include "GameTask_Stage.hpp"
#include "amorphous/Stage/Stage.hpp"

#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Font/BuiltinFonts.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


std::string GameTask_GlobalStageLoader::ms_strStageTask = "Stage";


GameTask_GlobalStageLoader::GameTask_GlobalStageLoader()
{
	m_bRendered = false;

	m_bStageLoaded = false;

	int w = 16 * GetScreenWidth() / 800;
	int h = w * 2;
	m_pFont = CreateDefaultBuiltinFont();
}


GameTask_GlobalStageLoader::~GameTask_GlobalStageLoader()
{
}


int GameTask_GlobalStageLoader::FrameMove( float dt )
{
	int ret = GameTask::FrameMove(dt);
	if( ret != ID_INVALID )
		return ret;

	if( !m_bRendered )
		return GameTask::ID_INVALID;
	
	if( m_bStageLoaded )
		return GameTask::ID_INVALID;

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
		LOG_PRINT( "No global stage has been specified" );
		return GameTask::ID_PREVTASK;
	}

	m_bStageLoaded = true;


	//
	// stage has been loaded
	//

	// move on to the stage task
	RequestTaskTransition( ms_strStageTask, 0, 0.0f, 0.0f, 0.0f );

	return GameTask::ID_INVALID;
}


void GameTask_GlobalStageLoader::Render()
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



} // namespace amorphous
