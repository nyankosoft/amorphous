#ifndef __ENTITYMOTIONPATHREQUEST_H__
#define __ENTITYMOTIONPATHREQUEST_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/3DMath/Quaternion.hpp"
#include "gds/Support/memory_helpers.hpp"
#include "fwd.hpp"
#include <vector>


/**
 * holds a position of a path which an entity should follow
 */
struct KeyPose
{
	float time;
	Matrix34 pose;

	KeyPose() : time(0)/*, pose(Matrix34Identity())*/ {}
	KeyPose( float _time, Matrix34 _pose ) : time(_time), pose(_pose) {}
};


class CBEC_MotionPath
{
	std::vector<KeyPose> m_vecKeyPose;

	float m_fStartTime;
	float m_fEndTime;

	int m_MPType;

public:

	CBEC_MotionPath() : m_MPType(MPTYPE_DEFAULT), m_fStartTime(0), m_fEndTime(0) { m_vecKeyPose.reserve(8); }

//	void SetKeyPoses( std::vector<KeyPose>& vecKeyPose ) { m_vecKeyPose = vecKeyPose; }
	inline void SetKeyPoses( std::vector<KeyPose>& vecKeyPose );

	inline Matrix34 GetPose( float time );

	inline void AddKeyPose( const KeyPose& key_pose );

	/// returns if a path exist at a given time
	inline bool IsAvailable( float time ) const;

	inline bool IsExpired( float time ) const;

	inline void ReleaseMotionPath() { m_vecKeyPose.resize(0); }

	float GetStartTime() const { return m_fStartTime; }

	float GetEndTime() const { return m_fEndTime; }

//	inline void Init( std::vector<KeyPose>& vecKeyPose, int motion_path_type );

	const std::vector<KeyPose>& GetKeyPose() const { return m_vecKeyPose; }

	enum eMotionPathType
	{
		MPTYPE_DEFAULT,
		MPTYPE_WAIT_INFINITELY_AT_END_POSE,
		MPTYPE_LOOP,
		NUM_MPTYES
	};

	int GetMotionPathType() const { return m_MPType; }
	void SetMotionPathType( int type ) { m_MPType = type; }
};


inline void CBEC_MotionPath::SetKeyPoses( std::vector<KeyPose>& vecKeyPose )
{
	CopyVector( m_vecKeyPose, vecKeyPose );

	if( 0 < vecKeyPose.size() )
	{
		m_fStartTime = vecKeyPose.front().time;
		m_fEndTime   = vecKeyPose.back().time;
	}
}


inline Matrix34 CBEC_MotionPath::GetPose( float time )
{
	size_t i, num_key_poses = m_vecKeyPose.size();

	// modify time for the current path type
	switch( GetMotionPathType() )
	{
	case MPTYPE_LOOP:
		if( num_key_poses == 1 )
			break;
		else
		{
			float time_frac = time - m_fStartTime;
			float time_length = m_fEndTime - m_fStartTime;

            while( time_length )
				time_frac -= time_length;

			time = time_frac + m_fStartTime;
		}
		break;

	case MPTYPE_WAIT_INFINITELY_AT_END_POSE:
		if( m_fEndTime < time )
			time = m_fEndTime;
		break;

	case MPTYPE_DEFAULT:
	default:
		break;
	}

	if( num_key_poses == 0 )
        return Matrix34();
	else if( num_key_poses == 1 )
		return m_vecKeyPose[0].pose;
	else
	{
		for( i=0; i<num_key_poses-1; i++ )
		{
			if( m_vecKeyPose[i].time <= time && time <= m_vecKeyPose[i+1].time )
			{
				Matrix34 pose;
				Matrix34 pose0 = m_vecKeyPose[i].pose;
				Matrix34 pose1 = m_vecKeyPose[i+1].pose;
				const float t0 = m_vecKeyPose[i].time;
				const float t1 = m_vecKeyPose[i+1].time;
//				pose.vPosition = ( (t1 - time) * pose0.vPosition + (time - t0) * pose1.vPosition ) / (t1 - t0);

				const Vector3 vDeltaPos = pose1.vPosition - pose0.vPosition;
				pose.vPosition = pose0.vPosition + vDeltaPos * (time - t0) / (t1 - t0);

				// interpolate pose
				Quaternion q0, q1, q;
				q0.FromRotationMatrix( pose0.matOrient );
				q1.FromRotationMatrix( pose1.matOrient );
				q = ( (t1 - time) * q0 + (time - t0) * q1 ) / (t1 - t0);
				q.ToRotationMatrix( pose.matOrient );
				return pose;
			}
		}

		return m_vecKeyPose.back().pose;
	}
}


inline void CBEC_MotionPath::AddKeyPose( const KeyPose& key_pose )
{
	if( m_vecKeyPose.size() == 0 || m_vecKeyPose.back().time <= key_pose.time )
	{
		m_vecKeyPose.push_back( key_pose );
		return;
	}

//	vector<KeyPose>::reverse_iterator ritr = m_vecKeyPose.end();
	for( int i=(int)(m_vecKeyPose.size())-1; 0<=i; i-- )
	{
		if( m_vecKeyPose[i].time < key_pose.time )
		{
			m_vecKeyPose.insert( m_vecKeyPose.begin() + i + 1, key_pose );
			return;
		}
	}

	// not added yet
	// - key_pose.time is smaller than any time of key_poses in m_vecKeyPose
	// - insert key_pose at the front of the list
	m_vecKeyPose.insert( m_vecKeyPose.begin(), key_pose );
}


inline bool CBEC_MotionPath::IsAvailable( float time ) const
{
	size_t num_key_poses = m_vecKeyPose.size();
	if( num_key_poses == 0 )
		return false;

	switch( GetMotionPathType() )
	{
	case MPTYPE_DEFAULT:
		if( num_key_poses == 1 )
			return false;
		else
		{
			return ( m_fStartTime <= time && time <= m_fEndTime );
		}
		break;

	case MPTYPE_LOOP:
		if( num_key_poses == 1 )
			return false;
		else
			return ( m_fStartTime <= time );
		break;

	case MPTYPE_WAIT_INFINITELY_AT_END_POSE:
		return ( m_fStartTime <= time );

	default:
		break;
	}

	return false;
}


inline bool CBEC_MotionPath::IsExpired( float time ) const
{
	size_t num_key_poses = m_vecKeyPose.size();
	if( num_key_poses == 0 )
		return true;

	switch( GetMotionPathType() )
	{
	case MPTYPE_DEFAULT:
		if( num_key_poses == 1 )
			return true; // only has one pose - Does this make the behavior consistent with IsAvailable()?
		else
		{
			return ( m_fEndTime < time );
		}
		break;

	case MPTYPE_LOOP:
		return false;

	case MPTYPE_WAIT_INFINITELY_AT_END_POSE:
		return false;

	default:
		break;
	}

	return true;
}



/**
 * holds a request of a motion path for an entity to follow
 */
class EntityMotionPathRequest
{
public:

	enum motion_type
	{
		SET_POSITION,
		CONTROL_FORCE,
		NUM_MOTION_TYPES
	};

	CCopyEntity *pTargetEntity;
	int mode;
	std::vector<KeyPose> vecKeyPose;	///< motion path

	int MotionPathType;

public:

	EntityMotionPathRequest()
		:
	pTargetEntity(NULL),
	mode(SET_POSITION),
	MotionPathType(CBEC_MotionPath::MPTYPE_DEFAULT)
	{}

	void Clear() { pTargetEntity = NULL; vecKeyPose.resize(0); }
};



#endif  /*  __ENTITYMOTIONPATHREQUEST_H__  */
