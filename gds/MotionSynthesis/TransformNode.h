#ifndef  __TransformNode_H__
#define  __TransformNode_H__


#include "3DMath/Vector3.h"
#include "3DMath/Quaternion.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
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

	void CopyFrame_r( CBVHBone& src_bone );

	void Scale_r( float factor );

	inline void Serialize( IArchive & ar, const unsigned int version );

	int GetNumChildren() const { return (int)m_vecChildNode.size(); }

	const CTransformNode& GetChildNode(int index) const { return m_vecChildNode[index]; }

	Quaternion GetLocalRotationQuaternion() const { return m_Rotation; }

	Vector3 GetLocalTranslation() const { return m_vTranslation; }

	friend class CKeyframe;
	friend class CMotionPrimitive;
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
