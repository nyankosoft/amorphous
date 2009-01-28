
#ifndef  __TANGENTSPACE_H__
#define  __TANGENTSPACE_H__

#include "3DMeshModelBuilder.h"

#include <vector>
using namespace std;


class CTangentSpaceInfo
{
	Vector3 vBinormal;
	Vector3 vTangent;

	CTangentSpaceInfo() { vBinormal = vTangent = Vector3(0,0,0); }
};


class CTangentSpace
{
	vector<CGeneral3DVertex>* m_pvecVertexBuffer;

	vector< vector<CTangentSpaceInfo> > m_vecLocalSpace;

public:
	CTangentSpace( vector<CGeneral3DVertex>& rvecVertexBuffer ) { m_pvecVertexBuffer = &rvecVertexBuffer; }
	~CTangentSpace();

	void Calculate( vector<CIndexedPolygon>& rvecPolygon );
	void Update();

};



#endif		/*  __TANGENTSPACE_H__  */


----------------------------------- cut here -----------------------------------

#include "TangentSpace.h"

CTangentSpace::CTangentSpace()
{
}


CTangentSpace::~CTangentSpace()
{
}


void CTangentSpace::Calculate( vector<CIndexedPolygon>& rvecPolygon, vector<CGeneral3DVertex>& rvecVertexBuffer )
{

	// normalize each light direction vector
//	D3DXVECTOR3 avAxis[2];	// local axes of each face. avAxis[0] & [1] are parallel to the directions of u & v of decal texture
//	D3DXVECTOR3 vNormal;
//	MAPVERTEX v, v01, v02;

	Vector3 vPos01, vPos02, vPos;
	int i, j, iNumVertices = m_pvecVertexBuffer->size();
	int iNumPolygons = rvecPolygon.size();
	vector<CGeneral3DVertex>& rvecVertex = *m_pvecVertexBuffer;

	TEXCOORD2 tex01, tex02, tex;

	for( i=0; i<iNumPolygons; i++ )
	{
		CIndexedPolygon& rPolygon = rvecPolygon[i];
		vector<int>& index = rPolygon.m_Index;

		// compute axis[0] (axis along 'u' direction)
		for(j=1; j<GetNumVertices()-1; j++)
		{
			tex01 = rvecVertex[ index[j] ].m_TextureCoord[0]   - rvecVertex[ index[0] ].m_TextureCoord[0];
			tex02 = rvecVertex[ index[j+1] ].m_TextureCoord[0] - rvecVertex[ index[0] ].m_TextureCoord[0];
			
			vPos01 = rvecVertex[ index[j] ].vPosition   - rvecVertex[ index[0] ].vPosition;
			vPos02 = rvecVertex[ index[j+1] ].vPosition - rvecVertex[ index[0] ].vPosition;

			if( fabs(tex01.v) < 0.00001 )
			{
				Vec3Normalize( &avAxis[0], &vPos01 );
				if( 0 < tex01.u )
					avAxis[0] *= -1;
				break;
			}

		vPos = vPos01 * tex02.v / tex01.v;
		avAxis[0] = vPos02 - vPos;

		tex = tex01 * tex02.v / tex01.v;
		if( 0 < tex02.u - tex.u )
			avAxis[0] *= -1;
		Vec3Normalize( &avAxis[0], &avAxis[0] );
		break;
	}

	// compute axis[1] (axis along 'v' direction)
	for(j=1; j<GetNumVertices()-1; j++)
	{
		v01 = rvecVertex[ index[j  ) - rvecVertex[ index[0);
		v02 = rvecVertex[ index[j+1) - rvecVertex[ index[0);

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

void CTangentSpace::Update()
{
}
