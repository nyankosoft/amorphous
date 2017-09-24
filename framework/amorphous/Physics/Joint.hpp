#ifndef __PhysJoint_H__
#define __PhysJoint_H__


#include "fwd.hpp"
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Support/SafeDelete.hpp"
#include "JointDesc.hpp"


namespace amorphous
{


namespace physics
{


class JointState
{
public:
	enum Name
	{
		UNKNOWN,
		NUM_JOINT_STATES
	};
};


class CJointImpl
{
	CActor *m_apActor[2];

public:
	CJointImpl() { m_apActor[0] = m_apActor[1] = NULL; }
	~CJointImpl() {}

	/// Retrieve the type of this joint. 
	virtual JointType::Name GetType() const = 0;

	virtual CActor *GetActor( int index ) { return m_apActor[index]; }

	/*
	virtual void  getActors (NxActor **actor1, NxActor **actor2) = 0;
	/// Retrieves the Actors involved. 

	*/
	/// Sets the point where the two actors are attached, specified in global coordinates. 
	virtual void SetGlobalAnchor(const Vector3 &vec) {}

	/// Sets the direction of the joint's primary axis, specified in global coordinates. 
	virtual void SetGlobalAxis(const Vector3 &vec) {}

	/// Retrieves the joint anchor. 
	virtual Vector3 GetGlobalAnchor() const { return Vector3(0,0,0); }

	/// Retrieves the joint axis. 
	virtual Vector3 GetGlobalAxis() const { return Vector3(0,1,0); }

	/// Returns the state of the joint. 
	virtual JointState::Name GetState() = 0;

	/// Sets the maximum force magnitude that the joint is able to withstand without breaking. 
//	virtual void SetBreakable (Scalar maxForce, Scalar maxTorque) = 0;

	/// Retrieves the max forces of a breakable joint. See setBreakable(). 
//	virtual void  getBreakable (Scalar &maxForce, Scalar &maxTorque) = 0;

	/// Sets a name string for the object that can be retrieved with getName(). 
//	virtual void SetName (const char *name) = 0;

	/// Retrieves the name string set with setName(). 
//	virtual const char *  getName () const =0 

	/// Retrieves owner scene. 
//	virtual CScene &GetScene () const { return *m_pScene; }

	/// Sets the limit point. 
	virtual void SetLimitPoint(const Vector3 &point, bool pointIsOnActor2 = true) {}

	/// Retrieves the global space limit point. 
	virtual bool GetLimitPoint(Vector3 &worldLimitPoint) { return false; }

	/// Adds a limit plane. 
	virtual bool AddLimitPlane(const Vector3 &normal, const Vector3 &pointInPlane, Scalar restitution=0.0f) { return false; }

	/// deletes all limit planes added to the joint. 
	virtual void  PurgeLimitPlanes() {}

	/// Restarts the limit plane iteration. 
	virtual void  ResetLimitPlaneIterator() {}

	/// Returns true until the iterator reaches the end of the set of limit planes. 
	virtual bool  HasMoreLimitPlanes() { return false; }

	/// Returns the next element pointed to by the limit plane iterator, and increments the iterator. 
	virtual bool GetNextLimitPlane( Vector3 &planeNormal, Scalar &planeD, Scalar *restitution=NULL ) { return false; }

	/// Sets the flags to enable/disable the spring/motor/limit. 
	virtual void SetFlags (U32 flags) {}

	/// Returns the current flag settings. 
	virtual U32 GetFlags () { return 0; }

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



class CJoint
{
	CScene *m_pScene;

protected:

	CJointImpl *m_pImpl;

public:

	CJoint() : m_pScene(NULL) {}

	CJoint( CScene *pScene ) : m_pScene(pScene) { /*m_apActor[0] = m_apActor[1] = NULL;*/ }

	virtual ~CJoint() { SafeDelete( m_pImpl ); }

	/// \param index 0 or 1

	CActor *GetActor( int index ) { return m_pImpl->GetActor( index ); }

	/*
	virtual void  getActors (NxActor **actor1, NxActor **actor2) { return m_pImpl->(); }
	/// Retrieves the Actors involved. 

	*/
	/// Sets the point where the two actors are attached, specified in global coordinates. 
	void SetGlobalAnchor(const Vector3 &vec) { m_pImpl->SetGlobalAnchor( vec ); }

	/// Sets the direction of the joint's primary axis, specified in global coordinates. 
	void SetGlobalAxis(const Vector3 &vec) { m_pImpl->SetGlobalAxis( vec ); }

	/// Retrieves the joint anchor. 
	Vector3 GetGlobalAnchor() const { return m_pImpl->GetGlobalAnchor(); }

	/// Retrieves the joint axis. 
	Vector3 GetGlobalAxis() const { return m_pImpl->GetGlobalAxis(); }

	/// Returns the state of the joint. 
	JointState::Name GetState() { return m_pImpl->GetState(); }

	/// Sets the maximum force magnitude that the joint is able to withstand without breaking. 
//	virtual void SetBreakable (Scalar maxForce, Scalar maxTorque) { return m_pImpl->(); }

	/// Retrieves the max forces of a breakable joint. See setBreakable(). 
//	virtual void  getBreakable (Scalar &maxForce, Scalar &maxTorque) { return m_pImpl->(); }

	/// Retrieve the type of this joint. 
	JointType::Name GetType() const { return m_pImpl->GetType(); }

	/// Sets a name string for the object that can be retrieved with getName(). 
//	virtual void SetName (const char *name) { return m_pImpl->(); }

	/// Retrieves the name string set with setName(). 
//	virtual const char *  getName () const =0 

	/// Retrieves owner scene. 
	CScene &GetScene() const { return *m_pScene; }

	/// Sets the limit point. 
	void SetLimitPoint(const Vector3 &point, bool pointIsOnActor2=true) { return m_pImpl->SetLimitPoint( point, pointIsOnActor2 ); }

	/// Retrieves the global space limit point. 
	bool GetLimitPoint(Vector3 &worldLimitPoint) { return m_pImpl->GetLimitPoint( worldLimitPoint ); }

	/// Adds a limit plane. 
	bool  AddLimitPlane(const Vector3 &normal, const Vector3 &pointInPlane, Scalar restitution=0.0f) { return m_pImpl->AddLimitPlane( normal, pointInPlane, restitution ); }

	/// deletes all limit planes added to the joint. 
	void  PurgeLimitPlanes() { m_pImpl->PurgeLimitPlanes(); }

	/// Restarts the limit plane iteration. 
	void  ResetLimitPlaneIterator() { m_pImpl->ResetLimitPlaneIterator(); }

	/// Returns true until the iterator reaches the end of the set of limit planes. 
	bool  HasMoreLimitPlanes() { return m_pImpl->HasMoreLimitPlanes(); }

	/// Returns the next element pointed to by the limit plane iterator, and increments the iterator. 
	bool GetNextLimitPlane( Vector3 &planeNormal, Scalar &planeD, Scalar *restitution=NULL ) { return m_pImpl->GetNextLimitPlane( planeNormal, planeD, restitution ); }

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

	friend class CScene;
};


} // namespace physics


//================== inline implementations =======================
//#include "Joint.inl"

} // namespace amorphous



#endif  /*  __PhysJoint_H__  */
