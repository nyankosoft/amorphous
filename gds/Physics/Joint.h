#ifndef __PhysJoint_H__
#define __PhysJoint_H__


#include "fwd.h"
#include "3DMath/all.h"

#include "JointDesc.h"

#include <vector>


namespace physics
{


class JointState
{
public:
	enum State
	{
	};
};


class CJoint
{
	CActor *m_apActor[2];

	Scene *m_pScene;

public:

public:

	CJoint( Scene *pScene ) : m_pScene(pScene) { m_apActor[0] = m_apActor[1] = NULL; }

	virtual ~CJoint() {}

	/// \param index 0 or 1

	virtual CActor *GetActor( int index ) { return m_apActor[index]; }

	/*
	virtual void  getActors (NxActor **actor1, NxActor **actor2) = 0;
	/// Retrieves the Actors involved. 

	*/
	/// Sets the point where the two actors are attached, specified in global coordinates. 
	virtual void SetGlobalAnchor (const Vector3 &vec) = 0;

	/// Sets the direction of the joint's primary axis, specified in global coordinates. 
	virtual void SetGlobalAxis (const Vector3 &vec) = 0;

	/// Retrieves the joint anchor. 
	virtual Vector3 GetGlobalAnchor () const = 0;

	/// Retrieves the joint axis. 
	virtual Vector3 GetGlobalAxis () const = 0;

	/// Returns the state of the joint. 
	virtual JointState GetState () = 0;

	/// Sets the maximum force magnitude that the joint is able to withstand without breaking. 
//	virtual void SetBreakable (Scalar maxForce, Scalar maxTorque) = 0;

	/// Retrieves the max forces of a breakable joint. See setBreakable(). 
//	virtual void  getBreakable (Scalar &maxForce, Scalar &maxTorque) = 0;

	/// Retrieve the type of this joint. 
//	virtual NxJointType GetType () const =0 

	/// Sets a name string for the object that can be retrieved with getName(). 
//	virtual void SetName (const char *name) = 0;

	/// Retrieves the name string set with setName(). 
//	virtual const char *  getName () const =0 

	/// Retrieves owner scene. 
	virtual CScene &GetScene () const { return *m_pScene; }

	/// Sets the limit point. 
	virtual void SetLimitPoint (const Vector3 &point, bool pointIsOnActor2=true) = 0;

	/// Retrieves the global space limit point. 
	virtual bool GetLimitPoint (Vector3 &worldLimitPoint) = 0;

	/// Adds a limit plane. 
	virtual bool  AddLimitPlane (const Vector3 &normal, const Vector3 &pointInPlane, Scalar restitution=0.0f) = 0;

	/// deletes all limit planes added to the joint. 
	virtual void  PurgeLimitPlanes () = 0;

	/// Restarts the limit plane iteration. 
	virtual void  ResetLimitPlaneIterator () = 0;

	/// Returns true until the iterator reaches the end of the set of limit planes. 
	virtual bool  HasMoreLimitPlanes () = 0;

	/// Returns the next element pointed to by the limit plane iterator, and increments the iterator. 
	virtual bool GetNextLimitPlane ( Vector3 &planeNormal, Scalar &planeD, Scalar *restitution=NULL ) = 0;

/*
Is... Joint Type 
virtual void *  is (NxJointType type) = 0;
  Type casting operator. The result may be cast to the desired subclass type. 
*/
/*
	/// Attempts to perform a cast to a NxRevoluteJoint. 
	inline NxRevoluteJoint *  isRevoluteJoint();

	/// Attempts to perform a cast to a NxPointInPlaneJoint. 
	inline NxPointInPlaneJoint *  isPointInPlaneJoint();

	/// Attempts to perform a cast to a NxPointOnLineJoint. 
	inline NxPointOnLineJoint *  isPointOnLineJoint();

	/// Attempts to perform a cast to a NxD6Joint. 
	inline NxD6Joint *  isD6Joint();

	/// Attempts to perform a cast to a NxPrismaticJoint. 
	inline NxPrismaticJoint *  isPrismaticJoint();

	/// Attempts to perform a cast to a NxCylindricalJoint. 
	inline NxCylindricalJoint *  isCylindricalJoint();

	/// Attempts to perform a cast to a NxSphericalJoint. 
	inline NxSphericalJoint *  isSphericalJoint();

	/// Attempts to perform a cast to a NxFixedJoint. 
	inline NxFixedJoint *  isFixedJoint();

	/// Attempts to perform a cast to a NxDistanceJoint. 
	inline NxDistanceJoint *  isDistanceJoint();

	/// Attempts to perform a cast to a NxPulleyJoint 
	inline NxPulleyJoint *  isPulleyJoint();*/
};


} // namespace physics


//================== inline implementations =======================
//#include "Joint.inl"


#endif  /*  __PhysJoint_H__  */
