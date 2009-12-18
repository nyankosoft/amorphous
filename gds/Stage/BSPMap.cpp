#include "BSPMap.hpp"

#include "Stage/Stage.hpp"
#include "Stage/EntitySet.hpp"
#include "Stage/trace.hpp"
#include "Stage/ScreenEffectManager.hpp"

#include "DynamicLightManagerForStaticGeometry.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/Direct3D/TextureTool.hpp"
#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "JigLib/JL_PhysicsManager.hpp"
#include "JigLib/JL_ShapeDesc_TriangleMesh.hpp"
#include "JigLib/JL_PhysicsActor.hpp"

#include "Support/memory_helpers.hpp"
//#include "PerformanceCheck.h"


using namespace std;


static const string gs_DefaultShaderFilename("Stage/BSPMapShader.fx");


CBSPMap::CBSPMap( CStage *pStage )
:
CStaticGeometryBase( pStage ),
m_pTriangleMesh(NULL),
m_pShaderManager(NULL),
m_paPolygon(NULL),
m_paTriangleSet(NULL),
m_paCellData(NULL),
m_pasVisibleCellIndex(NULL),
m_pacCurrentlyVisibleCell(NULL),
m_pVertex(NULL),
m_pVB(NULL),
m_pIB(NULL),
m_paFCModel(NULL)
{
	memset(&this->m_Fog, 0, sizeof(SFog));
	m_Fog.cFogType = 0;		// (cFogType == 0) means "No Fog"

	m_pDynamicLightManager = NULL;
	m_pLightmap_PL = NULL;
	m_pNormalmap_PL = NULL;

	for( int i=0; i<NUM_EXTRA_TEXTURES; i++)
		m_apExtraTexture[i] = NULL;

	m_pStaticGeometryVertexDecleration = NULL;

	m_aShaderTechHandle[ST_BUMP].SetTechniqueName( "StaticGeometry" );
	m_aShaderTechHandle[ST_NO_BUMP].SetTechniqueName( "StaticGeometry_Bump" );
}


CBSPMap::~CBSPMap()
{
	ReleaseGraphicsResources();

	SafeDelete( m_pDynamicLightManager );
}


void CBSPMap::ReleaseGraphicsResources()
{
	// triangle mesh is destroyed by physics manager so don't delete it here
//	DontSafeDeleteThis( m_pTriangleMesh );

	SafeDeleteArray(m_paPolygon);
	SafeDeleteArray(m_paTriangleSet);
	SafeDeleteArray(m_paCellData);
	SafeDeleteArray(m_pasVisibleCellIndex);
	SafeDeleteArray(m_pacCurrentlyVisibleCell);
	SafeDeleteArray( m_paFCModel );

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	int i;
/*	int iNumSurfaces = m_vecSurface.size();
	for(i=0; i<iNumSurfaces; i++)
	{
		SAFE_RELEASE( m_vecSurface[i].pTexture );
		SAFE_RELEASE( m_vecSurface[i].pNormalMap );
	}
*/

	m_vecSurface.clear();

	int iNumTextures = m_vecTextureCache.size();
	for( i=0; i<iNumTextures; i++ )
	{
		SAFE_RELEASE( m_vecTextureCache[i].pTexture );
	}
	m_vecTextureCache.clear();

	int iNumLightmaps = m_vecLightmap.size();
	for(i=0; i<iNumLightmaps; i++)
	{
		SAFE_RELEASE( m_vecLightmap[i].pTexture );
		SAFE_RELEASE( m_vecLightmap[i].pLightDirMap );
	}

	for(i=0; i<NUM_EXTRA_TEXTURES; i++)
		SAFE_RELEASE( m_apExtraTexture[i] );

	m_vecLightmap.clear();

	SAFE_RELEASE( m_pLightmap_PL );
	SAFE_RELEASE( m_pNormalmap_PL );

	SAFE_RELEASE( m_pStaticGeometryVertexDecleration );
}


void CBSPMap::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	if( m_strFilename.length() == 0 )
		return;

	// load the current map file again
	LoadFromFile( m_strFilename.c_str(), true );
}


bool CBSPMap::LoadFromFile( const std::string& filename, bool bLoadGraphicsOnly )
{
	FILE* fp = fopen(filename.c_str(), "rb");  // binary reading mode

	if( !fp )
	{
		LOG_PRINT_ERROR( " Cannot open the map flie: " + filename );
		return false;
	}

	// release the current map
	ReleaseGraphicsResources();

	m_strFilename = filename;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	SBSPFileHeader header;
	MAPVERTEX* pTempVB = NULL;
	short* pTempIB = NULL;
	int i,j;
	size_t texfile_bytes_read = 0;
	DWORD *padwTexData = NULL;
	HRESULT hr = 0;
	int iNumTextures, iTexWidth;

	SNode_f *paNode = NULL;
	SPlane *paPlane = NULL;
	DWORD num_nodes = 0, num_planes = 0;

	vector<Vector3> vecvVertex;
	vector<int> veciTriangleIndex;
	vector<int> veciMaterialIndex;
	vector<short> vecsMaterialIndex;
	bool b;
	size_t bytes_read = 0;
	char acFilename[512];

	int iNumSurfaces;
	CSG_Surface surface;

//	CJL_ShapeDesc_TriangleMesh mesh_desc;
//	CJL_PhysicsActorDesc desc;
//	CJL_PhysicsActor *pActor;

	m_vecLightmap.clear();
	m_vecSurface.clear();

	while(1)
	{
		SDataTAGLump dtag;
		bytes_read = fread(&dtag, sizeof(SDataTAGLump), 1, fp);
		if(bytes_read == 0)
			break;  //reached the end of the file;

		switch(dtag.dwTAG)
		{
		case TAG_HEADER:
			//Confirm the version of this bspfile
			fread(&header, dtag.dwSize, 1, fp);
			if( header.version != BSPFILE_VERSION )
			{
				LOG_PRINT_ERROR( "bsp-file version doesn't match" );
				fclose(fp);
				return false;
			}
			break;

		case TAG_FVFVERTEX:
			// first, read the vertex data to the temporary vertex buffer
			m_iNumVertices = dtag.dwSize / sizeof(MAPVERTEX);
			pTempVB = new MAPVERTEX [ m_iNumVertices ];
			fread(pTempVB, dtag.dwSize, 1, fp);
			// next, Create the D3D Vertex Buffer
			hr = pd3dDev->CreateVertexBuffer(dtag.dwSize, 0, 0, D3DPOOL_MANAGED, &m_pVB, NULL);
//			hr = pd3dDev->CreateVertexBuffer(dtag.dwSize, 0, D3DFVF_MAPVERTEX,
//				D3DPOOL_MANAGED, &m_pVB, NULL);
			if( FAILED(hr) )
				return false;

			VOID* pVBData;
			//Third, put the vertex data to the D3D Vertex Buffer
		    if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVBData, 0 ) ) )
				return false;
			memcpy(pVBData, pTempVB, dtag.dwSize);
			m_pVB->Unlock();
			delete [] pTempVB;

//			CSG_Polygon::s_iNumTotalVertices = m_iNumVertices;
			break;

		case TAG_FVFINDEX:
			// 1. read the index data to the temporary index buffer
			m_iNumIndices = dtag.dwSize / sizeof(short);
			pTempIB = new short [ m_iNumIndices ];
			fread(pTempIB, dtag.dwSize, 1, fp);

			// 2. create the D3D Index Buffer
			hr = pd3dDev->CreateIndexBuffer(dtag.dwSize, 0, D3DFMT_INDEX16,
				D3DPOOL_MANAGED, &m_pIB, NULL);
			if( FAILED(hr) )
			{
				b = (hr == D3DERR_INVALIDCALL);
				b = (hr == D3DERR_OUTOFVIDEOMEMORY);
				b = (hr == E_OUTOFMEMORY);
				return false;
			}
			unsigned short* pusIBData;

			// 3. put the index data to the D3D Index Buffer
		    if( FAILED( m_pIB->Lock( 0, 0, (VOID**)&pusIBData, 0 ) ) )
				return false;

			memcpy(pusIBData, pTempIB, dtag.dwSize);
			m_pIB->Unlock();
			delete [] pTempIB;
			break;

		case TAG_POLYGON:
			m_iNumPolygons = dtag.dwSize / sizeof(CSG_Polygon);
			m_paPolygon = new CSG_Polygon [ m_iNumPolygons ];
			fread(m_paPolygon, dtag.dwSize, 1, fp);
			for( i=0; i<m_iNumPolygons; i++ )
			{
				if( m_paPolygon[i].sIndexOffset < 0 )
				{
					CSG_Polygon& rPolygon = m_paPolygon[i];
					char acStr[128];
					sprintf( acStr, "static geometry with invalid vertex indices\npol index: %d\nrPolygon.sIndexOffset: %d", i, rPolygon.sIndexOffset );
					MessageBox( NULL, acStr, "error", MB_OK|MB_ICONWARNING );
				}
			}
			break;

		case TAG_TRIANGLESET:
			m_iNumTriangleSets = dtag.dwSize / sizeof(CSG_TriangleSet);
			m_paTriangleSet = new CSG_TriangleSet [ m_iNumTriangleSets ];
			fread(m_paTriangleSet, dtag.dwSize, 1, fp);
			break;

		case TAG_PLANE:
			num_planes = dtag.dwSize / sizeof(SPlane);
			paPlane = new SPlane [num_planes];
			fread(paPlane, dtag.dwSize, 1, fp);
			break;

		case TAG_BSPTREENODE:
			num_nodes = dtag.dwSize / sizeof(SNode_f);
			paNode = new SNode_f [num_nodes];
			fread(paNode, dtag.dwSize, 1, fp);
			break;

		case TAG_BSPCELL:
			m_iNumCells = dtag.dwSize / sizeof(CSG_Cell);
			m_paCellData = new CSG_Cell [ m_iNumCells ];
			fread(m_paCellData, dtag.dwSize, 1, fp);
			m_pacCurrentlyVisibleCell = new char [ m_iNumCells ];
			memset( m_pacCurrentlyVisibleCell, 0, sizeof(char) * m_iNumCells );
			break;

		case TAG_VISCELLINDEX:
			m_pasVisibleCellIndex = new short [dtag.dwSize / sizeof(short)];
			fread(m_pasVisibleCellIndex, dtag.dwSize, 1, fp);
			break;

		case TAG_SURFACE:
			iNumSurfaces = dtag.dwSize / sizeof(CSG_Surface);
			m_vecSurface.resize( iNumSurfaces );
			for( i=0; i<iNumSurfaces; i++ )
			{
				fread( &surface, sizeof(CSG_Surface), 1, fp );
				m_vecSurface[i] = surface;
			}
			break;

		case TAG_TEXTUREFILE:
			texfile_bytes_read=0;

			m_vecTextureCache.clear();
			while( texfile_bytes_read < (size_t)dtag.dwSize )
			{
				fgets(acFilename, 255, fp);
				texfile_bytes_read += strlen(acFilename);
				acFilename[strlen(acFilename) - 1] = '\0';  // change '\n' into '\0'

				m_vecTextureCache.push_back( CSG_TextureCache() );

				m_vecTextureCache.back().strFilename = acFilename;
			}

			break;

		case TAG_LIGHTMAPTEXTURE:
			CSG_LightmapTextureInfo lmpinfo;

			// first, we should check supported texture formats

			// get the information on lightmap texture
			fread(&lmpinfo, sizeof(CSG_LightmapTextureInfo), 1, fp);
			iNumTextures = lmpinfo.iNumLightmapTextures;
			iTexWidth = lmpinfo.iLightmapTextureWidth;

			m_vecLightmap.resize( iNumTextures );

//			m_aLightmapTexture.reserve( iNumTextures );
			for(i=0; i<iNumTextures; i++)
			{
				hr = D3DXCreateTexture(pd3dDev, iTexWidth, iTexWidth, 0, 0,
				D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_vecLightmap[i].pTexture);
				if( FAILED(hr) ) return false;
				D3DLOCKED_RECT locked_rect;
				hr = (m_vecLightmap[i].pTexture)->LockRect( 0, &locked_rect, NULL, 0);	//Lock and get the pointer to the first texel of the texture
				D3DCOLOR* pARGBColor = (DWORD *)locked_rect.pBits;
				BYTE red, green, blue;
				// Load the lightmap data from bspx file to that texture 
				for(j=0; j<iTexWidth * iTexWidth; j++)
				{
					fread(&red,   sizeof(BYTE), 1, fp);
					fread(&green, sizeof(BYTE), 1, fp);
					fread(&blue,  sizeof(BYTE), 1, fp);
					pARGBColor[j] = D3DCOLOR_ARGB(0, red, green, blue);
				}
				(m_vecLightmap[i].pTexture)->UnlockRect(0);
				D3DXFilterTexture(m_vecLightmap[i].pTexture, NULL, 0, D3DX_FILTER_TRIANGLE);
			}
//			CSG_Polygon::s_paLightmap = m_aLightmapTexture;
			break;

		case TAG_LIGHTDIRECTIONTEXTURE:
			CSG_LightmapTextureInfo light_dir_texinfo;
			// get the information on light direction texture
			fread(&light_dir_texinfo, sizeof(CSG_LightmapTextureInfo), 1, fp);
			iNumTextures = light_dir_texinfo.iNumLightmapTextures;
			iTexWidth = light_dir_texinfo.iLightmapTextureWidth;

			// there should be as many light direction textures as lightmaps
			assert( m_vecLightmap.size() == iNumTextures );

			padwTexData = new DWORD [ iTexWidth * iTexWidth ];
			for(i=0; i<iNumTextures; i++)
			{
				fread( padwTexData, sizeof(DWORD), iTexWidth * iTexWidth, fp );
				CTextureTool::CreateTexture( padwTexData, iTexWidth, iTexWidth,
					                            &(m_vecLightmap[i].pLightDirMap) );
			}
			delete [] padwTexData;
			break;

		case TAG_FOG:
			fread(&m_Fog, sizeof(SFog), 1, fp);
			break;

		case TAG_BSPTREE_COLLISIONMODEL:
			m_BSPCollisionModel.LoadFromFile(fp);
			break;

		case TAG_TRIANGLEMESH:
			{
				CSG_TriMeshHeader mesh_header;
				fread( &mesh_header, sizeof(CSG_TriMeshHeader), 1, fp );

				int num_vertices = mesh_header.iNumVertices;
				int num_triangles = mesh_header.iNumTriangles;

				vecvVertex.resize( num_vertices );
				veciTriangleIndex.resize( num_triangles * 3);
				veciMaterialIndex.resize( num_triangles, 0 );

				for( i=0; i<num_vertices; i++ )
					fread( &vecvVertex[i], sizeof(D3DXVECTOR3), 1, fp );
//					fread( vecvVertex.begin(), sizeof(D3DXVECTOR3), num_vertices, fp );

				for( i=0; i<num_triangles * 3; i++ )
					fread( &veciTriangleIndex[i], sizeof(int), 1, fp );
//				fread( veciTriangleIndex.begin(), sizeof(int), num_triangles * 3, fp );

				for( i=0; i<num_triangles; i++ )
					fread( &veciMaterialIndex[i], sizeof(int), 1, fp );
//				fread( veciMaterialIndex.begin(), sizeof(int), num_triangles, fp );

				if( bLoadGraphicsOnly )
					break;	// no need to reload physics resources

				vecsMaterialIndex.reserve( num_triangles );
				for( i=0; i<num_triangles; i++ )
					vecsMaterialIndex.push_back( (short)veciMaterialIndex[i] );
/*
				mesh_desc.pvecvVertex        = &vecvVertex;
				mesh_desc.pveciIndex         = &veciTriangleIndex;
				mesh_desc.pvecsMaterialIndex = &vecsMaterialIndex;
#ifndef NDEBUG
				mesh_desc.OptimizeEdgeCollision = false;	
#endif
				desc.ActorFlag = JL_ACTOR_STATIC;
				desc.iCollisionGroup = ENTITY_COLL_GROUP_STATICGEOMETRY;
				desc.vecpShapeDesc.push_back( &mesh_desc );
				pActor = m_pStage->GetPhysicsManager()->CreateActor( desc );
				m_pTriangleMesh = (CTriangleMesh *)pActor->GetShape(0);*/
			}
			break;

		default:
			char chr;
			for(DWORD k=0; k<dtag.dwSize; k++)
				fread(&chr, 1, 1, fp);
			break;
		}
	}

	fclose(fp);


	// 'm_BSPTree' has 2 pointers and doesn't do any memory allocation itself
//	m_BSPTree.m_paNode = m_paNode;
//	m_BSPTree.m_paPlane = m_paPlane;
	if( 0 < num_nodes && 0 < num_planes )
		m_BSPTree.Init( paNode, num_nodes, paPlane, num_planes );

	// load textures for polygons
	LoadTextures();

	if( FAILED(D3DXCreateTextureFromFileA(pd3dDev, "Texture\\Lightmap_PL.dds", &m_pLightmap_PL)) )
		m_pLightmap_PL = NULL;

	if( FAILED(D3DXCreateTextureFromFileA(pd3dDev, "Texture\\Lightmap_PL_NM.dds", &m_pNormalmap_PL)) )
		m_pNormalmap_PL= NULL;

	// create a default bump map texture for flat, bumpless surface
	// alpha channel of the normal map, which represents specular map, will be set to 0 by default. 
//	DWORD dwColor = D3DCOLOR_XRGB(128, 128, 255);
	DWORD dwColor = D3DCOLOR_ARGB(0, 128, 128, 255);
	CTextureTool::CreateTexture( &dwColor, 1, 1, &m_apExtraTexture[EXTRA_TEX_DEFAULT_NORMALMAP] );

	// create default texture for polygons without texture
	dwColor = D3DCOLOR_XRGB(255,255,255);
	CTextureTool::CreateTexture( &dwColor, 1, 1, &m_apExtraTexture[EXTRA_TEX_DEFAULT_COLOR] );

	// create texture for tex specular map
//	D3DXCreateTextureFromFileA(pd3dDev, "Texture\\TexSpecular00.dds", &m_aExtraTexture[EXTRA_TEX_SPECULAR] );



	// vertex decleration for static geometry (map)
	pd3dDev->CreateVertexDeclaration( MAPVERTEX_DECLARATION, &m_pStaticGeometryVertexDecleration );


	// load shader
	m_pShaderManager = new CShaderManager();
	m_pShaderManager->LoadShaderFromFile( gs_DefaultShaderFilename );

	return true;
}


void CBSPMap::LoadTextures()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;

	int i, num_surfaces = m_vecSurface.size();
	int index;

	// set deafult textures for surfaces without texture
	TCFixedVector<CSG_TextureCache,NUM_MAX_TEXTURES>& rvecTexCache = m_vecTextureCache;

	for( i=0; i<num_surfaces; i++ )
	{
		if( /*m_bUseFakeBumpTextures*/ true )
		{
			index = m_vecSurface[i].FakeBumpTexIndex;
			if( 0 <= index && !(rvecTexCache[index].pTexture) )
			{
				hr = D3DXCreateTextureFromFileA( pd3dDev,
												 rvecTexCache[index].strFilename.c_str(),
												 &(rvecTexCache[index].pTexture) );
			}
		}
		else
		{
			index = m_vecSurface[i].ColorTexIndex;
			if( 0 <= index && !rvecTexCache[index].pTexture )
			{
				hr = D3DXCreateTextureFromFileA( pd3dDev,
												 rvecTexCache[index].strFilename.c_str(),
												 &(rvecTexCache[index].pTexture) );
			}

			index = m_vecSurface[i].NormalMapTexIndex;
			if( 0 <= index && !rvecTexCache[index].pTexture )
			{
				hr = D3DXCreateTextureFromFileA( pd3dDev,
												 rvecTexCache[index].strFilename.c_str(),
												 &(rvecTexCache[index].pTexture) );
			}
		}
	}

	/// set default textures

	DWORD dwColor;
	if( /*m_bUseFakeBumpTextures*/ true )
	{
		// add a default fake bumpy texture
		m_vecTextureCache.push_back( CSG_TextureCache() );
		dwColor = 0xFFFFFFFF;
		CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_vecTextureCache.back().pTexture) );

		for( i=0; i<num_surfaces; i++ )
		{
			if( m_vecSurface[i].FakeBumpTexIndex == -1 )
				m_vecSurface[i].FakeBumpTexIndex = m_vecTextureCache.size() - 1;
		}
	}
	else
	{
		// add default surface & normal map textures
		m_vecTextureCache.push_back( CSG_TextureCache() );
		dwColor = 0xFFFFFFFF;
		CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_vecTextureCache.back().pTexture) );

		m_vecTextureCache.push_back( CSG_TextureCache() );
		dwColor = D3DCOLOR_ARGB(0, 128, 128, 255);
		CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_vecTextureCache.back().pTexture) );

		// set deafult textures for surfaces without texture
		for( i=0; i<num_surfaces; i++ )
		{
			if( m_vecSurface[i].ColorTexIndex == -1 )
				m_vecSurface[i].ColorTexIndex = m_vecTextureCache.size() - 2;

			if( m_vecSurface[i].NormalMapTexIndex == -1 )
				m_vecSurface[i].NormalMapTexIndex = m_vecTextureCache.size() - 1;
		}
	}

}

void CBSPMap::WriteToFile( const char *pFilename )
{
	FILE* fp;
	fp = fopen(pFilename, "w");

	int i;

	// write the vertex data of the buffer
	MAPVERTEX* pVBData = NULL;
	m_pVB->Lock(0, 0, (VOID**)&pVBData, 0);
	fprintf(fp, "\nTotal %d vertices\n [i] position(x,y,z), texture(u,v), color\n", m_iNumVertices);

	int iNumVertices = m_iNumVertices;
	for(i=0; i<iNumVertices; i++)
	{
		MAPVERTEX* pv = (MAPVERTEX*)pVBData + i;
		fprintf(fp, " [%d] (%.2f, %.2f, %.2f), (%.2f, %.2f), %d\n",
			                        i, pv->vPosition.x, pv->vPosition.y, pv->vPosition.z, pv->vTex0.u, pv->vTex0.v, pv->color );
	}
	m_pVB->Unlock();

	// write the index data of the buffer
	fprintf(fp, "\nTotal %d indices\n [i] index_value\n", m_iNumIndices);
	unsigned short* pusIBData = NULL;
	m_pIB->Lock(0, 0, (VOID**)&pusIBData, 0);
	int iNumIndices = m_iNumIndices;
	for(i=0; i<iNumIndices; i++)
	{
		fprintf(fp, "[%d] %d\n", i, pusIBData[i]);
	}
	m_pIB->Unlock();

	// write the polygon data of the buffer
	fprintf(fp, "\nTotal %d polygons\n [i] index_offset, num_points\n", m_iNumPolygons);
	for(i=0; i<m_iNumPolygons; i++)
	{
		fprintf(fp, " [%d] %d, %d", i, m_paPolygon[i].sIndexOffset, m_paPolygon[i].sNumVertices);
	}

	fclose(fp);
}


//==========================================================================
//
//					'Collision Detection' Functions
//
//==========================================================================

short CBSPMap::CheckPosition(STrace& tr)
{
	short sCellIndex = m_BSPTree.CheckPosition( tr );

	if( sCellIndex == CONTENTS_SOLID )
		return CONTENTS_SOLID;

	tr.fFraction = 0.0f;
	ClipTraceToInteriorModel_r( tr, 0 );

	return sCellIndex;
}


int CBSPMap::ClipTrace(STrace &tr)
{
	if( tr.sTraceType & TRACETYPE_GET_MATERIAL_INFO )
//	if( true )
	{	// clip trace using triangle mesh (experiment)
		m_pTriangleMesh->ClipTrace( tr );
	}
	else
	{
		//Check the collision with this map
		m_BSPTree.ClipTrace( tr );

		//Check the collision with the fixed models in each cell
		ClipTraceToInteriorModel_r( tr, 0 );
	}

	return 0;
}


//works as a 'CheckPosition() ' when tr.fFraction == 0
void CBSPMap::ClipTraceToInteriorModel_r(STrace& tr, short nodeindex)
{
//	SNode_f* paNode = m_paNode;
//	SPlane* paPlane = m_paPlane;
	short sFrontChild, sBackChild;
	float fDist, fRadius;
	CBSPTree& bsptree = m_BSPTree;

	while(1)
	{
//		SNode_f& rThisNode = paNode[nodeindex];
		SNode_f& rThisNode = bsptree.GetNode(nodeindex);
		sFrontChild = rThisNode.sFrontChild;
		sBackChild = rThisNode.sBackChild;

		if(sFrontChild == CONTENTS_EMPTY)  //reached the cell
		{
			//Clip line using BSP-Tree if there is any fixed model in this cell.
			CSG_Cell& rThisCell = m_paCellData[rThisNode.sCellIndex];
			if( 0 < rThisCell.sNumInteriorPolygons &&
				tr.aabb_swept.IsIntersectingWith(rThisCell.aabb) )
			{
//				CBSPTree bsptree;
//				bsptree.m_paNode = paNode + rThisCell.sBSPTreeIndex;
//				bsptree.m_paPlane = paPlane;

				if( tr.aabb_swept.IsIntersectingWith( bsptree.GetNode(0).aabb ) )
				{	// AABB in the root node of bsp-tree represents the volume which contains this interior model
					// check intersection with the swept AABB volume of the trace
					if( tr.fFraction == 0 )
						bsptree.CheckPosition( tr );
					else
						bsptree.ClipTrace( tr );
				}
			}

			//Check the backchild
			if( sBackChild == CONTENTS_SOLID )	//leaf node
				return;
			else
			{	//the frontchild is emtpy-node but the backchild has sub-tree: this shouldn't happen.
				nodeindex = sBackChild;
				continue;
			}
		}
		else if(sBackChild == CONTENTS_SOLID)
		{
			nodeindex = sFrontChild;
			continue;
		}

		// we are at a diverging node in the BSP-Tree (The both child nodes have the sub-trees)
		SPlane& rPlane = bsptree.GetPlane( rThisNode );

		fDist = rPlane.GetDistanceFromPoint( tr.aabb_swept.GetCenterPosition() );
		fRadius = tr.aabb_swept.GetRadiusForPlane( rPlane );

		if( fRadius < fDist ) //The trace is in front of the plane
		{
			nodeindex = sFrontChild;
			continue;
		}
		else if( fDist < -fRadius ) //The trace is behind the plane
		{
			nodeindex = sBackChild;
			continue;
		}
		else  //As the object is crossing the plane, we have to check the both children (sub-spaces)
		{
//			prev_frac = tr.fFraction;
			ClipTraceToInteriorModel_r(tr, sFrontChild);
//			if( tr.fFraction < prev_frac )
//				return;		//The fore part of the line-segment has bumped into something. No need to clip the rear-line
			ClipTraceToInteriorModel_r(tr, sBackChild);
			return;
		}
	}
}


//==========================================================================
//
//						'Rendering' Functions
//
//==========================================================================

bool CBSPMap::Render( const CCamera &rCam, const unsigned int EffectFlag )
{
	STrace tr;
	tr.vEnd = rCam.GetPosition();
	tr.bvType = BVTYPE_DOT;

	// We regard the camera object as a simple dot.
	short sCurrentCell = CheckPosition( tr );

	if(sCurrentCell == CONTENTS_SOLID)
		return false;	// camera is in an invalid position

	// clear the previous cell visibility information
	memset( m_pacCurrentlyVisibleCell, 0, sizeof(char) * m_iNumCells );

	// turn on the visibility of the current cell
	m_pacCurrentlyVisibleCell[sCurrentCell] = 1;

	CSG_Cell* paCellData = m_paCellData;


	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	// turn on the Z-buffer
	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	// cull back faces with counterclockwise vertices
	pd3dDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// turn off alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	// turn off light
	// pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	// enable alpha test - do not draw a pixel if its nearly transparent
	pd3dDev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_ALPHAREF,  (DWORD)0x00000001 );
	pd3dDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	// draw a pixel if its alpha value is greater than or equal to '0x00000001'

	pd3dDev->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA );
	pd3dDev->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );

	// set geometry
	pd3dDev->SetVertexDeclaration( m_pStaticGeometryVertexDecleration );

	hr = pd3dDev->SetStreamSource(0, m_pVB, 0, sizeof(MAPVERTEX));
	if(FAILED(hr)) MessageBox(NULL, "SetStreamSource() failed.", "Error", MB_ICONWARNING|MB_OK);

	hr = pd3dDev->SetIndices( m_pIB );
	if(FAILED(hr)) MessageBox(NULL, "SetIndices() failed.",		"Error", MB_ICONWARNING|MB_OK);


	// set transform
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMatrixIdentity( &matWorld );
	rCam.GetCameraMatrix(matView);
	rCam.GetProjectionMatrix(matProj);

	Matrix44 view, proj;
	rCam.GetCameraMatrix(view);
	rCam.GetProjectionMatrix(proj);

	CShaderManager *pShaderManager = m_pShaderManager;
	LPD3DXEFFECT pEffect = NULL;
	UINT pass, cPasses;

	if( pShaderManager &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		if( EffectFlag & ScreenEffect::PseudoNightVision )
		{
			pShaderManager->SetTechnique( m_aShaderTechHandle[1] );
//			pShaderManager->SetTechnique( SHADER_TECH_STATICGEOMETRY_PNV );
		}
		else
		{
			pShaderManager->SetTechnique( m_aShaderTechHandle[0] );
//			pShaderManager->SetTechnique( SHADER_TECH_STATICGEOMETRY );
		}

		pShaderManager->SetWorldViewProjectionTransform( Matrix44Identity(), view, proj );

		pEffect->SetVector( "g_vEyePos", (D3DXVECTOR4 *)&rCam.GetPosition() );

		pEffect->CommitChanges();
	}
///	else
///	{
		pd3dDev->SetTransform(D3DTS_PROJECTION, &matProj);
		pd3dDev->SetTransform(D3DTS_WORLD, &matWorld);
		pd3dDev->SetTransform(D3DTS_VIEW, &matView);
///	}


	// (output pixel) = (vertex diffuse color) * (texture color) * (lightmap texture color)

	// color operation on texture stage 0: (vertex diffuse color) * (texture color)
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// color operation on texture stage 1:  (output of stage0) * (lightmap texture color)
    pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// as for the alpha value, we directly use the alpha of the vertex diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );

	pd3dDev->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDev->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
	pd3dDev->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );
//	pd3dDev->SetSamplerState( 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
//	pd3dDev->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	pd3dDev->SetSamplerState( 5, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
	pd3dDev->SetSamplerState( 5, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );

	if( m_pDynamicLightManager )
	{
		if( pEffect )
		{
			pEffect->SetTexture( "Texture2", m_pLightmap_PL );
			hr = pEffect->SetTexture( "Texture5", m_pNormalmap_PL );
			if( FAILED(hr) )
				int iSetTexFailed = 1;
		}
		else
		{
			pd3dDev->SetTexture( 2, m_pLightmap_PL );
		}

		if( LockVertexBuffer() )
		{
			m_pDynamicLightManager->SetDynamicLights(this);
			UnlockVertexBuffer();
		}
		else
			MessageBox( NULL, "failed to lock the vertex buffer of the static geometry", "error", MB_OK|MB_ICONWARNING );
	}

//	hr = pd3dDev->SetFVF( D3DFVF_MAPVERTEX );
//	if(FAILED(hr)) MessageBox(NULL, "SetFVF() failed.",			"Error", MB_ICONWARNING|MB_OK);

	// Set up fog
	if(0 < m_Fog.cFogType)
	{
		pd3dDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
//		pd3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);	// use vertex fog
//		pd3dDev->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_NONE);

		pd3dDev->SetRenderState(D3DRS_FOGTABLEMODE,  D3DFOG_LINEAR);	//use pixel fog
		pd3dDev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE );

		pd3dDev->SetRenderState(D3DRS_FOGCOLOR,
			D3DCOLOR_ARGB(0, 
			(BYTE)(m_Fog.color.fRed * 255.0f),
			(BYTE)(m_Fog.color.fGreen * 255.0f),
			(BYTE)(m_Fog.color.fBlue * 255.0f)) );	//set color of the fog
		pd3dDev->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&m_Fog.fMinDist));
		pd3dDev->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&m_Fog.fMaxDist));
	}

//	m_iNumLightSourcePolygons = 0;
//	m_iNumNonLightSourcePolygons = 0;

	int iTex, iNumTextures = m_vecSurface.size();
	for(iTex=0; iTex<iNumTextures; iTex++)
	{
		m_aiNumPolygonsSortedByTexture[iTex] = 0;
	}
	m_iNumPolygonsWithNoTexture = 0;

	// First, draw the cell where the camera is currently staying
//	DrawCell( sCurrentCell, rCam );
	DrawCell_TS( sCurrentCell, rCam );

	// Next, draw the potentially visible cells
	CSG_Cell& rCell = paCellData[sCurrentCell];
	short i;
	short* pVisCellIndex = m_pasVisibleCellIndex + rCell.iVisCellIndexOffset;
	for(i=0; i<rCell.sNumVisCells; i++)
	{
		CSG_Cell& rVisCell = paCellData[ pVisCellIndex[i] ];
		if( rCam.ViewFrustumIntersectsWith(rVisCell.aabb) )
		{	// intersects with the view frustum of the camera - draw the cell
			m_pacCurrentlyVisibleCell[ pVisCellIndex[i] ] = 1;	// turn on the visibility of this cell
//			DrawCell( pVisCellIndex[i], rCam );
			DrawCell_TS( pVisCellIndex[i], rCam );
		}
	}

	//==================== render the polygons ====================
//	pd3dDev->BeginScene();

	// draw texture sorted polygons
	if( pEffect )
	{
		pEffect->Begin( &cPasses, 0 );
		for( pass=0; pass<cPasses; pass++ )
		{
			pEffect->BeginPass( pass );
			DrawTextureSortedPolygons_Shader_TS();
///			DrawTextureSortedPolygons_Shader();
			pEffect->EndPass();
		}
		pEffect->End();
	}
	else
		DrawTextureSortedPolygons();


	// turn off the fog
	pd3dDev->SetRenderState(D3DRS_FOGENABLE, FALSE);


	//==================== end of polygon rendering routine ====================
//	pd3dDev->EndScene();


	if( m_pDynamicLightManager )
	{
		if( LockVertexBuffer() )
		{
			m_pDynamicLightManager->ClearDynamicLightEffectsFromPolygons(this);
			UnlockVertexBuffer();
		}
	}

	return true;
}


//actually, some optimizaiton is necessary
void CBSPMap::DrawCell(short sCellIndex, CCamera& rCam)
{
	short i;
	CSG_Cell& rCell = m_paCellData[sCellIndex];
	CSG_Polygon* pPolygon = m_paPolygon + rCell.sCellPolygonIndex;
	CSG_Polygon* pInteriorPolygon = m_paPolygon + rCell.sInteriorPolygonIndex;

//	if( !rCam.ViewFrustumIntersectsWith(rCell.sphere) )
//		return;		//this cell is out of the view frustum of the camera

	if(m_pIB)
	{
		for(i=0; i<rCell.sNumCellPolygons; i++)
		{
//			pPolygon[i].Draw();	// draw without texture sorting
			short& sTextureID = pPolygon[i].sTextureID;
			if( sTextureID < 0 || pPolygon[i].sLightMapID < 0 )
				m_aiPolygonWithNoTexture[m_iNumPolygonsWithNoTexture++] = rCell.sCellPolygonIndex + i;
			else
				m_aiPolygonSortedByTexture[sTextureID][m_aiNumPolygonsSortedByTexture[sTextureID]++] = rCell.sCellPolygonIndex + i;

/*			if(pPolygon[i].bLightSource && m_iNumLightSourcePolygons < NUM_MAX_LIGHTSOURCEPOLYGONS - 1)
			{	// this is a light source polygon
				m_aiLightSourcePolygon[m_iNumLightSourcePolygons++] = rCell.sCellPolygonIndex + i;
			}
			else if( m_iNumNonLightSourcePolygons < NUM_MAX_NONLIGHTSOURCEPOLYGONS - 1)
			{
				m_aiNonLightSourcePolygon[m_iNumNonLightSourcePolygons++] = rCell.sCellPolygonIndex + i;
			}*/
//			g_PerformanceCheck.iNumDrawPrimCalls++;
		}

		for(i=0; i<rCell.sNumInteriorPolygons; i++)
		{
//			pInteriorPolygon[i].Draw();	// draw without texture sorting
			short& sTextureID = pInteriorPolygon[i].sTextureID;
			if( sTextureID < 0 || pInteriorPolygon[i].sLightMapID < 0 )
				m_aiPolygonWithNoTexture[m_iNumPolygonsWithNoTexture++] = rCell.sInteriorPolygonIndex + i;
			else
				m_aiPolygonSortedByTexture[sTextureID][m_aiNumPolygonsSortedByTexture[sTextureID]++] = rCell.sInteriorPolygonIndex + i;

/*			if(pInteriorPolygon[i].bLightSource && m_iNumLightSourcePolygons < NUM_MAX_LIGHTSOURCEPOLYGONS - 1)
			{
				m_aiLightSourcePolygon[m_iNumLightSourcePolygons++] = rCell.sInteriorPolygonIndex + i;
			}
			else if( m_iNumNonLightSourcePolygons < NUM_MAX_NONLIGHTSOURCEPOLYGONS - 1)
			{
				m_aiNonLightSourcePolygon[m_iNumNonLightSourcePolygons++] = rCell.sInteriorPolygonIndex + i;
			}*/
//			g_PerformanceCheck.iNumDrawPrimCalls++;
		}
	
	}
}


void CBSPMap::DrawCell_TS(short sCellIndex, const CCamera& rCam)
{
	if( !m_pIB )
		return;

	CSG_Cell& rCell = m_paCellData[sCellIndex];
	CSG_TriangleSet* paTriSet = m_paTriangleSet + rCell.iTriangleSetIndex;

	int i, iNumTriSets = rCell.iNumTriangleSets;
	short tex_id;
	for(i=0; i<iNumTriSets; i++)
	{
//		pPolygon[i].Draw();	// draw without texture sorting

		if( paTriSet[i].sTextureID < 0 || paTriSet[i].sLightMapID < 0 )
		{
			m_aiPolygonWithNoTexture[m_iNumPolygonsWithNoTexture++]
				= rCell.iTriangleSetIndex + i;
		}
		else
		{
			tex_id = paTriSet[i].sTextureID;
			m_aiPolygonSortedByTexture[tex_id][m_aiNumPolygonsSortedByTexture[tex_id]++]
				= rCell.iTriangleSetIndex + i;
		}

/*		if( paTriSet[i].bLightSource && m_iNumLightSourcePolygons < NUM_MAX_LIGHTSOURCEPOLYGONS - 1 )
		{	// this is a light source polygon
			m_aiLightSourcePolygon[m_iNumLightSourcePolygons++] = rCell.iTriangleSetIndex + i;
		}
		else if( m_iNumNonLightSourcePolygons < NUM_MAX_NONLIGHTSOURCEPOLYGONS - 1)
		{
			m_aiNonLightSourcePolygon[m_iNumNonLightSourcePolygons++] = rCell.iTriangleSetIndex + i;
		}*/

//		g_PerformanceCheck.iNumDrawPrimCalls++;

	}
}


void CBSPMap::DrawTextureSortedPolygons()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	float afDynamicLightIntensity[4] = {0,0,0,0};	// used to set the intensity of dynamic light to constant register of pixel shader


	// for polygons that have no textures, use vertex colors only
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	int iPolygon, iNumPolygonsWithNoTexture = m_iNumPolygonsWithNoTexture ;
	for(iPolygon=0; iPolygon<iNumPolygonsWithNoTexture; iPolygon++)
	{
		CSG_Polygon& rPolygon = m_paPolygon[ m_aiPolygonWithNoTexture[iPolygon] ];
		if( 0 <= rPolygon.sLightMapID )
		{
			pd3dDev->SetTexture( 1, m_vecLightmap[ rPolygon.sLightMapID ].pTexture );	// static lightmap (stage 1)
			pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
		}
		else
			pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

		if( 0 <= rPolygon.acDynamicLightIntensity[0] )
		{	// set the intensity of the dynamic light
			afDynamicLightIntensity[3] = (float)rPolygon.acDynamicLightIntensity[0] / 256.0f;
			DIRECT3D9.GetDevice()->SetPixelShaderConstantF( 3, afDynamicLightIntensity, 1 );
		}

		rPolygon.DrawWithoutTextureSettings( pd3dDev );
	}

	// for polygons that have textures, modulate vertex & texture colors
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	// blend with light map
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );

	int iTexture, iNumTextures = m_vecSurface.size();
	int iNumPolygons;
	for(iTexture=0; iTexture<iNumTextures; iTexture++)
	{
//		pd3dDev->SetTexture( 0, m_vecSurface[iTexture].pTexture );	// decal texture (stage 0)
		pd3dDev->SetTexture( 0, m_vecTextureCache[ m_vecSurface[iTexture].ColorTexIndex ].pTexture );	// decal texture (stage 0)

//		if( m_aNormalMapTexture[iTexture].pTexture )
//			pd3dDev->SetTexture( 3, m_aNormalMapTexture[iTexture].pTexture );	// normal map texture (stage 3)
//		else
//			pd3dDev->SetTexture( 3, m_apExtraTexture[EXTRA_TEX_DEFAULT_NORMALMAP] );

		iNumPolygons = m_aiNumPolygonsSortedByTexture[iTexture];	// number of polygons with the current texture
		for(iPolygon=0; iPolygon<iNumPolygons; iPolygon++)
		{
			CSG_Polygon& rPolygon = m_paPolygon[ m_aiPolygonSortedByTexture[iTexture][iPolygon] ];

			pd3dDev->SetTexture( 1, m_vecLightmap[ rPolygon.sLightMapID ].pTexture );	// static lightmap (stage 1)

			if( 0 <= rPolygon.acDynamicLightIntensity[0] )
			{	// set the intensity of the dynamic light
				afDynamicLightIntensity[3] = (float)rPolygon.acDynamicLightIntensity[0] / 256.0f;

				DIRECT3D9.GetDevice()->SetPixelShaderConstantF( 3, afDynamicLightIntensity, 1 );
			}

			rPolygon.DrawWithoutTextureSettings( pd3dDev );
		}
	}
}


void CBSPMap::DrawTextureSortedPolygons_Shader()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	LPD3DXEFFECT pEffect = pShaderMgr->GetEffect();


	float afDynamicLightIntensity[4] = {0,0,0,0};	// used to set the intensity of dynamic light to constant register of pixel shader

//	HRESULT hr;

	pShaderMgr->SetTexture( 0, m_apExtraTexture[EXTRA_TEX_DEFAULT_COLOR] );
//	if( FAILED(hr) ) int iError = 1;

	// render as smooth, bumpless surface
	pShaderMgr->SetTexture( 3, m_apExtraTexture[EXTRA_TEX_DEFAULT_NORMALMAP] );
//	if( FAILED(hr) ) int iError = 1;

	int iPolygon, iNumPolygonsWithNoTexture = m_iNumPolygonsWithNoTexture;
	for(iPolygon=0; iPolygon<iNumPolygonsWithNoTexture; iPolygon++)
	{
		CSG_Polygon& rPolygon = m_paPolygon[ m_aiPolygonWithNoTexture[iPolygon] ];
		if( 0 <= rPolygon.sLightMapID )
		{
			pShaderMgr->SetTexture( 1, m_vecLightmap[ rPolygon.sLightMapID ].pTexture );
			pShaderMgr->SetTexture( 4, m_vecLightmap[ rPolygon.sLightMapID ].pLightDirMap );
		}
		else
			pShaderMgr->SetTexture( 1, m_apExtraTexture[EXTRA_TEX_DEFAULT_COLOR] );

		if( 0 <= rPolygon.acDynamicLightIntensity[0] )
		{	// set the intensity of the dynamic light
			afDynamicLightIntensity[3] = (float)rPolygon.acDynamicLightIntensity[0] / 256.0f;

			pEffect->SetFloat( "g_fDLightMapIntensity", afDynamicLightIntensity[3] );
		}

		pEffect->CommitChanges();

		rPolygon.DrawWithoutTextureSettings( pd3dDev );
	}

	int iTexture, iNumTextures = m_vecSurface.size();
	int iNumPolygons;
	for(iTexture=0; iTexture<iNumTextures; iTexture++)
	{
		if( /*m_bUseFakeBumpTextures == */ true )
		{
            pShaderMgr->SetTexture( 0, m_vecTextureCache[ m_vecSurface[iTexture].FakeBumpTexIndex ].pTexture );
		}
		else
		{
			// set color & normal map textures for bump mapping
            pShaderMgr->SetTexture( 0, m_vecTextureCache[ m_vecSurface[iTexture].ColorTexIndex ].pTexture );
			pShaderMgr->SetTexture( 3, m_vecTextureCache[ m_vecSurface[iTexture].NormalMapTexIndex ].pTexture );
		}

		iNumPolygons = m_aiNumPolygonsSortedByTexture[iTexture];	// number of polygons with the current texture
		for(iPolygon=0; iPolygon<iNumPolygons; iPolygon++)
		{
			CSG_Polygon& rPolygon = m_paPolygon[ m_aiPolygonSortedByTexture[iTexture][iPolygon] ];

			pShaderMgr->SetTexture( 1, m_vecLightmap[ rPolygon.sLightMapID ].pTexture );
			pShaderMgr->SetTexture( 4, m_vecLightmap[ rPolygon.sLightMapID ].pLightDirMap );

			if( 0 <= rPolygon.acDynamicLightIntensity[0] )
			{	// set the intensity of the dynamic light
				afDynamicLightIntensity[3] = (float)rPolygon.acDynamicLightIntensity[0] / 256.0f;

				pEffect->SetFloat( "g_fDLightMapIntensity", afDynamicLightIntensity[3] );
			}

			pEffect->CommitChanges();

			rPolygon.DrawWithoutTextureSettings( pd3dDev );
		}
	}
}


void CBSPMap::DrawTextureSortedPolygons_Shader_TS()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	CShaderManager *pShaderMgr = CShader::Get()->GetCurrentShaderManager();
	LPD3DXEFFECT pEffect = pShaderMgr->GetEffect();

	// how about using vertex diffuse color alpha to store lightmap intensity?

//	HRESULT hr;

	pShaderMgr->SetTexture( 0, m_apExtraTexture[EXTRA_TEX_DEFAULT_COLOR] );

	pShaderMgr->SetTexture( 3, m_apExtraTexture[EXTRA_TEX_DEFAULT_NORMALMAP] );	// render as smooth, bumpless surface

	int iTriSet, num_trisets = m_iNumPolygonsWithNoTexture;
	for(iTriSet=0; iTriSet<num_trisets; iTriSet++)
	{
		CSG_TriangleSet& rTriSet = m_paTriangleSet[ m_aiPolygonWithNoTexture[iTriSet] ];
		if( 0 <= rTriSet.sLightMapID )
		{
			pShaderMgr->SetTexture( 1, m_vecLightmap[ rTriSet.sLightMapID ].pTexture );
			pShaderMgr->SetTexture( 4, m_vecLightmap[ rTriSet.sLightMapID ].pLightDirMap );
		}
		else
			pShaderMgr->SetTexture( 1, m_apExtraTexture[EXTRA_TEX_DEFAULT_COLOR] );

//		if( 0 <= rPolygon.acDynamicLightIntensity[0] ) {...}

		pEffect->CommitChanges();

		pd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			                           0,
									   rTriSet.iMinIndex,
									   rTriSet.iNumVertexBlocksToCover,
									   rTriSet.iStartIndex,
									   rTriSet.iNumTriangles );

	}

	int iTexture, iNumTextures = m_vecSurface.size();
	for(iTexture=0; iTexture<iNumTextures; iTexture++)
	{
		if( /*m_bUseFakeBumpTextures == */ true )
		{
			pShaderMgr->SetTexture( 0, m_vecTextureCache[ m_vecSurface[iTexture].FakeBumpTexIndex ].pTexture );
		}
		else
		{
			// set color & normal map textures for bump mapping
			pShaderMgr->SetTexture( 0, m_vecTextureCache[ m_vecSurface[iTexture].ColorTexIndex ].pTexture );
			pShaderMgr->SetTexture( 3, m_vecTextureCache[ m_vecSurface[iTexture].NormalMapTexIndex ].pTexture );
		}

		num_trisets = m_aiNumPolygonsSortedByTexture[iTexture];	// number of polygons with the current texture
		for(iTriSet=0; iTriSet<num_trisets; iTriSet++)
		{
			CSG_TriangleSet& rTriSet = m_paTriangleSet[ m_aiPolygonSortedByTexture[iTexture][iTriSet] ];

			pShaderMgr->SetTexture( 1, m_vecLightmap[ rTriSet.sLightMapID ].pTexture );
			pShaderMgr->SetTexture( 4, m_vecLightmap[ rTriSet.sLightMapID ].pLightDirMap );

/*			if( 0 <= rPolygon.acDynamicLightIntensity[0] )
			{	// set the intensity of the dynamic light
				afDynamicLightIntensity[3] = (float)rPolygon.acDynamicLightIntensity[0] / 256.0f;
				pEffect->SetFloat( "g_fDLightMapIntensity", afDynamicLightIntensity[3] );
			}*/

			pEffect->CommitChanges();

			pd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
			                               0,
									       rTriSet.iMinIndex,
									       rTriSet.iNumVertexBlocksToCover,
									       rTriSet.iStartIndex,
									       rTriSet.iNumTriangles );
		}
	}
}


void CBSPMap::MakeEntityTree( CBSPTree& bsptree )
{
	// copy the tree to dest
	bsptree = m_BSPTree;
}


void CBSPMap::SetDynamicLightManager( CEntitySet* pEntitySet )
{
	SafeDelete( m_pDynamicLightManager );
//	m_pDynamicLightManager = new CDynamicLightManagerForStaticGeometry;
//	m_pDynamicLightManager->SetLightEntityManager( pEntitySet->GetLightEntityManager() );
}









/*
void CBSPMap::RenderGlareImage()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	HRESULT hr;
	int i;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform(D3DTS_WORLD, &matWorld);

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	// turn on the Z-buffer
	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

	// disable color & alpha operation on texture stage 1
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// color operation on texture stage 0
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	// use only the texture alpha value
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	hr = pd3dDev->SetVertexShader( NULL );

	hr = pd3dDev->SetVertexDeclaration( m_pStaticGeometryVertexDecleration );
//	hr = pd3dDev->SetFVF( D3DFVF_MAPVERTEX );

	hr = pd3dDev->SetStreamSource(0, m_pVB, 0, sizeof(MAPVERTEX));
	hr = pd3dDev->SetIndices( m_pIB );

	pd3dDev->SetTexture( 0, m_apExtraTexture[EXTRA_TEX_TRANSPARENT] );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	// draw non light source polygons
	// Actually, no polygons are drawn. The following DrawWithoutTextureSettings() calls are made only to 
	// write depth values of non light source polygons to the depth buffer
	for(i=0; i<m_iNumNonLightSourcePolygons; i++)
	{
		m_paPolygon[ m_aiNonLightSourcePolygon[i] ].DrawWithoutTextureSettings( pd3dDev );
	}

	pd3dDev->SetTexture( 0, m_apExtraTexture[EXTRA_TEX_TRANSLUCENT] );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );	// light colors are represented by vertex colors
	// draw light source polygons
	for(i=0; i<m_iNumLightSourcePolygons; i++)
	{
		m_paPolygon[ m_aiLightSourcePolygon[i] ].DrawWithoutTextureSettings( pd3dDev );
	}

}*/

/*
void CBSPMap::RenderSkybox( const CCamera& rCamera )
{
	CD3DXMeshObject *pSkyboxMesh = m_pSkybox;
	if( !pSkyboxMesh || !pSkyboxMesh->GetBaseMesh() )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	CShaderManager *pShaderManager = CShader::Get()->GetCurrentShaderManager();
	LPD3DXEFFECT pEffect = pShaderManager->GetEffect();

	D3DXMATRIX matWorld, matScale;
	D3DXMatrixScaling( &matScale, 200.0f, 200.0f, 200.0f );
	D3DXMatrixIdentity( &matWorld );
	matWorld._41 = rCamera.GetPosition().x;
	matWorld._42 = rCamera.GetPosition().y;
	matWorld._43 = rCamera.GetPosition().z;
	D3DXMatrixMultiply( &matWorld, &matScale, &matWorld );

	hr = pd3dDev->SetVertexDeclaration( pSkyboxMesh->GetVertexDeclaration() );

	assert( !FAILED(hr) );

	// no lightmap or bumpmap textures for skybox
	if( pEffect )
	{
		UINT pass, cPasses;
//		Result::Name res = pShaderManager->SetTechnique( SHADER_TECH_SKYBOX );
		Result::Name res = pShaderManager->SetTechnique( m_aShaderTechHandle[4] );
		pEffect->Begin( &cPasses, 0 );

		assert( !FAILED(hr) );

		pShaderManager->SetWorldTransform( matWorld );

//		for( i=0; i<num_mats; i++ )
//		{
			pShaderManager->SetTexture( 0, m_vecpSkyboxTexture[0] );
			pEffect->CommitChanges();

			for( pass=0; pass<cPasses; pass++ )
			{
				pEffect->BeginPass( pass );
				hr = pSkyboxMesh->GetBaseMesh()->DrawSubset( 0 );
				assert( !FAILED(hr) );
				pEffect->EndPass();
			}
//		}

		pEffect->End();
	}
	else
	{
		pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

		// set texture stage states

		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

//		for( i=0; i<num_mats; i++ )
//		{
			pd3dDev->SetTexture( 0, m_vecpSkyboxTexture[0] );

			pSkyboxMesh->GetBaseMesh()->DrawSubset( 0 );
//		}
	}
}
*/