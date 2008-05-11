#include "StaticGeometryCompiler.h"
#include "Support/Serialization/BinaryDatabase.h"
#include "Support/StringAux.h"
#include "Support/fnop.h"
#include "Stage/StaticGeometry.h"
#include "BSPMapCompiler/_LightmapBuilder.h"

#include "LightWave/3DMeshModelBuilder_LW.h"


#include "Physics/PhysicsEngine.h"
//#include "Physics/Preprocessor.h"
#include "Physics/TriangleMeshDesc.h"

using namespace boost;


inline boost::shared_ptr<CGeneral3DMesh> CreateGeneral3DMesh()
{
	boost::shared_ptr<CGeneral3DMesh> pMesh
		= shared_ptr<CGeneral3DMesh>( new CGeneral3DMesh() );

	return pMesh;
}


CStaticGeometryCompiler::CStaticGeometryCompiler()
{
	m_DatabaseRelativeDirPathAtRuntime = ".";//"./Stage";
}


void CStaticGeometryCompiler::SaveToBinaryDatabase( const std::string& db_filename )
{
	/// save the static geometry archive
	CBinaryDatabase<string> db;

	bool db_open = db.Open( db_filename, CBinaryDatabase<string>::DB_MODE_NEW );
	if( !db_open )
	{
		LOG_PRINT_ERROR( "Cannot open a file for binary database: " + db_filename );
		return;
	}

	// add mesh textures to db
	// - need to change the texture filename of the mesh
	//   from (texture filepath) to (db name)::(basename of texture filepath)
	string db_filepath = "./" + fnop::get_nopath(db_filename);
	const size_t num_meshes = m_vecDestGraphicsMeshArchive.size();
	for( size_t i=0; i<num_meshes; i++ )
	{
		AddTexturesToBinaryDatabase( m_vecDestGraphicsMeshArchive[i], db_filepath, db );
	}

	string db_dir_relative_path = m_DatabaseRelativeDirPathAtRuntime;

	string db_file_relative_path = db_dir_relative_path + "/" + fnop::get_nopath(db_filename);

	// add mesh archives to db
	m_Archive.m_vecMesh.resize( num_meshes );
	for( size_t i=0; i<num_meshes; i++ )
	{
		const string keyname = GetGraphicsMeshArchiveKey( (int)i );

		m_Archive.m_vecMesh[i].Mesh.filename = db_file_relative_path + "::" + keyname;

		m_Archive.m_vecMesh[i].aabb = m_vecDestGraphicsMeshArchive[i].GetAABB();

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
		container.ShaderFilepath = itr->first;

		container.vecTechniqueHandle.resize( itr->second.TechniqueToIndex.size() );
		std::map<std::string,int>::iterator itrTechnique;
		for( itrTechnique = itr->second.TechniqueToIndex.begin();
			 itrTechnique != itr->second.TechniqueToIndex.end();
			 itrTechnique++ )
		{
			container.vecTechniqueHandle[ itrTechnique->second ].SetTechniqueName( itrTechnique->first.c_str() );
		}
	}

	db.AddData( CStaticGeometryDBKey::Main, m_Archive );

	// graphics mesh archives
//	db.AddData( CStaticGeometryDBKey::GraphicsMeshes, m_vecDestGraphicsMeshArchive );

	// tree with mesh subset information for each node
//	db.AddData( CStaticGeometryDBKey::MeshSubsetTree, m_MeshSubsetTree );


//	physics::CStream m_CollisionGeometryStream;

	// collision geometry stream for physics engine
	// - stream data of triangle mesh
//	db.AddData( CStaticGeometryDBKey::CollisionGeometryStream, /* triangulated collision mesh stream */ );
}

void CStaticGeometryCompiler::AddDestGraphicsMeshInstance()
{
	shared_ptr<CGeneral3DMesh> pGeneralMesh = CreateGeneral3DMesh();

	pGeneralMesh->SetVertexFormatFlags( m_GraphicsMesh.GetVertexFormatFlags() );

	m_vecpDestGraphicsMesh.push_back( pGeneralMesh );
}

/// Assumes that polygons at each node can be fit in a single mesh instance
/// - Assumes polygons of src_mesh have been copied to the geometry array inside src_tree
/// - Subdives src_mesh into multiple meshes if it has too many vertices / polygons
///   - Created meshes are stored to CStaticGeometryCompiler::m_vecpDestGraphicsMesh
void CStaticGeometryCompiler::CreateMeshSubsets_r( CAABTree<CIndexedPolygon>& src_tree,
												   int src_node_index,
												   CGeneral3DMesh& src_mesh,
					                               CAABTree<CMeshSubset>& dest_tree )
{
	map<int,int> OldToNewMatIndex;

	const CAABNode& node = src_tree.GetNode( src_node_index );

	if( node.veciGeometryIndex.size() == 0 )
		return;

	if( 12000 < m_vecpDestGraphicsMesh.back()->GetPolygonBuffer().size()
		+ node.veciGeometryIndex.size() )
	{
		// place the polygons and materials in this node on a new mesh instance
		AddDestGraphicsMeshInstance();
	}

	CGeneral3DMesh& dest_mesh = *m_vecpDestGraphicsMesh.back();

	vector<CMMA_Material>& dest_material_buffer = dest_mesh.GetMaterialBuffer();
	int mat_index_offset = (int)dest_material_buffer.size();

	// create old -> new material index mappings
	vector<CMMA_Material>& src_material_buffer = src_mesh.GetMaterialBuffer();
	const size_t num_polygons = node.veciGeometryIndex.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		CIndexedPolygon& polygon = src_tree.GetGeometryBuffer()[ node.veciGeometryIndex[i] ];
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

	const vector<CGeneral3DVertex>& src_vertex_buffer = *src_mesh.GetVertexBuffer().get();
	vector<CGeneral3DVertex>& dest_vertex_buffer = *dest_mesh.GetVertexBuffer().get();
	std::vector<CIndexedPolygon>& dest_polygon_buffer = dest_mesh.GetPolygonBuffer();

	int vert_offset = (int)dest_vertex_buffer.size();
	map<int,int> OldToNewVertIndex;
	for( size_t i=0; i<num_polygons; i++ )
	{
		CIndexedPolygon& polygon = src_tree.GetGeometryBuffer()[ node.veciGeometryIndex[i] ];

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
}


/**
 Creates a general 3d mesh from model file
 \param [in] model_filepath
 \param [in] geometry_filter
 \param [out] dest_general_mesh
*/
void CreateGeneral3DMesh( const std::string& model_filepath,
						  const CGeometryFilter& geometry_filter,
						  CGeneral3DMesh& dest_general_mesh )
{
	shared_ptr<C3DModelLoader> pLoader;
	if( fnop::get_ext( model_filepath ) == "lwo" )
	{
		pLoader = shared_ptr<C3DModelLoader>( new C3DMeshModelBuilder_LW() );
	}

	bool loaded = pLoader->LoadFromFile( model_filepath, geometry_filter );

	pLoader->GetGeneral3DMesh( dest_general_mesh );
}


bool CStaticGeometryCompiler::CreateCollisionMesh()
{
	// create collision mesh
	CreateGeneral3DMesh(
		m_Desc.m_InputFilepath, // [in] .lwo, .3ds, etc.
		m_Desc.m_CollisionGeometryFilter, // [in] filter that specifies target layers, surfaces, etc.
		m_CollisionMesh // [out] general 3d mesh that holds collision mesh
		);

	physics::CTriangleMeshDesc coll_mesh_desc;
/*	m_CollisionMesh.GetTriangulatedMeshData(
		coll_mesh_desc.m_vecVertex,
		coll_mesh_desc.m_vecIndex,
		coll_mesh_desc.m_vecMaterialIndex
		);
*/
	string physics_engine_name( "AgeiaPhysX" );

//	physics::PhysicsEngine.Init( physics_engine_name );

//	physics::CStream coll_mesh_stream;

//	physics::PhysPreprocessor.CreateTriangleMeshStream( coll_mesh_desc, coll_mesh_stream );

	return true;
}


int get_str_index( const std::string& name, vector<CGeometrySurfaceDesc>& vecDesc )
{
	for( size_t i=0; i<vecDesc.size(); i++ )
	{
		if( name == vecDesc[i].m_Name )
			return (int)i;
	}

	return 0;
}


bool CStaticGeometryCompiler::CompileGraphicsGeometry()
{
	// create a single graphics mesh that contains all the graphics geometry
	CreateGeneral3DMesh(
		m_Desc.m_InputFilepath, // [in] .lwo, .3ds, etc.
		m_Desc.m_GraphcisGeometryFilter, // [in] filter that specifies target layers, surfaces, etc.
		m_GraphicsMesh // [out] general 3d mesh that holds collision mesh
		);

	// separate graphics geometry
	// 1. geometry that uses lightmap
	// 2. geometry that does not use lightmap

	// create polygon mesh tree for ray test in lightmap creation

	bool m_CreatedPolygonTreeThroughTextureSubdivision = false;

	// subdivide over-sized textures
	// - also subdivide polygons on the edges of the split textures
	// - recalculate texture coords
	// - returns an array of general 3d meshes
//	for( each general 3d mesh )
//	{
//	}
//		SubdivideOversizedTextures(  );
/*	SubdivideOversizedTextures(  )
	{
		m_CreatedPolygonTreeThroughTextureSubdivision = true;
	}
*/
	// create lightmap
	// [in] polygons
	// [in] polygon mesh tree for raycast
	// [in] lights

	/// sort polygon by lightmap texture indices
//	vector<vector<CIndexedPolygon>> vecPolygonSortedByLightmapTexure;

	CLightmapBuilder lightmap_builder;

	/// make the polygon tree

	if( m_CreatedPolygonTreeThroughTextureSubdivision )
	{
		// use the polygon tree created during the process of
		// splitting oversized textures
	}
	else
	{
		// create tree
		CNonLeafyAABTree<CIndexedPolygon> tree;
		tree.SetMinimumCellVolume( 1000000.0f );
		tree.SetNumMaxGeometriesPerCell( 100 );
		tree.SetMaxDepth( 6 );
		tree.SetRecursionStopCondition( CAABTree<CIndexedPolygon>::COND_OR );
		tree.Build( m_GraphicsMesh.GetPolygonBuffer() );

		CNonLeafyAABTree<CMeshSubset>& mesh_subset_tree = m_Archive.m_MeshSubsetTree;

		// copy tree structure
		// - i.e. all the tree nodes
		mesh_subset_tree.GetNodeBuffer() = tree.GetNodeBuffer();

		// remove all the geometry indices at each node
		mesh_subset_tree.ResetRegisteredGeometries();

		// set the first graphics mesh instance to m_vecpDestGraphicsMesh
		AddDestGraphicsMeshInstance();

		CreateMeshSubsets_r( tree,
			                 0, // node index
							 m_GraphicsMesh,
							 mesh_subset_tree // tree that stores mesh subsets
							 );
	}

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


bool CStaticGeometryCompiler::CompileFromXMLDescFile( const std::string& xml_filepath )
{
	bool loaded = m_Desc.LoadFromXML( xml_filepath );

	// set the working directory to the directory path of the xml_filepath
	fnop::dir_stack dir_stk;
	dir_stk.setdir( fnop::get_path( xml_filepath ) );

	map<string,string>::const_iterator itr;
	for( itr = m_Desc.m_SurfaceToDesc.begin();
		 itr != m_Desc.m_SurfaceToDesc.end();
		 itr++ )
	{
		m_SurfaceNameToSurfaceDescIndex[ itr->first ] = get_str_index( itr->second, m_Desc.m_vecSurfaceDesc );
	}

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

	CreateCollisionMesh();

	CompileGraphicsGeometry();

	SaveToBinaryDatabase( m_Desc.m_OutputFilepath );

	// restore the original working directory
	dir_stk.prevdir();

	return true;
}
