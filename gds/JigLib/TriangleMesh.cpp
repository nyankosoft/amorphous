
#include "TriangleMesh.h"

#include "JL_PhysicsActor.h"
#include "JL_ShapeDesc_TriangleMesh.h"
#include "JL_LineSegment.h"

#include "Stage/trace.h"

#include "3DMath/Plane.h"

#include "Support/Serialization/BinaryDatabase.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Vec3_StringAux.h"

#include <assert.h>



//=================================================================================================================
// CTriangleMesh
//=================================================================================================================

void CTriangleMesh::Release()
{
	m_Tree.m_vecVertex.clear();
	m_Tree.Release();
}


bool CTriangleMesh::CreateMesh( vector<Vector3>& rvecVertex,
							    vector<int>& rveciIndex,		// vertex indices used by indexed triangles
								vector<short>& rvecsMatIndex )
{
	return m_Tree.CreateMesh( rvecVertex, rveciIndex, rvecsMatIndex );
}


void CTriangleMesh::ClipTrace( STrace& tr )
{
	static vector<int> s_veciTriList;
	s_veciTriList.resize( 0 );

	GetIntersectingTriangles( s_veciTriList, tr.aabb_swept );

	size_t i, iNumTris = s_veciTriList.size();

	if( iNumTris == 0 )
		return;	// no intersection

	Vector3 vStart = *tr.pvStart;
	Vector3 vEnd  = *tr.pvGoal;
//	Vector3 vEnd;	// store contact point
//	float frac = 1.0f;
	for( i=0; i<iNumTris; i++ )
	{
		CIndexedTriangle& tri = GetTriangle( s_veciTriList[i] );
		CTriangle triangle( GetVertex( tri.GetIndex(0) ),
			                GetVertex( tri.GetIndex(1) ),
							GetVertex( tri.GetIndex(2) ),
							tri.m_vNormal );

		// check intersection between ray and triangle
		if( triangle.RayIntersect( vStart, vEnd ) )
		{	// found intersection

			// save contact surface
			tr.plane = triangle.GetPlane();

			// record surface material to 'tr'
			tr.iMaterialIndex = tri.m_iMaterialID;
		}
	}

	Vector3 vOrigTrace = *tr.pvGoal - vStart;
	tr.fFraction = Vec3Dot( vEnd - vStart, vOrigTrace ) / Vec3LengthSq( vOrigTrace );
	tr.vEnd = vEnd;

}

	// clip trace at the point of contact
void CTriangleMesh::ClipLineSegment( CJL_LineSegment& segment )
{
	Vector3 vStart = segment.vStart;
	Vector3 vEnd;
	if( segment.fFraction == 1.0f )
		vEnd = segment.vGoal;
	else
		vEnd = segment.vEnd;	// the line segment has been already clipped - set the current end point

	static vector<int> s_veciTriList;
	s_veciTriList.resize( 0 );

	AABB3 aabb;
	aabb.Nullify();
	aabb.AddPoint( vStart );
	aabb.AddPoint( vEnd );

	GetIntersectingTriangles( s_veciTriList, aabb );

	size_t i, iNumTris = s_veciTriList.size();

	if( iNumTris == 0 )
		return;	// no intersection

	Vector3 vOrigLineSegment = vEnd - vStart;

	for( i=0; i<iNumTris; i++ )
	{
		CIndexedTriangle& tri = GetTriangle( s_veciTriList[i] );
		CTriangle triangle( GetVertex( tri.GetIndex(0) ),
			                GetVertex( tri.GetIndex(1) ),
							GetVertex( tri.GetIndex(2) ),
							tri.m_vNormal );

		// check intersection between ray and triangle
		if( triangle.RayIntersect( vStart, vEnd ) )
		{	// found intersection
			// 'vEnd' now represents the intersection point

			// save contact surface
			segment.plane = triangle.GetPlane();

			// record surface material to 'tr'
			segment.iMaterialIndex = tri.m_iMaterialID;
		}
	}

	segment.fFraction *= Vec3Dot( vEnd - vStart, vOrigLineSegment ) / Vec3LengthSq( vOrigLineSegment );
	segment.vEnd = vEnd;
}


static int g_iNumTotalEdges = 0;
static int g_iNumConvexEdges = 0;
static int g_iNumInteriorEdges = 0;
static int g_iNumConcaveEdges = 0;


CTriangleMesh::EdgeState CTriangleMesh::GetEdgeState( SEdgeRecord& edge0, SEdgeRecord& edge1 )
{
	vector<CIndexedTriangle>& rvecIndexedTriangle = m_Tree.GetGeometryBuffer();
	CIndexedTriangle& tri0 = rvecIndexedTriangle[ edge0.iTriangle ];
	CIndexedTriangle& tri1 = rvecIndexedTriangle[ edge1.iTriangle ];

	if( tri0.GetNormal() == tri1.GetNormal() )
		return INTERIOR;

	SPlane plane0;
	plane0.normal = tri0.GetNormal();
	plane0.dist = Vec3Dot( plane0.normal, m_Tree.m_vecVertex[ tri0.m_aiVertexIndex[0] ] );

	int i;
	Vector3 v;
	Scalar d;
	for( i=0; i<3; i++ )
	{
		Vector3 v = m_Tree.m_vecVertex[ tri1.m_aiVertexIndex[i] ];
		d = plane0.GetDistanceFromPoint( v );

		if( d < -0.0001 )
			return CONVEX;
		else if( 0.0001 < d )
			return CONCAVE;
		else
			continue;
	}

	return INTERIOR;
}


void CTriangleMesh::SetCollisionFlags()
{
//	vector<CIndexedTriangle>& rvecIndexedTriangle = m_Tree.m_vecIndexedTriangle;
	vector<CIndexedTriangle>& rvecIndexedTriangle = m_Tree.GetGeometryBuffer();
	const size_t iNumTriangles = rvecIndexedTriangle.size();

	vector< vector<SEdgeRecord> > vecSortedEdgeRecord;
	vector<SEdgeRecord> vecEdgeRecord;
	vecSortedEdgeRecord.reserve( iNumTriangles * 3 );
	SEdgeRecord edge;

	g_iNumTotalEdges = (int)iNumTriangles * 3;

	size_t i, j, k;
	size_t iNumEdgeRecordSets = 0;

	for( i=0; i<iNumTriangles; i++ )
	{
		CIndexedTriangle& triangle = rvecIndexedTriangle[i];
		for( j=0; j<3; j++ )
		{	// store edge record for each edge
			edge.iTriangle = (int)i;
			edge.iEdge = (int)j;
			edge.iVertex[0] = triangle.m_aiVertexIndex[j];
			edge.iVertex[1] = triangle.m_aiVertexIndex[(j+1)%3];

			iNumEdgeRecordSets = vecSortedEdgeRecord.size();
			for( k=0; k<iNumEdgeRecordSets; k++ )
			{
				SEdgeRecord& edge0 = vecSortedEdgeRecord[k][0];
				if( edge.iVertex[0] == edge0.iVertex[0] && edge.iVertex[1] == edge0.iVertex[1] ||
					edge.iVertex[0] == edge0.iVertex[1] && edge.iVertex[1] == edge0.iVertex[0] )
				{
					vecSortedEdgeRecord[k].push_back( edge );	// found a same edge
					break;
				}
			}

			if( k == iNumEdgeRecordSets )
			{
				// a same edge was not found - register as a new edge
				vecSortedEdgeRecord.push_back( vecEdgeRecord );
				vecSortedEdgeRecord.back().reserve( 2 );
				vecSortedEdgeRecord.back().push_back( edge );
			}
		}
	}

	// check the shared edges stored in 'vecSortedEdgeRecord'
	iNumEdgeRecordSets = vecSortedEdgeRecord.size();
	for( i=0; i<iNumEdgeRecordSets; i++ )
	{
		// prepare references to the first edge and its owner triangle
		SEdgeRecord& edge0 = vecSortedEdgeRecord[i][0];
		CIndexedTriangle& tri = rvecIndexedTriangle[ edge0.iTriangle ];

		if( vecSortedEdgeRecord[i].size() == 1 || 3 <= vecSortedEdgeRecord[i].size() )
		{
			// a boundary edge or a nonmanifold edge - treat as convex
			tri.m_bEdgeCollision[edge0.iEdge] = true;
			tri.m_bVertexCollision[edge0.iEdge] = true;
			tri.m_bVertexCollision[(edge0.iEdge + 1) % 3] = true;
		}
		else // i.e. the edge is shared by 2 triangles
		{
			// check the relation between these two edges
			SEdgeRecord& edge1 = vecSortedEdgeRecord[i][1];
			EdgeState state = GetEdgeState( edge0, edge1 );
			switch( state )
			{
			case CONVEX:
//				g_iNumConvexEdges++;
				// only one of the 2 shared edges needs to be checked for collision
				tri.m_bEdgeCollision[edge0.iEdge] = true;
				tri.m_bVertexCollision[edge0.iEdge] = true;
				tri.m_bVertexCollision[(edge0.iEdge + 1) % 3] = true;
				break;
			case CONCAVE:
//				g_iNumConcaveEdges++;
				// mark as concave and take no further action at this point
				edge0.bIsConcave = true;
				break;
			case INTERIOR:
//				g_iNumInteriorEdges++;
				break;
			}
		}
	}

	// find concave edge sets and clear flags of their vertices
	for( i=0; i<iNumEdgeRecordSets; i++ )
	{
		if( vecSortedEdgeRecord[i][0].bIsConcave )
		{
			for( j=0; j<2; j++ )
			{
				SEdgeRecord& edge = vecSortedEdgeRecord[i][j];
				CIndexedTriangle& tri = rvecIndexedTriangle[edge.iTriangle];
				tri.m_bVertexCollision[edge.iEdge] = false;
				tri.m_bVertexCollision[(edge.iEdge+1) % 3] = false;
			}
		}
	}

}


void CTriangleMesh::InitSpecific( CJL_ShapeDesc& rShapeDesc )
{
	CJL_ShapeDesc_TriangleMesh *pMeshDesc = (CJL_ShapeDesc_TriangleMesh *)&rShapeDesc;

	if( !pMeshDesc->pvecvVertex || !pMeshDesc->pveciIndex )
	{
		LOG_PRINT( " - mesh indices/vertices are not found. loading the mesh from the file: " + 
			pMeshDesc->BSPTreeTriangleMeshArchiveFilename );

		string src_filename = pMeshDesc->BSPTreeTriangleMeshArchiveFilename;
		size_t pos = pMeshDesc->BSPTreeTriangleMeshArchiveFilename.find( "::" );
		bool loaded;
		if( pos != string::npos )
		{
			// the filename includes '::'
			// - This is a binary database file
			// - filename = "(db filename)::(archive name)"
			CBinaryDatabase<string> db;
			string db_filename = src_filename.substr( 0, pos );
			loaded = db.Open( db_filename );
			if( !loaded )
			{
				LOG_PRINT_ERROR( " - cannot open binary database file: " + db_filename );
				return;
			}

			bool res = db.GetData( src_filename.substr( pos + 2 ), m_Tree );
			if( !res )
			{
				LOG_PRINT_ERROR( " - failed to retrieve mesh data from database: " + db_filename );
			}
		}
		else
		{
			// single archive file
			m_Tree.LoadFromFile( pMeshDesc->BSPTreeTriangleMeshArchiveFilename );
		}

		if( m_Tree.GetNumNodes() == 0 )
		{
			LOG_PRINT_WARNING( " - invalid mesh tree" );
			return;
		}

		// set world aabb
		m_WorldAABB = m_Tree.GetNode(0).aabb;
		return;
	}

	// create triangle mesh from an array of vertices and indices

	m_Tree.Release();
	m_Tree.SetNumMaxTrianglesPerCell( pMeshDesc->NumMaxTrianglesPerCell );
	m_Tree.SetMinimumCellVolume( pMeshDesc->MinimumCellVolume );
	m_Tree.SetRecursionStopCondition( pMeshDesc->RecursionStopCond );

	// create triangle mesh from the vertices and the indexed triangles
	if( pMeshDesc->pvecsMaterialIndex )
		CreateMesh( *pMeshDesc->pvecvVertex, *pMeshDesc->pveciIndex, *pMeshDesc->pvecsMaterialIndex );
	else
	{
		// set indices to the default material for all the triangles
		vector<short> vecMatIndex;
		vecMatIndex.resize( pMeshDesc->pveciIndex->size() / 3, 0 );
		CreateMesh( *pMeshDesc->pvecvVertex, *pMeshDesc->pveciIndex, vecMatIndex );
	}

	// mark edges that are involved in collision
	if( pMeshDesc->OptimizeEdgeCollision )
		SetCollisionFlags();

}


// only the static mesh is supported right now
void CTriangleMesh::UpdateWorldProperties()
{
	if( m_pPhysicsActor->GetActorFlag() & JL_ACTOR_STATIC )
	{
		return;
	}

	// write codes to update world properties here
}


CTriangleMesh CTriangleMesh::operator=( const CTriangleMesh& rTriMesh )
{
	// check behavior during debug
	m_Tree = rTriMesh.m_Tree;

	m_Tree.m_vecVertex.assign( rTriMesh.m_Tree.m_vecVertex.begin(), rTriMesh.m_Tree.m_vecVertex.end() );
//	m_Tree.m_vecIndexedTriangle.assign( rTriMesh.m_Tree.m_vecIndexedTriangle.begin(), rTriMesh.m_Tree.m_vecIndexedTriangle.end() );

	return *this;
}