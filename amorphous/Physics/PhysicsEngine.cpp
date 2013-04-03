#include "Ageia/NxPhysicsEngine.hpp"
#include "PhysicsEngine.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"


namespace amorphous
{

using namespace std;
using namespace physics;


singleton<CPhysicsEngine> CPhysicsEngine::m_obj;


bool CPhysicsEngine::Init( const std::string& physics_engine )
{
	if( physics_engine == "AgeiaPhysX" )
	{
		m_pEngine = new CNxPhysicsEngine();
//		PhysShapeDescImplFactoryHolder.Init(  );
	}
	else if( physics_engine == "JigLib" )
	{
		LOG_PRINT_ERROR( " JigLib physics engine - not implemented." );
//		m_pEngine = new CJigLibPhysicsEngine();
//		PhysShapeDescImplFactoryHolder.Init(  );
		return false;
	}
	else
	{
		LOG_PRINT_ERROR( " Invalid physics engine name: " + physics_engine );
		return false;
	}

	m_Name = physics_engine;

	bool initialized = m_pEngine->Init();
	if( !initialized )
		LOG_PRINT_ERROR( " Failed to initialize the physics engine: " + physics_engine );

	return initialized;
}


void CPhysicsEngine::Release()
{
	SafeDelete( m_pEngine );
}


//void CPhysicsEngine::SetDefault(){}


} // namespace amorphous
