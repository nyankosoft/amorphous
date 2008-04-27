
#include "BSPMapCompiler.h"
#include "BSPMapCompiler/lightmapbuilder.h"
#include "Stage/StaticGeometry.h"
#include "Stage/BSPMapFileExporter.h"

#include "LightWave/BSPMapData_LW.h"

#include "Support/TextFileScanner.h"
#include "Support/Log/DefaultLog.h"
#include "Support/fnop.h"
using namespace fnop;

#include "3DMath/3DStructs.h"
//#include "3DMath/PolygonMesh.h"

#include <direct.h>


void CLightmapOption::LoadFromFile( CTextFileScanner& scanner )
{
	scanner.TryScanLine( "TexelSize",			fTexelSize );
	scanner.TryScanLine( "TextureWidth",		TextureWidth );
	scanner.TryScanLine( "TextureWidth",		TextureHeight );

	bool light_dir_map = false;
	if( scanner.TryScanBool( "LightDirectionMap", "Yes/No", light_dir_map) )
	{
		bCreateLightDirectionMap = true;
	}

	scanner.TryScanLine( "AO.SceneResolution",	AO_SceneResolution );
//	scanner.TryScanLine( "EnvLightResolution",	EnvLightResolution );
}


void CMapCompileOption::LoadFromFile( const std::string& filename )
{
	CTextFileScanner scanner;
	
	if( !scanner.OpenFile( filename ) )
		return;

	for( ; !scanner.End(); scanner.NextLine() )
	{
//		scanner.TryScanLine( "DestDirectory",	DestDirectory );
		scanner.TryScanLine( "SourceFilename",	SrcFilename );
		scanner.TryScanLine( "DestFilename",	DestFilename );

		Lightmap.LoadFromFile( scanner );
	}
}



CBSPMapCompiler::CBSPMapCompiler()
:
m_pMapData(NULL),
m_pBSPTreeBuilder(NULL),
m_pPortalBuilder(NULL),
m_pCellBuilder(NULL),
m_pLightmapBuilder(NULL)
{}


CBSPMapData* CBSPMapCompiler::GenerateMapdataContainer( const char* filename )
{
	int len = strlen(filename);
	if( strcmp(filename + (len - 3),"lws") == 0 )
		return new CBSPMapData_LW;
	else
	{
		g_Log.Print( WL_ERROR, "file type must be *.lws ", "Error: unexpected file type" );
		return NULL;
	}
}


int CBSPMapCompiler::Compile( const string& desc_filename )
{
	m_Filename = desc_filename;

	m_CompileOption.LoadFromFile( desc_filename );

	m_pMapData = GenerateMapdataContainer( m_CompileOption.SrcFilename.c_str() );

	if(!m_pMapData)
		return 0;

	g_Log.Print( "start loading map data" );

	// load map data to 'm_pMapData'
	if( m_pMapData->LoadMapDataFromFile( m_CompileOption.SrcFilename.c_str() ) == 0 )
	{
		// failed to load the map file
		return 0;
	}

	g_Log.Print( "map data file has been loaded" );


	// create polygon mesh from main faces and interior faces
	// which can be used for ray tracing
	CreatePolygonMesh();

	// polygon mesh and polygon tree are created from main faces and interioir faces
	// for lightmap texture calculation
	// i.e. lightmap textures can be created without using BSP-Tree for ray clipping
	CreateLightmapTextures();

	g_Log.Print( "lightmaps have been created" );


	// Build BSPTree and make the level from m_pMapData
	m_pBSPTreeBuilder = new CBSPTreeBuilder;
	m_pBSPTreeBuilder->ConstructFrom( &m_pMapData->m_aMainFace );

	m_pBSPTreeBuilder->WriteBSPTree( MakeFilenameForDebugFile(" - bsp-tree.txt") );

	g_Log.Print( "bsp-tree has been constructed" );

	//Create cells from the BSP-Tree
	m_pCellBuilder = new CCellBuilder( m_pBSPTreeBuilder );

	m_pCellBuilder->WriteCells(MakeFilenameForDebugFile(" - cells.txt"));

	// check the change after cell-building
	m_pBSPTreeBuilder->WriteBSPTree( MakeFilenameForDebugFile(" - bsp-tree_.txt") );

	//Build portals from the BSP-Tree and attach it to the cells
	m_pPortalBuilder = new CPortalBuilder;
	m_pPortalBuilder->ConstructFrom( m_pBSPTreeBuilder , m_pCellBuilder );
	
	m_pPortalBuilder->WritePortals( MakeFilenameForDebugFile(" - portals.txt") );

	//The cells are equipped with the portals. Let's see how they changed
	m_pCellBuilder->WriteCells( MakeFilenameForDebugFile(" - cells_with_portals.txt") );

	m_pCellBuilder->MakePVS();

	// put interior faces in each cell
	m_pCellBuilder->AddInteriorModels( &m_pMapData->m_aInteriorFace );

	// the cells are equipped with the PVS information and interior models
	m_pCellBuilder->WriteCells( MakeFilenameForDebugFile("- cells_with_pvs.txt") );

	// output BSP file
	CBSPMapFileExporter* pMapFileExporter = new CBSPMapFileExporter(this);

	pMapFileExporter->OutputBSPFile( m_CompileOption.DestFilename );

	// check the output data
	pMapFileExporter->OutputVertexDataToFile( MakeFilenameForDebugFile(" - MAPVERTEX.txt") );

	SafeDelete( pMapFileExporter );
	SafeDelete( m_pMapData );
	SafeDelete( m_pBSPTreeBuilder );
	SafeDelete( m_pCellBuilder );
	SafeDelete( m_pPortalBuilder );
	SafeDelete( m_pLightmapBuilder );

	return 1;
}


void CBSPMapCompiler::CreatePolygonMesh()
{
	size_t i, iNumFaces;

	vector<CMapFace> vecFace;
	vecFace.reserve( m_pMapData->m_aMainFace.size() + m_pMapData->m_aInteriorFace.size() );

	iNumFaces = m_pMapData->m_aMainFace.size();
	for( i=0; i<iNumFaces; i++ )
		vecFace.push_back( m_pMapData->m_aMainFace[i] );

	iNumFaces = m_pMapData->m_aInteriorFace.size();
	for( i=0; i<iNumFaces; i++ )
		vecFace.push_back( m_pMapData->m_aInteriorFace[i] );

	// create polygon mesh
///	m_pPolygonMesh = new CPolygonMesh<CMapFace>;
///	m_pPolygonMesh->CreateMesh( vecFace );
	m_PolygonMesh.CreateMesh( vecFace );

}


void CBSPMapCompiler::CreateLightmapTextures()
{
	// buffer to temporarily hold polygons
	vector<CMapFace> vecTempFace;
	vecTempFace.reserve( m_pMapData->m_aMainFace.size() + m_pMapData->m_aInteriorFace.size() );

	size_t i, iNumFaces;

	// move main faces and interior faces to the temporary buffer
	iNumFaces = m_pMapData->m_aMainFace.size();
	for( i=0; i<iNumFaces; i++ )
		vecTempFace.push_back( m_pMapData->m_aMainFace[i] );

	iNumFaces = m_pMapData->m_aInteriorFace.size();
	for( i=0; i<iNumFaces; i++ )
		vecTempFace.push_back( m_pMapData->m_aInteriorFace[i] );

	m_pMapData->m_aMainFace.resize( 0 );
	m_pMapData->m_aInteriorFace.resize( 0 );


	// make lightmap textures for both 'main faces' and 'interior faces'
	// texture uv and lightmap id in each face will be updated
	m_pLightmapBuilder = new CLightmapBuilder;
	m_pLightmapBuilder->Init( this );

//	m_pLightmapBuilder->SetTextureWidth( 1024 );
//	m_pLightmapBuilder->SetTextureHeight( 1024 );

#ifdef _DEBUG
//	m_pLightmapBuilder->SetTexelsPerMeter( 5 );
//	m_pLightmapBuilder->SetTexelsPerMeter( 2 );
///	m_pLightmapBuilder->SetTexelsPerMeter( 1 );
/*	m_pLightmapBuilder->SetTexelSize( 1 ); */
#else
//	m_pLightmapBuilder->SetTexelsPerMeter( 8 );
//	m_pLightmapBuilder->SetTexelsPerMeter( 4 );
//	m_pLightmapBuilder->SetTexelsPerMeter( 2 );		// for debug
///	m_pLightmapBuilder->SetTexelsPerMeter( 1 );
#endif

	LightmapDesc lightmap_desc;
	lightmap_desc.Option = m_CompileOption.Lightmap;// [in]
	lightmap_desc.pPolygonMesh = &m_PolygonMesh;// [in]
	lightmap_desc.pvecFace = &vecTempFace;	// [in, out]
	lightmap_desc.AOLightmap.m_EnvLightMesh = m_pMapData->m_EnvLightMesh;	// [in]
	lightmap_desc.AOLightmap.m_MeshArchive  = m_pMapData->m_vecMesh;	// [in]
	lightmap_desc.AOLightmap.RenderTargetTexHeight = m_CompileOption.Lightmap.AO_SceneResolution;	// [in]
	lightmap_desc.AOLightmap.RenderTargetTexWidth  = m_CompileOption.Lightmap.AO_SceneResolution;	// [in]

///	m_pLightmapBuilder->CreateLightmapTexture( vecTempFace, *m_pPolygonMesh );
//	m_pLightmapBuilder->CreateLightmapTexture( vecTempFace, m_PolygonMesh );
	m_pLightmapBuilder->CreateLightmapTexture( lightmap_desc );

//	m_pLightmapBuilder->ScaleIntensityAndAddAmbientLight();

	g_Log.Print( "Lightmaps have been created." );

	// output lightmaps as .bmp files so that they can be previewed with graphics applications
	m_pLightmapBuilder->OutputLightmapTexturesToBMPFiles( MakeFilenameForDebugFile("_lightmap").c_str() );


	// copy the faces back to original buffers
	iNumFaces = vecTempFace.size();
	for( i=0; i<iNumFaces; i++ )
	{
		if( vecTempFace[i].GetTypeFlag() & CMapFace::TYPE_MAIN )
		{
			m_pMapData->m_aMainFace.push_back( vecTempFace[i] );
		}
		else if( vecTempFace[i].GetTypeFlag() & CMapFace::TYPE_INTERIOR )
		{
			m_pMapData->m_aInteriorFace.push_back( vecTempFace[i] );
		}
		else
			assert(0);
	}

}

/*
vector<SFloatRGBColor*>* CBSPMapCompiler::GetLightmapTexture()
{
	return m_pLightmapBuilder->GetLightmapTexture();
}
*/

int CBSPMapCompiler::GetLightmapTextureWidth() 
{
	return m_CompileOption.Lightmap.TextureWidth;//m_pLightmapBuilder->GetLightmapTextureWidth();
}


SFog* CBSPMapCompiler::GetFogData()
{
	if( m_pMapData->m_pFog )
		return m_pMapData->m_pFog;
	else
		return NULL;
}


short CBSPMapCompiler::LineCheck( D3DXVECTOR3& vP1, D3DXVECTOR3& vP2 )
{
	return this->m_pBSPTreeBuilder->LineCheck_r(vP1, vP2);
}


void CBSPMapCompiler::GetTriangleMeshForCollision( vector<D3DXVECTOR3>& rvecvTriangleVertex,
		                                           vector<int>& rveciTriangleIndex,
								                   vector<int>& rveciMaterialIndex )
{
	m_pMapData->GetTriangleMeshForCollision( rvecvTriangleVertex,
		                                     rveciTriangleIndex,
								             rveciMaterialIndex );
}

/*
void CBSPMapCompiler::GetOutputFilename( string& strDestFilename, const char *pcSrcFilename )
{
	string  acStr[512];
	string strDest;
	strcpy(acStr, pcSrcFilename);

	CFileNameOperation::ChangeExtension(acStr, "bspx");

	CFileNameOperation::GetBodyFilename( strDest, acStr, '\\' );

	strDest = "D:\\R&D\\Project\\App\\StageBase\\Stage\\" + strDest;


	strDestFilename = strDest;

}
*/

string CBSPMapCompiler::MakeFilenameForDebugFile( const string& debug_info )
{
	// create filename "DebugInfoFile\\" + (body filename of m_Filename) + pcDebugInfo

//	CFileNameOperation::GetBodyFilename( acBodyFilename, m_Filename );
	string filename = get_no_ext( m_Filename ) + debug_info;// + string(".txt");

	return get_path(m_Filename) + string("\\DebugInfoFile\\") + get_nopath(filename);
}