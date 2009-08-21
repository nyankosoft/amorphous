#include "StageLoader.hpp"
#include "Stage.hpp"
#include "Script/PyModule_StageUtility.hpp"

using namespace std;


CStageSharedPtr CStageLoader::LoadStage( const std::string& script_name )
{
	CStageSharedPtr pStage = this->CreateStage();

	if( pStage )
		stage_util::RegisterStageForScript( pStage );

	bool res = pStage->Initialize( script_name );

	if( res )
	{
		g_Log.Print( "%s - loaded a stage: %s", __FUNCTION__, script_name.c_str() );
	}
	else
	{
		g_Log.Print( "%s - unable to load a stage: %s", __FUNCTION__, script_name.c_str() );
		pStage.reset();
		stage_util::UnregisterStageForScript();
	}

	return pStage;
}


CStageSharedPtr CStageLoader::CreateStage()
{
	CStageSharedPtr pStage( new CStage() );
	pStage->m_pSelf = CStageWeakPtr( pStage );

	return pStage;
}



CASyncStageLoader::CASyncStageLoader()
:
m_bIsLoaded(false),
m_bFailedToLoadStage(false),
m_bExitLoaderThread(false)
{
	m_pLoader = NULL;
}


CASyncStageLoader::~CASyncStageLoader()
{
	Join();

	if( m_pLoader ) delete m_pLoader;
}


void CASyncStageLoader::CreateStageInstance()
{
	CStageLoader loader;

	m_pStage = loader.CreateStage();
}


bool CASyncStageLoader::LoadStage( const std::string& script_name )
{
	m_StageScriptName = script_name;

//	m_pLoader = boost::shared_ptr<boost::thread>( new boost::thread( *this ) );
	m_pLoader = new boost::thread( *this );

	return true;
}


void CASyncStageLoader::Join()
{
	m_bExitLoaderThread = true;

	if( m_pLoader )
		m_pLoader->join();
}


void CASyncStageLoader::operator()()
{
	CStageLoader loader;

	if( !m_pStage.get() )
	{
		/// stage instance has not been created
		/// - create an empty stage
		m_pStage = loader.CreateStage();
	}

	/// load stage
	/// - this could take a few seconds
	bool loaded = m_pStage->Initialize( m_StageScriptName );
	
	if( !loaded )
	{
		// failed to load stage
		m_bFailedToLoadStage = true;
		return;
	}

	m_bIsLoaded = true;

	while( !m_bExitLoaderThread )
	{
		m_StageScriptName = "";

		if( !m_bIsLoaded )
		{
			g_Log.Print( "'m_bIsLoaded' changed to false" );
		}

		Sleep( 10 );
	}
}
