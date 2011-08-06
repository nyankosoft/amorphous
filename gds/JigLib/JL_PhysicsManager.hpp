#ifndef __JL_PHYSICSMANAGER_H__
#define __JL_PHYSICSMANAGER_H__

//#include "constraint.hpp"


#include "Support/PreAllocDynamicLinkList.hpp"

#include "JL_PhysicsActorDesc.hpp"
#include "JL_PhysicsActor.hpp"
#include "JL_Shape_Box.hpp"
#include "JL_Shape_Capsule.hpp"
#include "TriangleMesh.hpp"
//#include "JL_Shape_.h"
//#include "JL_Shape_.h"

#include "JL_ContactInfo.hpp"
#include "JL_SurfaceMaterial.hpp"

#define NUM_MAX_COLLISIONS_PER_FRAME	512
//#define NUM_MAX_RIGID_BODIES			512

#define NUM_MAX_COLLISION_GROUPS	32

class Constraint;
class Physics_controller;


class CJL_PhysicsManager
{
public:
  CJL_PhysicsManager();

  void Init();

//  void SetTimestep(Scalar timestep) {m_timestep = timestep;}

//  void SetAllowSmallerTimesteps(bool allow) {m_allow_smaller_timesteps = allow;}

  void SetNumCollisionIterations(int num) {m_NumCollisionIterations = num;}

  void SetNumContactIterations(int num) {m_NumContactIterations = num;}

//  void SetNumPenetrationIterations(int num) {m_num_penetration_iterations = num;}
  
//  void SetPenetrationResolveFraction(Scalar frac) {m_penetration_resolve_fraction = frac;}

  void SetTimeScale(Scalar scale) {m_time_scale = scale;}

  /// we get a big speedup by only doing one collision check per update...
  /// at the cost of accuracy
//  void SetReuseCollisions(bool reuse) {m_reuse_collisions = reuse;}

  inline CJL_PhysicsActor *CreateActor( CJL_PhysicsActorDesc &rActorDesc );

  inline void ReleaseActor( CJL_PhysicsActor*& pPhysicsActor );

  void AddConstraint(Constraint * constraint);

  void AddController(Physics_controller * controller);

  void Integrate(Scalar total_time);

  Scalar PhysicsTime() const {return m_physics_time;}

//  static const Vector3& GetGravity() { return m_gravity; }
//  static void SetGravity(const Vector3& gravity) { m_gravity = gravity; }

  /// activate the object, and also pick up any collisions between it
  /// and the rest of the world.  Also activates any adjacent objects
  /// that would move if this object moved away Be careful about when
  /// you use this function - bear in mind that it adds elements to the
  /// internal list of collisions, which can relocate the collision
  /// list. I.e. be careful about calling it from within a traversal of
  /// the collision list.
  void ActivateObject(CJL_PhysicsActor *pActor);

  /// enable/disable object freezing. if freezing is disabled, all
  /// frozen object will be activated
  void EnableFreezing(bool freeze);

  /// indicates if freezing is currently allowed
  bool IsFreezingEnabled() const {return m_FreezingEnabled;}

  /// set surface material to a specified slot in array
  void SetMaterial( short sMaterialIndex, CJL_SurfaceMaterial& rMaterial );


  /// enable / disable collision detection between two collision groups
  void SetCollisionGroupState( int group0, int group1, bool bEnableCollision );

  /// enable / disable collision between a group and all the groups including itself
  void SetCollisionGroupState( int group, bool bEnableCollision );

  inline bool IsCollisionGroupEnabled( int iGroup0, int iGroup1 );


  inline TCPreAllocDynamicLinkList<CJL_PhysicsActor>& GetActorList() { return m_ActorList; }

  inline std::vector<CJL_CollisionInfo>& GetCollisionInfo() { return m_vecCollision; }
//  inline std::vector<CJL_ContactInfo>& GetCollisionInfo() { return m_vecCollision; }

  /// create static geometry from the vertices and indices.
  /// this is just a wrappter function.
  /// you can use this to create static geometry without filling out CJL_ActorDesc and CJL_ShapeDesc.
//  CJL_PhysicsActor *CreateStaticTriangleMesh( std::vector<Vector3>& rvecvVertex, std::vector<int>& rveciTriangleIndex );


  // allow actors direct access to lists of shapes
  friend class CJL_PhysicsActor;

	void ClipLineSegment( CJL_LineSegment& segment );


private:

  void DoTimestep(Scalar dt);

  // functions working on multiple bodies etc
//  void HandleAllCollisions(Scalar dt);

//  void HandleAllConstraints(Scalar dt);
  void HandleAllConstraints(Scalar dt, int num_coll_iterations, bool force_inelastic );

  void GetAllExternalForces(Scalar dt);

  void DoShockStep(Scalar dt);

  void UpdateAllVelocities(Scalar dt);

  void LimitAllVelocities();

  void UpdateAllPositions(Scalar dt);

  void DetectAllCollisions(Scalar dt);

  void ClearAllForces();

  void CopyAllCurrentStatesToOld();

  void TryToFreezeAllObjects(Scalar dt);

  void TryToActivateAllFrozenObjects();

  /// try to activate frozen objects that are affected by a touching
  /// active object moving away from them
  void ActivateAllFrozenObjectsLeftHanging();

  // ======== helpers for individual cases =========

  /// Handle an individual collision by classifying it, calculating
  /// impulse, applying impulse and updating the velocities of the
  /// objects. Allows over-riding of the elasticity. Ret val indicates
  /// if an impulse was applied
  bool ProcessCollision( CJL_CollisionInfo& collision, Scalar dt );
//  bool ProcessCollision( CJL_ContactInfo& collision, Scalar dt );

  bool ProcessCollisionCombined( CJL_CollisionInfo& collision, Scalar dt );

  bool ProcessCollisionForShock( CJL_CollisionInfo& collision, Scalar dt );
//  bool ProcessCollisionForShock( CJL_ContactInfo& collision, Scalar dt );

  /// Sets up any parameters that will not change as the collision
  /// gets processed - e.g. the relative position, elasticity etc.
  void PreProcessCollision( CJL_CollisionInfo& collision, Scalar dt);
//  void PreProcessCollision( CJL_ContactInfo& collision, Scalar dt);

//  std::vector<Collision_info> m_collisions;
  std::vector<Constraint *> m_constraints;
//  std::vector<Physics_controller *> m_controllers;

  struct SStoredData
  {
    inline SStoredData(CJL_PhysicsActor& rActor);
    SStoredData() {}
    Vector3 vPosition;
    Matrix33 matOrientation;
    Vector3 vVelocity;
    Vector3 vAngularVelocity;
  };

  std::vector<SStoredData> m_aStoredData;

  // used to store collisions occured during one time step
//  std::vector<CJL_ContactInfo> m_vecCollision;
  std::vector<CJL_CollisionInfo> m_vecCollision;

  /// list of the collision bodies - keep as a member var so it
  /// doesn't keep getting resized
//  std::vector<Collision_body *> m_collision_bodies;

  /// time left over from the last step
//  Scalar m_overlap_time;

  /// do a fixed timestep
//  Scalar m_timestep;

  /// allow the timestep to vary (so we always do at least one update per frame)
//  bool m_allow_smaller_timesteps;

  // timestep is not managed by CJL_PhysicsManager.
  // the user is responsible for calling Integrate( dt )
  // with certain timestep and synchronize the physics
  // with other systems

  /// Our idea of time
  Scalar m_physics_time;

  /// number of collision iterations
  int m_NumCollisionIterations;

  /// number of contact iteratrions
  int m_NumContactIterations;

  /// number of penetration-resolution iterations
//  int m_num_penetration_iterations;

  /// reuse the collision list from the first (collision) step when
  /// handling contacts
//  bool m_reuse_collisions;

  /// amount to resolve by each penetration resolution iteration
//  Scalar m_penetration_resolve_fraction;

  Scalar m_CollTolerance;

  bool m_DoingIntegration;

  /// traverse the contact list forward or backward (to reduce bias)
  enum Traversal_dir {TRAVERSE_FORWARD, TRAVERSE_BACKWARD} m_traverse_dir;

  /// allow objects to freeze
  bool m_FreezingEnabled;

  /// allow physics time to run faster/slower
  Scalar m_time_scale;

  bool m_DoShockStep;

  /// uniform gravity acceleration applied to all rigid bodies exept for kinematic actors
  Vector3 m_vGravity;

  /// enable/disable collisions between collision groups
  /// - each actor belongs to a collision group
  /// - by default, collision detection is applied between all the groups
  int m_CollisionGroupTable[NUM_MAX_COLLISION_GROUPS][NUM_MAX_COLLISION_GROUPS];

  /////====================== physics entities ======================/////

  /// store actors
  TCPreAllocDynamicLinkList<CJL_PhysicsActor> m_ActorList;

  /// store objects for each shapes
  TCPreAllocDynamicLinkList<CJL_Shape_Box> m_BoxShape;
  TCPreAllocDynamicLinkList<CJL_Shape_Capsule> m_CapsuleShape;
  TCPreAllocDynamicLinkList<CTriangleMesh> m_TriangleMesh;
//  TCPreAllocDynamicLinkList<CJL_Shape_Ball> m_BallShape;

  /////==============================================================/////

  std::vector<CJL_SurfaceMaterial> m_vecMaterial;

  typedef bool (CJL_PhysicsManager::*ProcessCollisionFn)( CJL_CollisionInfo& collision,
	                                                      Scalar dt );

  ProcessCollisionFn m_ProcessCollisionFn;


};


//===================================================================================
// inline implementations
//===================================================================================

inline CJL_PhysicsManager::SStoredData::SStoredData(CJL_PhysicsActor& rActor)
:
vPosition( rActor.GetPosition() ),
matOrientation( rActor.GetOrientation() ),
vVelocity( rActor.GetVelocity() ),
vAngularVelocity( rActor.GetAngularVelocity() )
{}


// returns pointer to the created actor
inline CJL_PhysicsActor *CJL_PhysicsManager::CreateActor( CJL_PhysicsActorDesc &rActorDesc )
{
	CJL_PhysicsActor *pActor = m_ActorList.GetNewObject();

	/// initialize actor
	pActor->SetPhysicsManager( this );
	pActor->Init( rActorDesc );

	return pActor;
}


inline void CJL_PhysicsManager::ReleaseActor( CJL_PhysicsActor*& pPhysicsActor )
{
	pPhysicsActor->ReleaseShapes();

	m_ActorList.Release( pPhysicsActor );
}

inline bool CJL_PhysicsManager::IsCollisionGroupEnabled( int iGroup0, int iGroup1 )
{
	if( m_CollisionGroupTable[iGroup0][iGroup1] == 1 )
		return true;
	else
		return false;
}



#endif  /*  __JL_PHYSICSMANAGER_H__  */
