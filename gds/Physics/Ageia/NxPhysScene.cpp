#include "NxPhysScene.hpp"
#include "NxPhysActor.hpp"
#include "NxPhysCloth.hpp"
//#include "NxPhysJoint.hpp"
#include "NxPhysMaterial.hpp"
#include "NxPhysConv.hpp"
#include "NxPhysShapeDescFactory.hpp"
#include "NxPhysShapeFactory.hpp"
#include "NxPhysClothMesh.hpp"

#include "../MaterialDesc.hpp"
#include "../ClothDesc.hpp"
#include "../RaycastHit.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/memory_helpers.hpp"

using namespace physics;


static NxU32 ToNxActorFlag( U32 src_flags )
{
	NxU32 dest_flags = 0;
	if( src_flags & CActor::DisableCollision )      dest_flags |= NX_AF_DISABLE_COLLISION;
	if( src_flags & CActor::DisableResponse )       dest_flags |= NX_AF_DISABLE_RESPONSE;
//	if( src_flags &  )                              dest_flags |= NX_AF_LOCK_COM;
	if( src_flags & CActor::FluidDisableCollision ) dest_flags |= NX_AF_FLUID_DISABLE_COLLISION;
//	if( src_flags &  )                              dest_flags |= NX_AF_CONTACT_MODIFICATION;
//	if( src_flags &  )                              dest_flags |= NX_AF_FORCE_CONE_FRICTION;
//	if( src_flags &  )                              dest_flags |= NX_AF_USER_ACTOR_PAIR_FILTERING;

	return dest_flags;
}


static NxU32 ToNxBodyFlags( U32 phys_body_flags )
{
	NxU32 nx_flags = 0;
	if( phys_body_flags & BodyFlag::Kinematic )      nx_flags |= NX_BF_KINEMATIC;
	if( phys_body_flags & BodyFlag::DisableGravity ) nx_flags |= NX_BF_DISABLE_GRAVITY;
//	if( phys_body_flags & PhysBodyFlag::??? )        nx_flags |= NX_BF_FILTER_SLEEP_VEL;

	return nx_flags;
}


CNxPhysScene::CNxPhysScene( NxScene *pScene, NxPhysicsSDK* pPhysicsSDK )
:
m_pScene(pScene),
m_pPhysicsSDK(pPhysicsSDK)
{}


CNxPhysScene::~CNxPhysScene()
{
	m_pPhysicsSDK->releaseScene( *m_pScene );
	m_pScene = NULL;
}


bool CNxPhysScene::Init( const CSceneDesc& desc )
{
/*	CNxPhysicsEngine *m_pNxPhysEngineImpl
		= dynamic_cast<CNxPhysicsEngine *> PhysicsEngine.GetImpl();

	if( m_pNxPhysEngineImpl )
	{
		m_pPhysicsSDK = m_pNxPhysEngineImpl->GetPhysicsSDK();
	}
*/

	// Create a scene
//	NxSceneDesc scene_desc;

//	scene_desc.gravity               = NxVec3(0.0f,-9.8f,0.0f); // gravity to apply to objects in the scene.

//	m_pScene = (engine_singleton).GetPhysicsSDK()->createScene(scene_desc);

	return true;
}


U32 CNxPhysScene::GetFlags() const
{
	return m_pScene->getFlags();
}


void CNxPhysScene::GetGravity( Vector3 &vec )
{
	NxVec3 dest;
	m_pScene->getGravity(dest);
	vec = ToVector3(dest);
}


void CNxPhysScene::SetTiming (Scalar maxTimestep, U32 maxIter, Timestep::Method timestep_method )
{
	m_pScene->setTiming( maxTimestep, maxIter, ToNxTimestepMethod(timestep_method) );
}


void CNxPhysScene::GetTiming (Scalar &maxTimestep, U32 &max_iter, Timestep::Method& timestep_method, U32 *num_sub_steps ) const
{
	NxU32 maxIter;
	NxTimeStepMethod timestepMethod;
	NxU32 numSubSteps;
	m_pScene->getTiming( maxTimestep, maxIter, timestepMethod, &numSubSteps );

	*num_sub_steps = numSubSteps;
	max_iter = maxIter;
	timestep_method = ToTimestepMethod( timestepMethod );
}

void CNxPhysScene::GetStats ( PhysSceneStats &stats ) const
{
}

void CNxPhysScene::GetLimits ( CSceneLimits &limits ) const
{
}


NxSimulationStatus ToNxPhysSimStatus( SimulationStatus::Status status )
{
	switch( status )
	{
	case SimulationStatus::RigidBodyFinished: return NX_RIGID_BODY_FINISHED;
	case SimulationStatus::AllFinished:       return NX_ALL_FINISHED;
	case SimulationStatus::PrimaryFinished:   return NX_PRIMARY_FINISHED;
	default: return NX_RIGID_BODY_FINISHED;
	}

	return NX_RIGID_BODY_FINISHED;
}


bool CNxPhysScene::CheckResults( SimulationStatus::Status status, bool block )
{
	return m_pScene->checkResults( ToNxPhysSimStatus( status ), block );
}


bool CNxPhysScene::FetchResults( SimulationStatus::Status status, bool block, U32 *errorState )
{
	NxU32 errStat = 0;
	bool res = m_pScene->fetchResults( ToNxPhysSimStatus( status ), block, &errStat );
	if( errorState )
		*errorState = errStat;

	return res;
}


CActor* CNxPhysScene::CreateActor( const CActorDesc& desc )
{
	if( !m_pScene )
		return NULL;

	NxActorDesc nx_actor_desc;
	nx_actor_desc.globalPose      = ToNxMat34( desc.WorldPose );
//	nx_actor_desc.???     = ToNxVec3( desc.vVelocity );
	nx_actor_desc.density         = desc.fDensity;
	nx_actor_desc.group           = desc.CollisionGroup;
//	nx_actor_desc.dominanceGroup  = desc.;
	nx_actor_desc.userData        = NULL; //desc.;
	nx_actor_desc.name            = ""; //desc.Name;
//	nx_actor_desc.compartment     = desc.;
	nx_actor_desc.flags           = ToNxActorFlag( desc.ActorFlags );

	NxBodyDesc nx_body_desc;
	if( ( (desc.BodyDesc.Flags) & (BodyFlag::Static) ) == 0 )
	{
		// non-static actor - set body desc
		nx_actor_desc.body                = &nx_body_desc;
//		nx_body_desc.massLocalPose        = ToNxMat34( desc.BodyDesc.MassLocalPose );
//		nx_body_desc.massSpaceInertia     = 

		// Mass is computed from a density and the shapes mass/density.
		// Setting mass here makes the actor desc invalid.
/*		nx_body_desc.mass                 = desc.BodyDesc.fMass;*/

		nx_body_desc.linearVelocity       = ToNxVec3( desc.BodyDesc.LinearVelocity );
		nx_body_desc.angularVelocity      = ToNxVec3( desc.BodyDesc.AngularVelocity );
//		nx_body_desc.wakeUpCounter        =
//		nx_body_desc.linearDamping        =
//		nx_body_desc.angularDamping       =
//		nx_body_desc.maxAngularVelocity 
//		nx_body_desc.CCDMotionThreshold 
		nx_body_desc.flags                = 0;//ToNxBodyFlags( desc.BodyDesc.Flags );
//		nx_body_desc.sleepLinearVelocity 
//		nx_body_desc.sleepAngularVelocity 
//		nx_body_desc.solverIterationCount 
//		nx_body_desc.sleepEnergyThreshold 
//		nx_body_desc.sleepDamping 

		if( !nx_body_desc.isValid() )
			LOG_PRINT_ERROR( "An invalid body desc" );
	}
 

	CNxPhysShapeDescFactory desc_factory;
//	vector<NxShapeDesc *> vecpNxShapeDesc;
	for( size_t i=0; i<desc.vecpShapeDesc.size(); i++ )
	{
		nx_actor_desc.shapes.push_back( desc_factory.CreateNxShapeDesc( *(desc.vecpShapeDesc[i]) ) );
		if( !nx_actor_desc.shapes.back() )
		{
			LOG_PRINT_WARNING( "Failed to create a shape." );
			continue;
		}

		// set material index
		nx_actor_desc.shapes.back()->materialIndex = desc.vecpShapeDesc[i]->MaterialIndex;
//		nx_actor_desc.shapes.push_back = vecpNxShapeDesc[i];
	}

	if( !nx_actor_desc.isValid() )
		LOG_PRINT_ERROR( "An invalid actor desc" );

	// create an actor of ageia physics
	NxActor *pNxActor = m_pScene->createActor( nx_actor_desc );

	for( unsigned int i=0; i<nx_actor_desc.shapes.size(); i++ )
		SafeDelete( nx_actor_desc.shapes[i] );

//	SafeDeleteVector( vecpNxShapeDesc );

	if( !pNxActor )
	{
		LOG_PRINT_ERROR( " - Failed to create an NxActor." );
		return NULL;
	}

	//
	// create an instance of CNxPhysActor
	//
	CNxPhysActor *pPhysActor = new CNxPhysActor( pNxActor );

	pPhysActor->m_BodyFlags = desc.BodyDesc.Flags;

	// retrieve shapes from 'pNxActor' and set them to 'pPhysActor'
	CNxPhysShapeFactory factory;
	NxU32 j, num_shapes = pNxActor->getNbShapes();
	NxShape *const*apShape = pNxActor->getShapes();
	for( j=0; j<num_shapes; j++ )
	{
		pPhysActor->AddPhysShape( factory.CreateShape( apShape[j] ) );
	}

	return pPhysActor;
}


void CNxPhysScene::ReleaseActor( CActor*& pActor )
{
	CNxPhysActor *pNxActor = dynamic_cast<CNxPhysActor *> (pActor);

	m_pScene->releaseActor( *(pNxActor->GetNxActor()) );

	SafeDelete( pActor );
}


CJoint* CNxPhysScene::CreateJoint( const CJointDesc& desc )
{
	if( !m_pScene )
		return NULL;
/*
	NxJointDesc nx_joint_desc;

	for( int i=0; i<2; i++ )
		nx_joint_desc.actor[i] = (dynamic_cast<CNxPhysActor *>desc.pActor[i])->GetNxActor();
	nx_joint_desc.localNormal[2] = 
	nx_joint_desc.localAxis[2]   = 
	nx_joint_desc.localAnchor[2] = 
	nx_joint_desc.maxForce       =
	nx_joint_desc.maxTorque      =
	nx_joint_desc.userData       =
//	nx_joint_desc.name           =
	nx_joint_desc.jointFlags     =

	CNxPhysShapeDescFactory desc_factory;
	vector<NxShapeDesc *> vecpNxShapeDesc;
	for( size_t i=0; i<nx_actor_desc.vecpShapeDesc.size(); i++ )
	{
		vecpNxShapeDesc.push_back( desc_factory.CreateNxShapeDesc( desc.vecpShapeDesc[i] ) );
		nx_actor_desc.shapes.push_back = vecpNxShapeDesc[i];
	}

	// create an actor of ageia physics
	NxActor *pNxJoint = m_pScene->createJoint( nx_joint_desc );

	SafeDeleteVector( vecpNxShapeDesc );

	if( !pNxJoint )
	{
		LOG_PRINT_ERROR( " - Failed to create an NxJoint." );
		return NULL;
	}

	//
	// create a derived class instance of CNxPhysJoint
	//
	CNxPhysJoint *pPhysJoint = new CreateNxJoint( pNxJoint );

	// retrieve shapes from 'pNxActor' and set them to 'pPhysActor'
	CNxPhysShapeFactory factory;
	NxU32 j, num_shapes = pNxActor->getNbShapes();
	NxShape *apShape = pNxActor->getShapes();
	for( j=0; j<num_shapes; j++ )
	{
		pPhysActor->AddPhysShape( factory.CreateNxPhysShape( apShape[j] ) );
	}
*/
	return NULL;
}


/// Deletes the specified joint. 
void CNxPhysScene::ReleaseJoint( CJoint*& pJoint )
{
//	m_pScene->releaseJoint( pJoint->GetNxJoint() );
}


CMaterial *CNxPhysScene::CreateMaterial (const CMaterialDesc &desc)
{
	NxMaterialDesc nx_desc;
	nx_desc.dynamicFriction         = (NxReal)desc.DynamicFriction;
	nx_desc.staticFriction          = (NxReal)desc.StaticFriction;
	nx_desc.restitution             = (NxReal)desc.Restitution;
//	nx_desc.dynamicFrictionV        = (NxReal)desc.;
//	nx_desc.staticFrictionV         = (NxReal)desc.;
//	nx_desc.dirOfAnisotropy         = (NxVec3)desc.;
//	nx_desc.flags                   = (NxU32)desc.;
//	nx_desc.frictionCombineMode     = (NxCombineMode)desc.;
//	nx_desc.restitutionCombineMode  = (NxCombineMode)desc.;
//	nx_desc.spring                  = (NxSpringDesc *)desc.;

	NxMaterial *pNxMaterial = m_pScene->createMaterial( nx_desc );
	return new CNxPhysMaterial( pNxMaterial, m_pScene );
}


void CNxPhysScene::ReleaseMaterial( CMaterial*& pMaterial )
{
//	CNxPhysMaterial *pNxMat = dynamic_cast<CNxPhysMaterial *> (pMaterial);

//	m_pScene->releaseMaterial( *(pNxMat->GetNxMaterial()) );

//	SafeDelete( pNxMat );

	SafeDelete( pMaterial );
}


void CNxPhysScene::SetGroupCollisionFlag( U16 group1, U16 group2, bool enable )
{
	m_pScene->setGroupCollisionFlag( group1, group2, enable );
}


bool CNxPhysScene::GetGroupCollisionFlag( U16 group1, U16 group2 ) const
{
	return m_pScene->getGroupCollisionFlag( group1, group2 );
}


CShape *CNxPhysScene::RaycastClosestShape( const physics::CRay& world_ray, CRaycastHit& hit, int coll_gorup, Scalar max_dist )
{
	NxRay nx_ray( ToNxVec3(world_ray.Origin), ToNxVec3(world_ray.Direction) );

	NxRaycastHit nx_hit;
	NxU32 groups = 0;

	NxShape *pNxShape = m_pScene->raycastClosestShape( nx_ray, NX_ALL_SHAPES, nx_hit/*, groups, max_dist, hint_flags, groups_mask*/ );

	if( pNxShape )
	{
		hit.pShape         = (CShape *)(nx_hit.shape->userData);
		hit.WorldImpactPos = ToVector3( nx_hit.worldImpact );
		hit.WorldNormal    = ToVector3( nx_hit.worldNormal );
		hit.fDistance      = nx_hit.distance;
		hit.MaterialID     = nx_hit.materialIndex;
//		hit.Flags          = nx_hit.;
//		hit.               = nx_hit.;
//		hit.               = nx_hit.;

		// retrieve NxShape pointer from CShape
		return NULL;/*(CShape *)pNxShape->userData*/
	}
	else
		return NULL;
}


CCloth *CNxPhysScene::CreateCloth( CClothDesc& desc )
{
	CNxPhysClothMesh *pClothMesh = dynamic_cast<CNxPhysClothMesh *>(desc.pClothMesh);
	if( !pClothMesh )
		return NULL;

	NxClothMesh *pNxClothMesh = pClothMesh->GetNxClothMesh();
	if( !pNxClothMesh )
		return NULL;

	NxClothDesc nx_cloth_desc;
	nx_cloth_desc.globalPose                   = ToNxMat34(desc.WorldPose);
	nx_cloth_desc.density                      = desc.Density;
	nx_cloth_desc.thickness                    = desc.Thickness;
	nx_cloth_desc.stretchingStiffness          = desc.StretchingStiffness;
	nx_cloth_desc.bendingStiffness             = desc.BendingStiffness;
	nx_cloth_desc.friction                     = desc.Friction;
	nx_cloth_desc.pressure                     = desc.Pressure;
	nx_cloth_desc.collisionGroup               = 0;//desc.CollisionGroup;
//	nx_cloth_desc.dampingCoefficient           = desc.DampingCoefficient;
	nx_cloth_desc.validBounds                  = ToNxBounds3(desc.ValidBounds);
	nx_cloth_desc.externalAcceleration         = ToNxVec3(desc.ExternalAcceleration);
	nx_cloth_desc.windAcceleration             = ToNxVec3(desc.WindAcceleration);
//	nx_cloth_desc.collisionResponseCoefficient = desc.CollisionResponseCoefficient
	nx_cloth_desc.clothMesh                    = pNxClothMesh;
//	nx_cloth_desc.meshData             = ???;
	nx_cloth_desc.flags                        = NX_CLF_GRAVITY;

	bool is_nx_desc_valid = nx_cloth_desc.isValid();

	NxCloth *pNxCloth = m_pScene->createCloth( nx_cloth_desc );
	if( !pNxCloth )
		return NULL;

	CCloth *pCloth = new CNxPhysCloth( pNxCloth );

	pCloth->SetMeshData( desc.MeshData );

	return pCloth;

//	return NULL;
}


void CNxPhysScene::ReleaseCloth( CCloth*& pCloth )
{
	CNxPhysCloth *pNxCloth = dynamic_cast<CNxPhysCloth *>(pCloth);
	m_pScene->releaseCloth( *(pNxCloth->GetNxCloth()) );
}
