
#include "StaticGeometryCompilerFG.h"
#include "Support/memory_helpers.h"
#include "Support/ParamLoader.h"
#include "Support/fnop.h"
#include "Support/StringAux.h"
#include "Support/Serialization/BinaryDatabase.h"
#include "Support/Log/DefaultLog.h"

#include "3DCommon/Shader/ShaderTechniqueHandle.h"
#include "3DCommon/MeshModel/TerrainMeshGenerator.h"

#include "LightWave/LightWaveObject.h"
#include "LightWave/3DMeshModelBuilder_LW.h"
using namespace MeshModel;

#include "JigLib/TriangleMesh.h"

using namespace std;
using namespace boost;


#include <direct.h>
#include <sys/stat.h>


// default size of the each split texture
#define DEFAULT_TEXTURE_SPLIT_WIDTH 1024
#define DEFAULT_TERRAIN_TEX_COORD_SHIFT_U -0.0078757f
#define DEFAULT_TERRAIN_TEX_COORD_SHIFT_V  0.0078757f


class LastBuildInfo : public IArchiveObjectBase
{
public:

	std::string m_LastModelFilename;

	unsigned long m_ModelFileLastModifiedTime;

public:

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


void LastBuildInfo::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_LastModelFilename;
	ar & m_ModelFileLastModifiedTime;
}



CStaticGeometryCompilerFG::CStaticGeometryCompilerFG()
{
	m_OutputTextureImageFormat = "bmp";
}


CStaticGeometryCompilerFG::~CStaticGeometryCompilerFG()
{
	Release();
}


void CStaticGeometryCompilerFG::Release()
{
	m_pObject.reset();
}


bool CStaticGeometryCompilerFG::CreateMeshArchive( const string& target_layer_name,
												   C3DMeshModelArchive& dest_mesh )
{
	LOG_PRINT( " - target layer name: " + target_layer_name );
	// collect layers that start with certain keywords
	vector<CLWO2_Layer *> vecpMeshLayer
	= m_pObject->GetLayersWithKeyword( target_layer_name, CLWO2_NameMatchCond::START_WITH );

	if( vecpMeshLayer.size() == 0 )
	{
		LOG_PRINT_WARNING( fmt_string(" - Target layer '%s' was not found.", target_layer_name.c_str()) );
		return false;
	}
	else
	{
		LOG_PRINT( fmt_string(" - Found %d target layer(s)", vecpMeshLayer.size()) );
	}

	SLayerSet layer_set(vecpMeshLayer);
//	vecLayerSet.push_back( SLayerSet(vecpMeshLayer) );

	shared_ptr<C3DMeshModelBuilder_LW> pModelLoader;

//	if( 1 < num_layer_sets ) num_layer_sets = 1;
//	for( i=0; i<num_layer_sets; i++ )
//	{
		pModelLoader
			= shared_ptr<C3DMeshModelBuilder_LW>( new C3DMeshModelBuilder_LW( m_pObject ) );

		// TODO: relative path for terrain geometry
		// A valid relative path has to be set for the terrain mesh
		// because terrain mesh generator needs to load, split & save them later
//		pModelBuilder->SetTextureFilenameOption( C3DMeshModelBuilder::TFN_FIXEDPATH_AND_FILENAME );
//		pModelBuilder->SetFixedPathForTextureFilename( "Texture" );
		pModelLoader->SetTexturePathnameOption( TexturePathnameOption::RELATIVE_PATH_AND_BODY_FILENAME );

		pModelLoader->BuildMeshModel( layer_set );

		LOG_PRINT( " - Loaded 3D model" );

		C3DMeshModelBuilder model_builder;

		model_builder.BuildMeshModel( pModelLoader );

		LOG_PRINT( " - Created a 3D mesh archive from layer(s): " + target_layer_name );

		dest_mesh = model_builder.GetArchive();
//	}
	return true;
}


const string gs_TempTextureDirectory = "temp";

bool CStaticGeometryCompilerFG::CreateTerrainMesh( int split_tex_width, double tex_coord_shift_u, double tex_coord_shift_v )
{
	g_Log.Print( "CStaticGeometryCompilerFG::CreateTerrainMesh()" );

	C3DMeshModelArchive src_mesh;
	bool loaded = CreateMeshArchive( "Terrain", src_mesh );

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Cannot create a terrain mesh archive." );
		return false;
	}
	else if( src_mesh.GetMaterial().size() == 0 )
	{
		LOG_PRINT_ERROR( " - Terrain mesh has no material." );
		return false;
	}

	LOG_PRINT( " - mesh archive for terrain has been created." );

	CTerrainMeshGenerator& generator = m_TerrainMeshGenerator;

	// set options for terrain mesh generation
	if( /* m_bAddTextureToBinaryDatabase == */ true )
	{
		mkdir( gs_TempTextureDirectory.c_str() );
		generator.SetTextureOutputDirectory( "./" + gs_TempTextureDirectory );
		generator.SetOutputTextureRelativePath( "./" + gs_TempTextureDirectory + "/" ); // used in AddTexturesToBinaryDatabase() to load texture files and create image archive
	}
	else
	{
		generator.SetTextureOutputDirectory( m_TextureOutputDirectory );
		generator.SetOutputTextureRelativePath( "./Texture/" ); // TODO: support correct path settings
	}

	generator.SetOutputTextureFormat( m_OutputTextureImageFormat );
	generator.SetSplitTextureWidth( split_tex_width );
	generator.SetTerrainTexCoordShiftU( tex_coord_shift_u );
	generator.SetTerrainTexCoordShiftV( tex_coord_shift_v );

	// - subdivide src_mesh
	// - fix texture coords
	generator.BuildTerrainMesh( src_mesh );

	const size_t num_terrain_mesh_archives = generator.GetNumMeshArchives();

	LOG_PRINT( fmt_string( " - %d mesh archives were created for terrain.", num_terrain_mesh_archives ) );

	if( num_terrain_mesh_archives == 0 )
	{
		LOG_PRINT_ERROR( " - No terrain mesh was generated." );
		return false;
	}

	// create mesh group for terrain meshes
	CStaticGeometryMeshGroup terrain_mesh_group;
	terrain_mesh_group.m_StartIndex = (int)m_vecMeshArchive.size();
	terrain_mesh_group.m_EndIndex   = (int)m_vecMeshArchive.size() + (int)num_terrain_mesh_archives - 1;

	terrain_mesh_group.m_vecShaderTechnique.resize( 1 );
	terrain_mesh_group.m_vecShaderTechnique[0].SetTechniqueName( m_TerrainMeshShaderTechnique.c_str() );

	terrain_mesh_group.m_fDepthShiftFactor = 0.15f;

	// save the subdivided meshes as terrain mesh archives
	size_t i;
	for( i=0; i<num_terrain_mesh_archives; i++ )
	{
		m_vecMeshArchive.push_back( generator.GetMeshArchive( (int)i ) );

		// add keys to retrieve mesh archives from binary database
		m_Archive.m_vecMeshArchiveKey.push_back( fmt_string( "TerrainMesh%02d", i ) );
	}

	m_Archive.m_vecMeshGroup.push_back( terrain_mesh_group );

	return true;
}


bool CStaticGeometryCompilerFG::CreateBuildingsMesh()
{
	vector<C3DMeshModelArchive> vecBuildingsMesh;

	// TODO: support multiple mesh archives
	vecBuildingsMesh.resize( 1 );

	CreateMeshArchive( "Buildings", vecBuildingsMesh[0] );

	if( vecBuildingsMesh.size() == 0 )
		return false; // no mesh for buildings

	// create mesh group for buildings

	CStaticGeometryMeshGroup buildings_mesh_group;
	buildings_mesh_group.m_StartIndex = (int)m_vecMeshArchive.size();
	buildings_mesh_group.m_EndIndex   = (int)m_vecMeshArchive.size() + (int)vecBuildingsMesh.size() - 1;

	buildings_mesh_group.m_vecShaderTechnique.resize( 1 );
	buildings_mesh_group.m_vecShaderTechnique[0].SetTechniqueName( "BuildingsFG" );

	// no depth shifting for building mesh since they are rendered on top of terrain
	buildings_mesh_group.m_fDepthShiftFactor = 0.0f;//0.10f;

	m_Archive.m_vecMeshGroup.push_back( buildings_mesh_group );

	// save mesh archives
	size_t i, num_meshes = vecBuildingsMesh.size();
	for( i=0; i<num_meshes; i++ )
	{
		m_vecMeshArchive.push_back( vecBuildingsMesh[i] );

		m_Archive.m_vecMeshArchiveKey.push_back( fmt_string( "BuildingsMesh%02d", i ) );
	}

	return true;
}

/*
bool CStaticGeometryCompilerFG::CreateStaticGeometryMesh()
{
	C3DMeshModelArchive mesh;
	bool loaded = CreateMeshArchive( "StaticGeometry", mesh );

	if( !loaded )
		return false;

	m_Archive.m_vecMeshArchive.push_back( mesh );

	return true;
}
*/

bool CStaticGeometryCompilerFG::CreateSkyboxMesh()
{
	if( !m_pObject )
		return false;

	C3DMeshModelArchive skybox_mesh;
	bool loaded = CreateMeshArchive( "Skybox", skybox_mesh );

	if( loaded )
	{
		// skybox mesh is found in the source model
		// - save it to the archive
		m_Archive.m_SkyboxMeshArchive = skybox_mesh;
	}
	else if( 0 < m_SkyboxTextureFilename.length() )
	{
		// no skybox mesh is found in the source model, but texture filename is
		// specified in the desc file
		// - overwrite the texture filename and save the default skybox mesh
		//   to the archive
		m_Archive.m_SkyboxMeshArchive = m_DefaultSkybox;

		CMMA_Material& mat0 = m_Archive.m_SkyboxMeshArchive.GetMaterial()[0];
//		mat0.SurfaceTexture.strFilename = m_SkyboxTextureFilename;
//		mat0.SurfaceTexture.type = CMMA_Texture::FILENAME;
		mat0.vecTexture.resize( 1 );
		mat0.vecTexture[0].strFilename = m_SkyboxTextureFilename;
		mat0.vecTexture[0].type = CMMA_Texture::FILENAME;
	}
	else
	{
		// no skybox info is provided
		// - skybox mesh is not saved in the archive.
		//   release any previous skybox mesh archive
		// Do this when you want to load a skybox later (e.g. from script)
		m_Archive.m_SkyboxMeshArchive = C3DMeshModelArchive();
	}


	return true;
}


void CStaticGeometryCompilerFG::CreateCollisionMesh()
{
	C3DMeshModelBuilder mesh_builder;
	shared_ptr<C3DMeshModelBuilder_LW> pModelLoader
		= shared_ptr<C3DMeshModelBuilder_LW>( new C3DMeshModelBuilder_LW( m_pObject ) );

	vector<CLWO2_Layer*> vecpCollMeshLayer = m_pObject->GetLayersWithKeyword( "CollisionMesh" );

	// TODO: build collision mesh from multiple layers
	if( 0 < vecpCollMeshLayer.size() )
	{
		SLayerSet layer_info;
		layer_info.vecpMeshLayer.push_back( vecpCollMeshLayer[0] );

		// load the model
		pModelLoader->BuildMeshModel( layer_info );

		mesh_builder.BuildMeshModel( pModelLoader );

		C3DMeshModelArchive src_mesh_archive = mesh_builder.GetArchive();

		if( /* create_separate_archive_file_for_collision_mesh == */ true )
		{
			vector<short> vecsTriangleMaterial;
			vecsTriangleMaterial.resize( src_mesh_archive.GetVertexIndex().size() / 3, 0 );

			vector<int> vecVertexIndex;
			vecVertexIndex.resize( src_mesh_archive.GetVertexIndex().size() );
			size_t i, num_indices = src_mesh_archive.GetVertexIndex().size();
			for( i=0; i<num_indices; i++ )
				vecVertexIndex[i] = src_mesh_archive.GetVertexIndex()[i];

			CBSPTreeForTriangleMesh& trimesh = m_TriangleMesh;
//			trimesh.SetNumMaxTrianglesPerCell( 16 );
//			trimesh.SetMinimumCellVolume( 1000000.0f );
			trimesh.SetNumMaxGeometriesPerCell( 128 );
			trimesh.SetMinimumCellVolume( 1000000000000.0f );
			trimesh.SetMaxDepth( 10 );
			trimesh.SetRecursionStopCondition( "and" );
			trimesh.CreateMesh(
				src_mesh_archive.GetVertexSet().vecPosition,
				vecVertexIndex,
				vecsTriangleMaterial );

			LOG_PRINT( " - created collision mesh and tree" );

			trimesh.SaveToFile( "trimesharchive" );

			LOG_PRINT( " - saved collision mesh archive" );
		}
		else
		{
			// copy vertices
			vector<Vector3>& vecPosition = src_mesh_archive.GetVertexSet().vecPosition;
			m_Archive.m_CollisionMeshArchive.vecVertex.assign( vecPosition.begin(), vecPosition.end() );

			// copy indices
			vector<unsigned int>& vecIndex = src_mesh_archive.GetVertexIndex();
			CopyVector( m_Archive.m_CollisionMeshArchive.vecIndex, vecIndex );

			// TODO: support material for each triangle
			int num_triangles = (int)vecIndex.size() / 3;
			m_Archive.m_CollisionMeshArchive.vecMaterialIndex.resize( num_triangles, 0 );
		}
	}
}


bool CStaticGeometryCompilerFG::SetDefaultSkyboxMesh( const std::string& mesh_archive_filename )
{
//	return m_Archive.m_SkyboxMeshArchive.LoadFromFile( mesh_archive_filename );
	return m_DefaultSkybox.LoadFromFile( mesh_archive_filename );
}


bool CStaticGeometryCompilerFG::Build( const string& desc_filename )
{
	LOG_FUNCTION_SCOPE();

	Release();

	string model_filename, skybox_tex_filename, output_filename;
	int split_tex_width = DEFAULT_TEXTURE_SPLIT_WIDTH;
	double tex_coord_shift_u = DEFAULT_TERRAIN_TEX_COORD_SHIFT_U;
	double tex_coord_shift_v = DEFAULT_TERRAIN_TEX_COORD_SHIFT_V;
	SFloatRGBAColor ambient_color = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f);
	SFloatRGBAColor fog_color = SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f);

	float tile_tex_weight = 0;
	string tile_tex_filename;

	float spec_min_y = 0, spec_max_y = 0;
	SFloatRGBColor spec;
	float spec_intensity = 0;
	string shader;

	CParamLoader loader( desc_filename );
	if( loader.IsReady() )
	{
		loader.LoadParam( "output",				output_filename );	// [out]

		loader.LoadParam( "model",				model_filename );	// [in] (also for all the rest)

		loader.LoadParam( "skybox_tex",			m_SkyboxTextureFilename );

		loader.LoadParam( "ambient_color",		ambient_color.fRed, ambient_color.fGreen, ambient_color.fBlue );

		loader.LoadParam( "fog_color",			fog_color.fRed, fog_color.fGreen, fog_color.fBlue );
		loader.LoadParam( "fog_start_dist",		m_Archive.m_FogStartDist );
		loader.LoadParam( "far_clip_dist",		m_Archive.m_FarClipDist );

		loader.LoadParam( "tex_outupt_format",	m_OutputTextureImageFormat );
		loader.LoadParam( "tex_output_dir",		m_TextureOutputDirectory );
		loader.LoadParam( "tex_width",			split_tex_width );
		loader.LoadParam( "tex_coord_shift_u",	tex_coord_shift_u );
		loader.LoadParam( "tex_coord_shift_v",	tex_coord_shift_v );

		loader.LoadParam( "spec_range_y",		spec_min_y, spec_max_y );
		loader.LoadParam( "spec_intensity",		spec_intensity );
		loader.LoadParam( "spec_color",			spec.fRed, spec.fGreen, spec.fBlue );

		loader.LoadParam( "tile_texture",		tile_tex_weight, tile_tex_filename );

		loader.LoadParam( "shader",				shader );
	}
	else
		return false;

	// save shader technique name
	m_TerrainMeshShaderTechnique = shader;

	LOG_PRINT( " - loaded a desc file: " + desc_filename );

	if( model_filename.length() == 0
	 || output_filename.length() == 0 )
		return false;

	// create static geometry mesh and collision mesh if necessary
	if( NeedToCompileModelFile( model_filename, output_filename ) )
		CompileModelFile( model_filename, split_tex_width, tex_coord_shift_u, tex_coord_shift_v );
	else
		LOG_PRINT( " - model file has not been modified since the last compilation: updating the archive" );

	// skybox compilation routine is called every time whether the model is updated or not
	// because the skybox option has been changed
	// - think what it would be like when the model is the same with the last time
	//   but the user has changed the option from 'skybox mesh in the source model'
	//   to 'default skybox' + 'texture filename in desc file'
	CreateSkyboxMesh();

	// static geometry properties
	m_Archive.m_SpecularColor = spec;
	m_Archive.m_fSpecularIntensity = spec_intensity;
	m_Archive.m_strTileTextureFilename = tile_tex_filename;
	m_Archive.m_fTileTextureBlendWeight = tile_tex_weight;
	m_Archive.m_AmbientColor = ambient_color;
	m_Archive.m_FogColor = fog_color;

	m_Archive.WriteToTextFile( fnop::get_no_ext(desc_filename) );

	return SaveToFile( output_filename );
}


bool CStaticGeometryCompilerFG::CompileModelFile( const string& model_filename,
												  int split_tex_width,
												  double tex_coord_shift_u,
												  double tex_coord_shift_v )
{
	// load light wave model data
	m_pObject =  shared_ptr<CLWO2_Object>( new CLWO2_Object() );

	if( !m_pObject->LoadLWO2Object( model_filename.c_str() ) )
	{
		Release();
		return false;
	}

	LOG_PRINT( " - loaded:" + m_pObject->GetFilename() );

	// special static geometry for terrain
	CreateTerrainMesh( split_tex_width, tex_coord_shift_u, tex_coord_shift_v );

	LOG_PRINT( " - Created a terrain mesh" );

	// general static geometry for terrain
//	CreateStaticGeometryMesh();

//	LOG_PRINT( " - Created a static geometry mesh" );

	// mesh for skybox.
	// default model is used when no layer is specified for skybox
	// 20:59 2007/09/22 moved to Build()
//	CreateSkyboxMesh();

//	g_Log.Print( "CStaticGeometryCompilerFG::CompileModelFile() - skybox mesh created" );

	AABB3 aabb;
	aabb.Nullify();
	for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
        aabb.MergeAABB( m_vecMeshArchive[i].GetAABB() );
	m_Archive.m_AABB = aabb;

	CreateCollisionMesh();

	LOG_PRINT( "- Created a collision mesh" );

	return true;
}


/**
 *
 * loads the previous static geometry archive if it is recorded in last build info
 * and still exists in the dest directory
 */
bool CStaticGeometryCompilerFG::NeedToCompileModelFile( const string& input_model_filename,
													    const string& output_filename )
{
	return true;

/*
	LastBuildInfo last_build;
	bool lastbuildinfoloaded = last_build.LoadFromFile( "lastbuild.bin" );
	if( !lastbuildinfoloaded )
	{
		last_build.m_LastModelFilename = input_model_filename;
		last_build.m_ModelFileLastModifiedTime = (unsigned long)0;
		last_build.SaveToFile( "lastbuild.bin" );

		return true;	// no previous build
	}

	struct stat prev_model_file;
	if( stat(last_build.m_LastModelFilename.c_str(), &prev_model_file) != 0 )
		return true;

	CStaticGeometryArchiveFG prev_archive;
	bool prev_archive_loaded = prev_archive.LoadFromFile( output_filename );
	if( !prev_archive_loaded )
		return true;

	if( last_build.m_LastModelFilename == input_model_filename
	 && last_build.m_ModelFileLastModifiedTime == (unsigned long)prev_model_file.st_mtime )
	{
		m_Archive.m_vecMeshArchive       = prev_archive.m_vecMeshArchive;
		m_Archive.m_SkyboxMeshArchive    = prev_archive.m_SkyboxMeshArchive;
		m_Archive.m_CollisionMeshArchive = prev_archive.m_CollisionMeshArchive;

		return false;
	}
	else
	{
		// update last build info
		last_build.m_LastModelFilename = input_model_filename;
		last_build.m_ModelFileLastModifiedTime = (unsigned long)prev_model_file.st_mtime;
		last_build.SaveToFile( "lastbuild.bin" );

		return true;
	}
*/
}


string gs_DBFilenameRelativePathForTextureResource = "./";


bool CStaticGeometryCompilerFG::SaveToFile( const string& output_filename )
{
//	return m_Archive.SaveToFile( output_filename );

	CBinaryDatabase<string> db;

	bool open = db.Open(  output_filename, CBinaryDatabase<string>::DB_MODE_NEW );
	if( !open )
	{
		LOG_PRINT_ERROR( "Cannot open file to create binary database: " + output_filename );
		return false;
	}

	// primary archive for static geometry
	db.AddData( "Main", m_Archive );

	if( /* m_bAddTextureToBinaryDatabase == */ true )
	{
		// - create image archives and add them to db
		// - change the texture file names stored in m_vecMeshArchive
		string db_filepath = gs_DBFilenameRelativePathForTextureResource + fnop::get_nopath(output_filename);
		for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
			AddTexturesToBinaryDatabase( m_vecMeshArchive[i], db_filepath, db );
	}

	// mesh archives
	size_t i, num_mesh_archives = m_vecMeshArchive.size();
	LOG_PRINT( fmt_string( " Saving %d mesh archives to database file...", num_mesh_archives ) );
	for( i=0; i<num_mesh_archives; i++ )
	{
		db.AddData( m_Archive.m_vecMeshArchiveKey[i], m_vecMeshArchive[i] );
	}

	// precompiled triangle mesh for collision
	if( 0 < m_TriangleMesh.GetNumNodes() )
	{
		LOG_PRINT( " - adding collish mesh to binary database..." );
		db.AddData( CStaticGeometryArchiveFG::ms_CollisionMeshTreeKey, m_TriangleMesh );
	}

	db.Close();

	return true;
}
