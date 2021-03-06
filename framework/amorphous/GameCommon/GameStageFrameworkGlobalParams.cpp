#include "GameStageFrameworkGlobalParams.hpp"

#include "amorphous/Support/ParamLoader.hpp"

#include "amorphous/Stage/EntitySet.hpp"
#include "amorphous/Stage/ScreenEffectManager.hpp"
#include "amorphous/App/GameApplicationBase.hpp"
#include "amorphous/Support/DebugOutput.hpp"


namespace amorphous
{


GameStageFrameworkGlobalParams::GameStageFrameworkGlobalParams()
:
m_fDefaultPhysicsSimulationTimestep( 0.01f ),
m_DefaultShaderFilename( "./Shader/Default.fx" ),
m_DefaultSleepTimeMS( 3 ),
m_StartupDebugWindow( "none" )
{}


bool GameStageFrameworkGlobalParams::LoadFromTextFile( const std::string& filename )
{
	ParamLoader loader( filename );

	if( !loader.IsReady() )
		return false;

	loader.LoadParam( "DefaultShaderFilename",            m_DefaultShaderFilename );

	loader.LoadParam( "DefaultPhysicsSimulationTimestep", m_fDefaultPhysicsSimulationTimestep );

	loader.LoadParam( "DefaultSleepTimeMS",               m_DefaultSleepTimeMS );

	loader.LoadParam( "StartupDebugWindow",               m_StartupDebugWindow );

	return true;
}


void GameStageFrameworkGlobalParams::UpdateParams()
{
	EntityManager::SetDefaultPhysicsTimestep( m_fDefaultPhysicsSimulationTimestep );

//	ScreenEffectManager::SetDefaultShBaderFilename( m_DefaultShaderFilename );

	GameApplicationBase::SetDefaultSleepTime( m_DefaultSleepTimeMS );

	if( m_StartupDebugWindow == "none" )
		DebugOutput.Hide();
	else
	{
		bool res = DebugOutput.SetDebugItem( m_StartupDebugWindow );
		if( res )
			DebugOutput.Show(); // successfully set a debug item from the item name - show the debug output
	}
}


void GameStageFrameworkGlobalParams::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_fDefaultPhysicsSimulationTimestep;
	ar & m_DefaultShaderFilename;
	ar & m_DefaultSleepTimeMS;
	ar & m_StartupDebugWindow;
}



} // namespace amorphous
