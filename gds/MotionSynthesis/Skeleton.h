#ifndef  __Skeleton_H__
#define  __Skeleton_H__


#include "3DMath/Vector3.h"
#include "BVH/fwd.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "fwd.h"
#include "TransformNode.h"


/// set a blend weight for a particular bone
//void SetBlendWeight( bone_name, weight );


namespace msynth
{


class CBone : public IArchiveObjectBase
{
	std::string m_Name;

	Vector3 m_vOffset;

	std::vector<CBone> m_vecChild;

public:

	CBone() : m_vOffset(Vector3(0,0,0)) {}

	CBone( CBVHBone& bvh_bone );

	const std::string& GetName() const { return m_Name; }

	void SetName( const std::string& name ) { m_Name = name; }

	Vector3 GetOffset() const { return m_vOffset; }

	void SetOffset( const Vector3& offset ) { m_vOffset = offset; }

	int GetNumChildren() const { return (int)m_vecChild.size(); }

	const CBone& GetChild( int index ) const { return m_vecChild[index]; }

	void CopyBones_r( CBVHBone& src_bone );

	void Scale_r( float factor );

	inline void CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const CTransformNode& input_node ) const;

	void Serialize( IArchive & ar, const unsigned int version );
};


inline void CBone::CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const CTransformNode& input_node ) const
{
	dest_transform
		= parent_transform
//		* Matrix34( input_node.GetLocalTranslation(), input_node.GetLocalRotationQuaternion().ToRotationMatrix() )
//		* Matrix34( m_vOffset, Matrix33Identity() );
		* Matrix34( input_node.GetLocalTranslation() + m_vOffset, input_node.GetLocalRotationQuaternion().ToRotationMatrix() );
}


class CSkeleton : public IArchiveObjectBase
{
	CBone m_RootBone;

	static const CSkeleton m_Null;

public:

	void CopyFromBVHSkeleton( CBVHBone& root_bone ) { m_RootBone.CopyBones_r( root_bone ); }

	const CBone& GetRootBone() const { return m_RootBone; }

	void Scale( float scaling_factor ) { m_RootBone.Scale_r( scaling_factor ); }

	void Serialize( IArchive & ar, const unsigned int version ) { ar & m_RootBone; }

	static const CSkeleton& GetNull() { return m_Null; }
};


/// created per target skeletal mesh ?
class CWeightBlendSkeleton
{
public:
};

class CWeightBlendBone
{
public:
	Vector3 m_vOffset;
	float m_afBlendWeight[8];
};


} // namespace msynth


#endif /* __Skeleton_H__ */
