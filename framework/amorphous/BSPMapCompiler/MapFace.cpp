
#include "MapFace.hpp"

#include "Graphics/MeshModel/NVMeshMender.hpp"

#include <assert.h>


namespace amorphous
{


//=====================================================================================
// CMapFace::Methods()                                 - class CMapFace : public CFace
//=====================================================================================

CMapFace::CMapFace()
{
	m_iTypeFlag = 0;

	m_sTextureID = -1;
	m_sLightMapID = -1;
	m_sNode = 0;
	m_bFlag = false;
	m_iSurfaceMaterialIndex = -1;
	m_acSurfaceName[0] = '\0';
	m_iPolygonIndex = -1;

}


CMapFace::CMapFace(const CMapFace& face) : CFace(face)
{
	m_sTextureID   = face.m_sTextureID;
	m_sLightMapID  = face.m_sLightMapID;
	m_sNode        = face.m_sNode;
	m_bFlag        = face.m_bFlag;

	m_iTypeFlag    = face.m_iTypeFlag;

	m_iSurfaceMaterialIndex = face.m_iSurfaceMaterialIndex;
	strcpy( m_acSurfaceName, face.m_acSurfaceName );

	m_iPolygonIndex = face.m_iPolygonIndex;
	m_vecvLightDir.assign( face.m_vecvLightDir.begin(), face.m_vecvLightDir.end() );
}


CMapFace CMapFace::operator =(CMapFace face)
{
	this->CFace::operator=(face);

	m_sTextureID   = face.m_sTextureID;
	m_sLightMapID  = face.m_sLightMapID;
	m_sNode        = face.m_sNode;
	m_bFlag        = face.m_bFlag;

	m_iTypeFlag    = face.m_iTypeFlag;

	m_iSurfaceMaterialIndex = face.m_iSurfaceMaterialIndex;
	strcpy( m_acSurfaceName, face.m_acSurfaceName );

	m_iPolygonIndex = face.m_iPolygonIndex;
	m_vecvLightDir.assign( face.m_vecvLightDir.begin(), face.m_vecvLightDir.end() );

	return *this;
}


MAPVERTEX& CMapFace::GetMapVertex(int iIndex)
{
//	if( iIndex < 0 || m_pVertices.size() <= iIndex )
//		return ;

	return m_pVertices[iIndex];
}


void CMapFace::Split(CFace& front, CFace& back, SPlane& cutplane)
{
	//copy CMapFace-specific features to 'front' and 'back' 
	//Such features include m_bFlag, m_sNode, m_sTextureID, m_sLightMapID, and the like
	CMapFace* pMapFace = NULL;
	if( pMapFace = dynamic_cast<CMapFace *> (&front) )
		*pMapFace = *this;
	if( pMapFace = dynamic_cast<CMapFace *> (&back) )
		*pMapFace = *this;

	//using 'cutplane', split this face into 'front' and 'back'
	//(use the function of the parent class 'CFace')
	CFace::Split(front, back, cutplane);
}


void CMapFace::GetTransformationToLocalFaceCoord_Tex0( D3DXMATRIX& rMat )
{

	// normalize each light direction vector
	D3DXVECTOR3 avAxis[2];	// local axes of each face. avAxis[0] & [1] are parallel to the directions of u & v of decal texture
	D3DXVECTOR3 vNormal;
	MAPVERTEX v, v01, v02;
	int i;

	// compute axis[0] (axis along 'u' direction)
	for(i=1; i<GetNumVertices()-1; i++)
	{
		v01 = GetMapVertex(i  ) - GetMapVertex(0);
		v02 = GetMapVertex(i+1) - GetMapVertex(0);

		if( fabs(v01.vTex0.v) < 0.00001 )
		{
			D3DXVec3Normalize( &avAxis[0], &v01.vPosition );
			if( 0 < v01.vTex0.u )
				avAxis[0] *= -1;
			break;
		}

		v = v01 * v02.vTex0.v / v01.vTex0.v;
		avAxis[0] = v02.vPosition - v.vPosition;

		if( 0 < v02.vTex0.u - v.vTex0.u )
			avAxis[0] *= -1;
		D3DXVec3Normalize( &avAxis[0], &avAxis[0] );
		break;
	}

	// compute axis[1] (axis along 'v' direction)
	for(i=1; i<GetNumVertices()-1; i++)
	{
		v01 = GetMapVertex(i  ) - GetMapVertex(0);
		v02 = GetMapVertex(i+1) - GetMapVertex(0);

		if( fabs(v01.vTex0.u) < 0.00001 )
		{
			D3DXVec3Normalize( &avAxis[1], &v01.vPosition );
			if( 0 < v01.vTex0.v )
				avAxis[1] *= -1;
			break;
		}

		v = v01 * v02.vTex0.u / v01.vTex0.u;
		avAxis[1] = v02.vPosition - v.vPosition;

		if( 0 < v02.vTex0.v - v.vTex0.v )
			avAxis[1] *= -1;
		D3DXVec3Normalize( &avAxis[1], &avAxis[1] );
		break;
	}

	vNormal = GetPlane().normal;

	D3DXMatrixIdentity( &rMat );
	rMat._11 = avAxis[0].x; rMat._12 = avAxis[1].x; rMat._13 = vNormal.x;
	rMat._21 = avAxis[0].y; rMat._22 = avAxis[1].y; rMat._23 = vNormal.y;
	rMat._31 = avAxis[0].z; rMat._32 = avAxis[1].z; rMat._33 = vNormal.z;

}


void CMapFace::CalculateTangentSpace_Tex0()
{
	std::vector< MeshMender::Vertex > theVerts;
	std::vector< unsigned int > theIndices;
	std::vector< unsigned int > mappingNewToOld;

	// put indexed triangle polygons into a single index array
	int i, iNumTriangles = GetNumVertices() - 2;
	theIndices.reserve( iNumTriangles * 3 );
	for( i=0; i<iNumTriangles; i++ )
	{
		theIndices.push_back( 0 );
		theIndices.push_back( i + 1 );
		theIndices.push_back( i + 2 );
	}


	// fill up the vectors with your mesh's data
    int iNumVertices = GetNumVertices();
	for( i = 0; i < iNumVertices; ++i )
	{
		MeshMender::Vertex v;
		v.pos = GetMapVertex(i).vPosition;
		v.s   = GetMapVertex(i).vTex0.u;
		v.t   = GetMapVertex(i).vTex0.v;

		v.normal = GetMapVertex(i).vNormal;
		// meshmender will computer normals, tangents, and binormals, no need to fill those in.
		// however, if you do not have meshmender compute the normals, you _must_ pass in valid
		// normals to meshmender
		theVerts.push_back(v);
	}

	MeshMender mender;

	int iNumIndices = theIndices.size();

    // pass it in to the mender to do it's stuff
	mender.Mend( theVerts,
		         theIndices,
				 mappingNewToOld,
				 -1.0f,						// minNormalCreaseCos
				 -1.0f,						// minTangentCreaseCos
				 -1.0f,						// minBinormalCreaseCos
				 0.0f,						// weightNormalsByArea
		         MeshMender::DONT_CALCULATE_NORMALS,
				 MeshMender::RESPECT_SPLITS );


	// check indices
	assert( iNumIndices == theIndices.size() );

	// update vertices
	// new vertices may have been created and added to 'theVerts' by mesh mender
	int iNumUpdatedVertices = theVerts.size();

	// suppose that all the vertices on the face have the same tangent space
	for( i=0; i<iNumVertices; i++ )
	{
		GetMapVertex(i).vTangent = theVerts[0].tangent;
	}

/*	vector<MAPVERTEX> vecNewVertexBuffer;	// temporary vertex buffer to hold new vertices
	vecNewVertexBuffer.resize( iNumUpdatedVertices );

	for( i=0; i<iNumUpdatedVertices; ++i )
	{
		vecNewVertexBuffer[i] = GetMapVertex[ mappingNewToOld[i] ];

		vecNewVertexBuffer[i].vNormal   = theVerts[i].normal;
//		vecNewVertexBuffer[i].vBinormal = theVerts[i].binormal;
		vecNewVertexBuffer[i].vTangent  = theVerts[i].tangent;
	}
	*/
}


bool CFace::IsOnTriangle( int iTriangleIndex, D3DXVECTOR3& rvPosition )
{
	D3DXVECTOR3 vPrevCross = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 vCross;

	int i;
	for( i = iTriangleIndex; i < iTriangleIndex + 3; i++ )
	{
		D3DXVECTOR3 v0 = GetVertex(i);
		D3DXVECTOR3 v1 = GetVertex(i+1);
		D3DXVec3Cross( &vCross, &(v1-v0), &(rvPosition - v0) );
		
		if( D3DXVec3Dot(&vPrevCross,&vCross) < -0.01f )
			return false;

		vPrevCross = vCross;
	}
	return true;
}


D3DXVECTOR3 CMapFace::GetInterpolatedNormal( D3DXVECTOR3& rvPosition )
{
	int i;
	SPlane plane_12;
	D3DXVECTOR3 vDir12, vPoint12, vNormal12, vDestNormal;
	float f, d1, d2, dx, dp;

	// find triangle which includes 'rvPosition'
	for( i=0; i < GetNumVertices() - 2; i++ )
	{
		if( IsOnTriangle( i, rvPosition ) )
			break;
	}

	if( GetNumVertices() - 2 <= i )
		return GetPlane().normal;	// couldn't find a triangle which includes 'rvPosition'

	MAPVERTEX& v0 = GetMapVertex(i);
	MAPVERTEX& v1 = GetMapVertex(i+1);
	MAPVERTEX& v2 = GetMapVertex(i+2);

	if( D3DXVec3LengthSq( &(rvPosition - v0.vPosition) ) < 0.0001f )
		return v0.vNormal;	// 'rvPosition' is close to 'v0' - just use the normal of 'v0' as an approximation

	// make a plane which is perpendicular to the triangle and includes 'rvPosition'
	D3DXVec3Normalize( &vDir12, &(rvPosition - v0.vPosition) );
	D3DXVECTOR3& rvFaceNormal = GetPlane().normal;
	D3DXVec3Cross( &plane_12.normal, &rvFaceNormal, &vDir12 );
	D3DXVec3Normalize( &plane_12.normal, &plane_12.normal);

	// calculate the normal at 'vPoint12' by interpolating the normals of v1 & v2
	plane_12.dist = D3DXVec3Dot( &plane_12.normal, &v0.vPosition );
	d1 = D3DXVec3Dot( &plane_12.normal, &v1.vPosition ) - plane_12.dist;
	d2 = D3DXVec3Dot( &plane_12.normal, &v2.vPosition ) - plane_12.dist;
	f = - d1 / (d2 - d1);
	vPoint12  = v1.vPosition + (v2.vPosition - v1.vPosition) * f;
	vNormal12 = v1.vNormal   + (v2.vNormal   - v1.vNormal)   * f;

	// calculate the normal at 'rvPosition' by interpolating the normals of v0 & vPoint12
	dx = D3DXVec3Dot( &vDir12, &(rvPosition - v0.vPosition) );
	dp = D3DXVec3Dot( &vDir12, &(vPoint12   - v0.vPosition) );
	f = dx / dp;
	vDestNormal = v0.vNormal + (vNormal12 - v0.vNormal) * f;
	D3DXVec3Normalize( &vDestNormal, &vDestNormal );
	return vDestNormal;
}



// check whether the given point is included in the volume
// which is defined by sweeping the face in the direction of the face normal
// rvPosition : [in]
// rfDist : [out]
bool CMapFace::IsInSweptVolume( D3DXVECTOR3& rvPosition, float& rfDist )
{
	rfDist = -99999;
	int i;
	float d;
	D3DXVECTOR3 vEdge, vNormalOut;
	for( i=0; i<GetNumVertices() - 1; i++ )
	{
		vEdge = GetVertex(i+1) - GetVertex(i);
		D3DXVec3Cross( &vNormalOut, &vEdge, &(GetPlane().normal) );
		d = D3DXVec3Dot( &(rvPosition - GetVertex(i)), &vNormalOut );

		if( rfDist < d )
			rfDist = d;
	}
	if( rfDist <= 0 )
		return true;
	else
		return false;

}



} // amorphous
