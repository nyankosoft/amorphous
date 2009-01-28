#ifndef  __PhysBodyDesc_H__
#define  __PhysBodyDesc_H__


#include <float.h>

#include "3DMath/Matrix34.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "fwd.h"


namespace physics
{


class PhysBodyFlag
{
public:
	enum Flags
	{
		Static         = ( 1 << 0 ),
		Kinematic      = ( 1 << 1 ),
		DisableGravity = ( 1 << 2 ),
//		Static = ( 1 << 3 ),
//		Static = ( 1 << 4 ),
//		Static = ( 1 << 5 ),
	};
};


class CBodyDesc : public IArchiveObjectBase
{
	/// pointer to the owner - set by the program during the actor initialization
	/// the user does not have to care about this
//	PhysicsActor *m_pPhysicsActor;

public:

	/// position and orientation of the center of mass 
	Matrix34  MassLocalPose;

	/// Diagonal mass space inertia tensor in bodies mass frame. 
	Vector3  MassSpaceInertia;

	/// Mass of body. 
	Scalar  fMass;

	/// Linear Velocity of the body. 
	Vector3  LinearVelocity;

	/// Angular velocity of the body. 
	Vector3  AngularVelocity;

	/// The body's initial wake up counter. 
//	Scalar  WakeUpCounter;

	/// Linear damping applied to the body. 
//	Scalar  LinearDamping;

	/// Angular damping applied to the body. 
//	Scalar  AngularDamping;

	/// Maximum allowed angular velocity. 
	Scalar  MaxAngularVelocity;

	/// When CCD is globally enabled, it is still not performed if the motion distance of all points on the body is below this threshold. 
//	Scalar  CCDMotionThreshold;

	/// Combination of NxBodyFlag flags. 
	U32  Flags;

	/// Maximum linear velocity at which body can go to sleep. 
//	Scalar  sleepLinearVelocity;

	/// Maximum angular velocity at which body can go to sleep. 
//	Scalar  sleepAngularVelocity;

	/// Number of solver iterations performed when processing joint/contacts connected to this body. 
//	U32  solverIterationCount;

	/// Threshold for the energy-based sleeping algorithm. Only used when the NX_BF_ENERGY_SLEEP_TEST flag is set. 
//	Scalar  sleepEnergyThreshold;

	/// Damping factor for bodies that are about to sleep. 
//	Scalar  sleepDamping;

public:

	CBodyDesc() { SetDefault(); }

	virtual ~CBodyDesc() {}

	inline void SetDefault();

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


//=============================== inline implementations ===============================

inline void CBodyDesc::SetDefault()
{
	MassLocalPose.Identity();
	MassSpaceInertia   = Vector3(0,0,0);
	fMass = 1.0f;
	LinearVelocity     = Vector3(0,0,0);
	AngularVelocity    = Vector3(0,0,0);
//	WakeUpCounter;
//	LinearDamping;
//	AngularDamping;
	MaxAngularVelocity = FLT_MAX;//Vector3(FLT_MAX,FLT_MAX,FLT_MAX);
//	CCDMotionThreshold;
	Flags = 0;
//	sleepLinearVelocity;
//	sleepAngularVelocity;
//	solverIterationCount;
//	sleepEnergyThreshold;
//	sleepDamping;
}


inline void CBodyDesc::Serialize( IArchive& ar, const unsigned int version )
{
	ar & MassLocalPose;

	ar & MassSpaceInertia;

	ar & fMass;

	ar & LinearVelocity;

	ar & AngularVelocity;

//	ar & WakeUpCounter;

//	ar & LinearDamping;

//	ar & AngularDamping;

	ar & MaxAngularVelocity;

//	ar & CCDMotionThreshold;

	ar & Flags;

//	ar & sleepLinearVelocity;

//	ar & sleepAngularVelocity;

//	ar & solverIterationCount;

//	ar & sleepEnergyThreshold;

//	ar & sleepDamping;

}


} // namespace physics


#endif		/*  __PhysBodyDesc_H__  */
