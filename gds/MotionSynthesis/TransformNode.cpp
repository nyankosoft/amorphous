#include "BVH/BVHBone.hpp"
#include "TransformNode.hpp"

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


void CTransformNode::GetTransform( Transform& pose, const std::vector<int>& node_locator, uint& index ) const
{
	if( (uint)node_locator.size() == index )
	{
		pose.vTranslation = m_vTranslation;
		pose.qRotation    = m_Rotation;
		return;
	}

	int child_index = node_locator[index];
	if( 0 <= child_index && child_index < (int)m_vecChildNode.size() )
	{
		index++;
		m_vecChildNode[child_index].GetTransform( pose, node_locator, index );
	}
}


void CTransformNode::SetTransform( const Transform& pose, const std::vector<int>& node_locator, uint& index )
{
	if( (uint)node_locator.size() == index )
	{
		m_vTranslation = pose.vTranslation;
		m_Rotation     = pose.qRotation;
		return;
	}

	int child_index = node_locator[index];
	if( 0 <= child_index && child_index < (int)m_vecChildNode.size() )
	{
		index++;
		m_vecChildNode[child_index].SetTransform( pose, node_locator, index );
	}
}


CTransformNode *CTransformNode::GetNode( const std::vector<int>& node_locator, uint& index )
{
	if( (uint)node_locator.size() == index )
		return this;

	int child_index = node_locator[index];
	if( 0 <= child_index && child_index < (int)m_vecChildNode.size() )
	{
		index++;
		return m_vecChildNode[child_index].GetNode( node_locator, index );
	}
	else
		return NULL;
}
