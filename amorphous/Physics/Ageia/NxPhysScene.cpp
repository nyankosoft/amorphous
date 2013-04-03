#include "NxPhysScene.hpp"
#include "NxPhysActor.hpp"
#include "NxPhysJointImpls.hpp"
#include "NxPhysCloth.hpp"
#include "NxPhysJointDescVisitor.hpp"
#include "NxPhysMaterial.hpp"
#include "NxPhysConv.hpp"
#include "NxPhysShapeDescFactory.hpp"
#include "NxPhysShapeFactory.hpp"
#include "NxPhysClothMesh.hpp"

#include "../FixedJoint.hpp"
#include "../RevoluteJoint.hpp"
#include "../SphericalJoint.hpp"
#include "../MaterialDesc.hpp"
#include "../ClothDesc.hpp"
#include "../RaycastHit.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/memory_helpers.hpp"


namespace amorphous
{

using std::vector;
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


static NxU32 ToNxSweepFlags( U32 sweep_flags )
{
	NxU32 nx_flags = 0;
	if( sweep_flags & SweepFlag::STATICS )  nx_flags |= NX_SF_STATICS;
	if( sweep_flags & SweepFlag::DYNAMICS ) nx_flags |= NX_SF_DYNAMICS;
	if( sweep_flags & SweepFlag::ALL_HITS ) nx_flags |= NX_SF_ALL_HITS;

	return nx_flags;
}


//==================================================================================
// CNxPhysicsUserContactReport
//==================================================================================

void CNxPhysicsUserContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	NxContactStreamIterator nx_itr(pair.stream);
	CNxPhysContactStreamIterator itr( nx_itr );
	CContactPair cp(itr);

	CNxPhysActor *pActor0 = (CNxPhysActor *)pair.actors[0]->userData;
	CNxPhysActor *pActor1 = (CNxPhysActor *)pair.actors[1]->userData;

	cp.pActors[0] = pActor0;
	cp.pActors[1] = pActor1;

	U32 event_flags = events;

	m_pUserContactReport->OnContactNotify( cp, event_flags );
}


//==================================================================================
// CNxPhysScene
//==================================================================================

CNxPhysScene::CNxPhysScene( NxScene *pScene, NxPhysicsSDK* pPhysicsSDK )
:
m_pScene(pScene),
m_pPhysicsSDK(pPhysicsSDK)
{
	if( m_pScene )
	{
		m_pScene->setUserContactReport( &m_NxUserContactReport );
		m_pScene->setUserTriggerReport( &m_NxUserTriggerReport );
	}
	else
		LOG_PRINT_ERROR( " The ctor received a NULL pointer of NxScene." );

	// Set up the default material
	int defualt_nx_material_index = 0;
	NxMaterial *pNxMaterial = m_pScene->getMaterialFromIndex( defualt_nx_material_index );
	m_vecpNxPhysMaterial.resize( 1 );
	m_vecpNxPhysMaterial[0].reset( new CNxPhysMaterial( pNxMaterial, m_pScene ) );
}


CNxPhysScene::~CNxPhysScene()
{
	// clear the materials for the following 2 reasons
	// 1. Release the default material at m_vecpNxPhysMaterial[0]
	//    The default material is created in the ctor of CNxPhysScene
	//    and always released here.
	// 2. Set NULL to m_pScene of all the materials
	//    This prevents access to an already released m_pScene pointer
	//    when client code does not release the materials
	//    via CNxPhysScene::ReleaseMaterial() before releasing the scene.
	for( size_t i=0; i<m_vecpNxPhysMaterial.size(); i++ )
	{
		if( !m_vecpNxPhysMaterial[i] )
			continue;

		m_vecpNxPhysMaterial[i]->m_pScene = NULL;

		m_vecpNxPhysMaterial[i].reset();
	}

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
		nx_body_desc.flags                = ToNxBodyFlags( desc.BodyDesc.Flags );
//		nx_body_desc.sleepLinearVelocity 
//		nx_body_desc.sleepAngularVelocity 
//		nx_body_desc.solverIterationCount 
//		nx_body_desc.sleepEnergyThreshold 
//		nx_body_desc.sleepDamping 

		if( !nx_body_desc.isValid() )
			LOG_PRINT_ERROR( "An invalid body desc" );
	}
 

	CNxPhysShapeDescFactory desc_factory;
	for( size_t i=0; i<desc.vecpShapeDesc.size(); i++ )
	{
		CShapeDesc& src_shape_desc = *(desc.vecpShapeDesc[i]);

		nx_actor_desc.shapes.push_back( desc_factory.CreateNxShapeDesc( src_shape_desc ) );
		if( !nx_actor_desc.shapes.back() )
		{
			LOG_PRINT_WARNING( "Failed to create a shape." );
			continue;
		}
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

	// TODO: safe storage of the reference from pNxActor to pPhysActor
	pNxActor->userData = pPhysActor;

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
	if( !pActor )
		return;

	CNxPhysActor *pNxActor = dynamic_cast<CNxPhysActor *> (pActor);

	m_pScene->releaseActor( *(pNxActor->GetNxActor()) );

	SafeDelete( pActor );
}


physics::CJoint *CreateJoint( physics::JointType::Name type )
{
	using namespace physics;

	switch( type )
	{
	case JointType::FIXED:     return new CFixedJoint;
	case JointType::REVOLUTE:  return new CRevoluteJoint;
	case JointType::SPHERICAL: return new CSphericalJoint;
	default:
		return NULL;
	}
}


static physics::CNxPhysJointImpl *CreateNxJointImpl( physics::JointType::Name type )
{
	using namespace physics;

	switch( type )
	{
	case JointType::FIXED:     return new CNxPhysFixedJointImpl;
	case JointType::REVOLUTE:  return new CNxPhysRevoluteJointImpl;
	case JointType::SPHERICAL: return new CNxPhysSphericalJointImpl;
	default:
		return NULL;
	}
}


CJoint* CNxPhysScene::CreateJoint( const CJointDesc& desc )
{
	if( !m_pScene )
		return NULL;

	CNxPhysJointDescVisitor jd_visitor;
	desc.Accept( jd_visitor );

	if( !jd_visitor.m_pNxJointDesc )
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
*/

	// create an actor of ageia physics
	NxJoint *pNxJoint = m_pScene->createJoint( *(jd_visitor.m_pNxJointDesc) );

//	SafeDeleteVector( vecpNxShapeDesc );

	if( !pNxJoint )
	{
		LOG_PRINT_ERROR( " - Failed to create an NxJoint." );
		return NULL;
	}

	CJoint *pJoint = amorphous::CreateJoint( desc.GetType() );

	CNxPhysJointImpl *pNxJointImpl = CreateNxJointImpl( desc.GetType() );

	pNxJointImpl->m_pNxJoint = pNxJoint;

	SetJointImpl( *pJoint, *pNxJointImpl );

	return pJoint;
}


/// Deletes the specified joint. 
void CNxPhysScene::ReleaseJoint( CJoint*& pJoint )
{
	if( !m_pScene )
		return;

	if( !pJoint )
		return;

	CJointImpl *pImpl = GetJointImpl( *pJoint );
	if( !pImpl )
	{
		LOG_PRINT_ERROR( " The impl of the specified joint is missing." );
		return;
	}

	CNxPhysJointImpl* pNxJointImpl = dynamic_cast<CNxPhysJointImpl *>(pImpl);
	if( !pNxJointImpl )
	{
		LOG_PRINT_ERROR( " An invalid joint impl." );
		return;
	}

	if( !pNxJointImpl->m_pNxJoint )
	{
		LOG_PRINT_ERROR( " The NxJoint is missing." );
		return;
	}

	m_pScene->releaseJoint( *(pNxJointImpl->m_pNxJoint) );
}


CMaterial *CNxPhysScene::CreateMaterial (const CMaterialDesc &desc)
{
	if( !m_pScene )
		return NULL;

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
	if( !pNxMaterial )
		return NULL;

	NxMaterialIndex material_index = pNxMaterial->getMaterialIndex();
	if( 256 <= material_index )
		ONCE( LOG_PRINT_WARNING( " You are actually using quite a few materials or PhysX implementation changed?" ) );

	while( (NxMaterialIndex)m_vecpNxPhysMaterial.size() <= material_index )
		m_vecpNxPhysMaterial.push_back( boost::shared_ptr<CNxPhysMaterial>() );

	m_vecpNxPhysMaterial[material_index].reset( new CNxPhysMaterial( pNxMaterial, m_pScene ) );

	return new CNxPhysMaterial( pNxMaterial, m_pScene );
}


void CNxPhysScene::ReleaseMaterial( CMaterial*& pMaterial )
{
	int material_id = pMaterial->GetMaterialID();
	if( material_id < 0 || (int)m_vecpNxPhysMaterial.size() <= material_id )
		return;

	// the NxMaterial object is released from the NxScene object it belongs to in the dtor of CNxPhysMaterial
	m_vecpNxPhysMaterial[material_id].reset();
}


CMaterial *CNxPhysScene::GetMaterial( int material_id )
{
	if( material_id < 0 || (int)m_vecpNxPhysMaterial.size() <= material_id )
		return NULL;

	return m_vecpNxPhysMaterial[material_id] ? m_vecpNxPhysMaterial[material_id].get() : NULL;
}


void CNxPhysScene::SetGroupCollisionFlag( U16 group1, U16 group2, bool enable )
{
	m_pScene->setGroupCollisionFlag( group1, group2, enable );
}


bool CNxPhysScene::GetGroupCollisionFlag( U16 group1, U16 group2 ) const
{
	return m_pScene->getGroupCollisionFlag( group1, group2 );
}


void CNxPhysScene::SetActorGroupPairFlags( U16 actor_group1, U16 actor_group2, U32 flags )
{
	m_pScene->setActorGroupPairFlags( (NxActorGroup)actor_group1, (NxActorGroup)actor_group2, flags );
}


U32 CNxPhysScene::GetActorGroupPairFlags( U16 actor_group1, U16 actor_group2 ) const
{
	return m_pScene->getActorGroupPairFlags( (NxActorGroup)actor_group1, (NxActorGroup)actor_group2 );
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


static void SetDefaultNxSweepQueryHit( NxSweepQueryHit& query )
{
	query.t              = 0;
	query.hitShape       = NULL;
	query.sweepShape     = NULL;
	query.userData       = NULL;
	query.internalFaceID = 0;
	query.faceID         = 0;
	query.point          = NxVec3(0,0,0);
	query.normal         = NxVec3(0,0,0);
}


// Performs a linear sweep through space with an oriented box. 
U32 CNxPhysScene::LinearOBBSweep( const OBB3 &world_box,
								  const Vector3 &motion,
								  U32 flags,
								  void *pUserData,
								  U32 num_max_shapes,
								  CSweepQueryHit &shapes,
								  CUserEntityReport< CSweepQueryHit > *pCallback,
								  U32 active_groups,
								  const CGroupsMask *pGroupsMask )
{
	NxBox nx_box;
	nx_box.center  = ToNxVec3( world_box.center.vPosition );
	nx_box.extents = ToNxVec3( world_box.radii );
	nx_box.rot     = ToNxMat33( world_box.center.matOrient );
	if( !nx_box.isValid() )
	{
		LOG_PRINT_WARNING( " An invalid OBB. Returning without performing the specified sweep test." );
		return 0;
	}

	NxSweepQueryHit nx_query;
	SetDefaultNxSweepQueryHit( nx_query );
	NxU32 nx_sweep_flags = ToNxSweepFlags( flags );
	NxGroupsMask *pNxGroupMask = NULL;
	NxUserEntityReport<NxSweepQueryHit> *pNxCallback = NULL;
	NxU32 num_hits = m_pScene->linearOBBSweep( nx_box, ToNxVec3(motion), nx_sweep_flags, pUserData, num_max_shapes, &nx_query, pNxCallback, active_groups, pNxGroupMask );

	if( nx_query.hitShape && nx_query.hitShape->userData )
		shapes.pHitShape   = (CShape *)nx_query.hitShape->userData;

	if( nx_query.sweepShape && nx_query.sweepShape->userData )
		shapes.pSweepShape = (CShape *)nx_query.sweepShape->userData;

	shapes.t      = nx_query.t;
	shapes.Point  = ToVector3( nx_query.point );
	shapes.Normal = ToVector3( nx_query.normal );

	return num_hits;
}


// Performs a linear sweep through space with an oriented capsule. 
U32 CNxPhysScene::LinearCapsuleSweep( const Capsule &world_capsule,
									  const Vector3 &motion,
									  U32 flags,
									  void *pUserData,
									  U32 num_max_shapes,
									  CSweepQueryHit &shapes,
									  CUserEntityReport< CSweepQueryHit > *pCallback,
									  U32 active_groups,
									  const CGroupsMask *pGroupsMask )
{
	LOG_PRINT_ERROR( " Not implemented." );
//	m_pScene->linearCapsuleSweep();
	return 0;
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
	if( !pCloth )
		return;

	CNxPhysCloth *pNxCloth = dynamic_cast<CNxPhysCloth *>(pCloth);
	NxMeshData nx_mesh_data;
	pNxCloth->GetNxCloth()->setMeshData( nx_mesh_data );
	m_pScene->releaseCloth( *(pNxCloth->GetNxCloth()) );
}


void CNxPhysScene::SetUserTriggerReport( CUserTriggerReport *pCallback )
{
	m_NxUserTriggerReport.m_pUserTriggerReport = pCallback;
}


void CNxPhysScene::SetUserContactReport( CUserContactReport *pCallback )
{
	m_NxUserContactReport.m_pUserContactReport = pCallback;
}


} // namespace amorphous
