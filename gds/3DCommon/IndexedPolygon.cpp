
#include "IndexedPolygon.h"

using namespace std;

//vector<CGeneral3DVertex>* CIndexedPolygon::ms_pVertex;


bool CIndexedPolygon::IsOnTriangle( int iTriangleIndex, const Vector3& rvPosition ) const
{
	if( GetNumVertices() - 2 <= iTriangleIndex )
		return false;

	Vector3 vPrevCross = Vector3(0,0,0);
	Vector3 vCross;

	int indices[] = { 0, iTriangleIndex, iTriangleIndex + 1 };
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
