#ifndef  __MotionPrimitive_H__
#define  __MotionPrimitive_H__


#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "gds/3DMath/Matrix34.hpp"
#include "gds/3DMath/Quaternion.hpp"
#include "gds/3DMath/Transform.hpp"
#include "gds/Support/Serialization/Serialization_BoostSmartPtr.hpp"

#include "fwd.hpp"
#include "Skeleton.hpp"


namespace msynth
{


class CAnnotation
{
	std::string m_Name;

	int m_Index;

private:

	CAnnotation( const std::string& name, int index ) : m_Name(name), m_Index(index) {}

public:

	CAnnotation() : m_Index(-1) {}

	~CAnnotation() {}

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	friend class CMotionPrimitive;
	friend class CMotionDatabase;
};


/**
 Stores hierarchical transformations of a particular time
*/
class CKeyframe : public IArchiveObjectBase
{
	CTransformNode m_RootNode;

	float m_fTime;

public:

	CKeyframe() : m_fTime(0) {}

	CKeyframe( float time, const CTransformNode& root_node = CTransformNode() ) : m_fTime(time), m_RootNode(root_node) {}

	const CTransformNode& GetRootNode() const { return  m_RootNode; }

	CTransformNode& RootNode() { return m_RootNode; }

	float GetTime() const { return m_fTime; }

	void SetTime( float time ) { m_fTime = time; }

	inline Matrix34 GetRootPose() const;

	inline void GetRootPose( Matrix34& dest ) const;

	inline void SetRootPose( const Matrix34& src );

	inline void SetInterpolatedKeyframe( float frac, const CKeyframe& keyframe0, const CKeyframe& keyframe1 );

	inline void SetInterpolatedKeyframe( float frac, const CKeyframe& keyframe0, const CKeyframe& keyframe1, const CKeyframe& keyframe2, const CKeyframe& keyframe3 );

	void Scale( float scaling_factor ) { m_RootNode.Scale_r( scaling_factor ); }

	inline Transform GetTransform( const std::vector<int>& locator ) const;

	inline void SetTransform( const Transform& pose, const std::vector<int>& locator );

	inline CTransformNode *GetTransformNode( const std::vector<int>& locator );

	inline Result::Name SetBoneLocalRotation( const std::vector<int>& locator, float heading, float pitch, float bank );

	inline virtual void Serialize( IArchive & ar, const unsigned int version );

	friend class CMotionPrimitive;
};


inline Matrix34 CKeyframe::GetRootPose() const
{
	Matrix34 root_pose;
	GetRootPose( root_pose );

	return root_pose;
}


inline void CKeyframe::SetRootPose( const Matrix34& src )
{
	m_RootNode.m_vTranslation = src.vPosition;
	m_RootNode.m_Rotation.FromRotationMatrix( src.matOrient );
}


inline void CKeyframe::GetRootPose( Matrix34& dest ) const
{
	dest.vPosition = m_RootNode.m_vTranslation;
	m_RootNode.m_Rotation.ToRotationMatrix( dest.matOrient );
}

inline void CKeyframe::SetInterpolatedKeyframe( float frac, const CKeyframe& keyframe0, const CKeyframe& keyframe1 )
{
	m_RootNode.SetInterpolatedTransform_r( frac, keyframe0.m_RootNode, keyframe1.m_RootNode );
}

inline void CKeyframe::SetInterpolatedKeyframe( float frac, const CKeyframe& keyframe0, const CKeyframe& keyframe1, const CKeyframe& keyframe2, const CKeyframe& keyframe3 )
{
	m_RootNode.SetInterpolatedTransform_r( frac, keyframe0.m_RootNode, keyframe1.m_RootNode, keyframe2.m_RootNode, keyframe3.m_RootNode );
}


inline Transform CKeyframe::GetTransform( const std::vector<int>& locator ) const
{
	if( locator.empty() )
		return m_RootNode.GetTransform();

	Transform dest;
	uint index = 0;
	m_RootNode.GetTransform( dest, locator, index );
	return dest;
}


inline void CKeyframe::SetTransform( const Transform& pose, const std::vector<int>& locator )
{
	if( locator.empty() )
	{
		m_RootNode.SetTransform( pose );
		return;
	}

	uint index = 0;
	m_RootNode.SetTransform( pose, locator, index );
}


inline CTransformNode *CKeyframe::GetTransformNode( const std::vector<int>& locator )
{
	if( locator.empty() )
		return &m_RootNode;

	uint index = 0;
	return m_RootNode.GetNode( locator, index );
}


inline Result::Name CKeyframe::SetBoneLocalRotation( const std::vector<int>& locator, float heading, float pitch, float bank )
{
	const Quaternion rotation(
		  Matrix33RotationY( heading )
		* Matrix33RotationX( pitch )
		* Matrix33RotationZ( bank )
		);

	if( locator.empty() )
	{
		m_RootNode.SetRotation( rotation );
		return Result::SUCCESS;
	}

	uint index = 0;
	CTransformNode *pNode = m_RootNode.GetNode( locator, index );
	if( pNode )
	{
		pNode->SetRotation( rotation );
		return Result::SUCCESS;
	}
	else
		return Result::INVALID_ARGS;
}


inline void CKeyframe::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_RootNode;
	ar & m_fTime;
}



//=======================================================================================
// CMotionPrimitive
//   used to store segmented motions
//=======================================================================================


class Interpolation
{
public:

	enum Mode
	{
		Linear,
		Slerp,
		NumModes
	};
};


/**
 Stores keyframes
 - all the keyframes in a motion primitive must have the same hierarchical structure
   for transformations

 - Terms
 -- complete motion primitive: a motion primitive that has the transforms for all the bones
*/
class CMotionPrimitive : public IArchiveObjectBase
{
	std::string m_Name;

	/// motion segment data
	std::vector<CKeyframe> m_vecKeyframe;

	/// offset of the root node at the start of the motion primitive in world coordinates
	/// transform the original motion data into the body center space
//	Matrix34 m_WorldOffset;

	boost::shared_ptr<CSkeleton> m_pSkeleton;
//	CSkeleton m_pSkeleton;

	bool m_bIsLoopedMotion;

	/// 1: has annotation
	std::vector<char> m_vecAnnotation;

	/// user id (not serialized)
	int m_UserID;

	/// Name of the bone which is the root of this motion data
	/// For , m_StartBoneName == m_pSkeleton->GetRootBone().GetName()
	std::string m_StartBoneName;

	/// Used at runtime
	boost::shared_ptr<CBlendNode> m_pStartBlendNode;

public:

	CMotionPrimitive() : m_bIsLoopedMotion(false) {}

	CMotionPrimitive( const std::string& name ) : m_bIsLoopedMotion(false) { m_Name = name; }

	~CMotionPrimitive() {}

	bool IsEmpty() const { return m_vecKeyframe.size() == 0; }

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	void SetKeyframes( const std::vector<CKeyframe> &rvecKeyframe ) { m_vecKeyframe = rvecKeyframe; }

	void InsertKeyframe( const CKeyframe& keyframe );

	const CKeyframe& GetFirstKeyframe() const { return m_vecKeyframe.front(); }

	const CKeyframe& GetLastKeyframe() const { return m_vecKeyframe.back(); }

	inline void GetFirstKeyframe( CKeyframe& dest );

	inline void GetLastKeyframe( CKeyframe& dest );

	inline float GetTotalTime() const;

	bool IsLoopedMotion() const { return m_bIsLoopedMotion; }

	void SetLoopedMotion( bool looped ) { m_bIsLoopedMotion = looped; }

	Result::Name GetNearestKeyframeIndices( float time, int& i0, int& i1 );

	Result::Name GetNearestKeyframeIndices( float time, int& i0, int& i1, int& i2, int& i3 );

	Result::Name GetInterpolatedKeyframe( CKeyframe& dest_interpolated_keyframe, float time, Interpolation::Mode mode = Interpolation::Linear );

	void CalculateInterpolatedKeyframe( float time );

//	void ResizeKeyframeBuffer( int num_keyframes );

	/// let the user directly modify keyframe data
	std::vector<CKeyframe>& GetKeyframeBuffer() { return m_vecKeyframe; }

	Result::Name CreateEmptyKeyframes( uint num_keyframes );

//	void SetSkeleton( const CSkeleton& skeleton ) { m_pSkeleton = skeleton; }
	void SetSkeleton( const CSkeleton& skeleton ) { m_pSkeleton = boost::shared_ptr<CSkeleton>( new CSkeleton(skeleton) ); }

//	const CSkeleton& GetSkeleton() const { return m_pSkeleton; }
	const boost::shared_ptr<CSkeleton> GetSkeleton() const { return m_pSkeleton; }

	const std::string& GetStartBoneName() const { return m_StartBoneName; }

	void SetStartsBoneName( const std::string& start_bone_name ) { m_StartBoneName = start_bone_name; }

	void SearchAndSetStartBlendNode( boost::shared_ptr<CBlendNode>& pRootBlendNode );

	boost::shared_ptr<CBlendNode> GetStartBlendNode() { return m_pStartBlendNode; }

	void SetStartBlendNode( boost::shared_ptr<CBlendNode>& pBlendNode ) { m_pStartBlendNode = pBlendNode; }

	void Serialize( IArchive & ar, const unsigned int version );

	unsigned int GetVersion() const { return 1; }

	bool HasAnnotation( const CAnnotation& annotation ) const
	{
		if( annotation.m_Index < 0 || (int)m_vecAnnotation.size() <= annotation.m_Index )
			return false;

		return ( m_vecAnnotation[annotation.m_Index] == 1 ) ? true : false;
	}

	void SetUserID( int user_id ) { m_UserID = user_id; }

	int GetUserID() const { return m_UserID; }

	friend class CMotionDatabaseCompiler;
	friend class CMotionDatabaseBuilder;
};


inline void CMotionPrimitive::GetFirstKeyframe( CKeyframe& dest )
{
	if( 0 < m_vecKeyframe.size() )
		dest = m_vecKeyframe.front();
}


inline void CMotionPrimitive::GetLastKeyframe( CKeyframe& dest )
{
	if( 0 < m_vecKeyframe.size() )
		dest = m_vecKeyframe.back();
}


inline float CMotionPrimitive::GetTotalTime() const
{
	if( 0 < m_vecKeyframe.size() )
		return m_vecKeyframe.back().GetTime();
	else
		return 0;
}


/*
class CSkeletalAnimationController : public IArchiveObjectBase
{
	/// offset of the root node at the start of the motion primitive in world coordinates
	/// transform the original motion data into the body center space
	Matrix34 m_WorldOffset;

public:

	CSkeletalAnimationController() {}

	~CSkeletalAnimationController() {}

	void Serialize( IArchive & ar, const unsigned int version );
};
*/


} // namespace msynth


#endif  /*  __MotionPrimitive_H__  */
