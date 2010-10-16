#ifndef __TransformCacheTree_HPP__
#define __TransformCacheTree_HPP__


#include <set>
#include <string>
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Support/memory_helpers.hpp"
#include "fwd.hpp"


namespace msynth
{


class CTransformCacheNode
{
	std::string m_Name;

	Matrix34 m_WorldTransform;

//	std::vector<CTransformCacheNode> children;
	CTransformCacheNode *m_pChildren;

	int m_NumChildren;

	// true: calculate the world transform for this node and store the result to m_WorldTransform
	// false: do not calculate the world transform for this and all the child nodes
	bool m_Active;

private:

	bool AreAllChildrenInactive() const;

	bool AreAllChildrenLeafAndInactive() const;

public:

	CTransformCacheNode()
		:
	m_pChildren(NULL),
	m_NumChildren(0),
	m_WorldTransform( Matrix34Identity() ),
	m_Active(true)
	{}

	~CTransformCacheNode()
	{
		ReleaseChildren();
	}

	void ReleaseChildren()
	{
		SafeDeleteArray( m_pChildren );
		m_NumChildren = 0;
	}

	CTransformCacheNode *GetNode( const std::string& name );

	const CTransformCacheNode *GetNode( const std::string& name ) const;

//	void CreateTransformCacheNodeTree_r( const CBone& bone, const std::set<std::string>& target_bone_names );

	void CreateTransformCacheNodeTree_r( const CBone& bone );

	bool UpdateActiveNodes( const std::set<std::string>& target_bone_names );

	void RemoveInactiveNodes( const std::set<std::string>& target_bone_names );

	void CalculateWorldTransforms( const Matrix34& parent_transform, const CBone& bone, const CTransformNode& transform_node );

	void GetWorldTransform( Matrix34& dest ) const { dest = m_WorldTransform; }

	const Matrix34& GetWorldTransform() const { return m_WorldTransform; }
};



// not copiable
class CTransformCacheTree
{
	CTransformCacheNode *m_pRootNode;

public:

	CTransformCacheTree()
		:
	m_pRootNode( new CTransformCacheNode() )
	{}

	~CTransformCacheTree()
	{
		Release();
		SafeDelete( m_pRootNode );
	}

	void Release()
	{
		m_pRootNode->ReleaseChildren();
	}
	
	void Create( const CSkeleton& skeleton );

/*	void Create( CSkeleton& skeleton, const std::set<std::string>& target_bone_names );

	void Create( CSkeleton& skeleton, const std::vector<std::string>& target_bone_names )
	{
		std::set<std::string> names;
		names.insert( target_bone_names.begin(), target_bone_names.end() );
		Create( skeleton, names );
	}*/

	void UpdateActiveNodes( const std::set<std::string>& target_bone_names );

	void RemoveInactiveNodes( const std::set<std::string>& target_bone_names );

	CTransformCacheNode *GetNode( const std::string& name ) { return m_pRootNode->GetNode( name ); }

	const CTransformCacheNode *GetNode( const std::string& name ) const { return m_pRootNode->GetNode( name ); }

	void CalculateWorldTransforms( const CSkeleton& skeleton, const CKeyframe& keyframe, const Matrix34& parent_transform = Matrix34Identity() );
};


} // namespace msynth


#endif /* __TransformCacheTree_HPP__ */
