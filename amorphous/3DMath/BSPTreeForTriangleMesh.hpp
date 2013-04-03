#ifndef __BSPTreeForTriangleMesh__
#define __BSPTreeForTriangleMesh__

#include <vector>

#include "AABTree.hpp"
#include "IndexedTriangle.hpp"


namespace amorphous
{


class BSPTreeForTriangleMesh : public LeafyAABTree<IndexedTriangle>
{
	friend CTriangleMesh;

	/// binary tree
	/// - Holds an array of indexed triangles
//	LeafyAABTree<IndexedTriangle> m_AABTree;

	// 9/27/2007 - taken from CTriangleMesh
	// vertices of the triangle
	std::vector<Vector3> m_vecVertex;

	// 9/27/2007 - taken from CTriangleMesh
	// 4/20/2008 - moved to LeafyAABTree<IndexedTriangle>
//	std::vector<IndexedTriangle> m_vecIndexedTriangle;

public:

	BSPTreeForTriangleMesh();

	void Release();

	bool CreateMesh( std::vector<Vector3>& rvecVertex, std::vector<int>& rveciIndex, std::vector<short>& rvecsMatIndex );

	bool Build( std::vector<Vector3>& rvecVertex, std::vector<IndexedTriangle>& rvecIndexedTriangle );

//	void SetRecursionStopCondition( const string& cond );

	/// get a list of triangles whose aabb intersect with 'aabb'
	inline void GetIntersectingTriangles( const AABB3& aabb, std::vector<int>& rveciTriList );

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


//===============================================================================================
// inline implementations
//===============================================================================================


inline void BSPTreeForTriangleMesh::Serialize( IArchive& ar, const unsigned int version )
{
//	ar & m_AABTree;
	LeafyAABTree<IndexedTriangle>::Serialize( ar, version );

	ar & m_vecVertex;
//	ar & m_vecIndexedTriangle; // stored in m_AABTree
}


inline void BSPTreeForTriangleMesh::GetIntersectingTriangles( const AABB3& aabb, std::vector<int>& rveciTriList )
{
	LeafyAABTree<IndexedTriangle>::GetIntersectingAABBs( aabb, rveciTriList );
}

} // namespace amorphous



#endif /* __BSPTreeForTriangleMesh__ */
