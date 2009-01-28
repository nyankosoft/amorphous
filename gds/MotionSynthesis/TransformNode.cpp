#include "BVH/BVHBone.h"
#include "TransformNode.h"

using namespace msynth;


CTransformNode::CTransformNode( CBVHBone& bvh_bone )
:
m_Rotation(Quaternion(0,0,0,0)),
m_vTranslation(Vector3(0,0,0))
{
	CopyFrame_r( bvh_bone );
}


void CTransformNode::CopyFrame_r( CBVHBone& src_bone )
{
	const Matrix34& src_local_trans = src_bone.GetTransformationMatrix();

	// local rotation
	m_Rotation.FromRotationMatrix( src_local_trans.matOrient );

	// local translation it's a root bone
	// - assumes only the root bones have 6-DOFs
	if( src_bone.GetNumChannels() == 6 )
		m_vTranslation = src_local_trans.vPosition;

	const int num_children = src_bone.GetNumChildren();
	m_vecChildNode.resize( num_children );

	for( int i=0; i<num_children; i++ )
	{
		m_vecChildNode[i].CopyFrame_r( *src_bone.GetChild(i) );
	}
}


void CTransformNode::Scale_r( float factor )
{
	m_vTranslation *= factor;

	const int num_children = GetNumChildren();

	for( int i=0; i<num_children; i++ )
	{
		m_vecChildNode[i].Scale_r( factor );
	}
}
