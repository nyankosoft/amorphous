#include "StaticGeometryCompiler.h"
#include <boost/filesystem.hpp>
#include "Graphics/MeshModel/TerrainMeshGenerator.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/StringAux.hpp"
#include "Support/lfs.hpp"
#include "Support/ImageArchive.hpp"
#include "Stage/StaticGeometry.hpp"
#include "BSPMapCompiler/LightmapBuilder.hpp"

#include "LightWave/3DMeshModelBuilder_LW.hpp"

// physics engine - used to save triangle mesh as geometry for collision detection
#include "Physics/PhysicsEngine.hpp"
#include "Physics/Preprocessor.hpp"
#include "Physics/TriangleMeshDesc.hpp"

#include "MeshOperations.hpp"

using namespace std;
using std::map;
using std::shared_ptr;
using namespace boost::filesystem;


int get_str_index( const std::string& name, std::vector<CGeometrySurfaceDesc>& vecDesc )
{
	for( size_t i=0; i<vecDesc.size(); i++ )
	{
		if( name == vecDesc[i].m_Name )
			return (int)i;
	}

	return 0;
}


class CLightPolygonSet
{
public:

	CConnectedSet m_ConnectedSet;

	AABB3 m_AABB;
};


// TODO: refactor MeshModel::AddTexturesToBinaryDatabase()
bool AddImageFileToBinaryDatabase( const std::string& image_filepath,
								   const std::string& db_filepath,
								   CBinaryDatabase<std::string> &db,
								   bool bUseImageFilepathBasenameForKey,
								   std::string& resource_path )

{
	string img_key;
	if( bUseImageFilepathBasenameForKey )
		img_key = lfs::get_leaf(image_filepath);
	else
		img_key = image_filepath;

	if( db.KeyExists(img_key) )
	{
		// the texture file has been already saved to database
		// - skip this texture
		LOG_PRINT( "The DB already has a data with the key: " + img_key );
		return false;
	}

	// - (db filename) + "::" + (archive key name)
	resource_path = db_filepath + "::" + img_key;

	ImageArchive img_archive = ImageArchive( image_filepath );

	if( img_archive.IsValid() )
	{
		// add image archive to db
		db.AddData( img_key, img_archive );
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( " - An invalid image filepath: " + image_filepath );
		return false;
	}
}


CStaticGeometryCompiler::CStaticGeometryCompiler()
{
	m_DatabaseRelativeDirPathAtRuntime = "./Stage";

	m_pGraphicsMesh = CreateGeneral3DMesh();
}


void CStaticGeometryCompiler::SaveToBinaryDatabase( const std::string& db_filename )
{
	/// save the static geometry archive
	CBinaryDatabase<string> db;

	// make sure that the destination directory path exists
	create_directories( path(db_filename).parent_path() );

	bool db_open = db.Open( db_filename, CBinaryDatabase<string>::DB_MODE_NEW );
	if( !db_open )
	{
		LOG_PRINT_ERROR( " - Cannot open a file for binary database: " + db_filename );
		return;
	}

	string db_file_relative_path = m_DatabaseRelativeDirPathAtRuntime + "/" + lfs::get_leaf(db_filename);

	// add mesh textures to db
	// - need to change the texture filename of the mesh
	//   from (texture filepath) to (db name)::(basename of texture filepath)
	string db_filepath = "./" + lfs::get_leaf(db_filename);
	const size_t num_meshes = m_vecDestGraphicsMeshArchive.size();
	for( size_t i=0; i<num_meshes; i++ )
	{
//		AddTexturesToBinaryDatabase( m_vecDestGraphicsMeshArchive[i], db_filepath, db );
		AddTexturesToBinaryDatabase( m_vecDestGraphicsMeshArchive[i], db_file_relative_path, db );
	}


	// add lightmap textures
//	if( m_pLightmapBuilder )
//		m_pLightmapBuilder->AddLightmapTexturesToDB( db );

	// add mesh archives to db
	m_Archive.m_vecMesh.resize( num_meshes );
	for( size_t i=0; i<num_meshes; i++ )
	{
		const string keyname = GetGraphicsMeshArchiveKey( (int)i );

		m_Archive.m_vecMesh[i].m_Desc.ResourcePath = db_file_relative_path + "::" + keyname;

		m_Archive.m_vecMesh[i].m_AABB = m_vecDestGraphicsMeshArchive[i].GetAABB();

		db.AddData( keyname, m_vecDestGraphicsMeshArchive[i] );
	}

	// add shader and technique info
	m_Archive.m_vecShaderContainer.resize( m_ShaderNameToShaderInfo.size() );	

	std::map<std::string,CShaderInfo>::iterator itr;
	for( itr = m_ShaderNameToShaderInfo.begin();
		 itr != m_ShaderNameToShaderInfo.end();
		 itr++ )
	{
		CShaderContainer& container = m_Archive.m_vecShaderContainer[ itr->second.ShaderIndex ];
//		container.ShaderFilepath = itr->first;
//		container.m_ShaderHandle.filename = itr->first;
		container.m_Desc.ResourcePath = itr->first;

		container.m_vecTechniqueHandle.resize( itr->second.TechniqueToIndex.size() );
		std::map<std::string,int>::iterator itrTechnique;
		for( itrTechnique = itr->second.TechniqueToIndex.begin();
			 itrTechnique != itr->second.TechniqueToIndex.end();
			 itrTechnique++ )
		{
			container.m_vecTechniqueHandle[ itrTechnique->second ].SetTechniqueName( itrTechnique->first.c_str() );
		}
	}

	SetShaderParameterGroups();


	// add textures specified as shader params to db
	const size_t num_shader_containers = m_Archive.m_vecShaderContainer.size();
	for( size_t i=0; i<num_shader_containers; i++ )
	{
		vector< ShaderParameter<TextureParam> >& shader_param_tex = m_Archive.m_vecShaderContainer[i].m_ParamGroup.m_Texture;
		const size_t num_textures = shader_param_tex.size();
		for( size_t j=0; j<num_textures; j++ )
		{
			string tex_filepath = shader_param_tex[j].GetParameter().m_Desc.ResourcePath;

			// load the image from file
			ImageArchive img_archive( tex_filepath );

			if( img_archive.IsValid() )
			{
				// use the original image filepath as key in the db
				const string keyname = tex_filepath;

				db.AddData( keyname, img_archive );

				shader_param_tex[j].Parameter().m_Desc.ResourcePath
					= db_file_relative_path + "::" + keyname;
			}
			else
			{
				LOG_PRINT_ERROR( "An invalid texture image file: " + tex_filepath );
			}
		}
	}

	// add the main static geometry archive to the db
	db.AddData( CStaticGeometryDBKey::Main, m_Archive );

	// graphics mesh archives
//	db.AddData( CStaticGeometryDBKey::GraphicsMeshes, m_vecDestGraphicsMeshArchive );

	// tree with mesh subset information for each node
//	db.AddData( CStaticGeometryDBKey::MeshSubsetTree, m_MeshSubsetTree );

	// collision geometry stream for physics engine
	// - stream data of triangle mesh
	if( 0 < m_CollisionMeshStream.m_Buffer.buffer().size() )
		db.AddData( CStaticGeometryDBKey::CollisionGeometryStream, m_CollisionMeshStream );
}

void CStaticGeometryCompiler::AddDestGraphicsMeshInstance()
{
	shared_ptr<General3DMesh> pGeneralMesh = CreateGeneral3DMesh();

	pGeneralMesh->SetVertexFormatFlags( m_pGraphicsMesh->GetVertexFormatFlags() );

	m_vecpDestGraphicsMesh.push_back( pGeneralMesh );
}
	
/// Assumes that polygons at each node can be fit in a single mesh instance
/// - Assumes polygons of src_mesh have been copied to the geometry array inside src_tree
/// - Subdivides src_mesh into multiple meshes if it has too many vertices / polygons
///   - Created meshes are stored to CStaticGeometryCompiler::m_vecpDestGraphicsMesh
void CStaticGeometryCompiler::CreateMeshSubsets_r( AABTree<IndexedPolygon>& src_tree,
												   int src_node_index,
												   General3DMesh& src_mesh,
					                               AABTree<CMeshSubset>& dest_tree )
{
	map<int,int> OldToNewMatIndex;

	const AABNode& node = src_tree.GetNode( src_node_index );

//	if( node.veciGeometryIndex.size() == 0 )
//		return;

	if( 12000 < m_vecpDestGraphicsMesh.back()->GetPolygonBuffer().size()
		+ node.veciGeometryIndex.size() )
	{
		// place the polygons and materials in this node on a new mesh instance
		AddDestGraphicsMeshInstance();
	}

	General3DMesh& dest_mesh = *m_vecpDestGraphicsMesh.back();

	vector<CMMA_Material>& dest_material_buffer = dest_mesh.GetMaterialBuffer();
	int mat_index_offset = (int)dest_material_buffer.size();

	// create old -> new material index mappings
	vector<CMMA_Material>& src_material_buffer = src_mesh.GetMaterialBuffer();
	const size_t num_polygons = node.veciGeometryIndex.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		IndexedPolygon& polygon = src_tree.GetGeometryBuffer()[ node.veciGeometryIndex[i] ];
		int mat_index = polygon.m_MaterialIndex;

		if( OldToNewMatIndex.find( mat_index ) == OldToNewMatIndex.end() )
		{
			// not registered yet
			// - create a map of old index to new index
			OldToNewMatIndex[polygon.m_MaterialIndex] = mat_index_offset++;

			// create a material for new index
			dest_material_buffer.push_back( src_material_buffer[polygon.m_MaterialIndex] );
		}

		// update index
		polygon.m_MaterialIndex = OldToNewMatIndex[polygon.m_MaterialIndex];
	}

	// append vertices and polygons to dest mesh

	const vector<General3DVertex>& src_vertex_buffer = *src_mesh.GetVertexBuffer().get();
	vector<General3DVertex>& dest_vertex_buffer = *dest_mesh.GetVertexBuffer().get();
	vector<IndexedPolygon>& dest_polygon_buffer = dest_mesh.GetPolygonBuffer();

	int vert_offset = (int)dest_vertex_buffer.size();
	map<int,int> OldToNewVertIndex;
	for( size_t i=0; i<num_polygons; i++ )
	{
		IndexedPolygon& polygon = src_tree.GetGeometryBuffer()[ node.veciGeometryIndex[i] ];

		const size_t num_verts = polygon.m_index.size();
		for( size_t j=0; j<num_verts; j++ )
		{
			if( OldToNewVertIndex.find( polygon.m_index[j] ) == OldToNewVertIndex.end() )
			{
				// not registered yet
				// - create a map of old index to new index
				OldToNewVertIndex[polygon.m_index[j]] = vert_offset++;

				// create a vertex in the dest vertex buffer
				dest_vertex_buffer.push_back( src_vertex_buffer[ polygon.m_index[j] ] );
			}

			// update index
			polygon.m_index[j] = OldToNewVertIndex[polygon.m_index[j]];
		}

		// add a polygon with the new vertices to the dest polygon buffer
		dest_polygon_buffer.push_back( polygon );
	}

	// create mesh subsets
	map<int,CMeshSubset> SurfGroupIndexToSubset;
	CMeshSubset subset;
	subset.MeshIndex = (int)m_vecpDestGraphicsMesh.size() - 1;
	subset.AABB = node.aabb;

	map<int,int>::iterator itr;
	for( itr = OldToNewMatIndex.begin();
		itr != OldToNewMatIndex.end();
		itr++ )
	{
		CMMA_Material& src_mat = src_material_buffer[ itr->first ];

		if( m_SurfaceNameToSurfaceDescIndex.find( src_mat.Name )
		 == m_SurfaceNameToSurfaceDescIndex.end() )
		{
			LOG_PRINT_WARNING( " - Cannot find a surface desc for the surface: " + src_mat.Name );
			continue;
		}

		// find a surface group for the current surface

		int surf_group_index
			= m_SurfaceNameToSurfaceDescIndex[ src_mat.Name ];

		const CGeometrySurfaceDesc& surf_desc = m_Desc.m_vecSurfaceDesc[surf_group_index];

		map<int,CMeshSubset>::iterator itrSubset
			= SurfGroupIndexToSubset.find( surf_group_index );

		if( itrSubset == SurfGroupIndexToSubset.end() )
		{
			// mesh subset for the surface group has not been created yet
			// - add a new subset
			SurfGroupIndexToSubset[surf_group_index] = subset;
			itrSubset = SurfGroupIndexToSubset.find( surf_group_index );
		}

		CMeshSubset& dest_subset = itrSubset->second;

		// add the new material index to the subset
		dest_subset.vecMaterialIndex.push_back( itr->second );

		// set a shader file index and a shader technique index

		const map<string,CShaderInfo>::const_iterator itrShaderInfo
			= m_ShaderNameToShaderInfo.find( surf_desc.m_ShaderFilepath );

		if( itrShaderInfo != m_ShaderNameToShaderInfo.end() )
		{
			const CShaderInfo& shader_info = itrShaderInfo->second;

			dest_subset.ShaderIndex = shader_info.ShaderIndex;

			const map<string,int>::const_iterator itrShaderTechnique
				= shader_info.TechniqueToIndex.find( surf_desc.m_ShaderTechnique );

			if( itrShaderTechnique != shader_info.TechniqueToIndex.end() )
			{
				dest_subset.ShaderTechniqueIndex = itrShaderTechnique->second;
			}
		}

		// projection matrix for variaing z-shifts
		// - stored per surface desc
		dest_subset.ProjectionMatrixIndex = surf_group_index;
	}

	// add all the mesh subsets created for this node to the mesh subset tree
	map<int,CMeshSubset>::iterator itrMeshSubset;
	for( itrMeshSubset = SurfGroupIndexToSubset.begin();
		 itrMeshSubset != SurfGroupIndexToSubset.end();
		 itrMeshSubset++ )
	{
		dest_tree.AddGeometry( itrMeshSubset->second );
	}

	if( 0 <= node.child[0] )
		CreateMeshSubsets_r( src_tree, node.child[0], src_mesh, dest_tree );

	if( 0 <= node.child[1] )
		CreateMeshSubsets_r( src_tree, node.child[1], src_mesh, dest_tree );
}


bool CreateGeneral3DMesh( std::shared_ptr<LWO2_Object> pSrcLWO2Object,
						  const GeometryFilter& geometry_filter,
						  General3DMesh& dest_general_mesh )
{
	shared_ptr<C3DMeshModelBuilder_LW> pLoader( new C3DMeshModelBuilder_LW );
	bool loaded = pLoader->LoadFromLWO2Object( pSrcLWO2Object, geometry_filter );

	if( !loaded )
		return false;

	pLoader->GetGeneral3DMesh( dest_general_mesh );

	return true;
}


/**
 Creates a general 3d mesh from model file.
 Returns true on success.
 \param [in] model_filepath
 \param [in] geometry_filter
 \param [out] dest_general_mesh
*/
bool CreateGeneral3DMesh( const std::string& model_filepath,
						  const GeometryFilter& geometry_filter,
						  General3DMesh& dest_general_mesh )
{
	LOG_FUNCTION_SCOPE();

	shared_ptr<C3DModelLoader> pLoader;
	if( lfs::get_ext( model_filepath ) == "lwo" )
	{
		pLoader = shared_ptr<C3DModelLoader>( new C3DMeshModelBuilder_LW() );
	}
	else
	{
		LOG_PRINT_ERROR( "Only the '.lwo' files are supported." );
		return false;
	}

	bool loaded = pLoader->LoadFromFile( model_filepath, geometry_filter );

	if( !loaded )
		return false;

	pLoader->GetGeneral3DMesh( dest_general_mesh );

	return true;
}


bool CStaticGeometryCompiler::CreateCollisionMesh()
{
	LOG_PRINT( " Creating a collision geometry mesh..." );

	// create collision mesh
	bool mesh_created = CreateGeneral3DMesh(
		m_Desc.m_InputFilepath,           // [in] .lwo, .3ds, etc.
		m_Desc.m_CollisionGeometryFilter, // [in] filter that specifies target layers, surfaces, etc.
		m_CollisionMesh                   // [out] general 3d mesh that holds collision mesh
		);

	if( !mesh_created )
	{
		LOG_PRINT_ERROR( " Failed to create a collision geometry mesh..." );
		return false;
	}

	physics::CTriangleMeshDesc coll_mesh_desc;
	m_CollisionMesh.GetIndexedTriangles(
		coll_mesh_desc.m_vecVertex,
		coll_mesh_desc.m_vecIndex,
		coll_mesh_desc.m_vecMaterialIndex
		);

	string physics_engine_name( "AgeiaPhysX" );

	physics::PhysicsEngine().Init( physics_engine_name );

	// access the preprocessor singleton
	// - preprocessor of physics_engine_name is used
	// - TODO: support different preprocessors of different physics engine during runtime
	physics::Preprocessor().CreateTriangleMeshStream( coll_mesh_desc, m_CollisionMeshStream );

	return true;
}


void LoadLightsFromLightPolygons( General3DMesh& light_polygon_mesh,
								  std::vector< std::shared_ptr<Light> >& dest_lights )
{
	LOG_FUNCTION_SCOPE();

	vector<CConnectedSet> connected_sets;
	GetConnectedSets( light_polygon_mesh, connected_sets );

	if( connected_sets.empty() )
		return;

	vector<CLightPolygonSet> polygons_sets;
	polygons_sets.resize( connected_sets.size() );
	for( size_t i=0; i<connected_sets.size(); i++ )
	{
		polygons_sets[i].m_ConnectedSet = connected_sets[i];
	}

	for( size_t i=0; i<connected_sets.size(); i++ )
	{
		Sphere sphere;
		bool spherical = AreVerticesOnSphere( light_polygon_mesh, connected_sets[i].vecPoint, sphere );
		if( spherical )
		{
		}
	}
}


bool CStaticGeometryCompiler::CreateLightmaps()
{
	if( !m_Desc.m_Lightmap.m_State == CLightmapDesc::LIGHTMAP_DISABLED )
		return false;

	shared_ptr<General3DMesh> pLightPolygonMesh( new General3DMesh );

	// Target the surface with the name "light_polygons"
	GeometryFilter geom_filter;
	geom_filter.Include.Surfaces.push_back( "light_polygons" );

	CreateGeneral3DMesh( m_Desc.m_InputFilepath, geom_filter, *pLightPolygonMesh );

	LOG_PRINTF(( " num vertices: %d, lightmap option %d", (int)pLightPolygonMesh->GetVertexBuffer()->size(), (int)m_Desc.m_Lightmap.m_State ));

//	if( pLightPolygonMesh->GetVertexBuffer()->empty()
	if( pLightPolygonMesh->GetPolygonBuffer().empty()
	 && m_Desc.m_Lightmap.m_State == CLightmapDesc::LIGHTMAP_NOT_SPECIFIED )
	{
		LOG_PRINT( " Light polygon mesh has no vertices && lightmap is not specified; leaving the function..." );
		return false;
	}

	// Try loading lights from mesh
	LoadLightsFromLightPolygons( *pLightPolygonMesh, m_Desc.m_vecpLight );

//	if( !m_Desc.m_Lightmap.m_Enabled )
//		return false;

	// create lightmaps
	// [in] polygons
	// [in] polygon mesh tree for raycast
	// [in] lights

	CLightmapBuilder lightmap_builder;

	// a point light for test
//	shared_ptr<CPointLight> pPntLight = shared_ptr<CPointLight>( new CPointLight() );
//	pPntLight->Color = SFloatRGBColor( 1.0f, 0.0f, 0.0f );
//	pPntLight->vPosition = Vector3( 5, 2.8, 5 );
//	m_Desc.m_vecpLight.push_back( pPntLight );

	m_Desc.m_Lightmap.m_pvecpLight = &m_Desc.m_vecpLight;
	m_Desc.m_Lightmap.m_pMesh = m_pGraphicsMesh.get();
	m_Desc.m_Lightmap.m_vecEnableLightmapForSurface.resize( m_pGraphicsMesh->GetMaterialBuffer().size(), 1 );
	m_Desc.m_Lightmap.m_OutputDatabaseFilepath = lfs::get_leaf( m_Desc.m_OutputFilepath );

	return lightmap_builder.CreateLightmapTexture( m_Desc.m_Lightmap );
}


void CollectLightPolygons( General3DMesh& mesh )
{
	vector<IndexedPolygon>& polygons = mesh.GetPolygonBuffer();
}


void CStaticGeometryCompiler::CopyTreeNodes_r( TerrainMeshTree& src_tree, TerrainMeshNode& node,
					  CNonLeafyAABTree<IndexedPolygon>& dest_tree, int dest_node_index/*, AABNode& dest_node*/ )
{
	AABNode& dest_node = dest_tree.GetNodeBuffer()[dest_node_index];

	dest_node.aabb  = node.m_AABB;
	dest_node.iAxis = node.m_Axis;
	dest_node.fDist = node.m_AABB.GetCenterPosition()[node.m_Axis];

	if( node.IsLeaf() )
	{
		const size_t num_indices = node.m_vecPolygonIndex.size();
		dest_node.veciGeometryIndex.resize( num_indices );
		for( size_t i=0; i<num_indices; i++ )
			dest_node.veciGeometryIndex[i] = node.m_vecPolygonIndex[i];
	}
	else
	{
		// save child indices now since dest_nodes gets invalidated after calling dest_tree.Subdivide()
//		int child0 = dest_node.child[0];
//		int child1 = dest_node.child[1];
		dest_tree.Subdivide( dest_node, node.m_Axis );
		CopyTreeNodes_r( src_tree, node.m_child[0], dest_tree, /*child0*/dest_tree.GetNode( dest_node_index ).child[0] );
		CopyTreeNodes_r( src_tree, node.m_child[1], dest_tree, /*child1*/dest_tree.GetNode( dest_node_index ).child[1] );
	}
}


/// m_pGraphicsMesh gets updated if the mesh is successfully subdivided
bool CStaticGeometryCompiler::SubdivideGraphicsMesh( CTerrainMeshGenerator& mesh_divider )
{
	LOG_PRINT( " Subdividing the mesh..." );

	mesh_divider.SetOutputTextureFormat( m_Desc.m_TextureSubdivisionOptions.m_OutputImageFormat );
	mesh_divider.SetSplitTextureWidth( m_Desc.m_TextureSubdivisionOptions.m_SplitSize );
	mesh_divider.SetTextureOutputDirectory( "./temp/" );

	/// find surfaces that need texture subdivision
	/// i.e.) surface with textures larger than 2048x2048
	/// NOT IMPLEMENTED
	/// - manually specified through the desc file

	int surf_index = m_pGraphicsMesh->GetMaterialIndexFromName( m_Desc.m_TextureSubdivisionOptions.m_TargetSurfaceName );
	if( surf_index < 0 )
		surf_index = 0; // use the first material by default
	mesh_divider.SetTargetMaterialIndex( surf_index );

	// save the surface group (desc) of the target surface
	const string surf_group_name = m_Desc.m_SurfaceToDesc[m_pGraphicsMesh->GetMaterialBuffer()[surf_index].Name];

	/// save the index of the source surface
	/// TODO: support the surface other than the first one

	bool res = mesh_divider.BuildTerrainMesh( m_pGraphicsMesh );
	if( res )
		*m_pGraphicsMesh = *mesh_divider.GetDestMesh(); // overwrite the graphics mesh with the subdivided mesh
	else
		return false;

	// update the (surface name) : (surface group) name mappings
	// since new surfaces are added to the mesh during the texture subdivision
	vector<CMMA_Material>& material_buffer = m_pGraphicsMesh->GetMaterialBuffer();
	for( size_t mat=0; mat<material_buffer.size(); mat++ )
		m_Desc.m_SurfaceToDesc[material_buffer[mat].Name] = surf_group_name;

	return true;
}


void CStaticGeometryCompiler::SetShaderParameterGroups()
{
	std::map<std::string,ShaderParameterGroup>::iterator itr;
	for( itr = m_Desc.m_ShaderFileToParamGroup.begin();
		 itr != m_Desc.m_ShaderFileToParamGroup.end();
		 itr++ )
	{
		// find shader container that stores the file for the shader param group
		for( size_t i=0; i<m_Archive.m_vecShaderContainer.size(); i++ )
		{
			if( m_Archive.m_vecShaderContainer[i].m_Desc.ResourcePath == itr->first )
			{
				m_Archive.m_vecShaderContainer[i].m_ParamGroup = itr->second;
				break;
			}
		}
	}
}


bool CStaticGeometryCompiler::CompileGraphicsGeometry()
{
	LOG_PRINT( " Creating a mesh..." );

	// create a single graphics mesh that contains all the graphics geometry
	bool mesh_created = CreateGeneral3DMesh(
		m_Desc.m_InputFilepath, // [in] .lwo, .3ds, etc.
		m_Desc.m_GraphcisGeometryFilter, // [in] filter that specifies target layers, surfaces, etc.
		*m_pGraphicsMesh // [out] general 3d mesh that holds graphics mesh
		);

	if( !mesh_created )
	{
		LOG_PRINT_ERROR( " Failed to create the mesh..." );
		return false;
	}

	// separate graphics geometry
	// 1. geometry that uses lightmap
	// 2. geometry that does not use lightmap

	// create polygon mesh tree for ray test in lightmap creation?

	// subdivide over-sized textures
	// - also subdivide polygons on the edges of the split textures
	// - recalculate texture coords
	// - returns an array of general 3d meshes
	CTerrainMeshGenerator mesh_divider;
	if( m_Desc.m_TextureSubdivisionOptions.m_Enabled )
	{
		SubdivideGraphicsMesh( mesh_divider );
	}

	// create (surface name) to (surface group index) mapping
	// Must be called after SubdivideGraphicsMesh() since new surfaces are added
	// to the mesh during textrue subdivision
	UpdateSurfaceNameToSurfaceGroupIndexMapping();

	// lightmap
	CreateLightmaps();

	/// make the polygon tree

	CNonLeafyAABTree<IndexedPolygon> tree;
	if( false /*m_Desc.m_TextureSubdivisionOptions.m_Enabled*/ )
	{
		// use the polygon tree created during the process of
		// splitting oversized textures
		tree.CreateRootNode( mesh_divider.GetMeshTree().GetRootNode().m_AABB );
		CopyTreeNodes_r( mesh_divider.GetMeshTree(), mesh_divider.GetMeshTree().GetRootNode(), tree, 0 );
		tree.SetGeometry( m_pGraphicsMesh->GetPolygonBuffer() );
	}
	else
	{
		// create tree
		CMeshTreeOptions& tree_options = m_Desc.m_MeshTreeOptions;
		tree.SetMinimumCellVolume( tree_options.MinimumCellVolume /* 1000000.0f */ );
		tree.SetNumMaxGeometriesPerCell( tree_options.NumMaxGeometriesPerCell /* 100 */ );
		tree.SetMaxDepth( tree_options.MaxDepth /* 6 */ );
		tree.SetRecursionStopCondition( (AABTree<IndexedPolygon>::RecursionStopCond)tree_options.RecursionStopCondition /* AABTree<IndexedPolygon>::COND_OR */ );

		// build the tree
		// - copies of indexed polygons are created and stored in the tree
		tree.Build( m_pGraphicsMesh->GetPolygonBuffer() );
	}

	CNonLeafyAABTree<CMeshSubset>& mesh_subset_tree = m_Archive.m_MeshSubsetTree;

	// copy tree structure
	// - i.e. all the tree nodes
	mesh_subset_tree.GetNodeBuffer() = tree.GetNodeBuffer();

	// remove all the geometry indices at each node
	mesh_subset_tree.ResetRegisteredGeometries();

	// set the first graphics mesh instance to m_vecpDestGraphicsMesh
	AddDestGraphicsMeshInstance();

	if( m_Desc.m_vecSurfaceDesc.empty() )
	{
		LOG_PRINT_ERROR( " No surface descs were found." );
		return false;
	}

	CreateMeshSubsets_r( tree,
		                 0, // node index
						 *m_pGraphicsMesh,
						 mesh_subset_tree // tree that stores mesh subsets
						 );

	// create mesh archives
	const size_t num_graphics_meshes = m_vecpDestGraphicsMesh.size();
	m_vecDestGraphicsMeshArchive.resize( num_graphics_meshes );
	for( size_t i=0; i<num_graphics_meshes; i++ )
	{
		C3DMeshModelBuilder general_mesh_to_mesh_archive_converter;
		general_mesh_to_mesh_archive_converter.BuildMeshModelArchive( m_vecpDestGraphicsMesh[i] );

		m_vecDestGraphicsMeshArchive[i] = general_mesh_to_mesh_archive_converter.GetArchive();
	}

	return true;
}


void CStaticGeometryCompiler::UpdateSurfaceNameToSurfaceGroupIndexMapping()
{
	map<string,string>::const_iterator itr;
	for( itr = m_Desc.m_SurfaceToDesc.begin();
		 itr != m_Desc.m_SurfaceToDesc.end();
		 itr++ )
	{
		m_SurfaceNameToSurfaceDescIndex[ itr->first ] = get_str_index( itr->second, m_Desc.m_vecSurfaceDesc );
	}
}


bool CStaticGeometryCompiler::CompileFromXMLDescFile( const std::string& xml_filepath )
{
	LOG_FUNCTION_SCOPE();

	bool loaded = m_Desc.LoadFromXML( xml_filepath );

	// set relative directory path
	// e.g., m_Desc.m_OutputFilepath:           "../../app/models/sg.bin"
	// e.g., m_Desc.m_ProgramRootDirectoryPath: "../../app"

	size_t pos = m_Desc.m_OutputFilepath.find( m_Desc.m_ProgramRootDirectoryPath );// validate
	if( pos != string::npos )
	{
		m_DatabaseRelativeDirPathAtRuntime
			= lfs::get_parent_path(m_Desc.m_OutputFilepath).substr( m_Desc.m_ProgramRootDirectoryPath.length() );

		if( 0 < m_DatabaseRelativeDirPathAtRuntime.length()
		 && m_DatabaseRelativeDirPathAtRuntime[0] == '/' )
			m_DatabaseRelativeDirPathAtRuntime = "." + m_DatabaseRelativeDirPathAtRuntime;
	}

	// set the working directory to the directory path of the xml_filepath
	lfs::dir_stack dir_stk;
	dir_stk.push_cwd_and_chdir( lfs::get_parent_path( xml_filepath ) );

	int shader_index_offset = 0;
	const size_t num_surf_descs = m_Desc.m_vecSurfaceDesc.size();
	for( size_t i=0; i<num_surf_descs; i++ )
	{
		const string& shader_filepath = m_Desc.m_vecSurfaceDesc[i].m_ShaderFilepath;

		if( m_ShaderNameToShaderInfo.find( shader_filepath ) == m_ShaderNameToShaderInfo.end() )
		{
			// register the shader filepath and its index
			m_ShaderNameToShaderInfo[shader_filepath] = CShaderInfo(shader_index_offset);

			shader_index_offset++;
		}

		CShaderInfo& shader_info = m_ShaderNameToShaderInfo[shader_filepath];

		const string& shader_technique_name = m_Desc.m_vecSurfaceDesc[i].m_ShaderTechnique;

		if( shader_info.TechniqueToIndex.find( shader_technique_name )
		 == shader_info.TechniqueToIndex.end() )
		{
			shader_info.TechniqueToIndex[shader_technique_name]
			= shader_info.TechniqueIndexOffset;

			shader_info.TechniqueIndexOffset++;
		}
	}

	bool coll_mesh_created = CreateCollisionMesh();

	bool graphics_mesh_compiled = CompileGraphicsGeometry();

	SaveToBinaryDatabase( m_Desc.m_OutputFilepath );

	// restore the original working directory
	dir_stk.pop_and_chdir();

	return true;
}
