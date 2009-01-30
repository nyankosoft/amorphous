#ifndef  __MotionBlender_H__
#define  __MotionBlender_H__


#include <vector>
#include <string>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/foreach.hpp>

#include "3DMath/Matrix34.hpp"
#include "3DMath/MathMisc.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

#include "MotionPrimitive.hpp"


namespace msynth
{


class CMotionBlender
{
public:

	enum BlenderType
	{
		STEERING = 0,
		IK,
		SCALING,          ///< should be done in pre-processing or not
		NUM_BLEND_TYPES
	};

public:

	CMotionBlender();
	virtual ~CMotionBlender();

	virtual void Update( float dt ) {}

	virtual void CalculateKeyframe( CKeyframe& dest_keyframe ) = 0;
//	virtual void Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt ) = 0;
};


/**
 NOTES:
 - 2 types of blend weights
   - weight that is applied to the entire keyframe
   - weight set to each node/joint


*/
/*
class CSourceMotionBlender
{
	enum Params
	{
		NUM_MAX_SIMULTANEOUS_MOTIONS_TO_BLEND = 8;
	};

	/// holds borrowed references to motion primitives
	boost::array<CMotionPrimitive *, NUM_MAX_SIMULTANEOUS_MOTIONS_TO_BLEND> m_vecpCurrentMotionPrimitive;

public:

	/// create a source keyframe ?
	virtual void Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt );
};


void CSourceMotionBlender::Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt )
{
	num_motions_to_blend
	if( num_motions_to_blend == 0 )
	{
		return;
	}
	else if( num_motions_to_blend == 1 )
	{
		// calc keyframe
		current_keyframe = motion.GetKeyFrame( current_time );
	}
	else
	{
		// blend multiple keyframes
		for_each()
		{
			current_keyframe += m_vecpCurrentMotionPrimitive[i]->GetKeyFrame( current_time ) * blend_weight[0];
		}
	}
}
*/

class CSteeringMotionBlender : public CMotionBlender
{
	/// TODO: what about steering of non-horizontal motions?
	Vector3 m_vDestDir;

	/// rad per sec
	float m_fSteeringSpeed;

	boost::shared_ptr<CMotionPrimitiveBlender> m_pMotionPrimitiveBlender;

public:

	inline Vector3 GetRootPoseHorizontalDirection( const CKeyframe& keyframe );

public:

//	CSteeringMotionBlender() : m_fSteeringSpeed(0.1f), m_vDestDir(Vector3(0,0,1)) {}
	CSteeringMotionBlender( boost::shared_ptr<CMotionPrimitiveBlender> pMotionPrimitiveBlender )
		:
	m_pMotionPrimitiveBlender(pMotionPrimitiveBlender),
	m_fSteeringSpeed(0.0f),
	m_vDestDir(Vector3(0,0,0))
	{}
	virtual ~CSteeringMotionBlender() {}

	float GetSteeringSpeed() const { return m_fSteeringSpeed; }

	/// \param turn speed in degrees per second
	void SetSteeringSpeed( float speed ) { m_fSteeringSpeed = deg_to_rad(speed); }

	/// set the destination direction
	/// set Vector3(0,0,0) to specify no target direction
	void SetDestDirection( Vector3 vDestDir );

	virtual void Update( float dt );

	virtual void CalculateKeyframe( CKeyframe& dest_keyframe );
//	virtual void Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt );
};


inline Vector3 CSteeringMotionBlender::GetRootPoseHorizontalDirection( const CKeyframe& keyframe )
{
	const Vector3 vDir = keyframe.GetRootPose().vPosition;
//	const Vector3 vDir = keyframe.GetMotionDirection().vPosition;

	Vector3 vHDir = vDir;
	vHDir.y = 0;
	Vec3Normalize( vHDir, vHDir );

	return vHDir;
}


class CIKMotionBlender : public CMotionBlender
{
	// target joint

	// target pose

public:
};


/**
 Does the scale adjustment during runtime
 - pros.
   - save memory by avoid duplications of similar motion primitives ?

 - cons.
   - additional computation during runtime

*/
/*
class CScalingMotionBlender : public CMotionBlender
{
	float m_fFactor;

public:

	virtual void Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt );
};


void CScalingMotionBlender::Update( const CMotionPrimitive& motion, CKeyframe& current_keyframe, float current_time, float dt )
{
	current_keyframe.GetRootPose().vPosition * m_fFactor;
}
*/


} // namespace msynth


#endif		/*  __MotionBlender_H__  */
