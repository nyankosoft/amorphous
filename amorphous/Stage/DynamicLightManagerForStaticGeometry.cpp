#include "DynamicLightManagerForStaticGeometry.hpp"
#include "LightEntityManager.hpp"
#include "BSPMap.hpp"
#include "../Graphics/Direct3D/FVF_MapVertex.h"
#include "../Graphics/Shader/ShaderManager.hpp"


namespace amorphous
{


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
void CDynamicLightManagerForStaticGeometry::SetDynamicLight( LightEntity& rLight,
															 CSG_Polygon& rPolygon,
															 CBSPMap *pMap )
{
	float f, fTexWidth, fDistToSurface;
	Vector3 vLightPos, vSurfaceToLight, vTexCoordBase;
	Vector3 vNormal, avAxis[2];
	Vector3 p0,p1;

	vLightPos = rLight.GetWorldPose().vPosition;

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
		vNormal = Vector3( 1,0,0); avAxis[0] = Vector3(0, 1,0); avAxis[1] = Vector3(0,0, 1); break;
	case 1:
		vNormal = Vector3(-1,0,0); avAxis[0] = Vector3(0,-1,0); avAxis[1] = Vector3(0,0,-1); break;
	case 2:
		vNormal = Vector3(0, 1,0); avAxis[0] = Vector3(0,0, 1); avAxis[1] = Vector3( 1,0,0); break;
	case 3:
		vNormal = Vector3(0,-1,0); avAxis[0] = Vector3(0,0,-1); avAxis[1] = Vector3(-1,0,0); break;
	case 4:
		vNormal = Vector3(0,0, 1); avAxis[0] = Vector3( 1,0,0); avAxis[1] = Vector3(0, 1,0); break;
	case 5:
		vNormal = Vector3(0,0,-1); avAxis[0] = Vector3(-1,0,0); avAxis[1] = Vector3(0,-1,0); break;
	default:	// need to create 3 orthogonal vectors from polygon edges
		vNormal = Vector3(0.0f, 0.0f, 0.0f);
		i=0;
		while( (Vec3LengthSq(vNormal) < 0.0000001f) && (i < sNumVertices) )
		{
			p0 = ToVector3( v[i].vPosition )                    - ToVector3( v[(i+1) % sNumVertices].vPosition );
			p1 = ToVector3( v[(i+2) % sNumVertices].vPosition ) - ToVector3( v[(i+1) % sNumVertices].vPosition );
			Vec3Cross( vNormal, p1, p0 );
			Vec3Normalize( vNormal, vNormal );
			i++;
		}
		Vec3Cross(avAxis[0], vNormal, p0 );
		Vec3Normalize(avAxis[0], avAxis[0] );
		Vec3Cross(avAxis[1], vNormal, avAxis[0] );
		break;
	}

	if( Vec3LengthSq(vNormal) < 0.00001f )
		return;

	fDistToSurface = Vec3Dot( vNormal, vLightPos - ToVector3(v[0].vPosition) );

	if( fDistToSurface < 0 )
		return;	// polygon is facing away from the light

	// calculate the intensity of light based on the distance between the light source and the target surface
	float fLightRadius = rLight.GetRadius();
	rPolygon.acDynamicLightIntensity[0] = (char)( ( fLightRadius - fDistToSurface ) / fLightRadius * 127.0f );

	if( rPolygon.acDynamicLightIntensity[0] < 0 )
		rPolygon.acDynamicLightIntensity[0] = 0;

	vSurfaceToLight = vNormal * fDistToSurface;
	fTexWidth = 2.0f * (float)sqrt( rLight.GetRadius() * rLight.GetRadius() - fDistToSurface * fDistToSurface );

	Vector3 vLightToVertex;

	char intensity = rPolygon.acDynamicLightIntensity[0];

	for( i=0; i<sNumVertices; i++ )
	{
		vLightToVertex = ToVector3( v[i].vPosition ) - vLightPos;

		vTexCoordBase = vSurfaceToLight + vLightToVertex;
		f = Vec3Dot( avAxis[0], vTexCoordBase );
		v[i].vTex2.u = f / fTexWidth + 0.5f;

		f = Vec3Dot( avAxis[1], vTexCoordBase );
		v[i].vTex2.v = f / fTexWidth + 0.5f;

		// save light direction to vertex for bump mapping
		Vec3Normalize( vLightToVertex, vLightToVertex );
///		v[i].color = D3DCOLOR_XRGB( (int)( (-vLightToVertex.x + 1.0f) * 0.5f * 255.0f ),
///			                        (int)( (-vLightToVertex.y + 1.0f) * 0.5f * 255.0f ),
///			                        (int)( (-vLightToVertex.z + 1.0f) * 0.5f * 255.0f ) );

		// save the distance to the light in alpha component of the diffuse color
		v[i].color &= 0x00FFFFFF;
		v[i].color |= ( (intensity << 24) & 0xFF000000 );
	}
}


void CDynamicLightManagerForStaticGeometry::SetDynamicLights( CBSPMap *pMap, ShaderManager& shader_mgr )
{
	int i, j;
	int iNumCurrentDynamicLights;// = m_pLightEntityManager->GetNumDynamicLights();

	// get the pointer to the effect interface
	LPD3DXEFFECT pEffect = shader_mgr.GetEffect();
	if( !pEffect ) return;

	LightEntity *pLight;

	for( pLight = m_pLightEntityManager->GetDynamicLight(), i=0;
	     pLight != NULL && i < NUM_MAX_DYNAMIC_LIGHTS_PER_FRAME;
		 pLight = pLight->GetNextLight(), i++ )
	{
		m_apDynamicLight[i] = pLight;
	}
	m_iNumCurrentDynamicLights = i;

	CSG_Polygon *pMapPolygon = pMap->GetPolygon();

	HRESULT hr;
	Vector3 vLightPos;
	float fRadius;
	AABB3 aabb;
	int iNumLitPolygons;
	float rgba[4];
	for( i=0; i<m_iNumCurrentDynamicLights; i++ )
	{
		pLight = m_apDynamicLight[i];

		// set the light color to the pixel shader constant
		if( pLight->GetLightObject() )
		{
			rgba[0] = pLight->GetLightObject()->DiffuseColor.red;
			rgba[1] = pLight->GetLightObject()->DiffuseColor.green;
			rgba[2] = pLight->GetLightObject()->DiffuseColor.blue;
			rgba[3] = 1.0f;
		}
		else
		{
			rgba[0] = 1.0f;
			rgba[1] = 1.0f;
			rgba[2] = 1.0f;
			rgba[3] = 1.0f;
		}

		hr = pEffect->SetValue( "g_vDLightMapColor",
			                    rgba,
								sizeof(float) * 4 );

//		m_veciLitPolygonIndex[i].clear();
		m_veciLitPolygonIndex[i].resize(0);
		vLightPos = pLight->GetWorldPosition();
		fRadius = pLight->GetRadius();
		const AABB3& local_aabb = pLight->local_aabb;
		aabb.vMin = vLightPos + local_aabb.vMin;
		aabb.vMax = vLightPos + local_aabb.vMax;
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


} // namespace amorphous
