#include "TransformNode.hpp"
#include "gds/BVH/BVHBone.hpp"
#include "gds/3DMath/TCBSpline.hpp"

using namespace msynth;


inline Transform operator+( const Transform& lhs, const Transform& rhs )
{
	Quaternion q = lhs.qRotation + rhs.qRotation;
//	q.Normalize();
	return Transform(
//		lhs.qRotation    + rhs.qRotation,
		q,
		lhs.vTranslation + rhs.vTranslation
		);
}


inline Transform operator-( const Transform& lhs, const Transform& rhs )
{
	Quaternion q = lhs.qRotation - rhs.qRotation;
//	q.Normalize();
	return Transform(
//		lhs.qRotation    - rhs.qRotation,
		q,
		lhs.vTranslation - rhs.vTranslation
		);
}


inline Transform operator*( float lhs, const Transform& rhs )
{
	return Transform(
		lhs * rhs.qRotation,
		lhs * rhs.vTranslation
		);
}


inline Transform TCBSplineMultiply( const Transform& p1, const Transform& p2, const Transform& incoming_tangent, const Transform& outgoing_tangent, const Vector4& hS )
{
	Quaternion q = TCBSplineMultiply( p1.qRotation, p2.qRotation, incoming_tangent.qRotation, outgoing_tangent.qRotation, hS );
	q.Normalize();
	Vector3 t    = TCBSplineMultiply( p1.vTranslation, p2.vTranslation, incoming_tangent.vTranslation, outgoing_tangent.vTranslation, hS );

	return Transform( q, t );
}


CTransformNode::CTransformNode( CBVHBone& bvh_bone )
:
m_Rotation(Quaternion(0,0,0,0)),
m_vTranslation(Vector3(0,0,0))
{
	CopyFrame_r( bvh_bone );
}


void CTransformNode::SetInterpolatedTransform_r( float frac, const CTransformNode& node0, const CTransformNode& node1 )
{
	m_vTranslation = node0.m_vTranslation * ( 1.0f - frac ) + node1.m_vTranslation * frac;

//	m_Rotation = node0.m_Rotation * ( 1.0f - frac ) + node1.m_Rotation * frac;
	m_Rotation.Slerp( frac, node0.m_Rotation, node1.m_Rotation );

	const Matrix33 mat = m_Rotation.ToRotationMatrix(); // check the rotation for debugging

	const size_t num_children = node0.m_vecChildNode.size();

	if( m_vecChildNode.size() != num_children )
	{
		m_vecChildNode.resize( num_children );
	}

	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChildNode[i].SetInterpolatedTransform_r( frac, node0.m_vecChildNode[i], node1.m_vecChildNode[i] );
	}
}


void CTransformNode::SetInterpolatedTransform_r( float frac,
	const CTransformNode& node0,
	const CTransformNode& node1,
	const CTransformNode& node2,
	const CTransformNode& node3
	)
{
//	SetInterpolatedTransform_r( frac, node1, node2 );
//	return;

	Transform local_transform = InterpolateWithTCBSpline(
		frac,
		Transform( node0.GetLocalTransform() ),
		Transform( node1.GetLocalTransform() ),
		Transform( node2.GetLocalTransform() ),
		Transform( node3.GetLocalTransform() )
		);

	SetTransform( local_transform );

	const Matrix33 mat = m_Rotation.ToRotationMatrix(); // check the rotation for debugging

	const size_t num_children = node0.m_vecChildNode.size();

	if( m_vecChildNode.size() != num_children )
	{
		m_vecChildNode.resize( num_children );
	}

	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChildNode[i].SetInterpolatedTransform_r( frac,
			node0.m_vecChildNode[i],
			node1.m_vecChildNode[i],
			node2.m_vecChildNode[i],
			node3.m_vecChildNode[i]
			);
	}
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
