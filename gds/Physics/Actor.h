#ifndef __PhysActor_H__
#define __PhysActor_H__


#include <vector>
#include "3DMath.h"
#include "Support/SafeDeleteVector.h"

#include "fwd.h"
#include "ActorDesc.h"
#include "Shape.h"


namespace physics
{


class ForceMode
{
public:
	enum Mode
	{
		Force,
		Impulse,
		VelocityChange,
		SmoothImpulse,
		SmoothVelocityChange,
		Acceleration,
		NumModes
	};
};


class CActor
{
	/// derived actor classes must set shape pointers to this array
	/// - owned reference
	std::vector<CShape *> m_vecpShape;

	unsigned int m_ActorFlags;

protected:

public:

	/// \param pPhysShape owned reference
	void AddPhysShape( CShape *pPhysShape )
	{
		m_vecpShape.push_back( pPhysShape );
		m_vecpShape.back()->m_pActor = this;
	}

	enum Flag /// actor type flag
	{
//		TF_STATIC                  = ( 1 << 0 ), // moved to body desc
//		TF_TAKE_NO_GRAVITY         = ( 1 << 1 ), // moved to body desc
//		TF_KINEMATIC               = ( 1 << 4 ), // moved to body desc
		DisableResponse            = ( 1 << 0 ),
		DisableCollision           = ( 1 << 1 ),
		FluidDisableCollision      = ( 1 << 2 ), ///< Enable/disable collision with fluid
		ApplyNoImpulse             = ( 1 << 3 ),
		ApplyNoAnglarImpulse       = ( 1 << 4 ),
	};

public:

	CActor() {}

	virtual ~CActor() { SafeDeleteVector( m_vecpShape ); }

	// see comment in source file about why not virtual...
//	void SetPosition(const Vector3 & pos) = 0;

	virtual void SetWorldPosition(const Vector3 & pos) = 0;
	virtual const Vector3 GetWorldPosition() const = 0;

	virtual void SetWorldOrientation(const Matrix33 & orient) = 0;
	virtual const Matrix33 GetWorldOrientation() const = 0;

	virtual void SetWorldPose( const Matrix34& world_pose ) = 0;
	virtual void GetWorldPose( Matrix34& world_pose ) const = 0;

	inline Matrix34 GetWorldPose() const { Matrix34 pose; GetWorldPose( pose ); return pose; }

	//  virtual const Matrix34& GetWorldPose() const { return m_WorldPose; }

	virtual void SetLinearVelocity(const Vector3 & vel) = 0;
	virtual Vector3 GetLinearVelocity() const = 0;

	virtual void SetAngularVelocity(const Vector3 & rot) = 0;
	virtual Vector3 GetAngularVelocity() const = 0;

	/// returns the velocity at a point given in world coordinates
	virtual Vector3 GetPointVelocity(const Vector3 & vWorldPoint) = 0;
//	virtual void GetPointVelocity( Vector3 & vPointVelocity, const Vector3 & vWorldPoint ) = 0;

	/// returns the velocity at a point given in local coordinates
	virtual Vector3 GetLocalPointVelocity(const Vector3 & vLocalPoint) = 0;
//	virtual void GetLocalPointVelocity( Vector3 & vPointVelocity, const Vector3 & vLocalPoint ) = 0;

	// if mass = 0, then inv_mass will be very very large, but not infinite.
	// similarly if inv_mass = 0
	virtual void SetMass(Scalar mass) = 0;
//	virtual void SetInvMass(Scalar inv_mass) = 0;
	virtual Scalar GetMass() const = 0;
	virtual Scalar GetInvMass() const = 0;


/*
	void SetLocalInertia(Scalar Ixx, Scalar Iyy, Scalar Izz);
	void SetLocalInvInertia(Scalar inv_Ixx, Scalar inv_Iyy, Scalar inv_Izz);
	const Matrix33 & GetLocalInertia() const {return m_matLocalInertia;}
	const Matrix33 & GetLocalInvInertia() const {return m_matLocalInvInertia;}
	const Matrix33 & GetWorldInvInertia() const { return m_matWorldInvInertia; }
	const Matrix33 & GetWorldInertia() const { return m_matWorldInertia; }
*/

	//
	// functions to Add forces etc
	//
	virtual void AddWorldForceAtWorldPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode = ForceMode::Force ) = 0;
	virtual void AddWorldForceAtLocalPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode = ForceMode::Force ) = 0;
	virtual void AddLocalForceAtWorldPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode = ForceMode::Force ) = 0;
	virtual void AddLocalForceAtLocalPos( const Vector3 & force, const Vector3 & pos, ForceMode::Mode mode = ForceMode::Force ) = 0;

	virtual void AddWorldForce( const Vector3 & force,  ForceMode::Mode mode = ForceMode::Force ) = 0;
	virtual void AddLocalForce( const Vector3 & force,  ForceMode::Mode mode = ForceMode::Force ) = 0;

	virtual void AddWorldTorque(const Vector3 & torque, ForceMode::Mode mode = ForceMode::Force ) = 0;
	virtual void AddLocalTorque(const Vector3 & torque, ForceMode::Mode mode = ForceMode::Force ) = 0;

	/// bounding volume
//	inline AABB3 GetWorldAABB() const { return m_WorldAABB; }

	// calculate world positions, aabbs, etc.
	// must be called every frame
//	inline void UpdateWorldProperties();

	/// suggest that the derived class implements this
//	virtual void enable_collisions(bool enable) {}

	/// ensures that this object never moves, and reduces collision checking
//	virtual void SetStatic() = 0;
/*	{
		m_ActorFlags |= TF_STATIC; 
		SetInvMass(0.0f); 
//		SetLocalInvInertia(0.0f, 0.0f, 0.0f);
	}*/

	/// ensures that this object is not influenced by other objects and
	/// is directly controlled by the user
//	virtual void SetKinematic() = 0;
/*	{
		m_ActorFlags |= TF_KINEMATIC;
		SetInvMass(0.0f); 
//		SetLocalInvInertia(0.0f, 0.0f, 0.0f);
	}*/

	virtual int GetActorGroup() = 0;

	virtual void SetActorGroup( int actor_group ) = 0;

	/// indicates if we ever move (change our position - may still have
	/// a non-zero velocity!)

	inline unsigned int GetActorFlag() const { return m_ActorFlags; }
	inline void SetActorFlag( unsigned int flag ) { m_ActorFlags = flag; }
	inline void AddActorFlag( unsigned int flag ) { m_ActorFlags |= flag; }
	inline void ClearActorFlag( unsigned int flag ) { m_ActorFlags |= (!flag); }

//	enum Activity {ACTIVE, FROZEN};

	///  inline Activity GetActivityState() const {return m_ActivityState;}
//	inline Activity GetActivityState() const
//	{
//		if(this == NULL) {assert(0 && "invalid actor");return ACTIVE;}
//		else return m_ActivityState;
//	}
/*
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

	virtual void SetMaxAngularVelocity( Scalar max_ang_vel ) = 0;
	virtual Scalar GetMaxAngularVelocity() = 0;

	/// function for shapes
	inline int GetNumShapes() const { return (int)m_vecpShape.size(); }
	inline CShape *GetShape(int i) { return m_vecpShape[i]; }

	/// release a shape attached to this actor
//	virtual void ReleaseShape() {};
};


} // namespace physics


//================== inline implementations =======================

//#include "Actor.inl"


#endif  /*  __PhysActor_H__  */
