#include "TransformCacheTree.hpp"
#include "Skeleton.hpp"
#include "MotionPrimitive.hpp"


namespace amorphous
{


using namespace std;


namespace msynth
{


//=====================================================================
// CTransformCacheNode
//=====================================================================

CTransformCacheNode *CTransformCacheNode::GetNode( const std::string& name )
{
	if( m_Name == name )
		return this;

	for( int i=0; i<m_NumChildren; i++ )
	{
		CTransformCacheNode *pNode = m_pChildren[i].GetNode( name );
		if( pNode )
			return pNode;
	}

	return NULL;
}


const CTransformCacheNode *CTransformCacheNode::GetNode( const std::string& name ) const
{
	if( m_Name == name )
		return this;

	for( int i=0; i<m_NumChildren; i++ )
	{
		const CTransformCacheNode *pNode = m_pChildren[i].GetNode( name );
		if( pNode )
			return pNode;
	}

	return NULL;
}


void CTransformCacheNode::CreateTransformCacheNodeTree_r( const CBone& bone )
{
	ReleaseChildren();

	m_Name = bone.GetName();

	// found the name in the list
	m_Active = true;

	m_NumChildren = bone.GetNumChildren();

	if( m_NumChildren == 0 )
		return;

	m_pChildren = new CTransformCacheNode [m_NumChildren];
//	children.resize( bone.GetNumChildren() );

	for( int i=0; i<m_NumChildren; i++ )
	{
		m_pChildren[i].CreateTransformCacheNodeTree_r( bone.GetChild(i) );
	}
}


bool CTransformCacheNode::AreAllChildrenInactive() const
{
	if( m_NumChildren == 0 && !m_Active )
		return true;
	else
	{
		for( int i=0; i<m_NumChildren; i++ )
		{
			bool res = AreAllChildrenInactive();
			if( !res )
				return false;
		}

		return true;
	}
}


bool CTransformCacheNode::AreAllChildrenLeafAndInactive() const
{
	for( int i=0; i<m_NumChildren; i++ )
	{
		if( m_pChildren[i].m_NumChildren && m_pChildren[i].m_Active )
			continue;
		else
			return false;
	}

	return true;
}


/// Does not deactivate already active nodes
bool CTransformCacheNode::UpdateActiveNodes( const std::set<std::string>& target_bone_names )
{
//	m_Active = true;

	for( int i=0; i<m_NumChildren; i++ )
	{
		bool are_children_active = m_pChildren[i].UpdateActiveNodes( target_bone_names );

		// Return true if at least one of the child nodes is active
		if( are_children_active )
		{
			if( !m_Active )
				m_Active = true;

			// Cannot return here. need to check all the child nodes for all the bones in target_bone_names
		}
	}

	if( m_Active )
		return true; // This node, or at least one of the children is active. 

	set<string>::const_iterator itr = target_bone_names.find( m_Name );

	if( !m_Active ) // Do not deactivate an already active node.
		m_Active = (itr != target_bone_names.end()) ? true : false;

	return m_Active;
}


void CTransformCacheNode::RemoveInactiveNodes( const std::set<std::string>& target_bone_names )
{
	for( int i=0; i<m_NumChildren; i++ )
	{
		m_pChildren[i].RemoveInactiveNodes( target_bone_names );
	}

	if( AreAllChildrenLeafAndInactive() )
	{
		ReleaseChildren();
	}

	if( m_NumChildren == 0 )
	{
		set<string>::const_iterator itr = target_bone_names.find( m_Name );
		if( itr == target_bone_names.end() )
			m_Active = false;
	}
}


void CTransformCacheNode::CalculateWorldTransforms( const Matrix34& parent_transform, const CBone& bone, const CTransformNode& transform_node )
{
//	if( !cache_node.m_Active )
//		return;

	bone.CalculateWorldTransform( m_WorldTransform, parent_transform, transform_node );

	const int num_children = take_min( m_NumChildren, bone.GetNumChildren(), transform_node.GetNumChildren() );
	for( int i=0; i<num_children; i++ )
	{
//		if( m_pChildren[i].m_Active )
			m_pChildren[i].CalculateWorldTransforms( m_WorldTransform, bone.GetChild(i), transform_node.GetChildNode(i) );
	}
}



//=====================================================================
// CTransformCacheTree
//=====================================================================

void CTransformCacheTree::UpdateActiveNodes( const std::set<std::string>& target_bone_names )
{
	m_pRootNode->UpdateActiveNodes( target_bone_names );
}


void CTransformCacheTree::RemoveInactiveNodes( const std::set<std::string>& target_bone_names )
{
	m_pRootNode->RemoveInactiveNodes( target_bone_names );
}


void CTransformCacheTree::Create( const CSkeleton& skeleton )
{
	m_pRootNode->CreateTransformCacheNodeTree_r( skeleton.GetRootBone() );
}


void CTransformCacheTree::CalculateWorldTransforms( const CSkeleton& skeleton, const CKeyframe& keyframe, const Matrix34& parent_transform )
{
	m_pRootNode->CalculateWorldTransforms( parent_transform, skeleton.GetRootBone(), keyframe.GetRootNode() );
}


} // namespace msynth

/*
void CTransformCacheNode::CreateTransformCacheNodeTree_r( const CBone& bone, const std::set<std::string>& target_bone_names )
{
	set<string>::const_iterator itr = target_bone_names.find( bone.GetName() );
	if( target_bone_names.end() == itr )
		return;

	m_Name = bone.GetName();

	// found the name in the list
	m_Active = true;

	m_NumChildren = bone.GetNumChildren();

	if( m_NumChildren == 0 )
		return;

	m_pChildren = new CTransformCacheNode [m_NumChildren];
//	children.resize( bone.GetNumChildren() );

	for( int i=0; i<m_NumChildren; i++ )
	{
		m_pChildren[i].CreateTransformCacheNodeTree_r( bone.GetChild(i), target_bone_names );
	}
}
*/

/*
void CTransformCacheTree::Create( CSkeleton& skeleton, const std::set<std::string>& target_bone_names )
{
	m_pRootNode->CreateTransformCacheNodeTree_r( skeleton.GetRootBone(), target_bone_names );
}
*/


} // namespace amorphous
