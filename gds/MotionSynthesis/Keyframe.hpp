#ifndef __Keyframe_HPP__
#define __Keyframe_HPP__


#include "fwd.hpp"
#include "TransformNode.hpp"


namespace msynth
{

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


} // namespace msynth



#endif /* __Keyframe_HPP__ */
