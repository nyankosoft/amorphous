#include "MotionPrimitiveBlender.hpp"

#include "Support/Macro.h"
#include "Support/memory_helpers.hpp"

using std::vector;
using std::list;
using boost::shared_ptr;
using namespace msynth;


inline Vector3 horizontal( const Vector3& src )
{
	Vector3 dest = src;
	dest.y = 0;
	return dest;
}


inline Matrix33 horizontal( const Matrix33& src )
{
	Matrix33 dest = src;
	dest( 1, 1 ) = 1;
	dest( 0, 1 ) = 0;
	dest( 2, 1 ) = 0;
	dest( 1, 0 ) = 0;
	dest( 1, 2 ) = 0;
	dest.Orthonormalize();
	return dest;
}


inline Matrix34 horizontal( const Matrix34& src )
{
	Matrix34 dest;
	dest.vPosition = horizontal( src.vPosition );
	dest.matOrient = horizontal( src.matOrient );
	return dest;
}


/*
inline bool CMotionPrimitiveBlender::IsInterpolationMotion( shared_ptr<CMotionPrimitive> pMotion )
{
	return (pMotion->GetName() == "__InterpolationMotion__");
}
*/

const char *CMotionPrimitiveBlender::ms_pInterpolationMotionName = "__InterpolationMotion__";

CMotionPrimitiveBlender::CMotionPrimitiveBlender()
:
m_fFloorHeight(0),
m_RootWorldPoseOffset( Matrix34Identity() ),
m_fCurrentTime(0),
m_fPrevTime(0),
m_CurrentRootPose( Matrix34Identity() ),
m_HorizontalCurrentRootPose( Matrix34Identity() ),
m_LastOriginalRootPose( Matrix34Identity() ),
m_fInterpolationTime(0.2f),
m_FlipVal(0),
m_CurrentHorizontalPose( Matrix34Identity() )
{
	// create an empty motion primitive for interpolation
	// - interpolation motion is created during runtime

//	m_pInterpolationMotion = shared_ptr<CMotionPrimitive>( new CMotionPrimitive() );

//	vector<CKeyframe>& rvecDestKeyframe = m_pInterpolationMotion->GetKeyframeBuffer();
//	rvecDestKeyframe.resize( 2 );

	bool enable_interpolation = true;
	if( enable_interpolation )
	{
		m_vecpInterpolationMotion.resize( NUM_MAX_INTERPOLATION_MOTION_PRIMITIVES );
		for( size_t i=0; i<NUM_MAX_INTERPOLATION_MOTION_PRIMITIVES; i++ )
		{
			m_vecpInterpolationMotion[i] = shared_ptr<CMotionPrimitive>( new CMotionPrimitive() );
			m_vecpInterpolationMotion[i]->SetName( "__InterpolationMotion__" );
			vector<CKeyframe>& rvecDestKeyframe = m_vecpInterpolationMotion[i]->GetKeyframeBuffer();
			rvecDestKeyframe.resize( 2 );
		}
	}

	// empty motion primitive
	m_pNullMotionPrimitive = shared_ptr<CMotionPrimitive>( new CMotionPrimitive() );
}


CMotionPrimitiveBlender::~CMotionPrimitiveBlender()
{
	Release();
}


void CMotionPrimitiveBlender::Init()
{
}


void CMotionPrimitiveBlender::Release()
{
}


void CMotionPrimitiveBlender::ClearMotionPrimitiveQueue()
{
	// Return the interpolation primitives to the stock
	for( list< shared_ptr<CMotionPrimitive> >::iterator itr = m_MotionPrimitiveQueue.begin();
		 itr != m_MotionPrimitiveQueue.end();
		 itr++ )
	{
		if( (*itr)
		 && IsInterpolationMotion( *itr ) )
		{
			m_vecpInterpolationMotion.push_back( (*itr) );
		}
	}

	m_MotionPrimitiveQueue.clear();
}


void CMotionPrimitiveBlender::PushInterpolationMotionPrimitive( shared_ptr<CMotionPrimitive> pCurrentMotion,
															    shared_ptr<CMotionPrimitive> pNewMotion,
																float interpolation_time )
{
	if( m_vecpInterpolationMotion.empty() )
	{
		LOG_PRINT_WARNING( " Ran out of interpolation motion primitives." );
		return;
	}

	shared_ptr<CMotionPrimitive> pInterpolationMotion = m_vecpInterpolationMotion.back();
	m_vecpInterpolationMotion.pop_back();

	vector<CKeyframe>& rvecDestKeyframe = pInterpolationMotion->GetKeyframeBuffer();

	// Wait... there is only one interpolation motion instance.
	// -> An interpolation moiton must be created later when it is needed

	// create a motion primitive for interpolation
	// - start frame: last frame of the current motion
	pCurrentMotion->GetLastKeyframe( rvecDestKeyframe[0] );
	rvecDestKeyframe[0].SetTime( 0 );

	// - end frame: the first frame of the new motion
	pNewMotion->GetFirstKeyframe( rvecDestKeyframe[1] );
	rvecDestKeyframe[1].SetTime( interpolation_time );

	Matrix34 root_pose_0 = rvecDestKeyframe[0].GetRootPose();
	Matrix34 root_pose_1 = rvecDestKeyframe[1].GetRootPose();
	root_pose_1.vPosition.x = root_pose_0.vPosition.x;
	root_pose_1.vPosition.z = root_pose_0.vPosition.z;
	rvecDestKeyframe[1].SetRootPose( root_pose_1 );

	// copy the start blend node of the new motion to the interpolation motion
	pInterpolationMotion->SetStartBlendNode( pNewMotion->GetStartBlendNode() );

	if( !pNewMotion->GetStartBlendNode() )
	{
		LOG_PRINT_WARNING( "The new motion does not have start blend node. " );
	}

	LOG_PRINT( fmt_string(" addeding an interpolation motion (%f s)", interpolation_time) );

//	pInterpolationMotion->SetSkeleton( pNewMotion->GetSkeleton() );

	m_MotionPrimitiveQueue.push_back( pInterpolationMotion );
}


void CMotionPrimitiveBlender::AddMotionPrimitive( float interpolation_motion_length,
												  shared_ptr<CMotionPrimitive> pNewMotion,
												  int iFlag )
{
	if( !pNewMotion )
		return;

	LOG_PRINT( fmt_string( "interpolation time: %f / motion: %s", interpolation_motion_length, pNewMotion ? pNewMotion->GetName().c_str() : "-" ) );

	if( 0 < m_MotionPrimitiveQueue.size() )
	{
		if( 0 < m_vecpInterpolationMotion.size()
		 && 0.001f < interpolation_motion_length )
//		if( false )
		{
			// Push an interpolation motion before the new motion.
			shared_ptr<CMotionPrimitive> pInterpolationMotion
				= m_vecpInterpolationMotion.back();

			shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();
			PushInterpolationMotionPrimitive( pCurrentMotion, pNewMotion, interpolation_motion_length );
		}

		m_MotionPrimitiveQueue.push_back( pNewMotion );
	}
	else
	{
		m_MotionPrimitiveQueue.push_back( pNewMotion );
	}

	// if no motion is being played right now, set the root offset
//	if( m_MotionPrimitiveQueue.empty() )
//		m_pCharacter->GetRootWorldPose( m_RootWorldPoseOffset );

}


/// Cancel the current motion and start a new motion.
/// \param interpolation_motion_length length of interpolation motion [sec].
///        The interpolation motion is played before pNewMotion.
/// - Create an interpolation motion between the current motion and the new motion
/// - Start the interpolation motion
/// - When the interpolation motion is finished playing, a new motion, pNewMotion, will be played.
void CMotionPrimitiveBlender::StartNewMotionPrimitive( float interpolation_motion_length,
													   shared_ptr<CMotionPrimitive> pNewMotion )
{
	if( !pNewMotion )
	{
		LOG_PRINT( "pNewMotion == NULL" );
		return;
	}

	LOG_PRINT( fmt_string( "interpolation time: %f / motion: %s", interpolation_motion_length, pNewMotion ? pNewMotion->GetName().c_str() : "-" ) );

	if( 0 < m_MotionPrimitiveQueue.size() )
	{
		shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();

		// - clear all the previous motion in the queue
		ClearMotionPrimitiveQueue();
//		m_MotionPrimitiveQueue.clear();

		if( 0 < m_vecpInterpolationMotion.size() )
//		if( false )
		{
			// add an interpolation motion

			PushInterpolationMotionPrimitive( pCurrentMotion, pNewMotion, interpolation_motion_length );
/*
			shared_ptr<CMotionPrimitive> pInterpolationMotion = m_vecpInterpolationMotion.back();

			m_vecpInterpolationMotion.pop_back();

			vector<CKeyframe>& rvecDestKeyframe = pInterpolationMotion->GetKeyframeBuffer();

			// create a motion primitive for interpolation
			// - start frame: current frame of the current motion
			pCurrentMotion->GetInterpolatedKeyframe( rvecDestKeyframe[0], m_fCurrentTime, Interpolation::Linear );
			rvecDestKeyframe[0].SetTime( 0 );

			// - end frame: the first frame of the new motion
			pNewMotion->GetFirstKeyframe( rvecDestKeyframe[1] );
			rvecDestKeyframe[1].SetTime( interpolation_motion_length );

			// add the interpolation motion primitive to the queue
			m_MotionPrimitiveQueue.push_back( pInterpolationMotion );

//			m_LastOriginalRootPose = rvecDestKeyframe[0].GetRootPose();*/
		}
//		else
//			m_LastOriginalRootPose = pNewMotion->GetFirstKeyframe().GetRootPose();

		m_MotionPrimitiveQueue.push_back( pNewMotion );

		// reset the play time
		m_fCurrentTime = 0;
	}
	else
	{
		m_MotionPrimitiveQueue.push_back( pNewMotion );

		// reset the play time
		m_fCurrentTime = 0;
	}
}


void CMotionPrimitiveBlender::StartNewMotionPrimitive( shared_ptr<CMotionPrimitive> pNewMotion )
{
	StartNewMotionPrimitive( m_fInterpolationTime, pNewMotion );
}


void CMotionPrimitiveBlender::SetDefaultKeyframe( const CKeyframe& keyframe )
{
	for( int i=0; i<numof(m_InterpolatedKeyframe); i++ )
		m_InterpolatedKeyframe[i] = keyframe;

	for( size_t i=0; i<m_vecpInterpolationMotion.size(); i++ )
	{
		vector<CKeyframe>& rvecDestKeyframe = m_vecpInterpolationMotion[i]->GetKeyframeBuffer();

		for( size_t j=0; j<rvecDestKeyframe.size(); j++ )
			rvecDestKeyframe[j] = keyframe;
	}
}


void CMotionPrimitiveBlender::SetRootPose( const Matrix34& pose )
{
	m_CurrentRootPose = pose;
	m_HorizontalCurrentRootPose = horizontal( pose );
}


void CMotionPrimitiveBlender::SetInterpolationMotion( shared_ptr<CMotionPrimitive> pCurrentMotion,
														 shared_ptr<CMotionPrimitive> pNextMotion )
{
/*	vector<CKeyframe>& rvecDestKeyframe = m_pInterpolationMotion->GetKeyframeBuffer();

	// create a motion primitive for interpolation
	// - start frame: current frame of the current motion
	pCurrentMotion->GetLastKeyframe( rvecDestKeyframe[0] );
	rvecDestKeyframe[0].SetTime( 0 );

	// - end frame: the first frame of the new motion
	pNextMotion->GetFirstKeyframe( rvecDestKeyframe[1] );
	rvecDestKeyframe[1].SetTime( m_fInterpolationTime );*/
}


inline void fix_horizontal_pose( Matrix34& pose )
{
	Vector3 vHeadingAxis = pose.matOrient.GetColumn(1);
//	vHeadingAxis.y = 1;
//	pose.matOrient.SetColumn( 1, vHeadingAxis );
	pose.matOrient.SetColumn( 1, Vector3(0,1,0) );
	pose.matOrient.Orthonormalize();
}


void CMotionPrimitiveBlender::UpdatePoseAndRootNodePose( shared_ptr<CMotionPrimitive>& pMotion,
														float time_0,
														float time_1 )
{
	Matrix34& current_horizontal_pose = m_CurrentHorizontalPose;

	if( IsInterpolationMotion(pMotion) )
	{
		int break_here = 1;
	}

	CKeyframe k0, k1;
	pMotion->GetInterpolatedKeyframe( k0, time_0 );
	pMotion->GetInterpolatedKeyframe( k1, time_1 );
	Matrix34 root_pose_0 = k0.GetRootPose();
	Matrix34 root_pose_1 = k1.GetRootPose();
	Matrix34 root_pose_0_to_1 = root_pose_1 * root_pose_0.GetInverseROT();

	// update the horizontal orientation
/*
	current_horizontal_pose.matOrient
		= current_horizontal_pose.matOrient
		* pCurrentMotion->GetLastOrientation()	
		* Matrix33Transpose( pCurrentMotion->GetOrientation( prev_time ) );
*/
	// experiment with walk motion - always walk on the z-axis
//	root_pose_0_to_1.vPosition.x = 0;

	// translation of the pose
	current_horizontal_pose.vPosition += current_horizontal_pose.matOrient * root_pose_0_to_1.vPosition;
	current_horizontal_pose.vPosition.y = root_pose_1.vPosition.y;
/*
//	if( pMotion->AreFeetOnGround() )
	bool are_feet_on_ground = (pMotion->GetName() != "fall" );
	if( are_feet_on_ground )
	{
		// Y is fixed to the root pose y + floor height
		current_horizontal_pose.vPosition.y = root_pose_1.vPosition.y;// + m_fFloorHeight;
	}
//	else
//	{
//		current_horizontal_pose.vPosition.y += m_fFloorHeightAtMotionStart;
//	}
*/
	fix_horizontal_pose( current_horizontal_pose );

	// update the pose of the root node

	m_CurrentRootPose.matOrient
		= current_horizontal_pose.matOrient
		* root_pose_1.matOrient;

	// Use the position the pose for the root node
	m_CurrentRootPose.vPosition	= current_horizontal_pose.vPosition;

	m_CurrentRootPose.matOrient.Orthonormalize();
}


void CMotionPrimitiveBlender::Update( float dt )
{
	float prev_time = m_fCurrentTime;

	m_fCurrentTime += dt * 1.0f;//spd_factor;


	if( m_MotionPrimitiveQueue.size() == 0 )
		return;

	CKeyframe prev_keyframe;

	Matrix34& current_horizontal_pose = m_CurrentHorizontalPose;

	shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();
	float current_motion_total_time = pCurrentMotion->GetTotalTime();

	while( current_motion_total_time < m_fCurrentTime )
	{
		// dt is crossing the boundary of motion primitives

		float last_keyframe_time = pCurrentMotion->GetKeyframeBuffer().back().GetTime() - 0.001f;
		UpdatePoseAndRootNodePose( pCurrentMotion, prev_time, last_keyframe_time );

		// move on to the next motion primitive in the queue
		m_fCurrentTime -= current_motion_total_time;

		prev_time = 0;

		// If the current motion is a looping motion, push the same motion
		// to the queue to keep playing the same motion.
		if( pCurrentMotion->IsLoopedMotion() )
		{
			// assumes no other motion is currently in the queue
			AddMotionPrimitive( 0.0f, pCurrentMotion, 0 );
		}

		// Return the current motion to the stock of interpolation motions
		// if it is an interpolation motion.
		if( IsInterpolationMotion(pCurrentMotion) )
			m_vecpInterpolationMotion.push_back( pCurrentMotion );

		// Pop the current motion here because the motions in the queue may be cleared in the callback function calls below
//		m_MotionPrimitiveQueue.pop_front();

		if( m_pCallback )
		{
			if( 1 < m_MotionPrimitiveQueue.size() )
			{
				list< shared_ptr<CMotionPrimitive> >::iterator first = m_MotionPrimitiveQueue.begin();
				first++;
				shared_ptr<CMotionPrimitive> pNextMotion = *first;
				m_pCallback->OnMotionPrimitiveChanged( pCurrentMotion, pNextMotion ); // 2nd argument - hand the element 2nd from the front (= next motion primitive) in the queue
			}
			else
				m_pCallback->OnMotionPrimitiveFinished( pCurrentMotion ); // pCurrentMotion is the only primitive left in the queue
		}

		// The motions in the queue may have been changed in the callback function calls above.
		m_MotionPrimitiveQueue.pop_front();
		if( 0 < m_MotionPrimitiveQueue.size() )
		{
			shared_ptr<CMotionPrimitive> pPrevMotion = pCurrentMotion;
			pCurrentMotion = m_MotionPrimitiveQueue.front();

			LOG_PRINT( fmt_string(" new motion: %s", pCurrentMotion->GetName().c_str()) );

			current_motion_total_time = pCurrentMotion->GetTotalTime();
		}
		else
		{
			m_fCurrentTime = 0;
			return;
		}
	}

//	if( m_UpdateRootNodePose )
//		UpdateWorldNodePose( dt );
//	void UpdateWorldNodePose( float dt ) {

	// move the root pose from the prev to the current

	Matrix34 next_root_pose;

	UpdatePoseAndRootNodePose( pCurrentMotion, prev_time, m_fCurrentTime );
/*
	pCurrentMotion->GetInterpolatedKeyframe( prev_keyframe, prev_time );
	const Matrix34 root_pose_at_prev_time = prev_keyframe.GetRootPose();

	// root pose at the last keyframe of the current motion
	CKeyframe current_keyframe;
	pCurrentMotion->GetInterpolatedKeyframe( current_keyframe, m_fCurrentTime );
	const Matrix34 root_pose_at_current_keyframe = current_keyframe.GetRootPose();

	Matrix34 prev_to_current = root_pose_at_current_keyframe * root_pose_at_prev_time.GetInverseROT();

	const Vector3 vTranslaiton = prev_to_current.vPosition;
	const Vector3 vFwdHorizontal = Vector3( vTranslaiton.x, 0, vTranslaiton.z );

	const Matrix33 matHorizontalOrientation = m_HorizontalCurrentRootPose.matOrient;

	Matrix34 next_pose( Matrix34Identity() );

	// orientation
	next_pose.matOrient
		= root_pose_at_current_keyframe.matOrient
		* m_HorizontalCurrentRootPose.matOrient;

	// x & z of new position
	next_pose.vPosition
		= m_HorizontalCurrentRootPose.vPosition
		+ matHorizontalOrientation * vFwdHorizontal;

	next_pose.vPosition.y = root_pose_at_current_keyframe.vPosition.y;

	// >>> previous code???

//	Vector3 vDist = horizontal( prev_to_current.vPosition );
	Vector3 vDist = root_pose_at_current_keyframe.vPosition - root_pose_at_prev_time.vPosition;

//	m_CurrentRootPose.vPosition += vHorizontalTranslation;

	Matrix33 prev_to_current_horizontal = horizontal( prev_to_current.matOrient );

	if( true )// == strict )
	{
//		vDist.x *= 0.01f;

		// horizontal components of root pose
		next_root_pose.vPosition
			= m_HorizontalCurrentRootPose.vPosition
//			+ m_HorizontalCurrentRootPose.matOrient * prev_to_current_horizontal * vDist;
			+ m_HorizontalCurrentRootPose.matOrient * vDist;

		// vertical component of root pose
		// - just use the value of the original captured motion
		next_root_pose.vPosition.y = root_pose_at_current_keyframe.vPosition.y;

		next_root_pose.matOrient
			= m_HorizontalCurrentRootPose.matOrient
			* root_pose_at_current_keyframe.matOrient
			* Matrix33Transpose( horizontal( root_pose_at_prev_time.matOrient ) );
	}
	else
	{
		next_root_pose.matOrient = m_HorizontalCurrentRootPose.matOrient * root_pose_at_current_keyframe.matOrient;
		next_root_pose.vPosition = next_root_pose.matOrient * vDist + m_CurrentRootPose.vPosition;

//		next_root_pose = root_pose_at_current_keyframe;

		next_root_pose.vPosition.y = root_pose_at_current_keyframe.vPosition.y;
	}

///	m_CurrentRootPose.vPosition += m_HorizontalCurrentRootPose * vHorizontalTranslation;

	m_CurrentRootPose = next_root_pose;

	m_HorizontalCurrentRootPose = horizontal( m_CurrentRootPose );

	if( false )// m_UserFloorHeightOffset )
	{
		m_CurrentRootPose.vPosition.y += m_fFloorHeight;
	}

	m_CurrentRootPose.matOrient.Orthonormalize();*/
}


void CMotionPrimitiveBlender::CalculateKeyframe( CKeyframe& dest_keyframe )
{
	if( m_MotionPrimitiveQueue.size() == 0 )
		return;

	shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();

	if( m_fCurrentTime < pCurrentMotion->GetTotalTime() )
	{
		pCurrentMotion->GetInterpolatedKeyframe( dest_keyframe, m_fCurrentTime );
	}

	// root pose is updated in CMotionPrimitiveBlender::Update()
	dest_keyframe.SetRootPose( m_CurrentRootPose );

	return;

/*
	shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();

	while( pCurrentMotion->GetTotalTime() < m_fCurrentTime )
	{
		if( !pCurrentMotion )
			return; // no motion to play

		if( pCurrentMotion->IsLoopedMotion() )
//		 && m_MotionPrimitiveQueue.size() == 1 )
		{
			// the current motion is a looped motion // and no other motions are in the queue
			// - repeat the current motion

			m_fCurrentTime = fmodf( m_fCurrentTime, pCurrentMotion->GetTotalTime() );

			AddMotionPrimitive( 0.1f, pCurrentMotion, 0 );
	
			m_MotionPrimitiveQueue.pop_front();

			m_LastOriginalRootPose = pCurrentMotion->GetFirstKeyframe().GetRootPose();
		}
		else
		{
			m_fCurrentTime -= pCurrentMotion->GetTotalTime();

			if( m_MotionPrimitiveQueue.size() == 0 )
				return; // no more motion to play

			shared_ptr<CMotionPrimitive> pNextMotion = m_MotionPrimitiveQueue.front();

			m_MotionPrimitiveQueue.pop_front();

			// create motion primitive that interpolates pCurrentMotion & pNextMotion here?
			// - done in AddMotionPrimitive()

			pCurrentMotion = pNextMotion;
		}
	}

	int prev    = m_FlipVal;
	m_FlipVal = ( m_FlipVal + 1 ) % 2; // flip
	int current = m_FlipVal;

	pCurrentMotion->GetInterpolatedKeyframe( m_InterpolatedKeyframe[current], m_fCurrentTime, Interpolation::Linear );

	Matrix34 inv_last;
	m_LastOriginalRootPose.GetInverseROT( inv_last );

	Matrix34 last_to_current = m_InterpolatedKeyframe[current].GetRootPose() * inv_last;

	m_LastOriginalRootPose = m_InterpolatedKeyframe[current].GetRootPose();

	Vector3 vDistToCover
		= m_InterpolatedKeyframe[current].GetRootPose().vPosition
		- m_InterpolatedKeyframe[prev].GetRootPose().vPosition;

	Matrix34 inv_prev_pose;
	m_InterpolatedKeyframe[prev].GetRootPose().GetInverseROT( inv_prev_pose );
	Matrix34 prev_to_current_pos
//		= m_InterpolatedKeyframe[current].GetRootPose() * inv_prev_pose;
		= last_to_current;

	Matrix34 next_root_pose = prev_to_current_pos * m_CurrentRootPose;

	next_root_pose.matOrient.Orthonormalize();

	m_CurrentRootPose = next_root_pose;

	m_InterpolatedKeyframe[current].SetRootPose( next_root_pose );

	dest_keyframe = m_InterpolatedKeyframe[current];*/
}


void CMotionPrimitiveBlender::CalculateKeyframe()
{
//	CKeyframe dest_keyframe;
//	CalculateKeyframe( dest_keyframe );

	if( m_MotionPrimitiveQueue.size() == 0 )
		return;

	shared_ptr<CMotionPrimitive> pCurrentMotion = m_MotionPrimitiveQueue.front();

	if( IsInterpolationMotion( pCurrentMotion ) )
	{
		int break_here = 1;
	}

	if( m_fCurrentTime < pCurrentMotion->GetTotalTime() )
	{
		pCurrentMotion->CalculateInterpolatedKeyframe( m_fCurrentTime );
	}
}


/*
void CPVC_MotionPrimitivePlayManager::PlayMotionPrimitive( float dt )
{
	m_fCurrentTime += dt;

	CPVC_MotionPrimitive *pMotionPrimitive = m_MotionPrimitiveQueue.front();

	while( pMotionPrimitive->m_vecKeyFrame.back().m_fTime <= m_fCurrentTime )
	{	// the current motion primitive has been finished - go into interpolation for the next primitive

		// update time for the interpolation motion
		m_fCurrentTime -= pMotionPrimitive->m_vecKeyFrame.back().m_fTime;

		if( m_MotionPrimitiveQueue.size() == 1 )
		{	// the current motion is the last motion primitive in the queue
			m_MotionPrimitiveQueue.pop_front();
			m_sPlayMode = MPPM_NO_MOTION;
			m_fCurrentTime = 0;
			m_pCharacter->GetRootWorldPose( m_RootWorldPoseOffset );
			return;	// no more motion to play
		}
		else
		{
			// get the next motion primitive
			CPVC_MotionPrimitive *pPrevMotion, *pNextMotion;
			pPrevMotion = pMotionPrimitive;
			m_MotionPrimitiveQueue.pop_front();
			pNextMotion = m_MotionPrimitiveQueue.front();

			// get the current root position of the character
//			m_pCharacter->GetRootWorldPose( m_RootWorldPoseOffset );

			m_RootWorldPoseOffset.vPosition.x += pPrevMotion->m_vecKeyFrame.back().m_vecfFrameData[0];
			m_RootWorldPoseOffset.vPosition.z += pPrevMotion->m_vecKeyFrame.back().m_vecfFrameData[2];

			// start interpolation
			m_apInterpolatePrimitive[0] = pPrevMotion;
			m_apInterpolatePrimitive[1] = pNextMotion;
			m_fInterpolationTime = 0.25f;
			m_sPlayMode = MPPM_INTERPOLATION;

			return;
		}
	}

	static CPVC_KeyFrame s_keyframe;

	pMotionPrimitive->GetInterpolatedKeyFrame( s_keyframe, m_fCurrentTime, MP_INTERPOLATION_LINEAR );

	// offset the horizontal position to the current position of the root node
//	Matrix34 matRootWorldPose;
//	m_pCharacter->GetRootWorldPose( matRootWorldPose );
	s_keyframe.m_vecfFrameData[0] += m_RootWorldPoseOffset.vPosition.x;
	s_keyframe.m_vecfFrameData[2] += m_RootWorldPoseOffset.vPosition.z;


	// set pose from the root position and quaternions for each joint rotation
	m_pCharacter->SetPose_Quat( s_keyframe );
}


void CPVC_MotionPrimitivePlayManager::PlayInterpolaitonMotion( float dt )
{
	m_fCurrentTime += dt;
	if( m_fInterpolationTime < m_fCurrentTime )
	{	// finished interpolation - move on to the next motion primitive in the queue
		m_fCurrentTime -= m_fInterpolationTime;
		m_fInterpolationTime = 0.0f;
//		m_RootWorldPoseOffset = m_RootWorldPoseOffset;
		m_sPlayMode = MPPM_MOTIONPRIMITIVE;
		return;
	}

	const CPVC_KeyFrame& rSrcKeyFrame0 = m_apInterpolatePrimitive[0]->m_vecKeyFrame.back();
	const CPVC_KeyFrame& rSrcKeyFrame1 = m_apInterpolatePrimitive[1]->m_vecKeyFrame.front();

	// supress the root offset of the last key frame of the last motion primitive
	// and use the root offset for the next motion primitive
//	rSrcKeyFrame0.m_vecfFrameData[0] = 0;
//	rSrcKeyFrame0.m_vecfFrameData[2] = 0;

	CPVC_MotionPrimitive motion_primitive;

	motion_primitive.m_vecKeyFrame.resize( 0 );
	motion_primitive.m_vecKeyFrame.push_back( rSrcKeyFrame0 );
	motion_primitive.m_vecKeyFrame.push_back( rSrcKeyFrame1 );

	motion_primitive.m_vecKeyFrame[0].m_fTime = 0.0f;
	motion_primitive.m_vecKeyFrame[1].m_fTime = m_fInterpolationTime;

	// reset the root translation
//	motion_primitive.m_vecKeyFrame[0].m_vecfFrameData[0] = 0;
//	motion_primitive.m_vecKeyFrame[0].m_vecfFrameData[2] = 0;
//	motion_primitive.m_vecKeyFrame[1].m_vecfFrameData[0] = 0;
//	motion_primitive.m_vecKeyFrame[1].m_vecfFrameData[2] = 0;

	static CPVC_KeyFrame s_keyframe;

	motion_primitive.GetInterpolatedKeyFrame( s_keyframe, m_fCurrentTime, MP_INTERPOLATION_SLERP );

	// offset the horizontal position to the current position of the root node
	// no translation of the root node during the interpolation
//	Matrix34 matRootWorldPose;
//	m_pCharacter->GetRootWorldPose( matRootWorldPose );
	s_keyframe.m_vecfFrameData[0] = m_RootWorldPoseOffset.vPosition.x;
	s_keyframe.m_vecfFrameData[2] = m_RootWorldPoseOffset.vPosition.z;


	// set pose from the root position and quaternions for each joint rotation
	m_pCharacter->SetPose_Quat( s_keyframe );

}


*/


#include "Support/StringAux.hpp"
#include "Support/Vec3_StringAux.hpp"

void CMotionPrimitiveBlenderStatistics::Update()
{
	m_Buffer.resize( 0 );

	Matrix34 current_root_pose_h = m_pTarget->m_HorizontalCurrentRootPose;

	m_Buffer.push_back( fmt_string( "Current Play Time: %0.3f", m_pTarget->m_fCurrentTime ) );
	m_Buffer.push_back( fmt_string( "Current Root Pose:" ) );
	m_Buffer.push_back( fmt_string( "Pos(H): %s", to_string(current_root_pose_h.vPosition).c_str() ) );
	m_Buffer.push_back( fmt_string( "Fwd(H): %s", to_string(current_root_pose_h.matOrient.GetColumn(2)).c_str() ) );
}
