#ifndef  __MotionPrimitiveBlender_H__
#define  __MotionPrimitiveBlender_H__

//#include <queue>
#include <list>
#include "3DMath/Matrix34.hpp"
#include "MotionPrimitive.hpp"
#include "MotionBlender.hpp"
#include "fwd.hpp"


namespace msynth
{


#define VCM_PLAY_IMMEDIATE	1
#define VCM_ADD_TO_QUEUE	2
#define VCM_PLAY_NEXT		3


class CMotionPrimitivePlayCallback
{
public:

	/// Called when a motion primitive is finished playing and a new primitive is being started playing
	virtual void OnMotionPrimitiveChanged( boost::shared_ptr<CMotionPrimitive>& pPrev, boost::shared_ptr<CMotionPrimitive>& pNew ) {}

	/// Called when a motion primitive is finished playing and queue does not have any more primitive to play
	virtual void OnMotionPrimitiveFinished( boost::shared_ptr<CMotionPrimitive>& pPrev );
};


class CMotionPrimitiveBlender : public CMotionBlender
{
//	CVirtualCharacter *m_pCharacter;

	enum Params
	{
		NUM_MAX_INTERPOLATION_MOTION_PRIMITIVES = 4
	};

	float m_fFloorHeight;

	/// stores pointers to motion primitives to play
	std::list< boost::shared_ptr<CMotionPrimitive> > m_MotionPrimitiveQueue;

	/// root pose at the beginning of the current motion primitive
	Matrix34 m_RootWorldPoseOffset;

	/// time in current motion primitive
	float m_fCurrentTime;

	float m_fPrevTime;

	Matrix34 m_CurrentRootPose;

	/// horizontal
	Matrix34 m_HorizontalCurrentRootPose;

	float m_fInterpolationTime;

	/// root pose of the last keyframe
	Matrix34 m_LastOriginalRootPose;

	CKeyframe m_InterpolatedKeyframe[2];

	unsigned int m_FlipVal;
	
	/// hold temporary motion primitive created for interpolation
//	boost::shared_ptr<CMotionPrimitive> m_pInterpolationMotion;
	std::vector< boost::shared_ptr<CMotionPrimitive> > m_vecpInterpolationMotion;

	/// used by GetCurrentMotionPrimitive() to return an empty motion primitive when
	/// no motion in the queue
	boost::shared_ptr<CMotionPrimitive> m_pNullMotionPrimitive;

//	boost::shared_ptr<CMotionPrimitive> m_pCurrentMotion;

	boost::shared_ptr<CMotionPrimitivePlayCallback> m_pCallback;

private:

	/// calculate the pose of the character from the time updated by 'dt'
	/// set the resulting pose to the character
	void PlayMotionPrimitive( float dt );

	/// calculate interpolation motion between two motion primitives
	void PlayInterpolaitonMotion( float dt );

	void SetInterpolationMotion( boost::shared_ptr<CMotionPrimitive> pCurrentMotion,
								 boost::shared_ptr<CMotionPrimitive> pNextMotion );

public:

	CMotionPrimitiveBlender();

	~CMotionPrimitiveBlender();

	void Init();
	void Release();

	/// called by the client to update the pose of the character
	virtual void Update( float dt );

	virtual void CalculateKeyframe( CKeyframe& dest_keyframe );

	void CalculateKeyframe();

	/// play a new motion primitive
	///  - if a motion primitive is currently being played, it will be overridden
//	void StartMotion( boost::shared_ptr<CMotionPrimitive> ptr, float delay );

	void AddMotionPrimitive( boost::shared_ptr<CMotionPrimitive> pMotionPrimitive, int iFlag );

	void StartNewMotionPrimitive( float interpolation_motion_length,
		                          boost::shared_ptr<CMotionPrimitive> pNewMotion );

	void StartNewMotionPrimitive( boost::shared_ptr<CMotionPrimitive> pNewMotion );

	/// returns the motion at the front of the queue
	/// - Does not remove it from queue. Only returns a reference
	inline boost::shared_ptr<CMotionPrimitive> GetCurrentMotionPrimitive();

	const std::list< boost::shared_ptr<CMotionPrimitive> >& GetMotionPrimitiveList() const { return m_MotionPrimitiveQueue; }

	void SetDefaultKeyframe( const CKeyframe& keyframe );

	void SetRootPose( const Matrix34& pose );

	const Matrix34& GetCurrentRootPose() const { return m_CurrentRootPose; }

	const Matrix34& GetHorizontalCurrentRootPose() const { return m_HorizontalCurrentRootPose; }

	inline void RotateCurrentPoseHorizontally( Scalar angle_in_radian );

	void SetCallback( boost::shared_ptr<CMotionPrimitivePlayCallback>& pCallback ) { m_pCallback = pCallback; }

	friend CMotionPrimitiveBlenderStatistics;

	/*
	enum eMotionPrimitivePlayMode
	{
		MPPM_NO_MOTION = 0,
		MPPM_MOTIONPRIMITIVE,
		MPPM_INTERPOLATION
	};*/
};


inline void CMotionPrimitiveBlender::RotateCurrentPoseHorizontally( Scalar angle_in_radian )
{
	Matrix33 matRotationY = Matrix33RotationY( angle_in_radian );

	m_CurrentRootPose.matOrient           = m_CurrentRootPose.matOrient           * matRotationY;
	m_HorizontalCurrentRootPose.matOrient = m_HorizontalCurrentRootPose.matOrient * matRotationY;

	m_CurrentRootPose.matOrient.Orthonormalize();
	m_HorizontalCurrentRootPose.matOrient.Orthonormalize();
}


inline boost::shared_ptr<CMotionPrimitive> CMotionPrimitiveBlender::GetCurrentMotionPrimitive()
{
	if( 0 < m_MotionPrimitiveQueue.size() )
	{
		return m_MotionPrimitiveQueue.front();
	}

	// no motion is currently in the queue
	// - return an empty motion primitive
	return m_pNullMotionPrimitive;
}



class CMotionPrimitiveBlenderStatistics
{
	CMotionPrimitiveBlender *m_pTarget;

	std::vector<std::string> m_Buffer;

public:

	CMotionPrimitiveBlenderStatistics( CMotionPrimitiveBlender *pTarget ) : m_pTarget(pTarget) {}

	const std::vector<std::string>& GetStatistics() const { return m_Buffer; }

	void Update();
};


/*
class CMotionSynthesizer
{
	float m_fCurrentTime;

	float m_fPrevTime;

	Matrix34 m_CurrentRootPose;

public:

//	virtual void Update( float dt );

	virtual void CalculateKeyframe( float dt, CKeyframe& dest_keyframe );
};


void CMotionSynthesizer::CalculateKeyframe( float dt, CKeyframe& dest_keyframe )
{
	motion = current_motion_primitive;

	float play_speed_factor = 1.0f;

	current_time += dt * play_speed_factor;

	if( motion.time < m_fCurrentTime )
	{
		if( motion.IsLoopingMotion() )
		{
		}
		else
		{
		}
	}
	else
	{
		// play motion primitive
	}
}
*/


/**

- [in] frametime
- [out] keyframe



*/
/*
class CInterpolationMotionSynthesizer
{
public:

	float m_fInterpolationTime;

	float m_fTime_Out;
	float m_fTime_In;

	/// cache calculated keyframes
	CKeyframe m_KeyFrame_Out;
	CKeyframe m_KeyFrame_In;

	/// moition primitives to interpolate
	CMotionPrimitive *m_apInterpolatePrimitive[2];

public:

	CInterpolationMotionSynthesizer() {}
	virtual ~CInterpolationMotionSynthesizer() {}
};
*/


} // namespace msynth


#endif		/*  __MotionPrimitiveBlender_H__  */
