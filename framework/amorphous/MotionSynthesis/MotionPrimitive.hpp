#ifndef  __MotionPrimitive_H__
#define  __MotionPrimitive_H__


#include "amorphous/Support/Serialization/Serialization_BoostSmartPtr.hpp"
#include "amorphous/Support/FixedVector.hpp"
#include "fwd.hpp"
#include "Skeleton.hpp"
#include "Keyframe.hpp"


namespace amorphous
{


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

	friend class MotionPrimitive;
	friend class MotionDatabase;
};


//=======================================================================================
// MotionPrimitive
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
class MotionPrimitive : public IArchiveObjectBase
{
	std::string m_Name;

	/// motion segment data
	std::vector<Keyframe> m_vecKeyframe;

	/// offset of the root node at the start of the motion primitive in world coordinates
	/// transform the original motion data into the body center space
//	Matrix34 m_WorldOffset;

	std::shared_ptr<Skeleton> m_pSkeleton;
//	Skeleton m_pSkeleton;

	bool m_bIsLoopedMotion;

	/// 1: has annotation
	std::vector<char> m_vecAnnotation;

	/// user id (not serialized)
	int m_UserID;

	/// Name of the bone which is the root of this motion data
	/// For , m_StartBoneName == m_pSkeleton->GetRootBone().GetName()
	std::string m_StartBoneName;

	/// Used at runtime
	std::shared_ptr<BlendNode> m_pStartBlendNode;

public:

	MotionPrimitive() : m_bIsLoopedMotion(false) {}

	MotionPrimitive( const std::string& name ) : m_bIsLoopedMotion(false) { m_Name = name; }

	~MotionPrimitive() {}

	bool IsEmpty() const { return m_vecKeyframe.size() == 0; }

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	void SetKeyframes( const std::vector<Keyframe> &rvecKeyframe ) { m_vecKeyframe = rvecKeyframe; }

	void InsertKeyframe( const Keyframe& keyframe );

	const Keyframe& GetFirstKeyframe() const { return m_vecKeyframe.front(); }

	const Keyframe& GetLastKeyframe() const { return m_vecKeyframe.back(); }

	inline void GetFirstKeyframe( Keyframe& dest );

	inline void GetLastKeyframe( Keyframe& dest );

	inline float GetTotalTime() const;

	bool IsLoopedMotion() const { return m_bIsLoopedMotion; }

	void SetLoopedMotion( bool looped ) { m_bIsLoopedMotion = looped; }

	Result::Name GetNearestKeyframeIndices( float time, int& i0, int& i1 );

	Result::Name GetNearestKeyframeIndices( float time, int& i0, int& i1, int& i2, int& i3, float& frac, fixed_vector<Vector3,4>& root_position_offsets );

	Result::Name GetInterpolatedKeyframe( Keyframe& dest_interpolated_keyframe, float time, Interpolation::Mode mode = Interpolation::Linear );

	void CalculateInterpolatedKeyframe( float time );

//	void ResizeKeyframeBuffer( int num_keyframes );

	/// let the user directly modify keyframe data
	std::vector<Keyframe>& GetKeyframeBuffer() { return m_vecKeyframe; }

	Result::Name CreateEmptyKeyframes( uint num_keyframes );

//	void SetSkeleton( const Skeleton& skeleton ) { m_pSkeleton = skeleton; }
	void SetSkeleton( const Skeleton& skeleton ) { m_pSkeleton = std::shared_ptr<Skeleton>( new Skeleton(skeleton) ); }

//	const Skeleton& GetSkeleton() const { return m_pSkeleton; }
	const std::shared_ptr<Skeleton> GetSkeleton() const { return m_pSkeleton; }

	const std::string& GetStartBoneName() const { return m_StartBoneName; }

	void SetStartsBoneName( const std::string& start_bone_name ) { m_StartBoneName = start_bone_name; }

	void SearchAndSetStartBlendNode( std::shared_ptr<BlendNode>& pRootBlendNode );

	std::shared_ptr<BlendNode> GetStartBlendNode() { return m_pStartBlendNode; }

	void SetStartBlendNode( std::shared_ptr<BlendNode>& pBlendNode ) { m_pStartBlendNode = pBlendNode; }

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

	friend class MotionDatabaseCompiler;
	friend class MotionDatabaseBuilder;
};


inline void MotionPrimitive::GetFirstKeyframe( Keyframe& dest )
{
	if( 0 < m_vecKeyframe.size() )
		dest = m_vecKeyframe.front();
}


inline void MotionPrimitive::GetLastKeyframe( Keyframe& dest )
{
	if( 0 < m_vecKeyframe.size() )
		dest = m_vecKeyframe.back();
}


inline float MotionPrimitive::GetTotalTime() const
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

} // namespace amorphous



#endif  /*  __MotionPrimitive_H__  */
