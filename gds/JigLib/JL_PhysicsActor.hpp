
#ifndef __JL_PHYSICSACTOR_H__
#define __JL_PHYSICSACTOR_H__


#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix33.hpp"
#include "3DMath/Matrix34.hpp"
#include "3DMath/Quaternion.hpp"

#include "3DMath/AABB3.hpp"

#include "JL_PhysicsActorDesc.hpp"
#include "JL_ShapeBase.hpp"

#include <vector>
using namespace std;

class CJL_ShapeBase;
class CJL_PhysicsManager;


enum eJL_ActorTypeFlag
{
	JL_ACTOR_STATIC						= 1,
	JL_ACTOR_TAKE_NO_GRAVITY 			= 2,
	JL_ACTOR_APPLY_NO_IMPULSE	 		= 4,
	JL_ACTOR_APPLY_NO_ANGULARIMPULSE 	= 8,
	JL_ACTOR_KINEMATIC	 				= 16,
};


//#define JL_ACTOR_NUM_MAX_SHAPES		16

class CJL_PhysicsManager;

class CJL_PhysicsActor
{
public:
	CJL_PhysicsActor() {}
//  CJL_PhysicsActor(CJL_PhysicsManager* pPhysicsManager);
  virtual ~CJL_PhysicsActor() {}

  inline void SetDefault();

  inline void SetPhysicsManager(CJL_PhysicsManager* pPhysicsManager) { m_pPhysicsManager = pPhysicsManager; }

  void Init( CJL_PhysicsActorDesc &rActorDesc );

  /// This Sets the position, but it also tries to make sure that any
  /// frozen bodies resting against this one Get activated if
  /// necessary.  Not very efficient. Be a little careful about when
  /// you call it - it will mess with the physics collision list.
  /// Also, after this call the object will be active.
  void MoveTo(const Vector3 & pos);

  // see comment in source file about why not inline...
  void SetPosition(const Vector3 & pos);
  const Vector3 & GetPosition() const { return m_vPosition; }

  inline void SetOrientation(const Matrix33 & orient);
  const Matrix33 & GetOrientation() const { return m_matOrientation; }

  inline void SetWorldPose( const Matrix34& world_pose )
  { m_vPosition = world_pose.vPosition; m_matOrientation = world_pose.matOrient; }

  inline void GetWorldPose( Matrix34& world_pose ) const
  { world_pose.vPosition = m_vPosition; world_pose.matOrient = m_matOrientation; }

//  inline const Matrix34& GetWorldPose() const { return m_WorldPose; }

  void SetVelocity(const Vector3 & vel) { m_vVelocity = vel; }
  const Vector3 & GetVelocity() const { return m_vVelocity; }

  void SetAngularVelocity(const Vector3 & rot) { m_vAngularVelocity = rot; }
  const Vector3 & GetAngularVelocity() const { return m_vAngularVelocity; }

  // returns the velocity at a point given in world coordinates
  inline Vector3 GetPointVelocity(const Vector3 & vWorldPoint);
  inline void GetPointVelocity( Vector3 & vPointVelocity, const Vector3 & vWorldPoint );

  // returns the velocity at a point given in local coordinates
  inline Vector3 GetLocalPointVelocity(const Vector3 & vLocalPoint);
  inline void GetLocalPointVelocity( Vector3 & vPointVelocity, const Vector3 & vLocalPoint );

 void SetForce(const Vector3 & f) { m_vForce = f; }
  const Vector3 & GetForce() const { return m_vForce; }

  void SetTorque(const Vector3 & t) { m_vTorque = t; }
  const Vector3 & GetTorque() const { return m_vTorque; }

  // Adds the global gravitational force to this object
  void AddGravity();

  // if mass = 0, then inv_mass will be very very large, but not infinite.
  // similarly if inv_mass = 0
  void SetMass(Scalar mass);
  void SetInvMass(Scalar inv_mass);
  Scalar GetMass() const {return m_fMass; }
  Scalar GetInvMass() const {return m_fInvMass; }

  void SetLocalInertia(Scalar Ixx, Scalar Iyy, Scalar Izz);
  void SetLocalInvInertia(Scalar inv_Ixx, Scalar inv_Iyy, Scalar inv_Izz);
  const Matrix33 & GetLocalInertia() const {return m_matLocalInertia;}
  const Matrix33 & GetLocalInvInertia() const {return m_matLocalInvInertia;}
  const Matrix33 & GetWorldInvInertia() const { return m_matWorldInvInertia; }
  const Matrix33 & GetWorldInertia() const { return m_matWorldInertia; }

  inline Vector3 GetOldPosition() const { return m_OldWorldPose.vPosition; }
//  inline const Vector3& GetOldPosition() { return m_OldWorldPose.vPosition; }

  // functions to Add forces etc in the world coordinate frame
  void AddWorldForce(const Vector3 & force) {m_vForce += force;}
  void AddWorldForce(const Vector3 & force, const Vector3 & pos);
  void AddWorldTorque(const Vector3 & torque) {m_vTorque += torque;}
  void AddWorldTorque(const Vector3 & torque, const Vector3 & pos);

  // functions to add impulses at a position offset in world space
  inline void ApplyBodyWorldImpulse(const Vector3& impulse, const Vector3& delta );

  inline void ApplyWorldImpulse(const Vector3 & impulse);
  inline void ApplyWorldImpulse(const Vector3 & impulse, const Vector3 & pos);
  inline void ApplyWorldAngularImpulse(const Vector3 & angImpulse);
  void ApplyWorldAngularImpulse(const Vector3 & angImpulse, const Vector3 & pos);

  // functions to Add forces etc in the body coordinate frame
  void AddLocalForce(const Vector3 & force);
  void AddLocalForce(const Vector3 & force, const Vector3 & pos);
  void AddLocalTorque(const Vector3 & torque);
  void AddLocalTorque(const Vector3 & torque, const Vector3 & pos);

  void ApplyLocalImpulse(const Vector3 & impulse);
  void ApplyLocalImpulse(const Vector3 & impulse, const Vector3 & pos);
  void ApplyLocalAngularImpulse(const Vector3 & angImpulse);
  void ApplyLocalAngularImpulse(const Vector3 & angImpulse, const Vector3 & pos);

  inline void CopyCurrentStateToOld();

  /// bounding volume
  inline AABB3 GetWorldAABB() const { return m_WorldAABB; }

  // calculate world positions, aabbs, etc.
  // must be called every frame
  inline void UpdateWorldProperties();

  /// may return 0 if this body doen't engage in collisions
//  virtual CollisionLocal * collisionLocal() { return 0; }

  /// suggest that the derived class implements this
  virtual void enable_collisions(bool enable) {}

  /// allow the body to Add on any Additional forces (including
  /// gravity)/impulses etc
//  void AddExternalForces(Scalar dt) {}

  /// This just Sets all forces/impulses etc to zero. over-ride if you
  /// want to do more.
  void ClearForces();

  /// implementation updates the velocity/angular rotation with the
  /// force/torque/impulses.
  void UpdateVelocities(Scalar dt);

  /// implementation updates the position/orientation with the current
  /// velocties.
  void UpdatePositions(Scalar dt);

  /// ensures that this object never moves, and reduces collision checking
  void SetStatic()
  {
    m_ActorFlag |= JL_ACTOR_STATIC; 
    SetInvMass(0.0f); 
    SetLocalInvInertia(0.0f, 0.0f, 0.0f);
  }

  /// ensures that this object is not influenced by other objects and
  /// is directly controlled by the user
  void SetKinematic()
  {
	m_ActorFlag |= JL_ACTOR_KINEMATIC;
    SetInvMass(0.0f); 
    SetLocalInvInertia(0.0f, 0.0f, 0.0f);
  }

  /// indicates if we ever move (change our position - may still have
  /// a non-zero velocity!)

  inline unsigned int GetActorFlag() const { return m_ActorFlag; }
  inline void SetActorFlag( unsigned int flag ) { m_ActorFlag = flag; }
  inline void AddActorFlag( unsigned int flag ) { m_ActorFlag |= flag; }
  inline void ClearActorFlag( unsigned int flag ) { m_ActorFlag |= (!flag); }

  inline int GetCollisionGroup() { return m_iCollisionGroup; }

  enum Activity {ACTIVE, FROZEN};

///  inline Activity GetActivityState() const {return m_ActivityState;}
  inline Activity GetActivityState() const
  {
	  if(this == NULL) {assert(0 && "invalid actor");return ACTIVE;}
	  else return m_ActivityState;
  }

  /// allow the activity to be explicitly Set - be careful about
  /// explicitly freezing an object (may become unfrozen anyway).
  /// If Setting to ACTIVE, then the activity factor indicates how
  /// active the object is considered to be - a value of 1 means
  /// completely active - a value nearer 0 means that freezing
  /// will happen much sooner (assuming no further movement).
  void SetActivityState(Activity state, Scalar activity_factor = 1.0f);

  /// Set how long it takes to deactivate
  void SetDeactivationTime(Scalar seconds);

  /// Set what the velocity threshold is for deactivation
  /// rot is in deg per second.
  void SetActivityThreshold(Scalar vel, Scalar rot);

  /// set the thresholds for deactivation
  void SetDeactivationThreshold( Scalar fPosThreshold, Scalar fOrientTheshold );

  /// values > 1 make the body less likely to wake up following an impulse.
  void SetActivationFactor(Scalar factor);

  /// allows Setting of whether this body ever freezes
  void SetAllowFreezing(bool allow);

  /// Bodies that use a lot of constraints don't behave very well during the
  /// physics shock step, so they can bypass it
  void SetDoShockProcessing(bool doit) { m_bDoShockProcessing = doit; }
  bool GetDoShockProcessing() const { return m_bDoShockProcessing; }

  /// function provided for the use of Physics
  inline void TryToFreeze(Scalar dt);

  // function provided for use of physics - indicates if the velocity
  // is above the threshold for freezing
  inline bool ShouldBeActive()
  {
    return ( (Vec3LengthSq(m_vVelocity) > m_sqrActivationFactor * m_sqrVelocityActivityThreshold) ||
             (Vec3LengthSq(m_vAngularVelocity) > m_sqrActivationFactor * m_sqrAngularVelocityActivityThreshold) );
  }

  /// function provided for use of physics. Activates any body in its
  /// list if it's moved more than a certain distance, in which case
  /// it also clears its list.
  void DoMovementActivations();

  /// Adds the other body to the list of bodies to be activated if
  /// this body moves more than a certain distance from either a
  /// previously stored position, or the position passed in.
  inline void AddMovementActivation(const Vector3 & pos, 
                                      CJL_PhysicsActor * pOther);

  inline bool VelocityChanged() const { return m_bVelocityChanged; }
  inline void ClearVelocityChanged() { m_bVelocityChanged = false; }

  void LimitVel();
  void LimitAngVel();

  // TODO: make this inline
  void SetConstraintsAndCollisionsUnsatisfied();

  inline void StoreState();

  inline void RestoreState();

  /// Used by physics to temporarily make an object immovable -
  /// needs to restore afterwars!
  inline void InternalSetImmovable()
  {
	  m_bOrigStatic = (GetActorFlag() & JL_ACTOR_STATIC) != 0;
	  m_ActorFlag |= JL_ACTOR_STATIC;
  }

  inline void InternalRestoreImmovable()
  {
	  if(m_bOrigStatic)
		  m_ActorFlag |= JL_ACTOR_STATIC;
	  else
		  m_ActorFlag &= (~JL_ACTOR_STATIC);
  }

  /// collision index
  inline vector<int>& GetCollisionIndexBuffer() { return m_veciCollisionIndex; }


  /// function for shapes
  inline int GetNumShapes() { return (int)m_vecpShape.size(); }
  inline CJL_ShapeBase *GetShape(int i) { return m_vecpShape[i]; }

  /// Set/read this variable as a hint for whether objects should
  /// render in a way that makes it obvious they're frozen. Not
  /// actually used by CJL_PhysicsActor.
  static bool m_indicate_frozen_objects;

  /// release shapes attached to this actor
  void ReleaseShapes();

  /// check whether a line segment intersects with the actor
  /// NOT IMPLEMENTED
//  bool ClipTrace( const Vector3& vStartPos, const Vector3& vGoalPos, Vector3& vEndPos, float& fFraction );


private:
  CJL_PhysicsManager * m_pPhysicsManager;

  Vector3 m_vPosition;
  Matrix33 m_matOrientation;
  Matrix33 m_matInvOrientation;
  Vector3 m_vVelocity;
  Vector3 m_vAngularVelocity;

  Scalar m_fMass;
  Scalar m_fInvMass;

  unsigned int m_ActorFlag;

  /// inertia is expected to be diagonal
  Matrix33 m_matLocalInertia;

  /// inertia tensor in world space - not diagonal!
  Matrix33 m_matWorldInertia;

  /// inverse inertia in body frame
  Matrix33 m_matLocalInvInertia;

  /// inverse inertia in world frame
  Matrix33 m_matWorldInvInertia;

  /// forces etc in world frame. torque is about com.
  Vector3 m_vForce;
  Vector3 m_vTorque;

  /// axis-aligned box in world coordinates. must be updated at every frame
  AABB3 m_WorldAABB;

  /// the previous state - copied explicitly using
  /// CopyCurrentStateToOld.
  Matrix34 m_OldWorldPose;
  Vector3 m_vOldVelocity;
  Vector3 m_vOldAngularVelocity;

  /// stored state - used internally by physics during the updates
  Matrix34 m_StoredWorldPose;
  Vector3 m_vStoredVelocity;
  Vector3 m_vStoredAngularVelocity;

  /// for deactivation
  Activity m_ActivityState;

  /// How long we've been still
  Scalar m_inactive_time;

  /// how long it takes to go from active to frozen when stationary
  Scalar m_deactivation_time;

  /// last position for when trying to deactivate
  Vector3 m_vLastPositionForDeactivation;

  /// last orientation for when trying to deactivate
  Quaternion m_qLastOrientForDeactivation;

  /// change for detecting position changes during deactivation
  Scalar m_fSqDeltaPosThreshold;

  /// change (todo - hacky) for detecting orientation changes during
  /// deactivation.
  Scalar m_fSqDeltaQuatOrientThreshold;

  /// factor applied during ShouldBeActive(). Value greater than one decrease
  /// the tendency to wake up from little impulses.
  Scalar m_sqrActivationFactor;

  /// velocity below which we're considered still
  Scalar m_sqrVelocityActivityThreshold;
  /// velocity below which we're considered still - in (radians per
  /// sec)^2
  Scalar m_sqrAngularVelocityActivityThreshold;

  /// The position stored when we need to notify other bodies
  Vector3 m_vStoredPositionForActivation;
  /// The list of bodies that need to be activated when we move away
  /// from our stored position
  std::vector<CJL_PhysicsActor *> m_vecpActorsToBeActivatedOnMovement;

  // indices to collisions on this actor
  // set and cleared every frame
  std::vector<int> m_veciCollisionIndex;
  
  /// whether this body can freeze (assuming Physics freezing is
  /// enabled)
  bool m_allow_freezing;

  bool m_bDoShockProcessing;

  bool m_bOrigStatic;

  bool m_bVelocityChanged;

  // index to the collision group to which this actor belongs
  int m_iCollisionGroup;

  // shapes
//  TCFixedVector<CJL_ShapeBase *, JL_ACTOR_NUM_MAX_SHAPES> m_vecpShape;
  vector<CJL_ShapeBase *> m_vecpShape;
};


//================== inline implementations =======================
#include "JL_PhysicsActor.inl"


#endif  /*  __JL_PHYSICSACTOR_H__  */
