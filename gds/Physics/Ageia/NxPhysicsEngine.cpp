#include "NxPhysicsEngine.hpp"
#include "NxPhysScene.hpp"
#include "NxPhysStream.hpp"
#include "NxPhysTriangleMesh.hpp"
#include "NxPhysConvexMesh.hpp"
#include "NxPhysClothMesh.hpp"
#include "../Stream.hpp"
#include "../SceneDesc.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"

using namespace physics;


CNxPhysicsEngine::CNxPhysicsEngine()
:
m_pPhysicsSDK(NULL)
{
}


CNxPhysicsEngine::~CNxPhysicsEngine()
{
//	MSGBOX_FUNCTION_SCOPE();

	if( m_pPhysicsSDK )
		m_pPhysicsSDK->release();

//	SafeDelete( m_pPhysicsSDK );
}


bool CNxPhysicsEngine::Init()
{
	// Initialize PhysicsSDK
//	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, NULL);
//	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, &m_ErrorStream);
	m_pPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, &NxPhysOutputStream() );

	if(!m_pPhysicsSDK)
		return false;

	return true;
}


CScene *CNxPhysicsEngine::CreateScene( CSceneDesc& desc )
{
	if( !m_pPhysicsSDK )
	{
		LOG_PRINT_ERROR( "Cannot create scene. Physics SDK has not been initialized." );
		return NULL;
	}

	NxSceneDesc nx_desc;

//	nx_desc. = desc.;

	nx_desc.gravity = ToNxVec3( desc.Gravity );

//	nx_desc.userNotify = desc.;
	
//	nx_desc.fluidUserNotify = desc.;

//	nx_desc.userContactModify = desc.;

//	nx_desc.userTriggerReport = desc.;

//	nx_desc.userContactReport = desc.;

//	nx_desc.userActorPairFiltering = desc.;

//	nx_desc.maxTimestep = desc.;

//	nx_desc.maxIter = desc.;


/*
	nx_desc.timeStepMethod = desc.;

	nx_desc.maxBounds = desc.;

	nx_desc.limits = desc.;

	nx_desc.simType = desc.;

	nx_desc.groundPlane = desc.;

	nx_desc.boundsPlanes = desc.;

	nx_desc.flags = desc.;

	nx_desc.customScheduler = desc.;

	nx_desc.simThreadStackSize = desc.;

	nx_desc.simThreadPriority = desc.;

	nx_desc.simThreadMask = desc.;

	nx_desc.internalThreadCount = desc.;

	nx_desc.workerThreadStackSize = desc.;

	nx_desc.workerThreadPriority = desc.;

	nx_desc.threadMask = desc.;

	nx_desc.backgroundThreadCount = desc.;

	nx_desc.backgroundThreadMask = desc.;

	//Defines the up axis for your world. This is used to accelerate scene queries like raycasting or sweep tests. Internally, a 2D structure is used instead of a 3D one whenever an up axis is defined. This saves memory and is usually faster. = desc.;
	nx_desc.upAxis = desc.;

	nx_desc.subdivisionLevel = desc.;

	nx_desc.staticStructure = desc.;

	nx_desc.dynamicStructure = desc.;
*/
//	nx_desc.userData = NULL;

	NxScene *pNxScene = m_pPhysicsSDK->createScene( nx_desc );

	if( !pNxScene )
		LOG_PRINT_ERROR( "NxPhysicsSDK::createScene() failed." );

	CNxPhysScene *pNxPhysScene = new CNxPhysScene( pNxScene, m_pPhysicsSDK );

	pNxPhysScene->SetUserContactReport( desc.pUserContactReport );

	return pNxPhysScene;
}


void CNxPhysicsEngine::ReleaseScene( CScene*& pScene )
{
	// m_pPhysicsSDK->releaseScene() is called in dtor of CNxPhysScene

/*	CNxPhysScene *pNxPhysScene = dynamic_cast<CNxPhysScene *> pScene;

	if( pNxPhysScene )
	{
		m_pPhysicsSDK->releaseScene( *(pNxPhysScene->GetNxScene()) );
	}
	else
		LOG_PRINT_ERROR( "Incompatible scene instance" );
*/
	SafeDelete( pScene );

}


CConvexMesh *CNxPhysicsEngine::CreateConvexMesh( CStream& phys_stream )
{
	CNxPhysStream nx_stream( &(phys_stream.m_Buffer), true );

	NxConvexMesh *pNxMesh = m_pPhysicsSDK->createConvexMesh( nx_stream );
	if( !pNxMesh )
		LOG_PRINT_ERROR( " NxPhysicsSDK::createConvexsMesh() failed. Cannot create a convex mesh." );

	return NULL;
}


void CNxPhysicsEngine::ReleaseConvexMesh( CConvexMesh*& pConvexMesh )
{
	SafeDelete( pConvexMesh );
}


CTriangleMesh *CNxPhysicsEngine::CreateTriangleMesh( physics::CStream& phys_stream )
{
	CNxPhysStream nx_stream( &(phys_stream.m_Buffer), true );

	NxTriangleMesh *pNxMesh = m_pPhysicsSDK->createTriangleMesh( nx_stream );
	if( !pNxMesh )
		LOG_PRINT_ERROR( " NxPhysicsSDK::createTriangleMesh() failed. Cannot create a triangle mesh." );

	return new CNxPhysTriangleMesh( pNxMesh, m_pPhysicsSDK );
}


//CClothMesh *CNxPhysicsEngine::CreateClothMesh( physics::CStream& phys_stream )
CTriangleMesh *CNxPhysicsEngine::CreateClothMesh( physics::CStream& phys_stream )
{
	CNxPhysStream nx_stream( &(phys_stream.m_Buffer), true );

	NxClothMesh *pNxMesh = m_pPhysicsSDK->createClothMesh( nx_stream );
	if( !pNxMesh )
		LOG_PRINT_ERROR( " NxPhysicsSDK::createClothMesh() failed. Cannot create a cloth mesh." );

	return new CNxPhysClothMesh( pNxMesh, m_pPhysicsSDK );
}


void CNxPhysicsEngine::ReleaseTriangleMesh( CTriangleMesh*& pTriangleMesh )
{
	// m_pPhysicsSDK->releaseTriangleMesh() is called in dtor of CNxPhysTriangleMesh

/*	CNxPhysTriangleMesh *pNxTriangleMesh = dynamic_cast<CNxPhysTriangleMesh *> pTriangleMesh;

	if( pNxTriangleMesh )
	{
		m_pPhysicsSDK->releaseTriangleMesh( *(pNxTriangleMesh->GetNxTriangleMesh()) );
	}
	else
		LOG_PRINT_ERROR( "Incompatible triangle mesh instance" );
*/
	SafeDelete( pTriangleMesh );
}


void CNxPhysicsEngine::ReleaseClothMesh( CTriangleMesh*& pClothMesh )
{
	SafeDelete( pClothMesh );
}
