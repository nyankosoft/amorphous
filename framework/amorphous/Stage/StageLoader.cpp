#include "StageLoader.hpp"
#include "Stage.hpp"
#include "amorphous/Script/PyModule_StageUtility.hpp"
#include "amorphous/Graphics/ResourceLoadingStateHolder.hpp"


namespace amorphous
{

using namespace std;


// draft 
extern void CreateResourceLoadingStateHolderForCurrentThread();
extern ResourceLoadingStateSet::Name GetGraphicsResourceLoadingState();


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


CStageSharedPtr StageLoader::LoadStage( const std::string& script_name )
{
	CStageSharedPtr pStage = this->CreateStage();

	if( pStage )
		stage_util::RegisterStageForScript( pStage );

	bool res = pStage->Initialize( script_name );

	if( res )
	{
		LOG_PRINT( " Loaded a stage: " + script_name );
	}
	else
	{
		LOG_PRINT( " Unable to load a stage: " + script_name );
		pStage.reset();
		stage_util::UnregisterStageForScript();
	}

	return pStage;
}


CStageSharedPtr StageLoader::CreateStage()
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
	StageLoader loader;

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
	StageLoader loader;

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

	while( GetGraphicsResourceLoadingState() == ResourceLoadingStateSet::NOT_READY )
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
			LOG_PRINT( " 'm_bIsLoaded' changed to false" );
		}

		sleep_milliseonds( 10 );
	}
}


} // namespace amorphous
