#include "BSPTreeForTriangleMesh.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Macro.h"


//=================================================================================================================
// CBSPTreeForTriangleMesh
//=================================================================================================================

CBSPTreeForTriangleMesh::CBSPTreeForTriangleMesh()
{
}


void CBSPTreeForTriangleMesh::Release()
{
	CLeafyAABTree<CIndexedTriangle>::Release();

	m_vecVertex.resize( 0 );
}

/*
inline void RecordNode( const STMBinNode& node, const char* text )
{
	const static char s_Axis[3] = { 'x', 'y', 'z' };
	g_Log.Print( "%s: (tris: %d / %s / volume: %f / axis: %c / depth: %d / children %d, %d )",
		text,
		node.veciTriangleIndex.size(),
		to_string(node.aabb).c_str(),
		node.aabb.GetVolume(),
		s_Axis[node.iAxis],
		node.depth,
		node.child[0], node.child[1] );
}
*/

/*
inline int GetSplitPlaneAxis( const AABB3& aabb )
{
	Vector3 vLen = aabb.vMax - aabb.vMin;
	if( vLen.x < vLen.y )
	{
		if( vLen.y < vLen.z )
			return 2;//vLen.z;
		else
			return 1;//vLen.y;
	}
	else
	{
		if( vLen.z < vLen.x )
			return 0;//vLen.x;
		else
			return 2;//vLen.z;
	}
}
*/

struct LeafNodeStatistics
{
	int NumLeafNodes;
	int NumTotalTriangles;
	int NumTotalDepth;

	float fAveTrianglesPerLeafNode;
	float fAveDepthPerNode;
};


bool CBSPTreeForTriangleMesh::Build( vector<Vector3>& rvecVertex, vector<CIndexedTriangle>& rvecIndexedTriangle )
{
//	m_AABTree.Build( rvecIndexedTriangle );
//	CLeafyAABTree<CIndexedTriangle>::Build( rvecIndexedTriangle );
	CLeafyAABTree<CIndexedTriangle>::Build();

	return true;
}


/**
 create a triangle mesh object from vertices and indices
 - indices must be supplied in a	 single integer array
 - every three integers are interpreted as a set of indices to a triangle
 - vertices and indices are copied to member variables, so the user is responsible for releasing the original data
 */
bool CBSPTreeForTriangleMesh::CreateMesh( vector<Vector3>& rvecVertex,
							              vector<int>& rveciIndex,		// vertex indices used by indexed triangles
										  vector<short>& rvecsMatIndex )
{
	Release();

	assert( (rveciIndex.size() % 3) == 0 );

	size_t i,j;
	size_t iNumVertices = rvecVertex.size();
	size_t iNumTriangles = rveciIndex.size() / 3;
	Vector3 v[3];

//	AABB3 world_aabb;

	// copy the vertices
	m_vecVertex.clear();
	m_vecVertex.resize( iNumVertices );
//	world_aabb.Nullify();
	for( i=0; i<iNumVertices; i++ )
	{
		m_vecVertex[i] = rvecVertex[i];
//		world_aabb.AddPoint( rvecVertex[i] );
	}

//	m_WorldAABB.vMin += Vector3(-1.0f,-1.0f,-1.0f);
//	m_WorldAABB.vMax += Vector3( 1.0f, 1.0f, 1.0f);

	// copy triangles and make aabb and normal for each triangle
	vector<CIndexedTriangle>& rvecIndexedTriangle = this->GetGeometryBuffer();
	rvecIndexedTriangle.reserve(iNumTriangles);
	j = 0;
	for( i=0; i<iNumTriangles; i++ )
	{
		rvecIndexedTriangle.push_back( CIndexedTriangle() );
		CIndexedTriangle& triangle = rvecIndexedTriangle.back();

		triangle.m_aiVertexIndex[0] = rveciIndex[i*3];
		triangle.m_aiVertexIndex[1] = rveciIndex[i*3+1];
		triangle.m_aiVertexIndex[2] = rveciIndex[i*3+2];

		// set material index
		// materials are stored in CJL_PhysicsManager object
		triangle.m_iMaterialID = rvecsMatIndex[i];

		v[0] = m_vecVertex[ triangle.GetIndex(0) ];
		v[1] = m_vecVertex[ triangle.GetIndex(1) ];
		v[2] = m_vecVertex[ triangle.GetIndex(2) ];

		triangle.m_aabb.Nullify();
		for(j=0; j<3; j++)
			triangle.m_aabb.AddPoint( v[j] );

		Vec3Cross( triangle.m_vNormal, (v[1]-v[0]), (v[2]-v[0]) );
		if( Vec3LengthSq( triangle.m_vNormal ) < 0.000000001f )
			triangle.m_vNormal = Vector3(0,0,0);
		else
			Vec3Normalize( triangle.m_vNormal, triangle.m_vNormal );
	}

	// set vertex / edge flags for collision
//	SetCollisionFlags();

	g_Log.Print( "building a bsp tree for a triangle mesh - %d vectices, %d triangles",
		m_vecVertex.size(),
		rvecIndexedTriangle.size() );

	// build bsp-tree
	Build( m_vecVertex, rvecIndexedTriangle );

	g_Log.Print( "built a triangle mesh with bsp tree - %d vectices, %d triangles, %d nodes",
		m_vecVertex.size(),
		rvecIndexedTriangle.size(),
		this->GetNumNodes() );

	return true;
}
