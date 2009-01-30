
#include "IndexedPolygon.hpp"

using namespace std;

//vector<CGeneral3DVertex>* CIndexedPolygon::ms_pVertex;


bool CIndexedPolygon::IsOnTriangle( int iTriangleIndex, const Vector3& rvPosition ) const
{
	if( GetNumVertices() - 2 <= iTriangleIndex )
		return false;

	Vector3 vPrevCross = Vector3(0,0,0);
	Vector3 vCross;

	int indices[] = { 0, iTriangleIndex + 1, iTriangleIndex + 2 };
	for( int i=0; i<3; i++ )
	{
		Vector3 v0 = GetVertex(indices[i]).m_vPosition;
		Vector3 v1 = GetVertex(indices[(i+1)%3]).m_vPosition;

		Vec3Cross( vCross, (v1-v0), (rvPosition - v0) );
		
		if( Vec3Dot(vPrevCross,vCross) < -0.01f )
			return false;

		vPrevCross = vCross;
	}

	return true;
}


Vector3 CIndexedPolygon::GetInterpolatedNormal( const Vector3& rvPosition ) const
{
	int i;
	SPlane plane_12;
	Vector3 vDir12, vPoint12, vNormal12, vDestNormal;
	float f, d1, d2, dx, dp;

	// find triangle which includes 'rvPosition'
	for( i=0; i < GetNumVertices() - 2; i++ )
	{
		if( IsOnTriangle( i, rvPosition ) )
			break;
	}

	if( GetNumVertices() - 2 <= i )
		return GetPlane().normal;	// couldn't find a triangle which includes 'rvPosition'

	const CGeneral3DVertex& v0 = GetVertex(i);
	const CGeneral3DVertex& v1 = GetVertex(i+1);
	const CGeneral3DVertex& v2 = GetVertex(i+2);

	if( Vec3LengthSq( &(rvPosition - v0.m_vPosition) ) < 0.0001f )
		return v0.m_vNormal;	// 'rvPosition' is close to 'v0' - just use the normal of 'v0' as an approximation

	// make a plane which is perpendicular to the triangle and includes 'rvPosition'
	Vec3Normalize( vDir12, (rvPosition - v0.m_vPosition) );
	const Vector3& rvFaceNormal = GetPlane().normal;
	Vec3Cross( plane_12.normal, rvFaceNormal, vDir12 );
	Vec3Normalize( plane_12.normal, plane_12.normal);

	// calculate the normal at 'vPoint12' by interpolating the normals of v1 & v2
	plane_12.dist = Vec3Dot( plane_12.normal, v0.m_vPosition );
	d1 = Vec3Dot( plane_12.normal, v1.m_vPosition ) - plane_12.dist;
	d2 = Vec3Dot( plane_12.normal, v2.m_vPosition ) - plane_12.dist;
	f = - d1 / (d2 - d1);
	vPoint12  = v1.m_vPosition + (v2.m_vPosition - v1.m_vPosition) * f;
	vNormal12 = v1.m_vNormal   + (v2.m_vNormal   - v1.m_vNormal)   * f;

	// calculate the normal at 'rvPosition' by interpolating the normals of v0 & vPoint12
	dx = Vec3Dot( vDir12, (rvPosition - v0.m_vPosition) );
	dp = Vec3Dot( vDir12, (vPoint12   - v0.m_vPosition) );
	f = dx / dp;
	vDestNormal = v0.m_vNormal + (vNormal12 - v0.m_vNormal) * f;
	Vec3Normalize( vDestNormal, vDestNormal );
	return vDestNormal;
}



//
// Global Functions
//


inline void UnweldVertices( const CIndexedPolygon& polygon0, CIndexedPolygon& polygon1 )
{
	const size_t num_vertices0 = polygon0.m_index.size();
	for( size_t i=0; i<num_vertices0; i++ )
	{
		const size_t num_vertices1 = polygon1.m_index.size();
		for( size_t j=0; j<num_vertices1; j++ )
		{
			if( polygon0.m_index[i] == polygon1.m_index[j] )
			{
				// A vertex is shared by polygon0 and polygon1
				// - Unweld them

				// copy the shared vertex
				polygon1.VertexBuffer()->push_back( polygon0.GetVertex( i ) );

				// update the vertex index
				polygon1.m_index[j] = (int)polygon1.VertexBuffer()->size() - 1;
			}
		}
	}
}


/// Unweld vertices of polygons that are not on the same plane
/// - Vertex buffer must be shared by all the polygons
/// - When a shared is found for a pair of polygons
///   - Add vertices to vertex buffer
///   - Update the vertex index of the second polygon
void UnweldVerticesOfPolygonsOnDifferentPlanes( std::vector<CIndexedPolygon>& polygon_buffer )
{
	const size_t num_polygons = polygon_buffer.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		const CIndexedPolygon& polygon0 = polygon_buffer[i];
		for( size_t j=i+1; j<num_polygons; j++ )
		{
			CIndexedPolygon& polygon1 = polygon_buffer[j];

			if( !polygon0.GetAABB().IsIntersectingWith( polygon1.GetAABB() ) )
				continue;

			if( AreOnSamePlane( polygon0, polygon1 ) )
				continue;

			UnweldVertices( polygon0, polygon1 );
		}
	}
}


void UnweldVerticesBetween2GroupsOfPolygons( std::vector<CIndexedPolygon>& polygon_buffer,
											 const std::vector<int>& polygon_indices0,
											 const std::vector<int>& polygon_indices1 )
{
	const size_t num_polygons0 = polygon_indices0.size();
	const size_t num_polygons1 = polygon_indices1.size();
	for( size_t i=0; i<num_polygons0; i++ )
	{
		for( size_t j=0; j<num_polygons1; j++ )
		{
			UnweldVertices(
				polygon_buffer[polygon_indices0[i]],
				polygon_buffer[polygon_indices1[j]]
			);
		}
	}

}


/*
// check whether the given point is included in the volume
// which is defined by sweeping the face in the direction of the face normal
// rvPosition : [in]
// rfDist : [out]
bool CIndexedPolygon::IsInSweptVolume( const Vector3& rvPosition, float& rfDist )
{
	rfDist = -99999;
	int i;
	float d;
	Vector3 vEdge, vNormalOut;
	for( i=0; i<GetNumVertices() - 1; i++ )
	{
		vEdge = GetVertex(i+1) - GetVertex(i);
		Vec3Cross( vNormalOut, vEdge, GetPlane().normal );
		d = Vec3Dot( (rvPosition - GetVertex(i)), vNormalOut );

		if( rfDist < d )
			rfDist = d;
	}
	if( rfDist <= 0 )
		return true;
	else
		return false;

}
*/
