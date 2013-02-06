#ifndef  __TRIANGLEMESH_H__
#define  __TRIANGLEMESH_H__


#include <vector>
#include "JL_ShapeBase.hpp"
#include "3DMath/Triangle.hpp"
#include "3DMath/IndexedTriangle.hpp"
#include "3DMath/BSPTreeForTriangleMesh.hpp"


namespace amorphous
{


struct STrace;


class CTriangleMesh : public CJL_ShapeBase
{
	BSPTreeForTriangleMesh m_Tree;

	// 9/27/2007 moved to BSPTreeForTriangleMesh
//	vector<Vector3> m_vecVertex;
//	vector<IndexedTriangle> m_vecIndexedTriangle;

	std::vector<int> veciNodeToCheck;	// used at runtime


	// used to set edge/vertex flags
	struct SEdgeRecord
	{
		int iEdge;		// index to an edge of a triangle (0, 1 or 2)
		int iVertex[2];	// indices to the vertices sharing the edge
		int iTriangle;	// index to the owner triangle
		bool bIsConcave;

		inline SEdgeRecord();
	};

	enum EdgeState { CONVEX, CONCAVE, INTERIOR };

	EdgeState GetEdgeState( SEdgeRecord& edge0, SEdgeRecord& edge1 );


public:

	void Release();

	/// returns i-th vertex ( should this be private? )
	inline Vector3& GetVertex( int i ) { return m_Tree.m_vecVertex[i]; }

	/// returns the i-th indexed triangle
	inline IndexedTriangle& GetTriangle( int i ) { return  m_Tree.GetGeometryBuffer()[i]; }

	// returns a general geometry triangle that corresponds to the i-th indexed triangle
//	inline void GetGeneralTriangle( Triangle& dest_triangle, int i );

	// 08:32 2007/09/28 most of the routines were moved to BSPTreeForTriangleMesh
	bool CreateMesh( std::vector<Vector3>& rvecVertex, std::vector<int>& rveciIndex, std::vector<short>& rvecsMatIndex );

	inline int GetIntersectingTriangles( std::vector<int>& rveciTriList, AABB3& raabb );	// get a list of triangles whose aabb intersect with 'raabb'

	/// set flags to edges and vertices to indicate which of them should be checked for collision
	/// takes some time to calculate when the mesh has many triangles
	void SetCollisionFlags();

	/// clip trace at the point of contact
	void ClipTrace( STrace& tr );

	/// clip trace at the point of contact
	void ClipLineSegment( CJL_LineSegment& segment );

	void InitSpecific( CJL_ShapeDesc& rShapeDesc );

	void UpdateWorldProperties();

	virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & m_Tree; }

	CTriangleMesh operator=( const CTriangleMesh& rTriMesh);
};


struct STriangleMeshHeader
{
	int iNumVertices;
	int iNumTriangles;
};




//===============================================================================================
// inline implementations
//===============================================================================================


// inline void CTriangleMesh::GetGeneralTriangle( Triangle& dest_triangle, int i ) {}


inline int CTriangleMesh::GetIntersectingTriangles( std::vector<int>& rveciTriList, AABB3& raabb )
{
	m_Tree.GetIntersectingTriangles( raabb, rveciTriList );

	return 0;
}


inline CTriangleMesh::SEdgeRecord::SEdgeRecord()
{
	iEdge = 0;
	iVertex[0] = iVertex[1] = 0;
	iTriangle = 0;
	bIsConcave = false;
}

} // namespace amorphous



#endif		/*  __TRIANGLEMESH_H__  */
