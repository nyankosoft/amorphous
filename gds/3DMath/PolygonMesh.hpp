#ifndef __POLYGONMESH_H__
#define __POLYGONMESH_H__


#include <vector>


namespace amorphous
{
using namespace std;


#include "3DMath/Vector3.hpp"
#include "3DMath/aabb3.hpp"
#include "3DMath/ray.hpp"
#include "3DMath/AABTree.hpp"


template <class TCPolygon>
class CBSPTreeForPolygonMesh;



//template<class T>
//class CPolygonMesh;

//==========================================================================================
// CBSPTreeForPolygonMesh
//==========================================================================================

template <class TCPolygon>
class CBSPTreeForPolygonMesh
{
public:

	vector<SPMBinNode> m_vecNode;

	int m_iNumMaxTrianglesPerCell;

	float m_fMinimumCellVolume;

	int m_iTestCounter;

public:

	CBSPTreeForPolygonMesh();

	void Release();

	bool Build( vector<TCPolygon>& rvecPolygon );

	void SetMinimumCellVolume( float fMinCellVolume ) { m_fMinimumCellVolume = fMinCellVolume; }

	void SetNumMaxTrianglesPerCell( int num_max_triangles ) { m_iNumMaxTrianglesPerCell = num_max_triangles; }

	inline size_t GetNumNodes() const { return m_vecNode.size(); }

	const CAABNode& GetNode( int index ) const { return m_vecNode[index]; }

	/// get a list of polygons whose aabb intersect with 'raabb'
//	int inline GetIntersectingPolygons( vector<int>& rveciPolygonList, AABB3& raabb );

//	friend class CPolygonMesh;

};



//==========================================================================================
// CPolygonMesh
//==========================================================================================

template <class TCPolygon>
class CPolygonMesh
{
	CBSPTreeForPolygonMesh<TCPolygon> m_Tree;

	vector<TCPolygon> m_vecPolygon;

	/// holds indices to nodes (used at runtime)
	vector<int> veciNodeToCheck;

	/// test conter for polygons (used at runtime)
	vector<int> m_veciTestCounter;

public:

	CPolygonMesh() {}

	~CPolygonMesh() { Release(); }

	bool CreateMesh( vector<TCPolygon>& rvecPolygon );

	void Release();

	/// access to polygons
	TCPolygon& GetPolygon( int i ) { return m_vecPolygon[i]; }

	int GetNumPolygons() { return m_vecPolygon.size(); }

	vector<TCPolygon>& GetPolygon() { return m_vecPolygon; }

	int GetIntersectingPolygons( vector<int>& rveciPolygonList, AABB3& aabb );
//	{ return m_Tree.GetIntersectingPolygons( rveciPolygonList, aabb ); }

	/// clip ray at the point of contact
	/// returns true if the ray hit the surface
	bool RayTrace( SRay& ray );

	void SetMinimumCellVolume( float fMinCellVolume ) { m_Tree.SetMinimumCellVolume( fMinCellVolume ); }

	void SetNumMaxTrianglesPerCell( int num_max_triangles ) { m_Tree.SetNumMaxTrianglesPerCell( num_max_triangles ); }

	inline size_t GetNumNodes() const { return m_Tree.GetNumNodes(); }

	const SPMBinNode& GetNode( int index ) const { return m_Tree.GetNode( index ); }

};



//================================= inline implementations =================================


//==========================================================================================
// CBSPTreeForPolygonMesh
//==========================================================================================

template <class TCPolygon>
CBSPTreeForPolygonMesh<TCPolygon>::CBSPTreeForPolygonMesh()
{
	m_iNumMaxTrianglesPerCell = 8;

	m_fMinimumCellVolume = 8.0f;

	m_iTestCounter = 0;
}


template <class TCPolygon>
void CBSPTreeForPolygonMesh<TCPolygon>::Release()
{
	m_vecNode.clear();
}


template <class TCPolygon>
bool CBSPTreeForPolygonMesh<TCPolygon>::Build( vector<TCPolygon>& rvecPolygon )
{
	Release();

	int i,j;
	int iNumPolygons = rvecPolygon.size();

	m_vecNode.reserve(256);

	// set up the root node
	m_vecNode.push_back( SPMBinNode() );

	// create aabb for the root node
	for(i=0; i<iNumPolygons; i++)
	{
		m_vecNode[0].aabb.MergeAABB( rvecPolygon[i].GetAABB() );
	}

	// set plane for the root node
	int iFirstAxis = 0;
	m_vecNode[0].iAxis = iFirstAxis;
	m_vecNode[0].fDist = (m_vecNode[0].aabb.vMax[iFirstAxis] + m_vecNode[0].aabb.vMin[iFirstAxis]) / 2.0f;

	// node (push all the indices to the root)
	for(i=0; i<iNumPolygons; i++)
	{
		m_vecNode[0].veciPolygonIndex.push_back(i);
	}

	// stack of nodes we need to process
	vector<int> veciNodeToProcess;
	veciNodeToProcess.push_back(0);	// put the root node

	while( !veciNodeToProcess.empty() )
	{
		int iNodeIndex = veciNodeToProcess.back();
		veciNodeToProcess.pop_back();

		// split into 2 subspaces if
		// - there are more triangles in the current subspace than 'm_iNumMaxTrianglesPerCell'
		// and
		// - the volume of the current subspace cell is larger than 'm_fMinimumCellVolume'
		if( m_iNumMaxTrianglesPerCell < m_vecNode[iNodeIndex].veciPolygonIndex.size() &&
			m_fMinimumCellVolume < m_vecNode[iNodeIndex].aabb.GetVolume() )
		{	// need to split
			int iAxis = m_vecNode[iNodeIndex].iAxis;
			float fMidDist = ( m_vecNode[iNodeIndex].aabb.vMax[iAxis] + m_vecNode[iNodeIndex].aabb.vMin[iAxis] ) / 2.0f;
			for( i=0; i<2; i++ )
			{
				m_vecNode[iNodeIndex].child[i] = m_vecNode.size();
				veciNodeToProcess.push_back( m_vecNode.size() );	// add to the stack

				// add new node
				m_vecNode.push_back( SPMBinNode() );

				// axis of the plane (will be not used when this turns out to be a leaf node)
				m_vecNode.back().iAxis = (iAxis + 1) % 3;

				// set aabb which represents subspace of the child nodes
				m_vecNode.back().aabb = m_vecNode[iNodeIndex].aabb;
				if( i==0 )
					m_vecNode.back().aabb.vMin[iAxis] = fMidDist;
				else
					m_vecNode.back().aabb.vMax[iAxis] = fMidDist;

				m_vecNode.back().fDist = ( m_vecNode.back().aabb.vMin[m_vecNode.back().iAxis]
					                     + m_vecNode.back().aabb.vMax[m_vecNode.back().iAxis] ) / 2.0f;

				// hand all the triangles of 'm_vecNode[iNodeIndex]' to either of its children.
				// triangles which crosses the plane are handed to both of the children
				for( j=0; j<m_vecNode[iNodeIndex].veciPolygonIndex.size(); j++ )
				{
					TCPolygon& polygon = rvecPolygon[ m_vecNode[iNodeIndex].veciPolygonIndex[j] ];
					if( m_vecNode.back().aabb.IsIntersectingWith( polygon.GetAABB() ) )
						m_vecNode.back().veciPolygonIndex.push_back( m_vecNode[iNodeIndex].veciPolygonIndex[j] );
				}
			}

			// triangles are stored to child nodes - clear them from the parent
			m_vecNode[iNodeIndex].veciPolygonIndex.clear();
		}
	}

	return true;
}



//=================================================================================================================
// CPolygonMesh
//=================================================================================================================

template <class TCPolygon>
void CPolygonMesh<TCPolygon>::Release()
{
	m_vecPolygon.clear();
	m_Tree.Release();
}


// create a triangle mesh object from vertices and indices
// indices must be supplied in a single integer array
// every three integers are interpreted as a set of indices to a triangle
// vertices and indices are copied to member variables, so the user is responsible for releasing the original data
template <class TCPolygon>
bool CPolygonMesh<TCPolygon>::CreateMesh( vector<TCPolygon>& rvecPolygon )
{
	Release();

	int i, iNumPolygons = rvecPolygon.size();

	if( iNumPolygons == 0 )
		return false;

	// copy polygon
	m_vecPolygon.resize( iNumPolygons );
	for( i=0; i<iNumPolygons; i++ )
	{
		m_vecPolygon[i] = rvecPolygon[i];
	}

	m_veciTestCounter.resize( iNumPolygons, 0 );

	// build bsp-tree
	m_Tree.Release();
	m_Tree.Build( rvecPolygon );

	return true;
}


template <class TCPolygon>
bool CPolygonMesh<TCPolygon>::RayTrace( SRay& ray )
{
	static vector<int> s_veciPolygonList;
	s_veciPolygonList.resize( 0 );

	AABB3 aabb;
	aabb.Nullify();

	aabb.AddPoint( ray.vStart );
	aabb.AddPoint( ray.vGoal );

	GetIntersectingPolygons( s_veciPolygonList, aabb );

	int i, iNumPolygons = s_veciPolygonList.size();

	if( iNumPolygons == 0 )
		return false;	// no intersection

	// copy ray
	SRay local_ray = ray;
	bool hit = false;

	for( i=0; i<iNumPolygons; i++ )
	{
		TCPolygon& polygon = m_vecPolygon[ s_veciPolygonList[i] ];

		// check intersection between ray and triangle
		if( polygon.ClipTrace( local_ray ) )
		{
			// found intersection
			hit = true;

			// save contact surface
//			ray.plane = triangle.GetPlane();

			// record surface material to 'tr'
			local_ray.iSurfaceIndex = polygon.GetSurfaceIndex();

			local_ray.iPolygonIndex = s_veciPolygonList[i];

			// update ray
			local_ray.vGoal = local_ray.vEnd;
			local_ray.fFraction = 1.0f;

		}
	}

	if( !hit )
		return false;

	// update original ray
	Vector3 vOrigRay = ray.vGoal - ray.vStart;
	ray.fFraction = Vec3Dot(vOrigRay,local_ray.vEnd - local_ray.vStart) / Vec3LengthSq(vOrigRay);
	ray.vEnd = local_ray.vEnd;

	return true;
}


template <class TCPolygon>
int CPolygonMesh<TCPolygon>::GetIntersectingPolygons( vector<int>& rveciPolygonList, AABB3& aabb )
{
	int i, iPolygonIndex;

	veciNodeToCheck.resize(0);

	veciNodeToCheck.push_back(0);

	while( !veciNodeToCheck.empty() )
	{
		SPMBinNode& rNode =  m_Tree.m_vecNode[ veciNodeToCheck.back() ];
		veciNodeToCheck.pop_back();

		if( rNode.IsLeaf() )
		{	// check intersection with triangles in this cell
			for( i=0; i<rNode.veciPolygonIndex.size(); i++ )
			{
				iPolygonIndex = rNode.veciPolygonIndex[i];
				TCPolygon& polygon = m_vecPolygon[ iPolygonIndex ];

				if( m_Tree.m_iTestCounter == m_veciTestCounter[ iPolygonIndex ] )
					continue;	// already tested

//				polygon.m_iTestCounter = m_Tree.m_iTestCounter;
				m_veciTestCounter[ iPolygonIndex ] =  m_Tree.m_iTestCounter;
				if( aabb.IsIntersectingWith(polygon.GetAABB()) && polygon.IsCollidable() )
					rveciPolygonList.push_back( iPolygonIndex );
			}
		}
		else
		{
			if( aabb.vMin[rNode.iAxis] <= rNode.fDist )
			{	// 'raabb' is intersecting with the negative half-space
				veciNodeToCheck.push_back( rNode.child[1] );
			}

			if( rNode.fDist <= aabb.vMax[rNode.iAxis] )
			{	// 'raabb' is intersecting with the positive half-space
				veciNodeToCheck.push_back( rNode.child[0] );
			}
		}
	}

	// increment test counter
	m_Tree.m_iTestCounter++;

	return 0;
}


} // amorphous



#endif  /*  __POLYGONMESH_H__  */