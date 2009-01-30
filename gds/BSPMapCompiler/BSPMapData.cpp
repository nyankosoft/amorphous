
#include "BSPMapData.hpp"
//#include "lightmapbuilder.h"
#include "../Graphics/LightStructs.hpp"
#include "../Stage/StaticGeometry.hpp"

#include "Support/Log/DefaultLog.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "Support/fnop.hpp"
using namespace fnop;


#include <string>
using namespace std;


//===============================================================================
// CBSPMapData
//===============================================================================

CBSPMapData::~CBSPMapData()
{
	m_aMainFace.clear();
	m_aInteriorFace.clear();
	m_aPlane.clear();
	m_vecTextureFile.clear();

	int i;
	for(i=0; i<m_vecpLight.size(); i++)
	{
		SafeDelete( m_vecpLight[i] );
	}
	m_vecpLight.clear();

	if( m_pFog ) delete m_pFog;
}


int CBSPMapData::LoadMapDataFromFile( const string& filename )
{
	// load predefined light sets
	LoadLightPresetsFromFile( "LightPresets.txt" );

//	MessageBox(NULL, "light presets have been loaded.", "progress report", MB_OK);

	// load the surface material list file - "(original file names).mst"
	//fstring mat_filename = fstring(filename).get_extchanged("mst");
	string mat_filename(filename);
	fnop::change_ext( mat_filename, "mst" );
	if( !LoadMaterialNames( mat_filename.c_str() ) )
	{
		LOG_PRINT_ERROR( "Cannot find the material file: " + mat_filename );
		return 0;
	}

	int iResult = LoadSpecificMapDataFromFile( filename.c_str() );

//	MessageBox(NULL, "LW map data has been loaded.", "progress report", MB_OK);

	// set MAIN and INTERIOR flags to each type of faces
	SetFaceProperties();

	// make planes for each face, planes are stored
	// in the plane buffer and the indices to planes are set to each face
	SetPlane();

	// faces which emit light are processed into 'light source faces'
	SetLightsFromLightSourceFaces();

	CalculateTangentSpace();
	
	// set transformation from world space to each face space
	// transformations are represented by quaternions and stored into diffuse color of each vertex 
	SetLocalTransformsToVertices();

	// set the local directions of static lights to each face
//	SetLocalLightDirections();

	// output loaded polygon faces to a text file
	char acDebugFileName[512], acBodyFilename[512];
	string body_without_ext = fnop::get_no_ext( filename );
	string dbg_file_path = get_path(filename) + string("\\DebugInfoFile\\");

	if( dbg_file_path[0] == '/' || dbg_file_path[0] == '\\' )
		dbg_file_path = "." + dbg_file_path;

	string dbg_filename = dbg_file_path	+ body_without_ext + string("-map_faces.txt");

	WriteFacesToFile( dbg_filename );

	// output mesh data
	size_t i, num_meshes = m_vecMesh.size();
	for( i=0; i<num_meshes; i++ )
		m_vecMesh[i].WriteToTextFile( fmt_string( "%socclusion_mesh%02d.txt", dbg_file_path.c_str(), i ) );

	m_EnvLightMesh.WriteToTextFile( dbg_file_path + "env_light_mesh.txt" );

	return iResult;
}


void CBSPMapData::SetFaceProperties()
{
	size_t i, iNumFaces;

	iNumFaces = m_aMainFace.size();
	for( i=0; i<iNumFaces; i++ )
		m_aMainFace[i].RaiseTypeFlag( CMapFace::TYPE_MAIN );

	iNumFaces = m_aInteriorFace.size();
	for( i=0; i<iNumFaces; i++ )
		m_aInteriorFace[i].RaiseTypeFlag( CMapFace::TYPE_INTERIOR );

	iNumFaces = m_aSkyboxFace.size();
	for( i=0; i<iNumFaces; i++ )
		m_aSkyboxFace[i].RaiseTypeFlag( CMapFace::TYPE_NOCLIP );
}


void CBSPMapData::SetPlane()
{
	vector<CMapFace>* pvecFace[3] = { &m_aMainFace, &m_aInteriorFace, &m_aSkyboxFace };

	size_t i;//, j, num_faces;
	vector<CMapFace>::iterator face;
	for( i=0; i<3; i++ )
	{
		vector<CMapFace>& vecFace = *pvecFace[i];
//		num_faces = vecFace.size();
		face = vecFace.begin();
		while( face != vecFace.end() )
		{
			if( !m_aPlane.AddPlaneFromFace(*face) )
				face = vecFace.erase( face );
			else
				face++;
		}
	}

/*	CMapFace* pFace = m_aMainFace.begin();
	while( pFace != m_aMainFace.end() )
	{
		if( !m_aPlane.AddPlaneFromFace(*pFace) )
			pFace = m_aMainFace.erase( pFace );		// invalid polygon
		else
			pFace++;
	}

	pFace = m_aInteriorFace.begin();
	while( pFace != m_aInteriorFace.end() )
	{
		if( !m_aPlane.AddPlaneFromFace(*pFace) )
			pFace = m_aInteriorFace.erase( pFace );		// invalid polygon
		else
			pFace++;
	}

	pFace = m_aSkyboxFace.begin();
	while( pFace != m_aSkyboxFace.end() )
	{
		if( !m_aPlane.AddPlaneFromFace(*pFace) )
			pFace = m_aSkyboxFace.erase( pFace );		// invalid polygon
		else
			pFace++;
	}*/
}


bool CBSPMapData::LoadLightPresetsFromFile( const char* pcFilename )
{
	FILE* fp = fopen( pcFilename, "r" );
	if(!fp)
	{
		g_Log.Print( WL_WARNING, "couldn't find light presets file" );
		return false;
	}

	SPointLightDesc light;
	char acLine[512];
	fgets(acLine, 511, fp);		// the first line is reserved for comments
	while( fgets(acLine, 511, fp) )
	{
		sscanf( acLine, "%s %f %f %f %f %f %f %f\n",
			light.acName,
			&light.fRed, &light.fGreen, &light.fBlue,
			&light.fIntensity,
			&light.fAttenuation[0], &light.fAttenuation[1], &light.fAttenuation[2] );

		m_vecLightDesc.push_back(light);

	}

	fclose(fp);
	return true;
}

int CBSPMapData::GetNumLights()
{
	return m_vecpLight.size();
}


CBaseLight* CBSPMapData::GetLight(int i)
{
	if( i < 0 || m_vecpLight.size() < i )
		return NULL;	// invalid index

	return m_vecpLight.at(i);
}


SPointLightDesc CBSPMapData::FindPointLightDesc(char *pcLightName)
{
	int i;
	for( i=0; i<m_vecLightDesc.size(); i++ )
	{
		if( strcmp(m_vecLightDesc[i].acName, pcLightName) == 0 )
			return m_vecLightDesc[i];
	}

	// requested light was not found - return a default light
	SPointLightDesc light;
	light.fRed   = 1.0f;
	light.fGreen = 1.0f;
	light.fBlue  = 1.0f;
	light.fIntensity = 0.5f;
	return light;

}


void CBSPMapData::SetLightsFromLightSourceFaces()
{
	vector<CMapFace> vecTempLightSourceFace;	// temporary buffer to hold light source faces
	size_t i,j;
	size_t iNumInteriorFaces = m_aInteriorFace.size();

	for(i=0; i<iNumInteriorFaces; i++)
	{	// set all the flags of light source faces to false
		m_aInteriorFace[i].m_bFlag = false;
	}

	// cluster light source faces	
	for(i=0; i<iNumInteriorFaces; i++)
	{
		vecTempLightSourceFace.clear();
		CMapFace& rFace1 = m_aInteriorFace[i];

		if( !rFace1.ReadTypeFlag(CMapFace::TYPE_LIGHTSOURCE) )
			continue;	// doesn't emit light by itself

		if( rFace1.m_bFlag == true )
			continue;	// already registered as a light source face

		rFace1.m_bFlag = true;
		vecTempLightSourceFace.push_back( rFace1 );

		// adjacent light source faces are converted into one point light
		SearchAdjacentLightSourceFaces_r( &vecTempLightSourceFace, rFace1, &m_aInteriorFace );

		// convert each cluster of light source faces into a point light
		AABB3 aabb;
		aabb.Nullify();
		for(j=0; j<vecTempLightSourceFace.size(); j++)
			vecTempLightSourceFace[j].AddToAABB( aabb );

		SPointLightDesc light = FindPointLightDesc( rFace1.m_acSurfaceName );
		CPointLight* pPointLight = new CPointLight;
		pPointLight->vPosition = aabb.GetCenterPosition();
		pPointLight->Color.fRed   = light.fRed;
		pPointLight->Color.fGreen = light.fGreen;
		pPointLight->Color.fBlue  = light.fBlue;
		pPointLight->fIntensity   = light.fIntensity;
		pPointLight->fAttenuation0 = light.fAttenuation[0];
		pPointLight->fAttenuation1 = light.fAttenuation[1];
		pPointLight->fAttenuation2 = light.fAttenuation[2];
		m_vecpLight.push_back( pPointLight );
	}
}


void CBSPMapData::SearchAdjacentLightSourceFaces_r( vector<CMapFace>* pvecAdjacentFace,
												    CMapFace& rFace1, vector<CMapFace>* pvecSourceFace)
{
	for(int i=0; i<pvecSourceFace->size(); i++)
	{
		CMapFace& rFace2 = pvecSourceFace->at(i);

		if( !rFace2.ReadTypeFlag(CMapFace::TYPE_LIGHTSOURCE) )
			continue;

		if( rFace2.m_bFlag == true )
			continue;

		if( rFace1.SharingPointWith(rFace2) )
		{
			rFace2.m_bFlag = true;
			pvecAdjacentFace->push_back(rFace2);

			SearchAdjacentLightSourceFaces_r( pvecAdjacentFace, rFace2 , pvecSourceFace);
		}
	}
}


void CBSPMapData::CalculateTangentSpace()
{
	vector<CMapFace> *apvecMapFace[2];	// array of pointers to vectors
	apvecMapFace[0] = &m_aMainFace;
	apvecMapFace[1] = &m_aInteriorFace;
	MAPVERTEX v;
	int t, iFace, iNumFaces;

	// initialize light direction vector for each vertex; 
	for( t=0; t<2; t++ )
	{
		iNumFaces = apvecMapFace[t]->size();
		for( iFace=0; iFace<iNumFaces; iFace++ )
		{
			apvecMapFace[t]->at(iFace).CalculateTangentSpace_Tex0();
		}
	}
}


void CBSPMapData::SetLocalTransformsToVertices()
{
	vector<CMapFace> *apvecMapFace[2];	// array of pointers to vectors
	apvecMapFace[0] = &m_aMainFace;
	apvecMapFace[1] = &m_aInteriorFace;
	MAPVERTEX v;
	int t, iFace, iNumFaces;
	int i;

	// Vector3 & Matrix33 version after this routine
	D3DXMATRIX matLocal;
	D3DXQUATERNION qLocal;
//	float x,y,z,w;
//	DWORD dwColor;
	D3DXVECTOR3 vTangent;

	// initialize light direction vector for each vertex; 
	for( t=0; t<2; t++ )
	{
		iNumFaces = apvecMapFace[t]->size();
		for( iFace=0; iFace<iNumFaces; iFace++ )
		{
			CMapFace& rFace = apvecMapFace[t]->at(iFace);
			rFace.GetTransformationToLocalFaceCoord_Tex0( matLocal );

/*			D3DXQuaternionRotationMatrix( &qLocal, &matLocal );

			x = (qLocal.x + 1.0f) * 0.5f;
			y = (qLocal.y + 1.0f) * 0.5f;
			z = (qLocal.z + 1.0f) * 0.5f;
			w = (qLocal.w + 1.0f) * 0.5f;
			dwColor = D3DCOLOR_ARGB( ((int)(w * 255.0f)),
								     ((int)(x * 255.0f)),
									 ((int)(y * 255.0f)),
									 ((int)(z * 255.0f)) );
*/

			vTangent  = D3DXVECTOR3( matLocal._11, matLocal._21, matLocal._31 );
//			vBinormal = D3DXVECTOR3( matLocal._12, matLocal._22, matLocal._32 );
//			vNormal   = D3DXVECTOR3( matLocal._13, matLocal._23, matLocal._33 );

			for( i=0; i<rFace.GetNumVertices(); i++ )
			{
				v = rFace.GetMAPVERTEX(i);

//				v.color = dwColor;	// use vertex diffuse color to store local transformation

				v.vTangent = vTangent;

				rFace.SetMAPVERTEX( i, v );
			}
		}
	}

/*
	Matrix34 matLocal;
	Quaternion qLocal;
//	float x,y,z,w;
//	DWORD dwColor;
	Vector3 vTangent;

	// initialize light direction vector for each vertex; 
	for( t=0; t<2; t++ )
	{
		iNumFaces = apvecMapFace[t]->size();
		for( iFace=0; iFace<iNumFaces; iFace++ )
		{
			CMapFace& rFace = apvecMapFace[t]->at(iFace);
			rFace.GetTransformationToLocalFaceCoord_Tex0( matLocal );

//			D3DXQuaternionRotationMatrix( &qLocal, &matLocal );
//
//			x = (qLocal.x + 1.0f) * 0.5f;
//			y = (qLocal.y + 1.0f) * 0.5f;
//			z = (qLocal.z + 1.0f) * 0.5f;
//			w = (qLocal.w + 1.0f) * 0.5f;
//			dwColor = D3DCOLOR_ARGB( ((int)(w * 255.0f)),
//								     ((int)(x * 255.0f)),
//									 ((int)(y * 255.0f)),
//									 ((int)(z * 255.0f)) );
//

			vTangent  = Vector3( matLocal._11, matLocal._21, matLocal._31 );
//			vBinormal = D3DXVECTOR3( matLocal._12, matLocal._22, matLocal._32 );
//			vNormal   = D3DXVECTOR3( matLocal._13, matLocal._23, matLocal._33 );

			for( i=0; i<rFace.GetNumVertices(); i++ )
			{
				v = rFace.GetMAPVERTEX(i);

//				v.color = dwColor;	// use vertex diffuse color to store local transformation

				v.vTangent = vTangent;

				rFace.SetMAPVERTEX( i, v );
			}
		}
	}
*/
}


void CBSPMapData::GetTriangleMeshForCollision( vector<D3DXVECTOR3>& rvecvTriangleVertex,
											   vector<int>& rveciTriangleIndex,	// indices used by indexed triangles
											   vector<int>& rveciMaterialIndex )	// material indices for each triangle
{
	// reserve vector based on rough estimation on the number of vertices and indices
	rvecvTriangleVertex.reserve( (m_aMainFace.size() + m_aInteriorFace.size()) * 5 );
	rveciTriangleIndex.reserve( (m_aMainFace.size() + m_aInteriorFace.size()) * 8 );
	rveciMaterialIndex.reserve( (m_aMainFace.size() + m_aInteriorFace.size()) * 3 );

	GetTriangleInfoFromFaceSet( rvecvTriangleVertex, rveciTriangleIndex, rveciMaterialIndex, m_aMainFace );
	GetTriangleInfoFromFaceSet( rvecvTriangleVertex, rveciTriangleIndex, rveciMaterialIndex, m_aInteriorFace );
}


void CBSPMapData::GetTriangleInfoFromFaceSet( vector<D3DXVECTOR3>& rvecVertex,
										      vector<int>& rveciTriangleIndex,
										      vector<int>& rveciMaterialIndex,
										      vector<CMapFace>& rvecSrcFace )
{
	size_t i, num_faces = rvecSrcFace.size();
	int j;
	for( i=0; i<num_faces; i++ )
	{
		CMapFace& rFace = rvecSrcFace[i];

		if( rFace.ReadTypeFlag(CMapFace::TYPE_NOCLIP) )
			continue;	// not used in collision detection

		for( j=0; j<rFace.GetNumVertices()-2 ; j++ )
		{
			// set up index for triangle vertices
			rveciTriangleIndex.push_back( GetMeshVertexIndex( rFace.GetVertex(0),   rvecVertex ) );
			rveciTriangleIndex.push_back( GetMeshVertexIndex( rFace.GetVertex(j+1), rvecVertex ) );
			rveciTriangleIndex.push_back( GetMeshVertexIndex( rFace.GetVertex(j+2), rvecVertex ) );

			// save material index
			rveciMaterialIndex.push_back( rFace.m_iSurfaceMaterialIndex );
		}
	}
}


// return an index to the given vertex
int CBSPMapData::GetMeshVertexIndex( D3DXVECTOR3& v, vector<D3DXVECTOR3>& rvecVertex )
{
	int i;
	for( i=0; i<rvecVertex.size(); i++ )
	{
		if( D3DXVec3LengthSq( &(v - rvecVertex[i]) ) < 0.0000000001f )
			return i;	// close enough to treat as the same vertices
	}
	// vertex was not found in the current vertex buffer - add 'v' as a new vertex and return its index
	rvecVertex.push_back( v );
	return rvecVertex.size() - 1;
}


void CBSPMapData::WriteFacesToFile( const string& filename )
{
	FILE* fp = fopen(filename.c_str(), "w");
	if( !fp )
		return;

	vector<CMapFace>* paFace;
	int numfaces, numplanes, i, j, k;
	char acStr[512];
	MAPVERTEX v;
	short tex_id;


  for(k=0; k<2; k++)
  {
	if(k==0)
	{
		paFace = &m_aMainFace;
		sprintf(acStr, "Main Face");
	}
	else 
	{
		paFace = &m_aInteriorFace;
		sprintf(acStr, "Interior Face");
	}
	numfaces = paFace->size();
	numplanes = m_aPlane.size();
	fputs("------------------\n",fp);
	fprintf(fp, "%s\n%d faces\n%d planes\n", acStr, numfaces, numplanes);
	fputs("------------------\n\n",fp);

	// write face data
	for(i=0; i<numfaces; i++)
	{
		CMapFace& rFace = paFace->at(i);
		SPlane& rPln = rFace.GetPlane();
//		D3DXVec3toStr(acPln, rPln.normal, 1);
//		ftos(rPln.dist, 1, acDist);
		tex_id = rFace.m_sTextureID;
		sprintf(acStr, "[%d] plane: n%s, dist=%.2f  / TexID: %d\n",
			i, to_string(rPln.normal,2).c_str(), rPln.dist, tex_id );
		fputs(acStr, fp);
		for(j=0; j<rFace.GetNumVertices(); j++)
		{
			v = rFace.GetMAPVERTEX(j);
//			D3DXVec3toStr(acVec, v.vPosition, 1);
			fprintf(fp, "   pos%s  tex(%.2f, %.2f)\n",
				to_string(v.vPosition,2).c_str(), v.vTex0.u, v.vTex0.v);
		}

	}
  }

	// write plane data
	fputs("\n----------planes----------\n", fp);
	for(j=0; j<m_aPlane.size(); j++)
	{
		SPlane& rPlane = m_aPlane[j];
//		D3DXVec3toStr(acVec, rPlane.normal, 2);
		fprintf(fp, "[%d] n%s, d=%.2f\n", j, to_string(rPlane.normal,2).c_str(), rPlane.dist );
	}

	fclose(fp);

}


bool CBSPMapData::LoadMaterialNames( const string& strMaterialListFile )
{
	FILE* fp = fopen( strMaterialListFile.c_str(), "r");

	if(!fp)
		return false;

	char acLine[1024], acSlag[256], acName[256];
	while( fgets(acLine, 1024, fp) )
	{
		if( strncmp(acLine, "NAME", 4) == 0 )
		{
			sscanf( acLine, "%s %s\n", acSlag, acName );
			m_vecMaterial.push_back( CSurfaceMaterialData() );
			m_vecMaterial.back().strName = acName;
		}
	}
	
	fclose(fp);
	return true;

}


/*
void CBSPMapData::SetLocalLightDirections()
{
	vector<CMapFace> *apvecMapFace[2];	// array of pointers to vectors
	apvecMapFace[0] = &m_aMainFace;
	apvecMapFace[1] = &m_aInteriorFace;
	m_aMainFace.size();
	int iNumLights = m_vecpLight.size();
	CPointLight *pPLight;
	int t, iFace, iNumFaces;
	int i,j;
	float fDistToSurface, fLightRange;

	// initialize light direction vector for each vertex; 
	for( t=0; t<2; t++ )
	{
		iNumFaces = apvecMapFace[t]->size();
		for( iFace=0; iFace<iNumFaces; iFace++ )
		{
			CMapFace& rFace = apvecMapFace[t]->at(iFace);
			rFace.m_vecvLightDir.clear();
			for( j=0; j<rFace.GetNumVertices(); j++ )
				rFace.m_vecvLightDir.push_back( D3DXVECTOR3(0,0,0) );
		}
	}


	for( i=0; i<iNumLights; i++ )
	{
		pPLight = dynamic_cast<SPointLight *> (m_vecpLight[i]);
		if(!pPLight)
			continue;	// at present, only the point lights are supported

//		fLightRange = pPLight->fRange;
		fLightRange = 25.0f;
		for( t=0; t<2; t++ )
		{
			iNumFaces = apvecMapFace[t]->size();
			for( iFace=0; iFace<iNumFaces; iFace++ )
			{
				CMapFace& rFace = apvecMapFace[t]->at(iFace);

				fDistToSurface = D3DXVec3Dot( &pPLight->vPosition, &rFace.GetPlane().normal ) - rFace.GetPlane().dist;

				if( fDistToSurface < 0 || fLightRange < fDistToSurface )
					continue;	// light is either behind the polygon or not reaching the polygon

				for( j=0; j<rFace.GetNumVertices(); j++ )
				{
					MAPVERTEX& v = rFace.GetMapVertex(j);
					D3DXVECTOR3 vVertexToLight = pPLight->vPosition - v.vPosition;
					float fVertexToLight = D3DXVec3Length( &vVertexToLight );
					if( fLightRange * 2.0f < fVertexToLight )
						continue;	// TODO: more presice distance check

//					D3DXVec3Normalize( &vVertexToLight, &vVertexToLight );
					vVertexToLight /= fVertexToLight;

					// calculate intensity so that lights have different influences
					// according to distance and attenuation
					float fIntensity = 1.0f / ( pPLight->fAttenuation0
						                      + pPLight->fAttenuation1 * fVertexToLight
											  + pPLight->fAttenuation2 * fVertexToLight * fVertexToLight );

					// add the direction to the light
					rFace.m_vecvLightDir[j] += vVertexToLight * fIntensity;
				}
			}
		}
	}

	// normalize each light direction vector
	D3DXVECTOR3 avAxis[2];	// local axes of each face. avAxis[0] & [1] are parallel to the directions of u & v of decal texture
	D3DXVECTOR3 vLightDir_Local;	// surface to light direction in local space of a face
	D3DXVECTOR3 vNormal;
	MAPVERTEX v, v01, v02;
	D3DXMATRIX matLocal;
	D3DXMatrixIdentity( &matLocal );
	for( t=0; t<2; t++ )
	{
		iNumFaces = apvecMapFace[t]->size();
		for( iFace=0; iFace<iNumFaces; iFace++ )
		{
			CMapFace& rFace = apvecMapFace[t]->at(iFace);
			vNormal = rFace.GetPlane().normal;

			v01 = rFace.GetMapVertex(1) - rFace.GetMapVertex(0);
			v02 = rFace.GetMapVertex(2) - rFace.GetMapVertex(0);
			if( fabs(v01.tv) < 0.00001 )
			{
				D3DXVec3Normalize( &avAxis[0], &v01.vPosition );
				if( v01.tu < 0 )
					avAxis[0] *= -1;
				D3DXVec3Cross( &avAxis[1], &vNormal, &avAxis[0] );
			}

			else if( fabs(v01.tu) < 0.00001 )
			{
				D3DXVec3Normalize( &avAxis[1], &v01.vPosition );
				if( v01.tv < 0 )
					avAxis[1] *= -1;
				D3DXVec3Cross( &avAxis[0], &avAxis[1], &vNormal );
			}

			else
			{
//				v01 = v01 * v02.tu * v01.tu;
				v01 = v01 * v02.tu / v01.tu;
				avAxis[1] = v02.vPosition - v01.vPosition;
				if( v02.tv - v01.tv < 0 )
					avAxis[1] *= -1;
				D3DXVec3Normalize( &avAxis[1], &avAxis[1] );
				D3DXVec3Cross( &avAxis[0], &avAxis[1], &vNormal );
			}

			matLocal._11 = avAxis[0].x; matLocal._11 = avAxis[1].x; matLocal._11 = vNormal.x;
			matLocal._21 = avAxis[0].y; matLocal._21 = avAxis[1].y; matLocal._21 = vNormal.y;
			matLocal._31 = avAxis[0].z; matLocal._31 = avAxis[1].z; matLocal._31 = vNormal.z;

			for( j=0; j<rFace.GetNumVertices(); j++ )
			{
				v = rFace.GetMAPVERTEX(j);

				D3DXVec3Normalize( &rFace.m_vecvLightDir[j], &rFace.m_vecvLightDir[j] );

				D3DXVec3TransformCoord( &vLightDir_Local, &rFace.m_vecvLightDir[j], &matLocal );

				vLightDir_Local += D3DXVECTOR3(1,1,1);
				vLightDir_Local *= 0.5f;
				v.color = D3DCOLOR_XRGB( ((int)(vLightDir_Local.x * 255.0f)),
				                         ((int)(vLightDir_Local.y * 255.0f)),
										 ((int)(vLightDir_Local.z * 255.0f)) );

				rFace.SetMAPVERTEX( j, v );
			}
		}
	}
}
*/