#ifndef  __TransformNode_H__
#define  __TransformNode_H__


#include "gds/3DMath/Quaternion.hpp"
#include "gds/3DMath/Transform.hpp"
#include "gds/BVH/fwd.hpp"
#include "gds/Support/Serialization/Serialization.hpp"
#include "gds/Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;


namespace msynth
{


//=======================================================================================
// CTransformNode
//   used to keep a pose of a joint
//=======================================================================================

class CTransformNode : public IArchiveObjectBase
{
	Quaternion m_Rotation;

	Vector3 m_vTranslation;

	std::vector<CTransformNode> m_vecChildNode;

	/// used during runtime to blend motions of particular parts of the body
//	float m_fBlendWeight;

public:


	CTransformNode() : m_Rotation(Quaternion(0,0,0,0)), m_vTranslation(Vector3(0,0,0))// {}
	{
		m_Rotation.FromRotationMatrix( Matrix33Identity() );
	}

	CTransformNode( CBVHBone& bvh_bone );

	~CTransformNode() {}

	void SetInterpolatedTransform_r( float frac, const CTransformNode& node0, const CTransformNode& node1 );

	void SetInterpolatedTransform_r( float frac, const CTransformNode& node0, const CTransformNode& node1, const CTransformNode& node2, const CTransformNode& node3 );

	void CopyFrame_r( CBVHBone& src_bone );

	void Scale_r( float factor );

	inline void Serialize( IArchive & ar, const unsigned int version );

	int GetNumChildren() const { return (int)m_vecChildNode.size(); }

	const CTransformNode& GetChildNode(int index) const { return m_vecChildNode[index]; }

	CTransformNode& ChildNode(int index) { return m_vecChildNode[index]; }

	Quaternion GetLocalRotationQuaternion() const { return m_Rotation; }

	Vector3 GetLocalTranslation() const { return m_vTranslation; }

	Transform GetLocalTransform() const { return Transform( m_Rotation, m_vTranslation ); }

	void SetLocalTransform( const Transform& local_transform ) { m_vTranslation = local_transform.vTranslation; m_Rotation = local_transform.qRotation; }

	// setters

	void SetRotation( const Quaternion& rotation ) { m_Rotation = rotation; }

	void SetTranslation( const Vector3& vTranslation ) { m_vTranslation = vTranslation; }

	void SetNumChildren( int num_children ) { m_vecChildNode.resize( num_children ); }

	std::vector<CTransformNode>& Child() { return m_vecChildNode; }

	Transform GetTransform() const { return Transform( m_Rotation, m_vTranslation ); }

	void SetTransform( const Transform& transform ) { m_Rotation = transform.qRotation; m_vTranslation = transform.vTranslation; }

	void GetTransform( Transform& pose, const std::vector<int>& node_locator, uint& index ) const;

	void SetTransform( const Transform& pose, const std::vector<int>& node_locator, uint& index );

	CTransformNode *GetNode( const std::vector<int>& node_locator, uint& index );

	friend class CKeyframe;
	friend class CMotionPrimitive;
	friend class CBone;
};


//========================== inline implementations ==========================

inline void CTransformNode::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Rotation;
	ar & m_vTranslation;
	ar & m_vecChildNode;
}


} // namespace msynth


#endif /* __TransformNode_H__ */
