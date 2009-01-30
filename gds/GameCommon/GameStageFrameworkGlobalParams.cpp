#include "GameStageFrameworkGlobalParams.hpp"

#include "Support/ParamLoader.hpp"

#include "Stage/EntitySet.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "App/ApplicationBase.hpp"


CGameStageFrameworkGlobalParams::CGameStageFrameworkGlobalParams()
:
m_fDefaultPhysicsSimulationTimestep( 0.01f ),
m_DefaultShaderFilename( "./Shader/Default.fx" ),
m_DefaultSleepTimeMS( 3 )
{}


bool CGameStageFrameworkGlobalParams::LoadFromTextFile( const std::string& filename )
{
	CParamLoader loader( filename );

	if( !loader.IsReady() )
		return false;

	loader.LoadParam( "DefaultShaderFilename",            m_DefaultShaderFilename );

	loader.LoadParam( "DefaultPhysicsSimulationTimestep", m_fDefaultPhysicsSimulationTimestep );

	loader.LoadParam( "DefaultSleepTimeMS",               m_DefaultSleepTimeMS );

	return true;
}


void CGameStageFrameworkGlobalParams::UpdateParams()
{
	CEntitySet::SetDefaultPhysicsTimestep( m_fDefaultPhysicsSimulationTimestep );

//	CScreenEffectManager::SetDefaultShBaderFilename( m_DefaultShaderFilename );

	CApplicationBase::SetDefaultSleepTime( m_DefaultSleepTimeMS );
}


void CGameStageFrameworkGlobalParams::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_fDefaultPhysicsSimulationTimestep;
	ar & m_DefaultShaderFilename;
	ar & m_DefaultSleepTimeMS;
}

