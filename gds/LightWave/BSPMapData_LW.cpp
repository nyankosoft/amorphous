#include "BSPMapData_LW.hpp"

#include "../BSPMapCompiler/mapcompiler.hpp"
#include "../BSPMapCompiler/lightmapbuilder.h"
#include "../Stage/StaticGeometry.hpp"
#include "LightWaveSceneLoader.hpp"
#include "3DMeshModelBuilder_LW.hpp"

#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;

#include "Support/FilenameOperation.h"

#include <string.h>


namespace amorphous
{

//================================================================================
// CBSPMapData_LW::Methods()                                     - CBSPMapData_LW
//================================================================================


int CBSPMapData_LW::LoadSpecificMapDataFromFile( const char *pFilename )
{
	int i;
	bool bResult = false;
	CLightWaveSceneLoader lightwave_scene;

	// load the LightWave scene file
	lightwave_scene.LoadFromFile( pFilename );

//	MessageBox(NULL, "LW scene file has been loaded", "progress report", MB_OK);


	//First, we have to get the name of the mapfile in this "*.lws" file
	//The name of the mapfile has to be "_MAP_*.lwo" (* is can be an arbitrary string)

	// find the object file that has the filename starting with "_MAP_"
	CLWS_ObjectLayer* pObjectLayer = NULL;
//	char* pLWOFileName;
//	string strBodyFilename;
	char acBodyFilename[512];
	for( i=0; i<lightwave_scene.GetNumObjectLayers(); i++ )
	{
		pObjectLayer = lightwave_scene.GetObjectLayer(i);

		if( !pObjectLayer )
			MessageBox(NULL, "invalid object layer", "error", MB_OK|MB_ICONWARNING);

//		pLWOFileName = pObjectLayer->GetObjectFilename().c_str();
		string& strLWOFilename = pObjectLayer->GetObjectFilename();

		CFileNameOperation::GetBodyFilenameBySlash( acBodyFilename, strLWOFilename.c_str() );

		//find lwo2 file that represents the map object.
		if( strncmp( acBodyFilename, "_MAP_", 5 ) == 0 )
		{
			// load the object
			bResult = m_LWO2Object.LoadLWO2Object(acBodyFilename);
			break;
		}
	}

	// compute normals on each face(polygon) in the LightWave object 
//	m_LWO2Object.ComputeFaceNormals();


	string log_filename = "DebugInfoFile\\lwo2_loaded_data.txt";
	m_LWO2Object.WriteDebug(log_filename.c_str());

	list<CLWO2_Layer>::iterator itrLayer;

	this->m_aPlane.reserve(DEFAULT_NUM_PLANES);
	this->m_aMainFace.reserve(DEFAULT_NUM_MAINFACES);
	this->m_aInteriorFace.reserve(DEFAULT_NUM_MAINFACES);

//	itrLayer = m_LWO2Object.m_layer.begin();
	list<CLWO2_Layer>& lstLayer = m_LWO2Object.GetLayer();
	itrLayer = lstLayer.begin();
	for(; itrLayer != lstLayer.end() ; itrLayer++)
	{
		if( itrLayer->GetName() == "LYR_Slag" )
			continue;

		if( itrLayer->GetName() == "LYR_Main" )
			SetFace(&m_aMainFace, itrLayer);

		if( itrLayer->GetName() == "LYR_Interior" )
			SetFace(&m_aInteriorFace, itrLayer);

		if( itrLayer->GetName() == "LYR_Skybox" )
			SetFace(&m_aSkyboxFace, itrLayer);

		if( itrLayer->GetName() == "LYR_EnvLight" )
		{
			C3DMeshModelBuilder_LW mesh_builder( &m_LWO2Object );
			mesh_builder.BuildMeshFromLayer( *itrLayer );
			m_EnvLightMesh = mesh_builder.GetArchive();
		}
/*		if( itrLayer->GetName() == "LYR_BoundingVolume" )
			SetFace;
		if( itrLayer->GetName() == "LYR_NoClip" )
			SetFace;*/
	}

	// Sort textures and others
	SetTextureFilename();

	// set texture indices
	SetSurface();

	// create additional filenames to support fake bumpy textures
	SetFakeBumpTextures();

	//convert lightwave fog into generic fog
	CLWS_Fog* pLWSFog = lightwave_scene.GetFog();
	if( pLWSFog )
	{
		m_pFog = new SFog;
		m_pFog->cFogType = (char)pLWSFog->iType;
		m_pFog->fMinDist = pLWSFog->fMinDist;
		m_pFog->fMaxDist = pLWSFog->fMaxDist;
		m_pFog->fMinAmount = pLWSFog->fMinAmount;
		m_pFog->fMaxAmount = pLWSFog->fMaxAmount;
		m_pFog->color.fRed   = pLWSFog->afColor[0];	// red
		m_pFog->color.fGreen = pLWSFog->afColor[1];	// green
		m_pFog->color.fBlue  = pLWSFog->afColor[2];	// blue
	}

	// convert lightwave lights into generic lights
	SetLight( lightwave_scene );

	// for occlusion testing
	CreateTriangleMesh();

//	SetUVsForLightmaps();

	return 1;

}


void CBSPMapData_LW::CreateTriangleMesh()
{
	m_vecMesh.resize(1);
	C3DMeshModelArchive &archive = m_vecMesh[0];
	CMMA_VertexSet& vert_set = archive.GetVertexSet();
	vector<unsigned int>& index_buffer = archive.GetVertexIndex();

	vector<CMapFace> *apFaceBuffer[2] = { &m_aMainFace, &m_aInteriorFace };
	for( int i=0; i<2; i++ )
	{
		vector<CMapFace>& vecFace = *apFaceBuffer[i];
		size_t j, num_faces = vecFace.size();
		for( j=0; j<num_faces; j++ )
		{
			CMapFace& face = vecFace[j];

			if( face.ReadTypeFlag(CMapFace::TYPE_INVISIBLE) )
				continue;

			int index_offset = vert_set.vecPosition.size();

			int k, num_verts = face.GetNumVertices();
			for( k=0; k<num_verts; k++ )
			{
				vert_set.vecPosition.push_back( face.GetVertex(k) );
				vert_set.vecNormal.push_back( face.GetMAPVERTEX(k).vNormal );
			}

			int num_triangles = num_verts - 2;
			for( k=0; k<num_triangles; k++ )
			{
				index_buffer.push_back( index_offset );
				index_buffer.push_back( index_offset + k + 1 );
				index_buffer.push_back( index_offset + k + 2 );
			}
		}
	}

	vert_set.m_VertexFormatFlag
		= CMMA_VertexSet::VF_POSITION
		| CMMA_VertexSet::VF_COLORVERTEX
		| CMMA_VertexSet::VF_NORMAL;

	vert_set.vecDiffuseColor.resize( vert_set.vecPosition.size(), SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// triangle set
	CMMA_TriangleSet tri_set;
	tri_set.m_iMinIndex = 0;
	tri_set.m_iNumTriangles = index_buffer.size() / 3;
	tri_set.m_iStartIndex = 0;
	tri_set.m_iNumVertexBlocksToCover = vert_set.vecPosition.size();
	archive.GetTriangleSet().push_back( tri_set );

	// material
	archive.GetMaterial().push_back( CMMA_Material() );

	archive.UpdateAABBs();
}


void CBSPMapData_LW::SetVertexColor( MAPVERTEX& rvDestVertex, DWORD dwPointIndex, DWORD dwPolygonIndex,
									 CLWO2_VertexColorMap *pVertexColorMap )
{
	int i;
	for( i=0; i<pVertexColorMap->iNumIndices; i++ )
	{
		if( dwPointIndex == DWORD(pVertexColorMap->paVertexColor[i].iIndex) )
		{	// point indices matched
			rvDestVertex.color = pVertexColorMap->paVertexColor[i].GetColor_UINT4_ARGB();

			if( dwPolygonIndex == DWORD(pVertexColorMap->paVertexColor[i].iIndex) )
				break;
			else
				continue;	// continue searching in case there is a vertex color map with a corresponding polygon index (IS THIS CORRECT?)
		}
	}
}


int CBSPMapData_LW::GetMaterialIndex( CLWO2_Surface& rSurf )
{
	// find material name in the comment string of this surface
	string tag = "[MAT:";
	size_t iPos = rSurf.GetComment().find( tag );

	if( iPos == string::npos )
		return 0;	// material is not specified for this surface

	string strTemp, strMatName;
//	strTemp.assign( rSurf.GetComment().begin() + iPos + 5 );
	strTemp = rSurf.GetComment().substr( iPos + tag.length() );

	size_t i;
	for( i=0; i<strTemp.size(); i++ )
	{
		if( strTemp[i] == ']' )
		{
			strMatName.assign( strTemp, 0, i );
			break;
		}
	}
	if( i==strTemp.size() )
		return 0;	// invalid material name - terminating sign was not found

	for( i=0; i<m_vecMaterial.size(); i++ )
	{
		if( m_vecMaterial[i].strName == strMatName )
		{
			return i;
		}
	}

	return 0;	// material not found - return the index to the first material
}


void CBSPMapData_LW::SetFace(vector<CMapFace>* pvecFace, list<CLWO2_Layer>::iterator thislayer)
{
	int i;
	WORD j;
	DWORD pnt_index;
	MAPVERTEX vDest;
	SPlane plane;
	CLWO2_Object& lwobject = this->m_LWO2Object;
	CLWO2_TextureUVMap* pTexuvmap = NULL;
	CLWO2_VertexColorMap* pVertexColorMap = NULL;

	int iSurfaceIndex;

	int offset = pvecFace->size();

	// caution : this is a temporary solution. this will not work when multiple PNTS chunks exist
	// in a single layer.
	CLWO2_TextureUVMap *pLightmapTextureUVMap = lwobject.FindTextureUVMapByName( "TUV_Lightmap", *thislayer );

	vector<CLWO2_Face>& rvecPolygon = thislayer->GetFace();
	int iNumFaces = rvecPolygon.size();

	for(i=0; i<iNumFaces; i++)  // How many faces in the layer
	{
		CLWO2_Face& rSrcFace = rvecPolygon[i];

		if( rSrcFace.GetNumPoints() <= 2 )
			continue;

		CMapFace new_face;

		// Find the surface mapped on this face
		CLWO2_Surface& rSurf = lwobject.FindSurfaceFromTAG( rSrcFace.GetSurfaceIndex() );
		iSurfaceIndex = lwobject.GetSurfaceIndexFromSurfaceTAG( rSrcFace.GetSurfaceIndex() );

		strcpy( new_face.m_acSurfaceName, rSurf.GetName().c_str() );

		// set material index
		new_face.m_iSurfaceMaterialIndex = GetMaterialIndex( rSurf );


		// if the surface name contains "_NoClip"		
		if( strstr(rSurf.GetName().c_str(),"_NoClip") )
			new_face.RaiseTypeFlag(CMapFace::TYPE_NOCLIP);		// no collision detection against this face
		else
			new_face.ClearTypeFlag(CMapFace::TYPE_NOCLIP);	// usual case

		if( strstr(rSurf.GetName().c_str(),"_Invisible") )	//if the surface name contains "_Invisible"
			new_face.RaiseTypeFlag(CMapFace::TYPE_INVISIBLE);	// this face is not rendered
		else
			new_face.ClearTypeFlag(CMapFace::TYPE_INVISIBLE);	// usual case

		// find the image mapped on this face and get its index
		new_face.m_sTextureID = iSurfaceIndex;

		// The value of TextureID is given as the order of the still image 
		// in vector<m_clipstill> 
		// If a still image is the first element in vector<m_clipstill>, 
		// the TextureID of its owner face is 0

		// The image index of CLIP chunk in LWO2 file starts from 1. Therefore, if a surface doesn't
		// have any image, imagetag of the surface should be 0.  << Is this right?
		// And the 'm_sTextureID' remains -1

		// find the texture uvmap of this face	
		pTexuvmap = lwobject.FindTextureUVMapFromSurface(rSurf, ID_COLR, *thislayer);

		// find the vertex color map applied to this face		
		pVertexColorMap= lwobject.FindVertexColorMapFromSurface(rSurf, *thislayer);

		int iNumPoints = rSrcFace.GetNumPoints();
		for(j=0; j<iNumPoints ;j++)  // the number of points in this face
		{
			memset(&vDest, 0, sizeof(MAPVERTEX));
			vDest.color = 0xFFFFFFFF;	//default vertex color: white and opaque

			pnt_index = rSrcFace.GetVertexIndex()[j];   // the index to a point in the PNTS chunk
			vDest.vPosition = thislayer->GetVertex().at( pnt_index );

			// set texture-uv to the vertex
			// search uv map (VMAP chunk) until the corresponding uv-point is found
			if( pTexuvmap )  // if (pTexuvmap == NULL), we have no uv points for this face
			{
				if( !thislayer->GetUV( vDest.vTex0.u, vDest.vTex0.v, pnt_index, pTexuvmap ) )
				{
					vDest.vTex0.u = 32767;
					vDest.vTex0.v = 32767;
				}
			}

			new_face.m_sLightMapID = -1;
/*			if( pLightmapTextureUVMap )
			{
				if( !thislayer->GetUV( vDest.vTex1.u, vDest.vTex1.v, pnt_index, pLightmapTextureUVMap ) )
				{
					vDest.vTex1.u = -1;
					vDest.vTex1.v = -1;
				}
				else
					new_face.m_sLightMapID = 0;	// TODO: support for multiple lightmap textures
			}*/

			if( pVertexColorMap )
				SetVertexColor( vDest, pnt_index, (DWORD)i, pVertexColorMap );

			if( rSurf.GetMaxSmoothingAngle() < 3.141592f / 2.0f * 89.9f / 90.0f )
				vDest.vNormal = thislayer->GetInterpolatedNormal( rSrcFace, pnt_index );	// smooth shadeing
			else
				vDest.vNormal = rSrcFace.GetFaceNormal();	// flat shading

		//========== normal direction check (visual debugging) =============================================
//			vDest.color = D3DCOLOR_XRGB( abs((int)(vDest.vNormal.x * 250.0f)),
//				                         abs((int)(vDest.vNormal.y * 250.0f)),
//										 abs((int)(vDest.vNormal.z * 250.0f)) );
		//========== normal direction check (visual debugging) =============================================


			new_face.AddMAPVERTEX(vDest);
		}

		new_face.m_sNode = 0;
		new_face.m_bFlag = false;

		if( strstr(rSurf.GetName().c_str(),"_LightSource") )
		{	// mark as a light source
			new_face.RaiseTypeFlag( CMapFace::TYPE_LIGHTSOURCE );
			new_face.RaiseTypeFlag( CMapFace::TYPE_NOCLIP );		// light source faces are not used for collision detection - actually, this is not a good solution
		}


		//========== normal direction check =============================================
//		for(j=0; j<new_face.GetNumVertices()-1; j++)
//		{
//			if( 0.001f < D3DXVec3LengthSq( &(new_face.GetMapVertex(j).vNormal - new_face.GetMapVertex(j+1).vNormal) ) )
//				int iSmoothShadingPolygon = 1;
//		}
		//========== normal direction check =============================================


		pvecFace->push_back( new_face );

	}
}

// Set the name of the texture files
void CBSPMapData_LW::SetTextureFilename()
{
	short i;
	CSGA_TextureFile texfile;

	this->m_vecTextureFile.clear();

	vector<CLWO2_StillClip>& rvecStillClip = m_LWO2Object.GetStillClip();
	int iNumStillClips = rvecStillClip.size();
	for(i=0; i<iNumStillClips; i++)
	{
		memset(&texfile, 0, sizeof(CSGA_TextureFile));
		strcpy( texfile.acFilename, rvecStillClip[i].GetName().c_str() );
		this->m_vecTextureFile.push_back(texfile);
	}
}


// Set the name of the texture files
void CBSPMapData_LW::SetSurface()
{
	size_t i, iNumSurfaces = m_LWO2Object.GetSurface().size();

	m_vecSurface.resize( iNumSurfaces );

	vector<CLWO2_StillClip>& rvecStillClip = m_LWO2Object.GetStillClip();
	int j, iNumStillClips = rvecStillClip.size();
	UINT4 image_tag;
	CLWO2_SurfaceBlock *pBlock;


	// find the index for the surface texture
	for(i=0; i<iNumSurfaces; i++)
	{
		CLWO2_Surface& rSurf = m_LWO2Object.GetSurface()[i];
		pBlock = rSurf.GetSurfaceBlockByChannel( ID_COLR );

		if( pBlock )
		{
			image_tag = pBlock->GetImageTag();
			for( j=0; j<iNumStillClips; j++ )
			{
				if( rvecStillClip[j].GetClipIndex() == image_tag )
				{
					m_vecSurface[i].ColorTexIndex = j;
					break;
				}
			}

			if( j == iNumStillClips )
				m_vecSurface[i].ColorTexIndex = -1;	// texture not found
		}
		else
		{	// the texture was not found
			m_vecSurface[i].ColorTexIndex = -1;
		}

		// find the index for normal map textrue
		pBlock = rSurf.GetSurfaceBlockByChannel( ID_BUMP );
		
		if( pBlock )
		{
			image_tag = pBlock->GetImageTag();
			for( j=0; j<iNumStillClips; j++ )
			{
				if( rvecStillClip[j].GetClipIndex() == image_tag )
				{
					m_vecSurface[i].NormalMapTexIndex = j;
					break;
				}
			}

			if( j == iNumStillClips )
				m_vecSurface[i].NormalMapTexIndex = -1;	// texture not found
		}
		else
		{	// the texture was not found
			m_vecSurface[i].NormalMapTexIndex = -1;
		}

	
	}
}


void CBSPMapData_LW::SetFakeBumpTextures()
{
	size_t i, num_surfs = m_vecSurface.size();
	string filename, colortex_filename, fakebumptex_filename;
	size_t pos;
	for( i=0; i<num_surfs; i++ )
	{
		CSG_Surface& rSurface = m_vecSurface[i];

		if( m_vecTextureFile.size() <= rSurface.ColorTexIndex )
			continue; // this happens when no surface has an image file for texture

		filename = m_vecTextureFile[ rSurface.ColorTexIndex ].acFilename;

		pos = filename.find( "_FB" );
		if( pos != string::npos )
		{
			// the filename contains substring "_FB"
			// - the image file is supposed to be used as a fake bump texture
			//   strip the "_FB" from the filename and create a filename for color texture
			colortex_filename = filename.substr( 0, pos ) + filename.substr( pos + 3, 64 );

			rSurface.FakeBumpTexIndex = rSurface.ColorTexIndex;

			rSurface.ColorTexIndex = m_vecTextureFile.size();

			m_vecTextureFile.push_back( CSGA_TextureFile() );
			strcpy( m_vecTextureFile.back().acFilename, colortex_filename.c_str() );
		}
		else // if( support fake bump texture )
		{
			if( rSurface.NormalMapTexIndex == -1 )
			{
				// no normal map texture is assigned to this surface
				// use the color texture file as the fake bump texture
				rSurface.FakeBumpTexIndex = rSurface.ColorTexIndex;
			}
			else
			{
				// create a new filename for fake bump texture by adding "_FB" to the source filaneme
				fakebumptex_filename = filename;
				CFileNameOperation::AppendStringToBodyFilename( fakebumptex_filename, "_FB" );

				rSurface.FakeBumpTexIndex = m_vecTextureFile.size();

				m_vecTextureFile.push_back( CSGA_TextureFile() );
				strcpy( m_vecTextureFile.back().acFilename, fakebumptex_filename.c_str() );
			}
		}
	}
}

// load point & distant lights from light wave scene file
// distant lights in the scene is loaded as directional lights
void CBSPMapData_LW::SetLight( CLightWaveSceneLoader& rLightWaveScene )
{
	float* pafAmbientColor = rLightWaveScene.GetAmbientColor();
	float fAmbientIntensity = rLightWaveScene.GetAmbientIntensity();

	if( pafAmbientColor[0] != 0 || pafAmbientColor[1] != 0 || pafAmbientColor[2] != 0 )
	{
		if( fAmbientIntensity != 0 )
		{
			CAmbientLight* pAmbientLight = new CAmbientLight;
			pAmbientLight->fIntensity   = fAmbientIntensity;
			pAmbientLight->Color.fRed   = pafAmbientColor[0];
			pAmbientLight->Color.fGreen = pafAmbientColor[1];
			pAmbientLight->Color.fBlue  = pafAmbientColor[2];

			this->m_vecpLight.push_back( pAmbientLight );
		}
	}

	float* pafColor;
	CPointLight* pPointLight;
	CDirectionalLight* pDirLight;
	Matrix33 matOrient;
	for(int i=0; i<rLightWaveScene.GetNumLights(); i++)
	{
		CLWS_Light* pLWS_Light = NULL;
		pLWS_Light = rLightWaveScene.GetLight(i);
		switch( pLWS_Light->GetType() )
		{
		case CLWS_Light::TYPE_POINT:
			pPointLight = new CPointLight;
			pafColor = pLWS_Light->GetColor();
			pPointLight->Color.fRed   = pafColor[0];
			pPointLight->Color.fGreen = pafColor[1];
			pPointLight->Color.fBlue  = pafColor[2];
			pPointLight->fIntensity   = pLWS_Light->GetIntensity();
			pPointLight->vPosition = pLWS_Light->GetPositionAt( 0 );

			this->m_vecpLight.push_back( pPointLight );

			break;

		case CLWS_Light::TYPE_DISTANT:
			pDirLight = new CDirectionalLight;
			pafColor = pLWS_Light->GetColor();
			pDirLight->Color.fRed   = pafColor[0];
			pDirLight->Color.fGreen = pafColor[1];
			pDirLight->Color.fBlue  = pafColor[2];
			pDirLight->fIntensity   = pLWS_Light->GetIntensity();
			pDirLight->vPseudoPosition = pLWS_Light->GetPositionAt( 0 );
			pLWS_Light->GetOrientationAt( 0, matOrient );
			pDirLight->vDirection = matOrient.GetColumn(2);

			this->m_vecpLight.push_back( pDirLight );

			break;

		default:
			break;	// ignore lights other that point / distant

		}

	}

}

} // amorphous
