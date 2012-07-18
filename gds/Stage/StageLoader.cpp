#include "StageLoader.hpp"
#include "Stage.hpp"
#include "Script/PyModule_StageUtility.hpp"
#include "Graphics/ResourceLoadingStateHolder.hpp"

using namespace std;


// draft 
extern void CreateResourceLoadingStateHolderForCurrentThread();
extern CResourceLoadingStateSet::Name GetGraphicsResourceLoadingState();


inline static void sleep_milliseonds( int64_t ms )
{
	boost::this_thread::sleep( boost::posix_time::milliseconds(ms) );
}


class CASyncStageLoaderThread
{
	CASyncStageLoader *m_pStageLoader;

public:

	CASyncStageLoaderThread( CASyncStageLoader *pStageLoader )
		:
	m_pStageLoader(pStageLoader)
	{}

	void operator()();
};


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
//	m_pLoader = new boost::thread( *this );
	m_pLoader = new boost::thread( CASyncStageLoaderThread(this) );

	return true;
}


void CASyncStageLoader::Join()
{
	m_bExitLoaderThread = true;

	if( m_pLoader )
		m_pLoader->join();
}

static int sg_NumLoaded = 0;


void CASyncStageLoaderThread::operator()()
{
	CStageLoader loader;

	// catch any resource loading calls
	CreateResourceLoadingStateHolderForCurrentThread();

	if( !m_pStageLoader->m_pStage )
	{
		/// stage instance has not been created
		/// - create an empty stage
		m_pStageLoader->m_pStage = loader.CreateStage();
	}

	/// load stage
	/// - this could take a few seconds
	bool loaded = m_pStageLoader->m_pStage->Initialize( m_pStageLoader->m_StageScriptName );
	
	if( !loaded )
	{
		// failed to load stage
		m_pStageLoader->m_bFailedToLoadStage = true;
		return;
	}

	while( GetGraphicsResourceLoadingState() == CResourceLoadingStateSet::NOT_READY )
	{
		sleep_milliseonds( 10 );
	}

	m_pStageLoader->m_bIsLoaded = true;

	sg_NumLoaded += 1;


	while( !m_pStageLoader->m_bExitLoaderThread )
	{
		m_pStageLoader->m_StageScriptName = "";

		if( !m_pStageLoader->m_bIsLoaded )
		{
			g_Log.Print( "'m_bIsLoaded' changed to false" );
		}

		sleep_milliseonds( 10 );
	}
}
