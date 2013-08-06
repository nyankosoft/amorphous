#ifndef  __TransformNode_H__
#define  __TransformNode_H__


#include "amorphous/3DMath/Quaternion.hpp"
#include "amorphous/3DMath/Transform.hpp"
#include "amorphous/BVH/fwd.hpp"
#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{
using namespace serialization;


namespace msynth
{


//=======================================================================================
// TransformNode
//   used to keep a pose of a joint
//=======================================================================================

class TransformNode : public IArchiveObjectBase
{
	Quaternion m_Rotation;

	Vector3 m_vTranslation;

	std::vector<TransformNode> m_vecChildNode;

	/// used during runtime to blend motions of particular parts of the body
//	float m_fBlendWeight;

public:


	TransformNode() : m_Rotation(Quaternion(0,0,0,0)), m_vTranslation(Vector3(0,0,0))// {}
	{
		m_Rotation.FromRotationMatrix( Matrix33Identity() );
	}

	TransformNode( BVHBone& bvh_bone );

	~TransformNode() {}

	void SetInterpolatedTransform_r( float frac, const TransformNode& node0, const TransformNode& node1 );

	void SetInterpolatedTransform_r( float frac, const TransformNode& node0, const TransformNode& node1, const TransformNode& node2, const TransformNode& node3 );

	void CopyFrame_r( BVHBone& src_bone );

	void Scale_r( float factor );

	inline void Serialize( IArchive & ar, const unsigned int version );

	int GetNumChildren() const { return (int)m_vecChildNode.size(); }

	const TransformNode& GetChildNode(int index) const { return m_vecChildNode[index]; }

	TransformNode& ChildNode(int index) { return m_vecChildNode[index]; }

	Quaternion GetLocalRotationQuaternion() const { return m_Rotation; }

	Vector3 GetLocalTranslation() const { return m_vTranslation; }

	Transform GetLocalTransform() const { return Transform( m_Rotation, m_vTranslation ); }

	void SetLocalTransform( const Transform& local_transform ) { m_vTranslation = local_transform.vTranslation; m_Rotation = local_transform.qRotation; }

	// setters

	void SetRotation( const Quaternion& rotation ) { m_Rotation = rotation; }

	void SetTranslation( const Vector3& vTranslation ) { m_vTranslation = vTranslation; }

	void SetNumChildren( int num_children ) { m_vecChildNode.resize( num_children ); }

	std::vector<TransformNode>& Child() { return m_vecChildNode; }

	Transform GetTransform() const { return Transform( m_Rotation, m_vTranslation ); }

	void SetTransform( const Transform& transform ) { m_Rotation = transform.qRotation; m_vTranslation = transform.vTranslation; }

	void GetTransform( Transform& pose, const std::vector<int>& node_locator, uint& index ) const;

	void SetTransform( const Transform& pose, const std::vector<int>& node_locator, uint& index );

	TransformNode *GetNode( const std::vector<int>& node_locator, uint& index );

	friend class Keyframe;
	friend class MotionPrimitive;
	friend class Bone;
};


//========================== inline implementations ==========================

inline void TransformNode::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Rotation;
	ar & m_vTranslation;
	ar & m_vecChildNode;
}


} // namespace msynth

} // namespace amorphous



#endif /* __TransformNode_H__ */
