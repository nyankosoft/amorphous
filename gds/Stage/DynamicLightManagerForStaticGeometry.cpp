
#include "DynamicLightManagerForStaticGeometry.h"
#include "LightEntityManager.h"
#include "BSPMap.h"
#include "../3DCommon/FVF_MapVertex.h"
#include "../3DCommon/Direct3D9.h"
#include "../3DCommon/Shader/Shader.h"
#include "../3DCommon/Shader/ShaderManager.h"


CDynamicLightManagerForStaticGeometry::CDynamicLightManagerForStaticGeometry()
:
m_pLightEntityManager(NULL)
{
	for( int i=0; i<NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME; i++ )
		m_veciLitPolygonIndex[i].reserve( 64 );
}


CDynamicLightManagerForStaticGeometry::~CDynamicLightManagerForStaticGeometry()
{
}


// set the texture coordinates to static geometry for dynamic lighting
void CDynamicLightManagerForStaticGeometry::SetDynamicLight( CLightEntity& rLight,
															 CSG_Polygon& rPolygon,
															 CBSPMap *pMap )
{
	float f, fTexWidth, fDistToSurface;
	D3DXVECTOR3 vLightPos, vSurfaceToLight, vTexCoordBase;
	D3DXVECTOR3 vNormal, avAxis[2];
	D3DXVECTOR3 p0,p1;

	vLightPos = rLight.GetPosition();

	MAPVERTEX *v = pMap->GetVertex() + rPolygon.sIndexOffset;

	if( rPolygon.sIndexOffset < 0 )
	{
		char acStr[128];
		sprintf( acStr, "static geometry with invalid vertex indices\nrPolygon.sIndexOffset = %d", rPolygon.sIndexOffset );
		MessageBox( NULL, acStr, "error", MB_OK|MB_ICONWARNING );
		return;
	}
/*
	char acText[256];
	sprintf( acText, "sg vert.\npol.sIndexOffset: %d\npol.tex_id: %d", rPolygon.sIndexOffset, rPolygon.sTextureID );
	MessageBox( NULL, acText, "info", MB_OK|MB_ICONWARNING );
*/

/*	vNormal =   v[0].vNormal;
	avAxis[0] = v[0].vTangent;
	D3DXVec3Cross( &avAxis[1], &vNormal, &avAxis[0] );
*/
	short i, sNumVertices = rPolygon.sNumVertices;

	// create 3 orthogonal vectors for the polygon. one of them is the normal of the polygon
	switch( rPolygon.cPlaneType )
	{
	case 0:
		vNormal = D3DXVECTOR3( 1,0,0); avAxis[0] = D3DXVECTOR3(0, 1,0); avAxis[1] = D3DXVECTOR3(0,0, 1); break;
	case 1:
		vNormal = D3DXVECTOR3(-1,0,0); avAxis[0] = D3DXVECTOR3(0,-1,0); avAxis[1] = D3DXVECTOR3(0,0,-1); break;
	case 2:
		vNormal = D3DXVECTOR3(0, 1,0); avAxis[0] = D3DXVECTOR3(0,0, 1); avAxis[1] = D3DXVECTOR3( 1,0,0); break;
	case 3:
		vNormal = D3DXVECTOR3(0,-1,0); avAxis[0] = D3DXVECTOR3(0,0,-1); avAxis[1] = D3DXVECTOR3(-1,0,0); break;
	case 4:
		vNormal = D3DXVECTOR3(0,0, 1); avAxis[0] = D3DXVECTOR3( 1,0,0); avAxis[1] = D3DXVECTOR3(0, 1,0); break;
	case 5:
		vNormal = D3DXVECTOR3(0,0,-1); avAxis[0] = D3DXVECTOR3(-1,0,0); avAxis[1] = D3DXVECTOR3(0,-1,0); break;
	default:	// need to create 3 orthogonal vectors from polygon edges
		vNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		i=0;
		while( (D3DXVec3LengthSq(&vNormal) < 0.0000001f) && (i < sNumVertices) )
		{
			p0 = v[i].vPosition                    - v[(i+1) % sNumVertices].vPosition;
			p1 = v[(i+2) % sNumVertices].vPosition - v[(i+1) % sNumVertices].vPosition;
			D3DXVec3Cross(&vNormal, &p1, &p0 );
			D3DXVec3Normalize(&vNormal, &vNormal );
			i++;
		}
		D3DXVec3Cross(&avAxis[0], &vNormal, &p0 );
		D3DXVec3Normalize(&avAxis[0], &avAxis[0] );
		D3DXVec3Cross(&avAxis[1], &vNormal, &avAxis[0] );
		break;
	}

	if( D3DXVec3LengthSq(&vNormal) < 0.00001f )
		return;

	fDistToSurface = D3DXVec3Dot( &vNormal, &(vLightPos - v[0].vPosition) );

	if( fDistToSurface < 0 )
		return;	// polygon is facing away from the light

	// calculate the intensity of light based on the distance between the light source and the target surface
	float fLightRadius = rLight.GetRadius();
	rPolygon.acDynamicLightIntensity[0] = (char)( ( fLightRadius - fDistToSurface ) / fLightRadius * 127.0f );

	if( rPolygon.acDynamicLightIntensity[0] < 0 )
		rPolygon.acDynamicLightIntensity[0] = 0;

	vSurfaceToLight = vNormal * fDistToSurface;
	fTexWidth = 2.0f * (float)sqrt( rLight.GetRadius() * rLight.GetRadius() - fDistToSurface * fDistToSurface );

	D3DXVECTOR3 vLightToVertex;

	char intensity = rPolygon.acDynamicLightIntensity[0];

	for( i=0; i<sNumVertices; i++ )
	{
		vLightToVertex = v[i].vPosition - vLightPos;

		vTexCoordBase = vSurfaceToLight + vLightToVertex;
		f = D3DXVec3Dot( &avAxis[0], &vTexCoordBase );
		v[i].vTex2.u = f / fTexWidth + 0.5f;

		f = D3DXVec3Dot( &avAxis[1], &vTexCoordBase );
		v[i].vTex2.v = f / fTexWidth + 0.5f;

		// save light direction to vertex for bump mapping
		D3DXVec3Normalize( &vLightToVertex, &vLightToVertex );
///		v[i].color = D3DCOLOR_XRGB( (int)( (-vLightToVertex.x + 1.0f) * 0.5f * 255.0f ),
///			                        (int)( (-vLightToVertex.y + 1.0f) * 0.5f * 255.0f ),
///			                        (int)( (-vLightToVertex.z + 1.0f) * 0.5f * 255.0f ) );

		// save the distance to the light in alpha component of the diffuse color
		v[i].color &= 0x00FFFFFF;
		v[i].color |= ( (intensity << 24) & 0xFF000000 );
	}
}


void CDynamicLightManagerForStaticGeometry::SetDynamicLights(CBSPMap *pMap)
{
	int i, j;
	int iNumCurrentDynamicLights;// = m_pLightEntityManager->GetNumDynamicLights();

	// get the pointer to the effect interface
	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	if( !pShaderMgr ) return;

	LPD3DXEFFECT pEffect = pShaderMgr->GetEffect();
	if( !pEffect ) return;

	CLightEntity *pLight;

	for( pLight = m_pLightEntityManager->GetDynamicLight(), i=0;
	     pLight != NULL && i < NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME;
		 pLight = pLight->GetNextLight(), i++ )
	{
		m_apDynamicLight[i] = pLight;
	}
	m_iNumCurrentDynamicLights = i;

	CSG_Polygon *pMapPolygon = pMap->GetPolygon();

	HRESULT hr;
	D3DXVECTOR3 vLightPos;
	float fRadius;
	AABB3 aabb;
	int iNumLitPolygons;
	for( i=0; i<m_iNumCurrentDynamicLights; i++ )
	{
		pLight = m_apDynamicLight[i];

		// set the light color to the pixel shader constant
		hr = pEffect->SetValue( "g_vDLightMapColor",
			                    pLight->GetColor(),
								sizeof(float) * 4 );

//		m_veciLitPolygonIndex[i].clear();
		m_veciLitPolygonIndex[i].resize(0);
		vLightPos = pLight->GetPosition();
		fRadius = pLight->GetRadius();
		aabb.vMin = vLightPos + pLight->GetLocalAABB().vMin;
		aabb.vMax = vLightPos + pLight->GetLocalAABB().vMax;
		pMap->GetIntersectingPolygons( vLightPos, fRadius, aabb, m_veciLitPolygonIndex[i] );

		iNumLitPolygons = m_veciLitPolygonIndex[i].size();
		for( j=0; j<iNumLitPolygons; j++ )
		{
			int polygon_index = m_veciLitPolygonIndex[i][j];
			if( polygon_index < 0 )
				continue;
/*			{
				char acStr[128];
				sprintf( acStr, "static geometry contains invalid polygon indices\n polygon_index = %d", polygon_index );
				MessageBox( NULL, acStr, "error", MB_OK|MB_ICONWARNING );
				return;
			}*/

			// save the index to dynamic light in each polygon
			// this is used to reset the texture coordinates after polygon is rendered
			pMapPolygon[ m_veciLitPolygonIndex[i][j] ].acDynamicLightIndex[0] = i;

			SetDynamicLight( *pLight, pMapPolygon[ m_veciLitPolygonIndex[i][j] ], pMap );
		}
	}

}


void CDynamicLightManagerForStaticGeometry::ClearDynamicLightEffectsFromPolygons(CBSPMap *pMap)
{
	CSG_Polygon *pMapPolygon = pMap->GetPolygon();
	MAPVERTEX *pMapVertex = pMap->GetVertex();
	MAPVERTEX *v;

	int i, j, k, iNumCurrentDynamicLights = m_pLightEntityManager->GetNumDynamicLights();
	short s;

	for( i=0; i<m_iNumCurrentDynamicLights; i++ )
	{
		int num_lit_polygons = m_veciLitPolygonIndex[i].size();
		for( j=0; j<num_lit_polygons; j++ )
		{
//			for( k=0; k< /* # of lights that is hitting the current polygon? */ ; k++ )
//			{
				int polygon_index = m_veciLitPolygonIndex[i][j];

				if( polygon_index < 0 )
					continue;

				CSG_Polygon& rPolygon = pMapPolygon[ polygon_index ];

				if( rPolygon.sIndexOffset < 0  || 65535 < rPolygon.sIndexOffset )
					continue;

				if( 0 <= rPolygon.acDynamicLightIndex[0/*k*/] )
				{
					rPolygon.acDynamicLightIndex[0/*k*/] = -1;
					rPolygon.acDynamicLightIntensity[0/*k*/] = -1;

					v = pMapVertex + rPolygon.sIndexOffset;
					for( s=0; s<rPolygon.sNumVertices; s++ )
					{
						v[s].vTex2.u = v[s].vTex2.v = 0.0f;

						// reset to default vertex color since it has been modified to represent bump normal
						v[s].color &= 0x00FFFFFF;
///						v[s].color = 0xFFFFFFFF;
					}

				}
//			}
		}
	}
}