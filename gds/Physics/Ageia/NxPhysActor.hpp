#ifndef  __NxPhysActor_H__
#define  __NxPhysActor_H__

#include <math.h>

#include "../Actor.h"
#include "NxMathConv.h"
#include "NxPhysConv.h"


#include "NxPhysics.h"


namespace physics
{


class CNxPhysActor : public CActor
{
	NxActor *m_pActor;

	unsigned int m_NxActorFlags;

public:

	CNxPhysActor( NxActor *pActor ) : m_pActor(pActor), m_NxActorFlags(0) {}

	virtual ~CNxPhysActor() {}

	// see comment in source file about why not inline...
	void SetWorldPosition(const Vector3 & pos) { m_pActor->setGlobalPosition( ToNxVec3(pos) ); }
	const Vector3 GetWorldPosition() const { return ToVector3( m_pActor->getGlobalPosition() ); }

	inline void SetWorldOrientation(const Matrix33 & orient) { m_pActor->setGlobalOrientation( ToNxMat33(orient) ); }
	const Matrix33 GetWorldOrientation() const { return ToMatrix33( m_pActor->getGlobalOrientation() ); }

	inline virtual void SetWorldPose( const Matrix34& world_pose ) { m_pActor->setGlobalPose( ToNxMat34(world_pose) ); }
	inline virtual void GetWorldPose( Matrix34& world_pose ) const { NxMat34 src_pose = m_pActor->getGlobalPose(); world_pose = ToMatrix34(src_pose); }
///	inline const Matrix34& GetWorldPose( Matrix34& world_pose ) const { return ToMatrix34( m_pActor->getGlobalPose() ); }

	//  inline const Matrix34& GetWorldPose() const { return m_WorldPose; }

	virtual void SetLinearVelocity(const Vector3 & vel) { m_pActor->setLinearVelocity( ToNxVec3(vel) ); }
	virtual Vector3 GetLinearVelocity() const { return ToVector3( m_pActor->getLinearVelocity() ); }

	virtual void SetAngularVelocity(const Vector3 & rot) { m_pActor->setAngularVelocity( ToNxVec3(rot) ); }
	virtual Vector3 GetAngularVelocity() const { return ToVector3( m_pActor->getAngularVelocity() ); }

	// returns the velocity at a point given in world coordinates
	virtual Vector3 GetPointVelocity(const Vector3 & vWorldPoint) { return ToVector3( m_pActor->getPointVelocity( ToNxVec3(vWorldPoint) ) ); }
//	virtual void GetPointVelocity( Vector3 & vPointVelocity, const Vector3 & vWorldPoint );

	// returns the velocity at a point given in local coordinates
	virtual Vector3 GetLocalPointVelocity(const Vector3 & vLocalPoint) { return ToVector3( m_pActor->getLocalPointVelocity( ToNxVec3(vLocalPoint) ) ); }
//	virtual void GetLocalPointVelocity( Vector3 & vPointVelocity, const Vector3 & vLocalPoint );

	// if mass = 0, then inv_mass will be very very large, but not infinite.
	// similarly if inv_mass = 0
	void SetMass(Scalar mass) { m_pActor->setMass( (NxReal)mass ); }
//	void SetInvMass(Scalar inv_mass);
	Scalar GetMass() const { return (Scalar)m_pActor->getMass(); }
	Scalar GetInvMass() const { if( abs(m_pActor->getMass()) < 0.001f ) return 0; else return 1.0f / m_pActor->getMass(); }

//	void SetLocalInertia(Scalar Ixx, Scalar Iyy, Scalar Izz);
//	void SetLocalInvInertia(Scalar inv_Ixx, Scalar inv_Iyy, Scalar inv_Izz);
//	const Matrix33 & GetLocalInertia() const {return m_matLocalInertia;}
//	const Matrix33 & GetLocalInvInertia() const {return m_matLocalInvInertia;}
//	const Matrix33 & GetWorldInvInertia() const { return m_matWorldInvInertia; }
//	const Matrix33 & GetWorldInertia() const { return m_matWorldInertia; }

	//
	// functions to Add forces etc
	//
	inline virtual void AddWorldForceAtWorldPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode );
	inline virtual void AddWorldForceAtLocalPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode );
	inline virtual void AddLocalForceAtWorldPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode );
	inline virtual void AddLocalForceAtLocalPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode );

	inline virtual void AddWorldForce( const Vector3 & force,  ForceMode::Mode mode );
	inline virtual void AddLocalForce( const Vector3 & force,  ForceMode::Mode mode );

	inline virtual void AddWorldTorque(const Vector3 & torque, ForceMode::Mode mode );
	inline virtual void AddLocalTorque(const Vector3 & torque, ForceMode::Mode mode );

	/// bounding volume
///	virtual inline AABB3 GetWorldAABB() const { return m_WorldAABB; }

	/// allow the body to Add on any Additional forces (including
	/// gravity)/impulses etc
	//  void AddExternalForces(Scalar dt) {}

	/// This just Sets all forces/impulses etc to zero. over-ride if you
	/// want to do more.
//	void ClearForces();

	/// ensures that this object never moves, and reduces collision checking
/*	void SetStatic()
	{
		m_ActorFlag |= TF_STATIC; 
		SetInvMass(0.0f); 
		SetLocalInvInertia(0.0f, 0.0f, 0.0f);
	}

	/// ensures that this object is not influenced by other objects and
	/// is directly controlled by the user
	void SetKinematic()
	{
		m_ActorFlag |= TF_KINEMATIC;
		SetInvMass(0.0f); 
		SetLocalInvInertia(0.0f, 0.0f, 0.0f);
	}
*/
	/// indicates if we ever move (change our position - may still have
	/// a non-zero velocity!)

//	inline unsigned int GetActorFlag() const { return m_ActorFlag; }
//	inline void SetActorFlag( unsigned int flag ) { m_ActorFlag = flag; }
//	inline void AddActorFlag( unsigned int flag ) { m_ActorFlag |= flag; }
//	inline void ClearActorFlag( unsigned int flag ) { m_ActorFlag |= (!flag); }

	int GetActorGroup() { return (int)m_pActor->getGroup(); }

	void SetActorGroup( int actor_group ) { m_pActor->setGroup( (NxActorGroup)actor_group ); }
/*
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
*/
	/// allows Setting of whether this body ever freezes
//	void SetAllowFreezing(bool allow);

	virtual bool IsSleeping() { return m_pActor->isSleeping(); }
	virtual void WakeUp() { m_pActor->wakeUp(); }

	virtual void SetMaxAngularVelocity( Scalar max_ang_vel ) { m_pActor->setMaxAngularVelocity( max_ang_vel ); }
	virtual Scalar GetMaxAngularVelocity() { return m_pActor->getMaxAngularVelocity(); }

	/// release shapes attached to this actor
//	void ReleaseShapes();

	/// check whether a line segment intersects with the actor
	/// NOT IMPLEMENTED
	//  bool ClipTrace( const Vector3& vStartPos, const Vector3& vGoalPos, Vector3& vEndPos, float& fFraction );

	NxActor *GetNxActor() { return m_pActor; }
};


//================================= inline implementations =================================

inline void CNxPhysActor::AddWorldForceAtWorldPos( const Vector3 & force,
												  const Vector3 & pos,
												  ForceMode::Mode mode )
{
	m_pActor->addForceAtPos( ToNxVec3(force), ToNxVec3(pos), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddWorldForceAtLocalPos( const Vector3 & force,
												  const Vector3 & pos,
												  ForceMode::Mode mode )
{
	m_pActor->addForceAtLocalPos( ToNxVec3(force), ToNxVec3(pos), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddLocalForceAtWorldPos( const Vector3 & force,
												  const Vector3 & pos,
												  ForceMode::Mode mode )
{
	m_pActor->addLocalForceAtPos( ToNxVec3(force), ToNxVec3(pos), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddLocalForceAtLocalPos( const Vector3 & force,
												  const Vector3 & pos,
												  ForceMode::Mode mode )
{
	m_pActor->addLocalForceAtLocalPos( ToNxVec3(force), ToNxVec3(pos), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddWorldForce( const Vector3 & force,
										ForceMode::Mode mode )
{
	m_pActor->addForce( ToNxVec3(force), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddLocalForce( const Vector3 & force,
										ForceMode::Mode mode )
{
	m_pActor->addLocalForce( ToNxVec3(force), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddWorldTorque( const Vector3 & torque,
										  ForceMode::Mode mode )
{
	m_pActor->addTorque( ToNxVec3(torque), ToNxForceMode(mode) );
}

inline void CNxPhysActor::AddLocalTorque( const Vector3 & torque,
										  ForceMode::Mode mode )
{
	m_pActor->addLocalTorque( ToNxVec3(torque), ToNxForceMode(mode) );
}


} // namespace physics


#endif		/*  __NxPhysActor_H__  */
