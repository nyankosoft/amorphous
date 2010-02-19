#ifndef  __PhysScene_H__
#define  __PhysScene_H__


#include "../base.hpp"
#include "3DMath/Vector3.hpp"
#include "fwd.hpp"
#include "Ray.hpp"
#include "Enums.hpp"


namespace physics
{


class CSceneLimits
{
public:
	/// Expected max number of actors. 
	U32  MaxNumActors;
	 
	/// Expected max number of bodies. 
	U32  MaxNumBodies;
	 
	/// Expected max number of static shapes. 
	U32  MaxNumStaticShapes;
	 
	/// Expected max number of dynamic shapes. 
	U32  MaxNumDynamicShapes;
	 
	/// Expected max number of joints. 
	U32  MaxNumJoints;

public:

	CSceneLimits()
		:
	MaxNumActors(0),
	MaxNumBodies(0),
	MaxNumStaticShapes(0),
	MaxNumDynamicShapes(0),
	MaxNumJoints(0)
	{}
};


class CScene
{
public:

	CScene() {}

	virtual ~CScene() {}

	/// Initialize the scene
	/// called from physics engine?
	/// Returns true on success
	virtual bool Init( const CSceneDesc& desc ) = 0;

	/// Get the scene flags. 
//	virtual U32 GetFlags () const = 0; 

	/// Get the simulation type. 
//	virtual NxSimulationType GetSimType () const = 0; 

	/// Gets a private interface to an internal debug object. 
//	virtual void * GetInternal (void) = 0; 

	/// Sets a constant gravity for the entire scene. 
	virtual void SetGravity ( const Vector3 &vec ) = 0; 

	/// Retrieves the current gravity setting. 
	virtual void GetGravity ( Vector3 &vec ) = 0;

	inline Vector3 GetGravity();

	/// Flush the scene's command queue for processing. 
	virtual void FlushStream () = 0; 

	/// Sets simulation timing parameters used in simulate(elapsedTime). 
	virtual void SetTiming ( Scalar maxTimestep=1.0f/60.0f, U32 maxIter=8, Timestep::Method timestep_method = Timestep::Fixed ) = 0;

	/// Retrieves simulation timing parameters. 
	virtual void GetTiming ( Scalar &maxTimestep, U32 &maxIter, Timestep::Method& timestep_method, U32 *numSubSteps=NULL) const = 0;

	/// Retrieves the debug renderable. 
//	virtual const NxDebugRenderable * GetDebugRenderable () = 0; 

	/// Call this method to retrieve the Physics SDK. 
//	virtual NxPhysicsSDK & GetPhysicsSDK () = 0; 

	/// Call this method to retrieve statistics about the current scene. 
	virtual void GetStats ( PhysSceneStats &stats ) const {}

	/// Call this method to retrieve extended statistics about the current scene. 
//	virtual const PhysSceneStats2 * GetStats2 () const = 0; 

	/// Call to retrieve the expected object count limits set in	 the scene descriptor. 
	virtual void GetLimits ( CSceneLimits &limits ) const {}

	/// Not yet implemented! 
//	virtual void SetMaxCPUForLoadBalancing (Scalar cpuFraction) = 0; 

	/// Call to get the maximum CPU for use when load-balancing. 
//	virtual Scalar GetMaxCPUForLoadBalancing () = 0; 

	/// This is a query to see if the scene is in a state that allows the application to update scene state. 
//	virtual bool IsWritable () = 0; 

	/// Advances the simulation by an elapsedTime time. 
	virtual void Simulate ( Scalar elapsed_time ) = 0; 

	/// This checks to see if the part of the simulation run whose results you are interested in has completed. 
	virtual bool CheckResults ( SimulationStatus::Status status, bool block=false) = 0; 

	virtual bool FetchResults ( SimulationStatus::Status status, bool block=false, U32 *errorState=0) = 0; 

	virtual void FlushCaches () = 0; 

//	virtual const NxProfileData * ReadProfileData (bool clearData) = 0; 

	/// Poll for work to execute on the current thread. 
//	virtual NxThreadPollResult PollForWork (NxThreadWait waitType) = 0; 

	/// Reset parallel simulation. 
//	virtual void  resetPollForWork () = 0; 

	/// Polls for background work. 
//	virtual NxThreadPollResult  pollForBackgroundWork (NxThreadWait waitType) = 0; 

	/// Release  threads which are blocking to allow the SDK to be destroyed safely. 
//	virtual void  shutdownWorkerThreads () = 0; 

	/// Blocks all parallel raycast/overlap queries. 
//	virtual void  lockQueries () = 0; 

	/// Unlock parallel raycast/overlap queries. 
//	virtual void  unlockQueries () = 0; 

	/// Create a batched query object. 
//	virtual NxSceneQuery * CreateSceneQuery (const NxSceneQueryDesc &desc) = 0; 

	/// Release  a scene query object. 
//	virtual bool  releaseSceneQuery (NxSceneQuery &query) = 0; 

	/// Creates an actor in this scene. 
	virtual CActor * CreateActor(const CActorDesc &desc) = 0; 

	/// Deletes the specified actor. 
//	virtual void ReleaseActor (CActor &actor) = 0; 
	virtual void ReleaseActor( CActor*& pActor ) = 0; 

	/// Creates a joint. 
	virtual CJoint * CreateJoint(const CJointDesc &jointDesc) = 0; 

	/// Deletes the specified joint. 
	virtual void ReleaseJoint( CJoint*& pJoint ) = 0; 

	/// Creates an effector. 
//	virtual NxEffector * CreateEffector (const NxEffectorDesc &desc) = 0; 

	/// Deletes the effector passed. 
//	virtual void ReleaseEffector (NxEffector &effector) = 0; 

	/// Creates a force field. 
//	virtual NxForceField * CreateForceField (const NxForceFieldDesc &forceFieldDesc) = 0; 

	/// Deletes the force field passed. 
//	virtual void ReleaseForceField (NxForceField &forceField) = 0; 

	/// Gets the number of force fields in the scene. 
//	virtual U32 GetNbForceFields () const = 0; 

	/// Gets the force fields in the scene. 
//	virtual NxForceField ** GetForceFields () = 0; 

	/// Creates a new Material. 
	virtual CMaterial * CreateMaterial (const CMaterialDesc &matDesc) = 0;

	/// Deletes the specified material. 
	virtual void ReleaseMaterial( CMaterial*& pMaterial ) = 0;
/*
	/// Creates a scene compartment. 
	virtual NxCompartment * CreateCompartment (const NxCompartmentDesc &compDesc) = 0; 

	/// Returns the number of compartments created in the scene. 
	virtual U32 GetNbCompartments () const = 0; 

	/// Writes the scene's array of NxCompartment pointers to a user buffer.  
	virtual U32 GetCompartmentArray (NxCompartment **userBuffer, U32 bufferSize, U32 &usersIterator) const = 0; 
*/
/*
	virtual void SetActorPairFlags(NxActor& actorA, NxActor& actorB, NxU32 nxContactPairFlag) = 0;

	virtual U32 GetActorPairFlags(NxActor& actorA, NxActor& actorB) const = 0;

	virtual void SetShapePairFlags(NxShape& shapeA, NxShape& shapeB, NxU32 nxContactPairFlag) = 0;

	virtual U32 GetShapePairFlags(NxShape& shapeA, NxShape& shapeB) const = 0;

	virtual U32 GetNbPairs() const = 0;

	virtual U32 GetPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const = 0;
*/
	virtual void SetGroupCollisionFlag( U16 group1, U16 group2, bool enable ) = 0;

	virtual bool GetGroupCollisionFlag( U16 group1, U16 group2 ) const = 0;
/*
	virtual void SetDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance) = 0;

	virtual NxConstraintDominance getDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2) const = 0;

	virtual void setActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2, NxU32 flags) = 0;

	virtual U32 getActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2) const = 0;

	virtual U32 getNbActorGroupPairs() const = 0;

	virtual U32 getActorGroupPairArray(NxActorGroupPair * userBuffer, NxU32 bufferSize, NxU32 & userIterator) const = 0;
 */
	/// \param coll_group collision group of the ray
//	virtual CShape *RaycastClosestShape( const CRay& world_ray, shapetype, CRaycastHit& hit, int coll_group, Scalar max_dist ) = 0;

//	inline CShape *LineSegmentTestClosestShape( const CLineSegment& world_line_segment, shapetype, CRaycastHit& hit, int coll_group );

	/// test ray against all the types of shapes (both dynamic and static)
	/// \param world_ray [in]
	/// \param hit [out]
	/// \param coll_group [in]
	/// \param max_dist [in]
	virtual CShape *RaycastClosestShape( const CRay& world_ray, CRaycastHit& hit, int coll_group, Scalar max_dist ) = 0;

	/// test line segment against all the types of shapes (both dynamic and static)
	/// \param world_line_segment [in]
	/// \param hit [out]
	/// \param coll_group [in]
	inline CShape *LineSegmentTestClosestShape( const CLineSegment& world_line_segment, CRaycastHit& hit, int coll_group );

//	virtual U32 RaycastAllShapes( const CRay& world_ray, CRacastReport& report, shapetype, int ray_group,  );

	virtual CCloth *CreateCloth( CClothDesc& desc ) = 0;

	virtual void ReleaseCloth( CCloth*& pCloth ) = 0; 
};

//============================== inline implementations ==============================


inline Vector3 CScene::GetGravity()
{
	Vector3 vec;
	GetGravity( vec );
	return vec;
}


inline CShape *CScene::LineSegmentTestClosestShape( const CLineSegment& world_line_segment,
														    CRaycastHit& hit,
															int coll_group )
{
	Vector3 vSegment = world_line_segment.End - world_line_segment.Start;
	Scalar seg_length = Vec3Length( vSegment );
	CRay ray( world_line_segment.Start, vSegment / seg_length );

	return RaycastClosestShape( ray, hit, coll_group, seg_length );
}


} // namespace physics


#endif		/*  __PhysScene_H__  */
