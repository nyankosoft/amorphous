#ifndef __BSPTreeForTriangleMesh__
#define __BSPTreeForTriangleMesh__

#include <vector>

#include "AABTree.h"
#include "IndexedTriangle.h"


class CBSPTreeForTriangleMesh : public CLeafyAABTree<CIndexedTriangle>
{
	friend CTriangleMesh;

	/// binary tree
	/// - Holds an array of indexed triangles
//	CLeafyAABTree<CIndexedTriangle> m_AABTree;

	// 9/27/2007 - taken from CTriangleMesh
	// vertices of the triangle
	std::vector<Vector3> m_vecVertex;

	// 9/27/2007 - taken from CTriangleMesh
	// 4/20/2008 - moved to CLeafyAABTree<CIndexedTriangle>
//	std::vector<CIndexedTriangle> m_vecIndexedTriangle;

public:

	CBSPTreeForTriangleMesh();

	void Release();

	bool CreateMesh( std::vector<Vector3>& rvecVertex, std::vector<int>& rveciIndex, std::vector<short>& rvecsMatIndex );

	bool Build( std::vector<Vector3>& rvecVertex, std::vector<CIndexedTriangle>& rvecIndexedTriangle );

//	void SetRecursionStopCondition( const string& cond );

	/// get a list of triangles whose aabb intersect with 'aabb'
	inline void GetIntersectingTriangles( const AABB3& aabb, std::vector<int>& rveciTriList );

	inline virtual void Serialize( IArchive& ar, const unsigned int version );
};


//===============================================================================================
// inline implementations
//===============================================================================================


inline void CBSPTreeForTriangleMesh::Serialize( IArchive& ar, const unsigned int version )
{
//	ar & m_AABTree;
	CLeafyAABTree<CIndexedTriangle>::Serialize( ar, version );

	ar & m_vecVertex;
//	ar & m_vecIndexedTriangle; // stored in m_AABTree
}


inline void CBSPTreeForTriangleMesh::GetIntersectingTriangles( const AABB3& aabb, std::vector<int>& rveciTriList )
{
	CLeafyAABTree<CIndexedTriangle>::GetIntersectingAABBs( aabb, rveciTriList );
}


#endif /* __BSPTreeForTriangleMesh__ */
