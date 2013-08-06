#ifndef __TransformCacheTree_HPP__
#define __TransformCacheTree_HPP__


#include <set>
#include <string>
#include "amorphous/3DMath/Matrix34.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "fwd.hpp"


namespace amorphous
{


namespace msynth
{


class TransformCacheNode
{
	std::string m_Name;

	Matrix34 m_WorldTransform;

//	std::vector<TransformCacheNode> children;
	TransformCacheNode *m_pChildren;

	int m_NumChildren;

	// true: calculate the world transform for this node and store the result to m_WorldTransform
	// false: do not calculate the world transform for this and all the child nodes
	bool m_Active;

private:

	bool AreAllChildrenInactive() const;

	bool AreAllChildrenLeafAndInactive() const;

public:

	TransformCacheNode()
		:
	m_pChildren(NULL),
	m_NumChildren(0),
	m_WorldTransform( Matrix34Identity() ),
	m_Active(true)
	{}

	~TransformCacheNode()
	{
		ReleaseChildren();
	}

	void ReleaseChildren()
	{
		SafeDeleteArray( m_pChildren );
		m_NumChildren = 0;
	}

	TransformCacheNode *GetNode( const std::string& name );

	const TransformCacheNode *GetNode( const std::string& name ) const;

//	void CreateTransformCacheNodeTree_r( const Bone& bone, const std::set<std::string>& target_bone_names );

	void CreateTransformCacheNodeTree_r( const Bone& bone );

	bool UpdateActiveNodes( const std::set<std::string>& target_bone_names );

	void RemoveInactiveNodes( const std::set<std::string>& target_bone_names );

	void CalculateWorldTransforms( const Matrix34& parent_transform, const Bone& bone, const TransformNode& transform_node );

	void GetWorldTransform( Matrix34& dest ) const { dest = m_WorldTransform; }

	const Matrix34& GetWorldTransform() const { return m_WorldTransform; }
};



// not copiable
class TransformCacheTree
{
	TransformCacheNode *m_pRootNode;

public:

	TransformCacheTree()
		:
	m_pRootNode( new TransformCacheNode() )
	{}

	~TransformCacheTree()
	{
		Release();
		SafeDelete( m_pRootNode );
	}

	void Release()
	{
		m_pRootNode->ReleaseChildren();
	}
	
	void Create( const Skeleton& skeleton );

/*	void Create( Skeleton& skeleton, const std::set<std::string>& target_bone_names );

	void Create( Skeleton& skeleton, const std::vector<std::string>& target_bone_names )
	{
		std::set<std::string> names;
		names.insert( target_bone_names.begin(), target_bone_names.end() );
		Create( skeleton, names );
	}*/

	void UpdateActiveNodes( const std::set<std::string>& target_bone_names );

	void RemoveInactiveNodes( const std::set<std::string>& target_bone_names );

	TransformCacheNode *GetNode( const std::string& name ) { return m_pRootNode->GetNode( name ); }

	const TransformCacheNode *GetNode( const std::string& name ) const { return m_pRootNode->GetNode( name ); }

	void CalculateWorldTransforms( const Skeleton& skeleton, const Keyframe& keyframe, const Matrix34& parent_transform = Matrix34Identity() );
};


} // namespace msynth

} // namespace amorphous



#endif /* __TransformCacheTree_HPP__ */
