#ifndef  __MotionPrimitiveBlender_H__
#define  __MotionPrimitiveBlender_H__

//#include <queue>
#include <list>
#include "MotionPrimitive.hpp"
#include "MotionBlender.hpp"
#include "fwd.hpp"


namespace amorphous
{


namespace msynth
{


#define VCM_PLAY_IMMEDIATE	1
#define VCM_ADD_TO_QUEUE	2
#define VCM_PLAY_NEXT		3


class MotionPrimitivePlayCallback
{
public:

	/// Called when a motion primitive is finished playing and a new primitive is being started playing
	virtual void OnMotionPrimitiveChanged( std::shared_ptr<MotionPrimitive>& pPrev, std::shared_ptr<MotionPrimitive>& pNew ) {}

	/// Called when a motion primitive is finished playing and queue does not have any more primitive to play
	virtual void OnMotionPrimitiveFinished( std::shared_ptr<MotionPrimitive>& pPrev ) {}
};


class MotionPrimitiveBlender : public MotionBlender
{
//	CVirtualCharacter *m_pCharacter;

	enum Params
	{
		NUM_MAX_INTERPOLATION_MOTION_PRIMITIVES = 4
	};

	float m_fFloorHeight;

	/// stores pointers to motion primitives to play
	std::list< std::shared_ptr<MotionPrimitive> > m_MotionPrimitiveQueue;

	/// root pose at the beginning of the current motion primitive
	Matrix34 m_RootWorldPoseOffset;

	/// time in current motion primitive
	float m_fCurrentTime;

	float m_fPrevTime;

	Matrix34 m_CurrentRootPose;

	/// horizontal
	Matrix34 m_HorizontalCurrentRootPose;

	Matrix34 m_CurrentHorizontalPose;

	float m_fInterpolationTime;

	/// root pose of the last keyframe
	Matrix34 m_LastOriginalRootPose;

	Keyframe m_InterpolatedKeyframe[2];

	unsigned int m_FlipVal;
	
	/// stock of temporary motion primitives created for interpolation
	std::vector< std::shared_ptr<MotionPrimitive> > m_vecpInterpolationMotion;
//	std::vector< std::shared_ptr<MotionPrimitive> > m_vecpInterpolationMotionInUse;

	/// used by GetCurrentMotionPrimitive() to return an empty motion primitive when
	/// no motion in the queue
	std::shared_ptr<MotionPrimitive> m_pNullMotionPrimitive;

//	std::shared_ptr<MotionPrimitive> m_pCurrentMotion;

	std::shared_ptr<MotionPrimitivePlayCallback> m_pCallback;

	static const char *ms_pInterpolationMotionName;

private:

	/// calculate the pose of the character from the time updated by 'dt'
	/// set the resulting pose to the character
	void PlayMotionPrimitive( float dt );

	/// calculate interpolation motion between two motion primitives
	void PlayInterpolaitonMotion( float dt );

	void SetInterpolationMotion( std::shared_ptr<MotionPrimitive> pCurrentMotion,
								 std::shared_ptr<MotionPrimitive> pNextMotion );

	/// Modifies m_CurrentRootPose
	void UpdatePoseAndRootNodePose( std::shared_ptr<MotionPrimitive>& pMotion, float time_0, float time_1 );

	void PushInterpolationMotionPrimitive( std::shared_ptr<MotionPrimitive> pCurrentMotion,
		                                   std::shared_ptr<MotionPrimitive> pNewMotion,
										   float interpolation_time );

	void ClearMotionPrimitiveQueue();

public:

	MotionPrimitiveBlender();

	~MotionPrimitiveBlender();

	void Init();
	void Release();

	/// called by the client to update the pose of root node of the character
	virtual void Update( float dt );

	virtual void CalculateKeyframe( Keyframe& dest_keyframe );

	void CalculateKeyframe();

	/// play a new motion primitive
	///  - if a motion primitive is currently being played, it will be overridden
//	void StartMotion( std::shared_ptr<MotionPrimitive> ptr, float delay );

	void AddMotionPrimitive( float interpolation_motion_length, std::shared_ptr<MotionPrimitive> pMotionPrimitive, int iFlag );

	void StartNewMotionPrimitive( float interpolation_motion_length,
		                          std::shared_ptr<MotionPrimitive> pNewMotion );

	void StartNewMotionPrimitive( std::shared_ptr<MotionPrimitive> pNewMotion );

	/// returns the motion at the front of the queue
	/// - Does not remove it from queue. Only returns a reference
	inline std::shared_ptr<MotionPrimitive> GetCurrentMotionPrimitive();

	const std::list< std::shared_ptr<MotionPrimitive> >& GetMotionPrimitiveList() const { return m_MotionPrimitiveQueue; }

	void SetDefaultKeyframe( const Keyframe& keyframe );

	void SetRootPose( const Matrix34& pose );

	const Matrix34& GetCurrentRootPose() const { return m_CurrentRootPose; }

	const Matrix34& GetHorizontalCurrentRootPose() const { return m_HorizontalCurrentRootPose; }

	inline void RotateCurrentPoseHorizontally( Scalar angle_in_radian );

	void SetCallback( std::shared_ptr<MotionPrimitivePlayCallback>& pCallback ) { m_pCallback = pCallback; }

	void SetCurrentHorizontalPose( const Matrix34& pose ) { m_CurrentHorizontalPose = pose; }

	const Matrix34& GetCurrentHorizontalPose() const { return m_CurrentHorizontalPose; }

	static bool IsInterpolationMotion( std::shared_ptr<MotionPrimitive> pMotion ) { return pMotion && (pMotion->GetName() == "__InterpolationMotion__"); }

	friend MotionPrimitiveBlenderStatistics;

	/*
	enum eMotionPrimitivePlayMode
	{
		MPPM_NO_MOTION = 0,
		MPPM_MOTIONPRIMITIVE,
		MPPM_INTERPOLATION
	};*/
};


inline void MotionPrimitiveBlender::RotateCurrentPoseHorizontally( Scalar angle_in_radian )
{
	Matrix33 matRotationY = Matrix33RotationY( angle_in_radian );

	m_CurrentRootPose.matOrient           = m_CurrentRootPose.matOrient           * matRotationY;
	m_HorizontalCurrentRootPose.matOrient = m_HorizontalCurrentRootPose.matOrient * matRotationY;

	m_CurrentRootPose.matOrient.Orthonormalize();
	m_HorizontalCurrentRootPose.matOrient.Orthonormalize();
}


inline std::shared_ptr<MotionPrimitive> MotionPrimitiveBlender::GetCurrentMotionPrimitive()
{
	if( 0 < m_MotionPrimitiveQueue.size() )
	{
		return m_MotionPrimitiveQueue.front();
	}

	// no motion is currently in the queue
	// - return an empty motion primitive
	return m_pNullMotionPrimitive;
}



class MotionPrimitiveBlenderStatistics
{
	MotionPrimitiveBlender *m_pTarget;

	std::vector<std::string> m_Buffer;

public:

	MotionPrimitiveBlenderStatistics( MotionPrimitiveBlender *pTarget ) : m_pTarget(pTarget) {}

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

	virtual void CalculateKeyframe( float dt, Keyframe& dest_keyframe );
};


void CMotionSynthesizer::CalculateKeyframe( float dt, Keyframe& dest_keyframe )
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
	Keyframe m_KeyFrame_Out;
	Keyframe m_KeyFrame_In;

	/// moition primitives to interpolate
	MotionPrimitive *m_apInterpolatePrimitive[2];

public:

	CInterpolationMotionSynthesizer() {}
	virtual ~CInterpolationMotionSynthesizer() {}
};
*/


} // namespace msynth

} // namespace amorphous



#endif		/*  __MotionPrimitiveBlender_H__  */
