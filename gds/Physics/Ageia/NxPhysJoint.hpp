#ifndef __NxPhysJoint_H__
#define __NxPhysJoint_H__


#include "fwd.hpp"
#include "3DMath/all.hpp"

#include "NxPhysJointDesc.hpp"

#include <vector>
using namespace std;


namespace physics
{


class CNxPhysJoint
{
	NxJoint *m_pJoint;

public:

public:

	CNxPhysJoint() {}

	virtual ~CNxPhysJoint() {}

	/*
	virtual void  getActors (NxActor **actor1, NxActor **actor2)=0 
	/// Retrieves the Actors involved. 

	*/
	/// Sets the point where the two actors are attached, specified in global coordinates. 
	/// Sets the point where the two actors are attached, specified in global coordinates. 
	virtual void SetGlobalAnchor (const Vector3 &vec);

	/// Sets the direction of the joint's primary axis, specified in global coordinates. 
	virtual void SetGlobalAxis (const Vector3 &vec);

	/// Retrieves the joint anchor. 
	virtual Vector3 GetGlobalAnchor () const;

	/// Retrieves the joint axis. 
	virtual Vector3 GetGlobalAxis () const;

	/// Returns the state of the joint. 
	virtual JointState GetState ();

	/// Sets the maximum force magnitude that the joint is able to withstand without breaking. 
//	virtual void SetBreakable (Scalar maxForce, Scalar maxTorque);

	/// Retrieves the max forces of a breakable joint. See setBreakable(). 
//	virtual void  getBreakable (Scalar &maxForce, Scalar &maxTorque);

	/// Retrieve the type of this joint. 
//	virtual NxJointType GetType () const =0 

	/// Sets a name string for the object that can be retrieved with getName(). 
//	virtual void SetName (const char *name);

	/// Retrieves the name string set with setName(). 
//	virtual const char *  getName () const =0 

	/// Sets the limit point. 
	virtual void SetLimitPoint (const Vector3 &point, bool pointIsOnActor2);

	/// Retrieves the global space limit point. 
	virtual bool GetLimitPoint (Vector3 &worldLimitPoint);

	/// Adds a limit plane. 
	virtual bool  AddLimitPlane (const Vector3 &normal, const Vector3 &pointInPlane, Scalar restitution);

	/// deletes all limit planes added to the joint. 
	virtual void  PurgeLimitPlanes ();

	/// Restarts the limit plane iteration. 
	virtual void  ResetLimitPlaneIterator ();

	/// Returns true until the iterator reaches the end of the set of limit planes. 
	virtual bool  HasMoreLimitPlanes ();

	/// Returns the next element pointed to by the limit plane iterator, and increments the iterator. 
	virtual bool GetNextLimitPlane ( Vector3 &planeNormal, Scalar &planeD, Scalar *restitution );
};


} // namespace physics


//================== inline implementations =======================
//#include "NxPhysJoint.inl"


#endif  /*  __NxPhysJoint_H__  */
