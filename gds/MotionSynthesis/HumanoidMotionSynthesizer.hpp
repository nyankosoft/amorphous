#ifndef  __HumanoidMotionSynthesizer_H__
#define  __HumanoidMotionSynthesizer_H__


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


class HumanoidMotion
{
public:

	enum Type
	{
		Walk,
		Run,
		Turn,
		Stop,
		Jump,
		Crawl,
		Crouch,
		TurnRight,
		TurnLeft,
		NumActions
	};
};


class CHumanoidMotionHolder
{
public:
	
	std::vector<boost::shared_ptr<CMotionPrimitive>> m_vecpMotion;
};


class CHumanoidMotionSynthesizer
{
	vector<boost::shared_ptr<CMotionBlender>> m_vecpMotionBlender;

	boost::shared_ptr<CSteeringMotionBlender> m_pSteeringMotionBlender;

	boost::shared_ptr<CMotionPrimitiveBlender> m_pMotionPrimitiveBlender;

	CHumanoidMotionHolder m_aMotion[HumanoidMotion::NumActions];

	CAnnotation m_aAnnotation[HumanoidMotion::NumActions];

	CKeyframe m_CurrentKeyframe;


	boost::shared_ptr<CMotionPrimitiveBlenderStatistics> m_pMotionPrimitiveBlenderStatics;


private:

	const CAnnotation& GetAnnotation( HumanoidMotion::Type motion_type ) const { return m_aAnnotation[motion_type]; }

	void ProcessMotionTransitionRequest( HumanoidMotion::Type motion_type );

public:

	CHumanoidMotionSynthesizer();

	~CHumanoidMotionSynthesizer();

	void Forward();
	void Walk();
	void TurnTo( const Vector3 vDir );
	void Idle();
	void Stop();
	void TurnRight();
	void TurnLeft();

	/// \param turn speed in degrees per second
	/// set positive(negative) value to turn right(left)
	void Turn( Scalar turn_speed );

//	void LoadMotions( CBinaryDatabase<std::string>& db, const std::string& motion_table_key_name );
	void LoadMotions( CMotionDatabase& mdb, const std::string& motion_table_key_name );

//	void CalculateNodePoses();

	/// \param dest_keyframe [out] hierarchical transformations
//	void CalculateKeyframe( CKeyframe& dest_keyframe );
	void UpdateKeyframe();

	const CKeyframe& GetCurrentKeyframe() const { return m_CurrentKeyframe; }

	void Update( float dt );

//	const CSkeleton& GetSkeleton();
	const boost::shared_ptr<CSkeleton> GetSkeleton();

	const boost::shared_ptr<CMotionPrimitiveBlender> GetMotionPrimitiveBlender() const { return m_pMotionPrimitiveBlender; }


	const boost::shared_ptr<CMotionPrimitiveBlenderStatistics>
		GetStatistics() const { return m_pMotionPrimitiveBlenderStatics; }

	/// set the pose of the root node of the character
	/// - pose == position & orientation
	///   - Don't get confused with keyframe or any physical pose the character takes
	void SetRootPose( const Matrix34& pose );
};


/*



class MotionState
{
};


class MS_Running
{
};

class MS_Walking
{
};

class MS_Standing
{
};

class MS_Crouching
{
};

class CHumanoidCharacterMotionController
{
	enum Action
	{
		WALK,
		RUN,
		TURN,
		STOP,
		JUMP
		CRAWL,
		CROUCH,
		TURN_RIGHT,
		TURN_LEFT,
		NUM_ACTIONS
	};

	Vector3 m_vVelocity;

public:

	void Update( float dt );

	void UpdateInput( input );

	void Walk( Vector3 vVel );
	void Run( Vector3 vVel );
	
	void Crouch();
	void Prone();

	void WalkTo( Vector3 vDestPos );
	void RunTo( Vector3 vDestPos );
};


void CHumanoidCharacterMotionController::Update( float dt )
{
	float spd = GetSpeed();
	if( 0 < spd )
	{
		// walk / run
	}
	else
	{
		// stopped
	}
}


*/


} // namespace msynth


#endif		/*  __HumanoidMotionSynthesizer_H__  */
