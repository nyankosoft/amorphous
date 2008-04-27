
#include "Stage/Skybox.h"
#include "BSPMapCompiler/BSPMapCompiler.h"
#include "BSPMapCompiler/lightmapbuilder.h"
#include "3DCommon/FloatRGBColor.h"

#include "Support/fnop.h"
#include "Support/StatusDisplay/StatusDisplay.h"
#include "Support/StatusDisplay/StatusDisplayRenderer_D3DX.h"

#include "BSPMapFileExporter.h"

#include "BSPMap_Collision/BSPTree_CollisionModel_Exporter.h"

#include <algorithm>

void CBSPMapFileExporter::OutputBSPFile( const string filename )
{

	CStatusDisplay::Get()->RegisterTask( "Stage File Export" );


	FILE *fp;
	fp = fopen( filename.c_str(), "wb");

	//===== Fill out the header structure with the ID & version info. =====
	SBSPFileHeader header;
	memset(&header, 0, sizeof(SBSPFileHeader));
	strcpy(header.ID, "bspxfile");
	header.version = BSPFILE_VERSION;
	//=====================================================================

	// Set the BSP-Tree for the bspfile
	m_aBSPTree_f.clear();
	m_aBSPTree_f.reserve(DEFAULT_NUM_NODES);
	SetBSPTree(	m_pMapCompiler->GetBSPTree() );

	m_aVertexBuffer.reserve(DEFAULT_NUM_VERTICES);

	// Set cells and all the polygons in cells
	SetCells();

	// SetPlanes
	CPlaneBuffer* pPlnBuffer = m_pMapCompiler->GetPlaneBuffer();
	SPlane* pFirstPlane = &pPlnBuffer->at( 0 );

	// Set skybox
	SetSkybox();

	SDataTAGLump dtag;

	// Output BSP file header which contains the version of the format
	dtag.dwTAG = TAG_HEADER; dtag.dwSize = sizeof(SBSPFileHeader);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	fwrite(&header , sizeof(SBSPFileHeader), 1, fp);

	// Output plane data
	dtag.dwTAG = TAG_PLANE; dtag.dwSize = sizeof(SPlane) * pPlnBuffer->size();
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	fwrite( pFirstPlane, sizeof(SPlane), pPlnBuffer->size(), fp );

	// Output BSPTree
	dtag.dwTAG = TAG_BSPTREENODE; dtag.dwSize = m_aBSPTree_f.size() * sizeof(SNode_f);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
//	fwrite( m_aBSPTree_f.begin(), sizeof(SNode_f), m_aBSPTree_f.size(), fp );
	fwrite( &m_aBSPTree_f[0], sizeof(SNode_f), m_aBSPTree_f.size(), fp );

	// Output MAPVERTEX vertex buffer
	dtag.dwTAG = TAG_FVFVERTEX; dtag.dwSize = m_aVertexBuffer.size() * sizeof(MAPVERTEX);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
//	fwrite( m_aVertexBuffer.begin(), sizeof(MAPVERTEX), m_aVertexBuffer.size(), fp );
	fwrite( &m_aVertexBuffer[0], sizeof(MAPVERTEX), m_aVertexBuffer.size(), fp );

	// Output index buffer
	dtag.dwTAG = TAG_FVFINDEX; dtag.dwSize = m_asIndexBuffer.size() * sizeof(short);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
//	fwrite( m_asIndexBuffer.begin(), sizeof(short), m_asIndexBuffer.size(), fp );
	fwrite( &m_asIndexBuffer[0], sizeof(short), m_asIndexBuffer.size(), fp );

	// Output Polygons
	dtag.dwTAG = TAG_POLYGON; dtag.dwSize = m_vecPolygon.size() * sizeof(CSG_Polygon);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
//	fwrite( m_vecPolygon.begin(), sizeof(CSG_Polygon), m_vecPolygon.size(), fp );
	fwrite( &m_vecPolygon[0], sizeof(CSG_Polygon), m_vecPolygon.size(), fp );

	// Output triangle sets
	dtag.dwTAG = TAG_TRIANGLESET; dtag.dwSize = m_vecTriangleSet.size() * sizeof(CSG_TriangleSet);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
//	fwrite( m_vecTriangleSet.begin(), sizeof(CSG_TriangleSet), m_vecTriangleSet.size(), fp );
	fwrite( &m_vecTriangleSet[0], sizeof(CSG_TriangleSet), m_vecTriangleSet.size(), fp );

	// Output Cells
	dtag.dwTAG = TAG_BSPCELL; dtag.dwSize = m_aCellData.size() * sizeof(CSG_Cell);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	fwrite( &m_aCellData[0], sizeof(CSG_Cell), m_aCellData.size(), fp );

	// Output pvs info (i.e. indices of cells)
	dtag.dwTAG = TAG_VISCELLINDEX;
	dtag.dwSize = m_asVisibleCellIndex.size() * sizeof(short);
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	short sIndex;
	list<short>::iterator itr = m_asVisibleCellIndex.begin();
	while( itr != m_asVisibleCellIndex.end() )
	{
		sIndex = *itr;
		fwrite( &sIndex, sizeof(short), 1, fp );
		itr++;
	}

	//Output 'Fixed Convex Models'
	//dtag.dwTAG = TAG_FIXEDCONVEXMODEL;
	//dtag.dwSize = m_aFCModel.size() * sizeof(SFixedConvexModel);
	//fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	//fwrite( m_aFCModel.begin(), sizeof(SFixedConvexModel), m_aFCModel.size(), fp );

	m_strTextureFilePath = "Stage\\Texture\\";
	WriteTextures(fp);

	// output texture indices
	WriteSurfaces(fp);

	// output Lightmap Textures
	WriteLightmapTexture(fp);

	// output Fog Data
	SFog* pFog = m_pMapCompiler->GetFogData();
	if(pFog)
	{
		dtag.dwTAG = TAG_FOG;
		dtag.dwSize = sizeof(SFog);
		fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
		fwrite( pFog, sizeof(SFog), 1, fp );
	}

	// Output Skybox
	if(m_pSkybox)
	{
		dtag.dwTAG = TAG_SKYBOX;
		dtag.dwSize = sizeof(CSkybox);
		fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
		fwrite( this->m_pSkybox, sizeof(CSkybox), 1, fp );
	}


	CStatusDisplay::Get()->UpdateProgress( "Stage File Export", 0.5f );


	// set BSPTree collision model and save to file as a data chunk
	dtag.dwTAG = TAG_BSPTREE_COLLISIONMODEL;
	dtag.dwSize = 0;	// size is not used to load this collision model chunk
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);

	m_pBSPCollisionModelExporter = new CBSPTree_CollisionModel_Exporter;
	m_pBSPCollisionModelExporter->SetCollisionModel( m_pMapCompiler );
	m_pBSPCollisionModelExporter->SaveToFile( fp );
	SafeDelete( m_pBSPCollisionModelExporter );

	OutputTriangleMesh(fp);

	fclose(fp);


	CStatusDisplay::Get()->UpdateProgress( "Stage File Export", 1.0f );

}


void CBSPMapFileExporter::SetBSPTree( vector<SNode>* pBSPTree )
{
	SNode_f destnode;
	int offset, i;
	offset = m_aBSPTree_f.size();
	for(i=0; i<pBSPTree->size(); i++)
	{
		SNode &srcnode = pBSPTree->at(i);
		destnode.sPlaneIndex = srcnode.sPlaneIndex;
		destnode.sCellIndex = srcnode.sCellIndex;
		destnode.sFrontChild = srcnode.child[CLD_FRONT];
		destnode.sBackChild = srcnode.child[CLD_BACK];
		//We don't add the offset value for the node of the BSP-Tree that begins
		//in the middle of the 'SNode_f' array	(ver 0.31)
/*		if( 0 <= destnode.sFrontChild )
			destnode.sFrontChild += offset;
		if( 0 <= destnode.sBackChild )
			destnode.sBackChild += offset;
*/
		destnode.aabb = srcnode.aabb;

		m_aBSPTree_f.push_back( destnode );
	}

}


void CBSPMapFileExporter::SetCells()
{
	vector<SNode>* pParentBSPTree = m_pMapCompiler->GetBSPTree();
	vector<SNode>* pBSPTree = NULL;
	vector<CMapFace>* paFace = m_pMapCompiler->GetFace();
	vector<CCell>* paCell = m_pMapCompiler->GetCell();
	CSG_Cell newcell;

	int i,j;
	for(i=0; i<paCell->size(); i++)
	{
		CCell& rCell = paCell->at(i);
		memset(&newcell, 0, sizeof(CSG_Cell));

		//Set the polygons which compose this cell
		newcell.sCellPolygonIndex = m_vecPolygon.size();
		newcell.sNumCellPolygons = 0;
		for(j=0; j<paFace->size(); j++)
		{
			CMapFace& rFace = paFace->at(j);
			if(rCell.m_sNode == rFace.m_sNode)
			{
				//Convert 'rFace' into the polygon and store it to the 'm_vecPolygon'
				//Set index into the polygon in this celldata structure
				AddFaceToPolygonBuffer(rFace);
				newcell.sNumCellPolygons++;
			}
		}

		//Set visible cell indices
		newcell.iVisCellIndexOffset = m_asVisibleCellIndex.size();
		newcell.sNumVisCells = rCell.m_sVisCellIndex.size();
		m_asVisibleCellIndex.insert( m_asVisibleCellIndex.end(),
			rCell.m_sVisCellIndex.begin(),
			rCell.m_sVisCellIndex.end() );

		//Set AxisAligned BoundingBox of this cell
		newcell.aabb = pParentBSPTree->at( rCell.m_sNode ).aabb;

		//Set Bounding Sphere of this cell
		newcell.sphere = newcell.aabb.CreateBoundingSphere();

		//Set up interior models in this cell
		if( rCell.m_pBSPTreeBuilder )
		{	// This cell has the internal BSP-Tree
			// Get the BSP-Tree which represents the stationary interior models in this cell
			pBSPTree = rCell.m_pBSPTreeBuilder->GetBSPTree();

			// Append the BSP-Tree to the export version of the BSP-Tree structure
			newcell.sBSPTreeIndex = m_aBSPTree_f.size();
			SetBSPTree(pBSPTree);
		}

		if( 0 < rCell.m_aInteriorFace.size() )
		{
			// Set up the interior polygons
			newcell.sInteriorPolygonIndex = m_vecPolygon.size();
			newcell.sNumInteriorPolygons = rCell.m_aInteriorFace.size();

			for(j=0; j<rCell.m_aInteriorFace.size(); j++)
			{
				//Convert 'CMapFace' into the polygon and store it to the 'm_vecPolygon'
				AddFaceToPolygonBuffer( rCell.m_aInteriorFace[j] );
			}
		}

		// find polygons that share the same texture and lightmap and group them
		// to make triangle sets
		CreateTriangleSetsFromCellPolygons( newcell );
	

		m_aCellData.push_back(newcell);
	}


}


struct STempPolygonGroup
{
	vector<int> veciPolygonIndex;
};


void CBSPMapFileExporter::CreateTriangleSetsFromCellPolygons( CSG_Cell& rCell )
{
	int pol_index;
	int i, j, k;//, iNumTriSets;
//	int tri;
	int grp, iNumGroups;

	vector<STempPolygonGroup> vecPolygonGroup;
	vecPolygonGroup.reserve( 256 );

	// 1. group polygons by materials

	int aiIndexStart[2] = { rCell.sCellPolygonIndex, rCell.sInteriorPolygonIndex };
	int aiNumIndices[2] = { rCell.sNumCellPolygons,  rCell.sNumInteriorPolygons };

  for( i=0; i<2; i++ )
  {
	for( pol_index = aiIndexStart[i];
	     pol_index < aiIndexStart[i] + aiNumIndices[i];
		 pol_index++ )
	{
		CSG_Polygon& rPolygon = m_vecPolygon[pol_index];

		if( !rPolygon.bVisible )
			continue;

		iNumGroups = vecPolygonGroup.size();
		for( grp=0; grp<iNumGroups; grp++ )
		{
			CSG_Polygon& rGroupedPolygon =  m_vecPolygon[ vecPolygonGroup[grp].veciPolygonIndex[0] ];

			if( rPolygon.sTextureID   == rGroupedPolygon.sTextureID && 
				rPolygon.sLightMapID  == rGroupedPolygon.sLightMapID &&
				rPolygon.bLightSource == rGroupedPolygon.bLightSource )
			{
				// add to this group
				vecPolygonGroup[grp].veciPolygonIndex.push_back( pol_index );
				break;
			}
		}

		if( grp == iNumGroups )
		{	// create a new polygon group
			vecPolygonGroup.push_back( STempPolygonGroup() );
			vecPolygonGroup.back().veciPolygonIndex.push_back( pol_index );
		}

	}
  }

	int iNumGroupedPolygons;
	int min_index, max_index;

	// 2. create triangle sets for each polygon group
	iNumGroups = vecPolygonGroup.size();

	rCell.iTriangleSetIndex = m_vecTriangleSet.size();
	rCell.iNumTriangleSets  = iNumGroups;

	for( grp=0; grp<iNumGroups; grp++ )
	{
		m_vecTriangleSet.push_back( CSG_TriangleSet() );
		CSG_TriangleSet& rTriSet = m_vecTriangleSet.back();

		rTriSet.iStartIndex = m_asIndexBuffer.size();
		rTriSet.iNumTriangles = 0;

		min_index =  999999;
		max_index = -999999;

		iNumGroupedPolygons = vecPolygonGroup[grp].veciPolygonIndex.size();
		for( k=0; k<iNumGroupedPolygons; k++ )
		{
			CSG_Polygon& rPolygon = m_vecPolygon[ vecPolygonGroup[grp].veciPolygonIndex[k] ];
			int iNumTriangles = rPolygon.sNumVertices - 2;
			for( j=0; j<iNumTriangles; j++ )
			{
				m_asIndexBuffer.push_back( rPolygon.sIndexOffset );
				m_asIndexBuffer.push_back( rPolygon.sIndexOffset + j + 1 );
				m_asIndexBuffer.push_back( rPolygon.sIndexOffset + j + 2 );
			}
			rTriSet.iNumTriangles += iNumTriangles;

			if( rPolygon.sIndexOffset < min_index )
				min_index = rPolygon.sIndexOffset;

			if( max_index < rPolygon.sIndexOffset + rPolygon.sNumVertices - 1 )
				max_index = rPolygon.sIndexOffset + rPolygon.sNumVertices - 1;
		}

		CSG_Polygon& rPolygon = m_vecPolygon[ vecPolygonGroup[grp].veciPolygonIndex[0] ];
		rTriSet.sTextureID  = rPolygon.sTextureID;
		rTriSet.sLightMapID = rPolygon.sLightMapID;
		rTriSet.bLightSource = rPolygon.bLightSource;

		rTriSet.iNumVertexBlocksToCover = max_index - min_index + 1;
		rTriSet.iMinIndex = min_index;
	}
}


//Traverse the tree and set up a fixed convex model at each leaf node.
void CBSPMapFileExporter::AddFixedConvexModel_r(short sNodeIndex,
												vector<SNode>* pBSPTree,
												CCell& rSrcCell)
{
	SNode& rNode = pBSPTree->at( sNodeIndex );
	short sFrontChild = rNode.child[CLD_FRONT];
	short sBackChild = rNode.child[CLD_BACK];
	int i;

	if(0 <= sFrontChild)
	{	//recurse down
		AddFixedConvexModel_r(sFrontChild, pBSPTree, rSrcCell);
	}
	if( sBackChild < 0 )
	{	//leaf-node
		SFixedConvexModel fcmodel;
		fcmodel.aabb = rNode.aabb;
		fcmodel.sNumPolygons = 0;
		fcmodel.sPolygonIndex = this->m_vecPolygon.size();
		rNode.sCellIndex = this->m_aFCModel.size();

		for(i=0; i<rSrcCell.m_aInteriorFace.size(); i++)
		{
			CMapFace& rFace = rSrcCell.m_aInteriorFace[i];
			if(rFace.m_sNode == sNodeIndex)
			{
				AddFaceToPolygonBuffer(rFace);
				fcmodel.sNumPolygons++;
			}
		}
		m_aFCModel.push_back( fcmodel );
	}
	if(0 <= sBackChild)
	{	//recurse down
		AddFixedConvexModel_r(sBackChild, pBSPTree, rSrcCell);
	}
}


void CBSPMapFileExporter::AddFaceToPolygonBuffer( CMapFace& rFace )
{
	int i;
	short sIndex;
	int iNumPnts = rFace.GetNumVertices();

	// create a new polygon structure, fill it out, and put it on the polygon buffer
	CSG_Polygon newpolygon;
	newpolygon.sIndexOffset = m_aVertexBuffer.size();
///	newpolygon.sIndexOffset = m_asIndexBuffer.size();
	newpolygon.sNumVertices = iNumPnts;
	newpolygon.sTextureID   = rFace.m_sTextureID;
	newpolygon.sLightMapID  = rFace.m_sLightMapID;

	if( rFace.ReadTypeFlag(CMapFace::TYPE_INVISIBLE) )
		newpolygon.bVisible = false;
	else
		newpolygon.bVisible = true;

	if( rFace.ReadTypeFlag(CMapFace::TYPE_NOCLIP) )
		newpolygon.bNoClip = true;
	else
		newpolygon.bNoClip = false;

	if( rFace.ReadTypeFlag(CMapFace::TYPE_LIGHTSOURCE) )
		newpolygon.bLightSource = true;
	else
		newpolygon.bLightSource = false;

	SPlane& rPlane = rFace.GetPlane();
	if( rPlane.normal.x == 1.0f ) newpolygon.cPlaneType = 0;
	else if( rPlane.normal.x == -1.0f ) newpolygon.cPlaneType = 1;
	else if( rPlane.normal.y ==  1.0f ) newpolygon.cPlaneType = 2;
	else if( rPlane.normal.y == -1.0f ) newpolygon.cPlaneType = 3;
	else if( rPlane.normal.z ==  1.0f ) newpolygon.cPlaneType = 4;
	else if( rPlane.normal.z == -1.0f ) newpolygon.cPlaneType = 5;
	else newpolygon.cPlaneType = 6;

	// set default values to the variables that are used at runtime
	newpolygon.bDraw        = false;
	for(i=0; i<SG_NUM_MAX_DYNAMIC_LIGHTS_PER_POLYGON; i++)
	{
		newpolygon.acDynamicLightIndex[i] = -1;
		newpolygon.acDynamicLightIntensity[i] = -1;
	}
	// save the index to the converted polygon
	// this index is set to another type of polygon which is used for collision detection
	rFace.m_iPolygonIndex = m_vecPolygon.size();

	m_vecPolygon.push_back( newpolygon );

	for(i=0; i<iNumPnts; i++)
	{
		sIndex = AddVertexAndGetIndex( rFace.GetMAPVERTEX(i) );
///		m_asIndexBuffer.push_back(sIndex);
	}
}

//If the same vertex as the argument 'v' is already in the 'm_aVertexBuffer', 
//Just return its index.
//Otherwise, put 'v' on 'm_aVertexBuffer' and return its index.
short CBSPMapFileExporter::AddVertexAndGetIndex(MAPVERTEX v)
{
/*	int i;
	for(i=0; i<m_aVertexBuffer.size(); i++)
	{
		if(m_aVertexBuffer[i] == v)
			return i;
	}*/
	m_aVertexBuffer.push_back(v);
	return m_aVertexBuffer.size() - 1;

}


void CBSPMapFileExporter::WriteSurfaces(FILE* fp)
{
	vector<CSG_Surface>& rvecSurface = m_pMapCompiler->GetSurface();

	int i, iNumSurfaces = rvecSurface.size();

	SDataTAGLump dtag;
	dtag.dwTAG = TAG_SURFACE;
	dtag.dwSize = sizeof(CSG_Surface) * iNumSurfaces;

	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);

	for(i=0; i<iNumSurfaces; i++)
	{
		fwrite(&rvecSurface[i], sizeof(CSG_Surface), 1, fp);
	}
	
}


void CBSPMapFileExporter::WriteTextures(FILE* fp)
{
	// output texture filenames
	vector<CSGA_TextureFile>& rvecTexFileOrig = m_pMapCompiler->GetTextureFile();
	vector<CSGA_TextureFile> vecTexFileDest;
	CSGA_TextureFile new_texturefile;

	SDataTAGLump dtag;
	dtag.dwTAG = TAG_TEXTUREFILE;
	dtag.dwSize = 0;

	int i, iNumTextureFiles = rvecTexFileOrig.size();

	string strSrcTexFilename;

	// change the path of texture filenames
	for(i=0; i<iNumTextureFiles; i++)
	{
		// set path
		vecTexFileDest.push_back( CSGA_TextureFile() );

/*		strcpy( vecTexFileDest[i].acFilename, m_acTextureFilePath );

		// set body filename of the texture
		CFileNameOperation::GetBodyFilenameBySlash( acBodyFilename, rvecTexFileOrig[i].acFilename );
		strcat( vecTexFileDest[i].acFilename, acBodyFilename );*/

		// set the texture filename for output
		strSrcTexFilename = rvecTexFileOrig[i].acFilename;
//		ChangePath( str, strSrcTexFilename, m_strTextureFilePath );
		string dest = m_strTextureFilePath + fnop::get_nopath( strSrcTexFilename );
		strcpy( vecTexFileDest[i].acFilename, dest.c_str() );
	}

	// calc & output data size (the sum of the lengths of filename strings)
	for(i=0; i<iNumTextureFiles; i++)
		dtag.dwSize += strlen( vecTexFileDest[i].acFilename ) + 1;  // length of the texture filename (including NULL character)
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);

	// write texture filenames to file
	for(i=0; i<iNumTextureFiles; i++)
	{
		fputs(vecTexFileDest[i].acFilename, fp);
		fputc('\n', fp);	// add '\n' to seperate filenames  /// add terminating NULL in order to seperate each texture filename
	}
}


//Add Lightmap Texture lump to the ".bspx" file currently being made
void CBSPMapFileExporter::WriteLightmapTexture(FILE* fp)
{
	// Get lightmap texture from map compiler 
//	vector<SFloatRGBColor*>* papLightmapTexture;	//pointer to the array of 'SFloatRGBColor' pointers
//	papLightmapTexture = m_pMapCompiler->GetLightmapTexture();

	//get the number & the each size of the lightmaps
	CLightmapBuilder *pLightmapBuilder = m_pMapCompiler->GetLightmapBuilder();
	CSG_LightmapTextureInfo lmapinfo;
	int iNumTexs = lmapinfo.iNumLightmapTextures = pLightmapBuilder->GetNumLightmapTextures();
	int iTexWidth = lmapinfo.iLightmapTextureWidth = m_pMapCompiler->GetLightmapTextureWidth();//pLightmapBuilder->GetLightmapTextureWidth();
	int x,y;

	//Arrange the tag for this lightmap texture lump
	SDataTAGLump dtag;
	dtag.dwTAG = TAG_LIGHTMAPTEXTURE;
	dtag.dwSize = sizeof(CSG_LightmapTextureInfo) + sizeof(BYTE) * 3 * iTexWidth * iTexWidth * iNumTexs;

	//write the tag & texture information to the file
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
	fwrite(&lmapinfo, sizeof(CSG_LightmapTextureInfo), 1, fp);

	//Write the lightmap texture to the file
	int i;
	SFloatRGBColor color;
	BYTE red, green, blue;
	for(i=0; i<iNumTexs; i++)
	{
		CLightmapTexture& rLightmapTexture = pLightmapBuilder->GetLightmapTexture( i );

		for( y=0; y<iTexWidth; y++ )
		{
			for( x=0; x<iTexWidth; x++ )
			{
				color = rLightmapTexture.Texel(x,y);
				red   = (BYTE)(color.fRed   * 255); fwrite(&red,   sizeof(BYTE), 1, fp);
				green = (BYTE)(color.fGreen * 255); fwrite(&green, sizeof(BYTE), 1, fp);
				blue  = (BYTE)(color.fBlue  * 255); fwrite(&blue,  sizeof(BYTE), 1, fp);
			}
		}
//		for(j=0; j<iTexWidth*iTexWidth; j++)
//		{}
	}

	// create light direction texture if it is requested
	D3DXVECTOR3 vOffset = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	DWORD dwColor;
	if( m_pMapCompiler->GetLightmapBuilder()->GetCreationFlag() & LMB_CREATE_LIGHT_DIRECTION_MAP_TEXTURE )
	{
		dtag.dwTAG = TAG_LIGHTDIRECTIONTEXTURE;
		dtag.dwSize = sizeof(CSG_LightmapTextureInfo) + sizeof(DWORD) * iTexWidth * iTexWidth * iNumTexs;

		// write the tag & texture information to the file
		fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);
		fwrite(&lmapinfo, sizeof(CSG_LightmapTextureInfo), 1, fp);

//		vector<D3DXVECTOR3 *> *pvLightDirTex = m_pMapCompiler->GetLightmapBuilder()->GetLightDirectionTexture();

		D3DXVECTOR3 v;
		for(i=0; i<iNumTexs; i++)
		{
			CLightmapTexture& rLightmapTexture = pLightmapBuilder->GetLightmapTexture( i );

			for( y=0; y<iTexWidth; y++ )
			{
				for( x=0; x<iTexWidth; x++ )
				{
					v = rLightmapTexture.LightDirection(x,y);
//					v = (pvLightDirTex->at(i))[j];
					v = ( v + vOffset ) / 2.0f;
					dwColor = D3DCOLOR_XRGB( ((int)(v.x * 255.0f)),
										     ((int)(v.y * 255.0f)),
											 ((int)(v.z * 255.0f)) );

					fwrite( &dwColor, sizeof(DWORD), 1 ,fp );

//					red   = (BYTE)(v.x * 255); fwrite(&red,   sizeof(BYTE), 1, fp);
//					green = (BYTE)(v.y * 255); fwrite(&green, sizeof(BYTE), 1, fp);
//					blue  = (BYTE)(v.z * 255); fwrite(&blue,  sizeof(BYTE), 1, fp);
				}
			}
		}
	}

}


void CBSPMapFileExporter::SetSkybox()
{
	vector<CMapFace>* paSkyboxFace = m_pMapCompiler->GetSkyboxFace();
	if( 0 < paSkyboxFace->size() )
	{
		int iSkyboxPolygonOffset = m_vecPolygon.size();

		// convert skybox faces to polygons and put them on 'm_vecPolygon'
		for(int i=0; i<paSkyboxFace->size(); i++)
			AddFaceToPolygonBuffer( paSkyboxFace->at(i) );

		this->m_pSkybox = new CSkybox;
//		this->m_pSkybox->SetSkyboxPolygons( m_vecPolygon.begin() + iSkyboxPolygonOffset,
		this->m_pSkybox->SetSkyboxPolygons( &m_vecPolygon[0] + iSkyboxPolygonOffset,
										m_vecPolygon.size() - iSkyboxPolygonOffset );

		// skybox polygons have been copied to 'm_pSkyBox' , so delete them from
		// the polygon buffer 'm_vecPolygon'
		// vertices for skybox are kept in 'm_aVertexBuffer'
//		m_vecPolygon.erase( m_vecPolygon.begin() + iSkyboxPolygonOffset, &m_vecPolygon.back() );
		m_vecPolygon.erase( m_vecPolygon.begin() + iSkyboxPolygonOffset, m_vecPolygon.end() );
	}
	else
		this->m_pSkybox = NULL;

	return;
}


void CBSPMapFileExporter::OutputTriangleMesh( FILE *fp )
{
	vector<Vector3> vecvTriangleVertex;
	vector<int> veciTriangleIndex;
	vector<int> veciSurfaceMaterialIndex;
	m_pMapCompiler->GetTriangleMeshForCollision( vecvTriangleVertex, veciTriangleIndex, veciSurfaceMaterialIndex );
	
	SDataTAGLump dtag;
	dtag.dwTAG = TAG_TRIANGLEMESH;
	dtag.dwSize = 0;	// size is not used to save this data chunk
	fwrite(&dtag, sizeof(SDataTAGLump), 1, fp);

	// write header for triangle mesh
	CSG_TriMeshHeader header;
	header.iNumVertices = vecvTriangleVertex.size();
	header.iNumTriangles = veciTriangleIndex.size() / 3;
	fwrite( &header, sizeof(STriangleMeshHeader), 1, fp );

	// write vertices and triangles
	fwrite( &vecvTriangleVertex[0], sizeof(Vector3), vecvTriangleVertex.size(), fp );
	fwrite( &veciTriangleIndex[0], sizeof(int), veciTriangleIndex.size(), fp );
	fwrite( &veciSurfaceMaterialIndex[0], sizeof(int), veciSurfaceMaterialIndex.size(), fp );

//	pTriMesh->SaveMesh( fp );
}


void CBSPMapFileExporter::OutputVertexDataToFile( const std::string& filename )
{

	FILE *fp = fopen( filename.c_str(), "w" );
	if(!fp)
		return;

	int i, num_vertices = m_aVertexBuffer.size();

	fprintf( fp, "%03d vertices in the stage\n", num_vertices );
	for(i=0; i<num_vertices; i++)
	{
		const MAPVERTEX& v = m_aVertexBuffer[i];

		fprintf( fp, "[%03d]---------------------------------------------------\n", i );
		fprintf( fp, " pos(%03f %03f %03f)\n", v.vPosition.x, v.vPosition.y, v.vPosition.z );
		fprintf( fp, " tex(%03f %03f)\n", v.vTex0.u, v.vTex0.v );
		fprintf( fp, " lmp(%03f %03f)\n", v.vTex1.u, v.vTex1.v );
		fprintf( fp, "ARGB(%03d %03d %03d %03d)\n",
			(v.color >> 24) & 0x000000FF,
			(v.color >> 16) & 0x000000FF,
			(v.color >> 8) & 0x000000FF,
			v.color & 0x000000FF );

	}

	const int num_tri_sets = m_vecTriangleSet.size();
	fprintf( fp, "\n\n%d triangle sets in total\n", num_tri_sets );
	for( i=0; i<num_tri_sets; i++ )
	{
		const CSG_TriangleSet& rTriSet = m_vecTriangleSet[i];

		fprintf( fp, "[%03d]---------------------------------------------------\n", i );
		fprintf( fp, "start index:  %04d\n", rTriSet.iStartIndex );
		fprintf( fp, "# triangles:  %04d\n", rTriSet.iNumTriangles );
		fprintf( fp, "min. index:   %04d\n", rTriSet.iMinIndex );
		fprintf( fp, "vert. blocks: %04d\n", rTriSet.iNumVertexBlocksToCover );
		fprintf( fp, "texture id:   %04d\n", rTriSet.sTextureID );
		fprintf( fp, "lightmap id:  %04d\n", rTriSet.sLightMapID );
	}

	const int num_polygons = m_vecPolygon.size();
	fprintf( fp, "\n\n%d polygons in total\n", num_polygons );
	for( i=0; i<num_polygons; i++ )
	{
		const CSG_Polygon& rPolygon = m_vecPolygon[i];

		fprintf( fp, "[%03d]---------------------------------------------------\n", i );
		fprintf( fp, "index offset: %04d\n", rPolygon.sIndexOffset );
		fprintf( fp, "# vertices:   %04d\n", rPolygon.sNumVertices );
		fprintf( fp, "draw:         %s\n",   rPolygon.bDraw ? "yes" : "no" );
		fprintf( fp, "light source: %s\n",   rPolygon.bLightSource ? "yes" : "no" );
		fprintf( fp, "texture id:   %04d\n", rPolygon.sTextureID );
		fprintf( fp, "lightmap id:  %04d\n", rPolygon.sLightMapID );
	}


	fclose(fp);
}



CBSPMapFileExporter::~CBSPMapFileExporter()
{
	m_aBSPTree_f.clear();
	m_vecPolygon.clear();
	m_asIndexBuffer.clear();
	m_aVertexBuffer.clear();
	m_aCellData.clear();
	m_asVisibleCellIndex.clear();
	m_aFCModel.clear();
}