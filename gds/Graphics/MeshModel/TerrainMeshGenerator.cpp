#include "TerrainMeshGenerator.hpp"
#include "General3DMesh.hpp"


#include <boost/filesystem.hpp>
#include <stdio.h>

#include "Support/StringAux.hpp"
#include "Support/fnop.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/BitmapImage.hpp"
#include "Support/ImageSplitter.hpp"
#include "tbb/task_scheduler_init.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;


double log2( double scalar )
{
	return log( scalar ) / log( 2.0 );
}


/// filepath printer for Mesh Generator
class CMGSplitImageFilepathPrinter : public CSplitImageFilepathPrinter
{
	std::string m_SrcFilepath;

	/// borrowed reference
	TerrainMeshTree *m_pMeshTree;

public:

	CMGSplitImageFilepathPrinter( const std::string& src_filepath, TerrainMeshTree *pMeshTree )
		:
	m_SrcFilepath(src_filepath),
	m_pMeshTree(pMeshTree)
	{}

	std::string Print( int index )
	{
		return m_pMeshTree->CreateSubdividedTextureFilepath( m_SrcFilepath, index );
	}
};



TerrainMeshTree::TerrainMeshTree()
:
m_TargetMaterialIndex(0)
{
	m_NumMaxTrianglesPerMesh = 20000;

	m_NumMaxVerticesPerMesh = 50000;

	m_NumMaxBatchNodes = 4;

	m_TexCoordShiftU = 0.0f;//-0.0078757f;
	m_TexCoordShiftV = 0.0f;// 0.0078757f;
}


/**
  \param axis altenates 0(x) & 2(z)
 */
void TerrainMeshTree::MakeMeshNodes_r( TerrainMeshNode& node, int axis, int depth )
{
	vector<CIndexedPolygon>& vecPolygonBuffer = m_pDestMesh->GetPolygonBuffer();

	// calculate a plane that splits the subspace of this node
	SPlane split_plane;
	split_plane.normal = (axis == 0) ? Vector3(1,0,0) : Vector3(0,0,1);
	split_plane.dist = ( node.m_AABB.vMax[axis] + node.m_AABB.vMin[axis] ) * 0.5f;

	node.m_child.resize(2);

	node.m_Axis = axis;

	CIndexedPolygon front, back;

	size_t i, num_pols = node.m_vecPolygonIndex.size();
	node.m_child[0].m_vecPolygonIndex.reserve( num_pols / 2 );
	node.m_child[1].m_vecPolygonIndex.reserve( num_pols / 2 );
	int c;
	for( i=0; i<num_pols; i++ )
	{
		// put all the polygons of this node to either front or back child
		// If a polygon is crossing the plane, it will be split and each piece is 
		// sent to front / back child
		const int polygon_index = node.m_vecPolygonIndex[i];
		c = ClassifyPolygon( split_plane, vecPolygonBuffer[polygon_index] );

		switch( c )
		{
		case POLYGON_FRONT:
			node.m_child[1].m_vecPolygonIndex.push_back(polygon_index);
			break;

		case POLYGON_BACK:
			node.m_child[0].m_vecPolygonIndex.push_back(polygon_index);
			break;

		case POLYGON_ONPLANE:
			node.m_child[1].m_vecPolygonIndex.push_back(polygon_index);
			break;

		case POLYGON_INTERSECTING:
			// Split the polygon and push them into both front & back children.
			// Note that the original polygon is not added to either of the child nodes
			vecPolygonBuffer[polygon_index].Split( front, back, split_plane );
//			vecPolygonBuffer.push_back( front );
			vecPolygonBuffer[polygon_index] = front;
			vecPolygonBuffer.push_back( back );
//			node.m_child[1].m_vecPolygonIndex.push_back( (int)vecPolygonBuffer.size() - 2 );
			node.m_child[1].m_vecPolygonIndex.push_back( polygon_index );
			node.m_child[0].m_vecPolygonIndex.push_back( (int)vecPolygonBuffer.size() - 1 );

//			if( 32 < front.m_index.size() )
//			{
//				MessageBox( NULL, "polygon split failed for in the front side of the plane", "error", MB_OK|MB_ICONWARNING );
//				vecPolygonBuffer[polygon_index].Split( front, back, split_plane );
//			}

			break;
		}
	}

	node.m_child[0].m_AABB = node.m_AABB;
	node.m_child[0].m_AABB.vMax[axis] = split_plane.dist;

	node.m_child[1].m_AABB = node.m_AABB;
	node.m_child[1].m_AABB.vMin[axis] = split_plane.dist;

	node.m_vecPolygonIndex.clear();

	if( depth < m_TargetDepth - 2 )
	{
		MakeMeshNodes_r( node.m_child[0], axis ^ 2, depth+1 );
		MakeMeshNodes_r( node.m_child[1], axis ^ 2, depth+1 );
	}
}


// holds mapping from an old index to a new, registered index
class IndexMap
{
public:
	IndexMap( int _old, int _new ) : old_index(_old), new_index(_new) {}
	int old_index;
	int new_index;
};


inline int FindRegisteredIndex( int src_index, const vector<IndexMap>& index_map )
{
	size_t i, num_maps = index_map.size();
	for( i=0; i<num_maps; i++ )
	{
		if( src_index == index_map[i].old_index )
			return index_map[i].new_index;
	}
	return -1;
}


/// Add a new material to the material buffer of m_pDestMesh
/// Update the material index of the polygons stored in the argument node, 'node'
/// to the new material index.
/// Note that the polygons are actually stored as indices.
void TerrainMeshTree::AddToDestMesh( TerrainMeshNode& node )
{
	int new_mat_index = m_pDestMesh->GetNumMaterials();

	vector<CIndexedPolygon>& vecPolygonBuffer = m_pDestMesh->GetPolygonBuffer();

	// udpate the material indices of the polygons
	const size_t num_polygons_at_node = node.m_vecPolygonIndex.size();
	for( size_t i=0; i<num_polygons_at_node; i++ )
	{
		vecPolygonBuffer[ node.m_vecPolygonIndex[i] ].m_MaterialIndex = new_mat_index;
	}

	// set the new material. esp. the texture
	CMMA_Texture texture;
	texture.type = CMMA_Texture::FILENAME;

	const CMMA_Material& src_mat = m_pSrcMesh->GetMaterialBuffer()[m_TargetMaterialIndex];
	const string src_tex_filepath = src_mat.vecTexture[0].strFilename;

	texture.strFilename = CreateSubdividedTextureFilepath( src_tex_filepath, node.m_TextureIndex );

	CMMA_Material mat;
	mat.Name = src_mat.Name + fmt_string("%02d",node.m_TextureIndex);
	mat.vecTexture.resize( 1 );
	mat.vecTexture[0] = texture;

	m_pDestMesh->GetMaterialBuffer().push_back( mat );
}


void TerrainMeshTree::MakeMesh_r( TerrainMeshNode& node,
								 int& num_current_nodes,
								 int& num_current_vertices,
								 int& num_current_triangles )
{
	for( int i=0; i<2; i++ )
	{
		if( node.m_child[i].IsLeaf() )
		{
			// child is a leaf node - add to mesh archive

			// check if it should be batched to the current mesh archive
			if( num_current_nodes < m_NumMaxBatchNodes )
//			&& num_current_vertices + node.m_child[i].GetNumVertices() < m_NumMaxVerticesPerMesh
//			&& num_current_triangles + node.m_child[i].GetNumTriangles() < m_NumMaxTrianglesPerMesh )
			{
				int pass=1;
			}
			else
			{
				num_current_nodes = 0;
				num_current_vertices = 0;
				num_current_triangles= 0;
			}

			AddToDestMesh( node.m_child[i] );
		}
		else
		{
			MakeMesh_r( node.m_child[i],
				num_current_nodes,
				num_current_vertices,
				num_current_triangles );
		}
	}

}


/// Add materials to the material buffer of m_pDestMesh
/// Update the material indices of polygons in m_pDestMesh
void TerrainMeshTree::MakeMesh()
{
	int num_nodes = 0, num_vertices = 0, num_triangles = 0;
	if( !m_RootNode.IsLeaf() )
        MakeMesh_r( m_RootNode, num_nodes, num_vertices, num_triangles );	// create terrain mesh from sub-meshes
	else
		AddToDestMesh( m_RootNode );	// create terrain mesh from only one mesh archive
}


void TerrainMeshTree::SetOutputTextureImageFormat( const std::string& image_ext )
{
	if( image_ext != "bmp"
	 && image_ext != "jpg"
	 && image_ext != "tga" )
	{
		LOG_PRINT_ERROR( " An unsupported image format: " + image_ext );
	}

	m_OutputTextureImageFormat = image_ext;
}


std::string TerrainMeshTree::CreateSubdividedTextureFilepath( const string& src_tex_filename, int index )
{
	string dest_filename;

	// make sure that the dest directory exists
	create_directories( m_TextureOutputDirectory );

	path dest_filepath = path(m_TextureOutputDirectory) / path(src_tex_filename).leaf();
	dest_filename = dest_filepath.string();
//	dest_filename = ".\\temp" + fnop::get_nopath(src_tex_filename);

	fnop::append_to_body( dest_filename, fmt_string("%02d",index) );

	fnop::change_ext( dest_filename, m_OutputTextureImageFormat );

	return dest_filename;
}


void TerrainMeshTree::SetTextureOutputDirectory( const std::string& tex_output_dir )
{
	m_TextureOutputDirectory = tex_output_dir;

	size_t len = m_TextureOutputDirectory.length();

	char last_char = m_TextureOutputDirectory[len-1];

	if( last_char != '/' && last_char != '\\' )
		m_TextureOutputDirectory += "/";
}


/**
 Stores the subdivided mesh to m_pDestMesh
*/
bool TerrainMeshTree::Build( boost::shared_ptr<CGeneral3DMesh> pSrcMesh, int target_depth )
{
	LOG_FUNCTION_SCOPE();
	LOG_PRINT( " target_depth: " + to_string(target_depth) );

	m_pSrcMesh = pSrcMesh;

//	m_pVertexBuffer = m_pSrcMesh->GetVertexBuffer();

	// copy the properties of the source mesh to dest mesh
	// except for the materials
	m_pDestMesh = CreateGeneral3DMesh();

	(*m_pDestMesh) = (*m_pSrcMesh);

	m_pDestMesh->GetMaterialBuffer().clear();

//	m_pDestMesh->ClearPolygonBuffer();

	// set target depth
	m_TargetDepth = target_depth;

	std::vector<CIndexedPolygon>& vecPolygonBuffer = m_pDestMesh->GetPolygonBuffer();

	// push all polygons to the root node
	int i, num_pols = (int)vecPolygonBuffer.size();
	m_RootNode.m_vecPolygonIndex.resize(num_pols);
	for( i=0; i<num_pols; i++ )
		m_RootNode.m_vecPolygonIndex[i] = i;

	m_RootNode.m_AABB = GetAABB( vecPolygonBuffer );

	if( 2 < target_depth )
	{
		// make the space partitioning tree (recursive)
		MakeMeshNodes_r( m_RootNode, 0, 0 );
	}
//	else
//	{
//		// no need to split mesh into submeshes
//		// but, then, is it really necessary to use TerrainMeshGenerator in the first place?
//	}

	return true;
}

/*
bool TerrainMeshTree::Build( boost::shared_ptr<std::vector<CGeneral3DVertex>> pVertexBuffer,
							 vector<CIndexedPolygon>& vecPolygonBuffer,
							 int target_depth )
{
	LOG_PRINT( " target_depth: " + to_string(target_depth) );

	m_pVertexBuffer = pVertexBuffer;

	m_pvecPolygonBuffer = &vecPolygonBuffer;

	m_TargetDepth = target_depth;

	// push all polygons to the root node
	size_t i, num_pols = vecPolygonBuffer.size();
	m_RootNode.m_vecPolygonIndex.resize(num_pols);
	for( i=0; i<num_pols; i++ )
		m_RootNode.m_vecPolygonIndex[i] = i;

	m_RootNode.m_AABB = GetAABB( vecPolygonBuffer );

	if( 2 < target_depth )
	{
		// make the space partitioning tree (recursive)
		MakeMeshNodes_r( m_RootNode, 0, 0 );
	}
//	else
//	{
//		// no need to split mesh into submeshes
//		// but, then, is it really necessary to use TerrainMeshGenerator in the first place?
//	}

	// triangulate polygons at each leaf node
	vector<CIndexedPolygon> dest_triangle_buffer;
	dest_triangle_buffer.reserve( (size_t)(m_pvecPolygonBuffer->size() * 1.2f) );
	Triangulate_r( m_RootNode, dest_triangle_buffer );

	(*m_pvecPolygonBuffer) = dest_triangle_buffer;

	LOG_PRINT( " Leaving." );

	return true;
}*/


vector<int> s_processed;


void TerrainMeshTree::ScaleTexCoords_r( TerrainMeshNode& node )
{
	LOG_FUNCTION_SCOPE();

	vector<CIndexedPolygon>& vecPolygonBuffer = m_pDestMesh->GetPolygonBuffer();

	if( 0 < node.m_child.size() )
	{
		ScaleTexCoords_r( node.m_child[0] );
		ScaleTexCoords_r( node.m_child[1] );
	}
	else
	{
		CScopeLog sl( " Scaling texture coords..." );

		// leaf node
//		double min_u = 0.000007, max_u = 0.999996;		// taken from arctic2/test30.07.lwo
//		double tex_scale_u = 1.0 / ( max_u - min_u );

		AABB3 &root_aabb = m_RootNode.m_AABB;
		const double mesh_length_x = root_aabb.vMax.x - root_aabb.vMin.x;
		const double mesh_length_z = root_aabb.vMax.z - root_aabb.vMin.z;
//		const double num_segs_x = (double)(m_TargetDepth - 1);
		const double num_segs_x = exp( (double)(m_TargetDepth - 1) / 2.0 * log(2.0) );
		size_t i, num_pols = node.m_vecPolygonIndex.size();
		size_t j, num_verts;
		ONCE( g_Log.Print( "mesh_side_length: %f, root_aabb.vMin.x: %f, root_aabb.vMax.x: %f, num_segs_x: %f",
			mesh_length_x, root_aabb.vMin.x, root_aabb.vMax.x, num_segs_x ) );
		for( i=0; i<num_pols; i++ )
		{
			CIndexedPolygon& polygon = vecPolygonBuffer[node.m_vecPolygonIndex[i]];
			num_verts = polygon.m_index.size();
			for( j=0; j<num_verts; j++ )
			{
				if( s_processed[polygon.m_index[j]] == 1 )
					continue;	// already done

				CGeneral3DVertex& vert = polygon.Vertex((int)j);//m_index[j] )m_pVertexBuffer[node.m_vecPolygonIndex];
				TEXCOORD2& tex = vert.m_TextureCoord[0];

				double u = tex.u, v = tex.v;

//				u = ( u - min_u ) * tex_scale_u;	// original tex coord is slightly shifted inward - reverse the offset

//				u = ( ( u - (  node.m_AABB.vMin.x / mesh_side_length + 0.5 ) ) * num_segs_x );
//				v = ( ( v - (- node.m_AABB.vMax.z / mesh_side_length + 0.5 ) ) * num_segs_x );

				// offset
				u = u - (   node.m_AABB.vMin.x - root_aabb.vMin.x  ) / mesh_length_x;
				v = v - (- (node.m_AABB.vMax.z - root_aabb.vMax.z) ) / mesh_length_z;

				// scaling
				u = u * num_segs_x;
				v = v * num_segs_x;
/*
//				u -= 0.0078757f;
//				v += 0.0078757f;
				u += m_TexCoordShiftU;
				v += m_TexCoordShiftV;

				double tex_width = 1024.0;//128.0f;
				double delta = 1.0 / tex_width;
//				tex.u = ( tex.u * ( tex_width - 1.0f ) / tex_width ) + delta * 0.5f;
//				tex.v = ( tex.v * ( tex_width - 1.0f ) / tex_width ) + delta * 0.5f;
				u = ( u * ( 1.0 - delta ) ) + delta * 0.5;
				v = ( v * ( 1.0 - delta ) ) + delta * 0.5;
*/
				tex.u = u;
				tex.v = v;

				s_processed[polygon.m_index[j]] = 1;
			}
		}
	}
}


void TerrainMeshTree::ScaleTexCoords()
{
	LOG_FUNCTION_SCOPE();

//	LOG_PRINT( fmt_string(" Resizing an array of flags for processed vertices: %x", (unsigned int)(m_pVertexBuffer.get())) );

//	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

//	LOG_PRINT( "Vertices: " + to_string( (int)m_pVertexBuffer->size() ) );

	s_processed.resize( m_pSrcMesh->GetVertexBuffer()->size(), 0 );

	LOG_PRINT( " Calling ScaleTexCoords_r()..." );

	ScaleTexCoords_r( m_RootNode );
}


void TerrainMeshTree::SetTextureIndices_r( TerrainMeshNode& node,
										   int num_tex_edge_splits )
{
	float root_min_x = m_RootNode.m_AABB.vMin.x;
	float root_max_z = m_RootNode.m_AABB.vMax.z;
	float mesh_length = m_RootNode.m_AABB.vMax.x - m_RootNode.m_AABB.vMin.x;
	float node_length = mesh_length / (float)num_tex_edge_splits;
	if( node.IsLeaf() )
	{
		const Vector3 vCenterPos = node.m_AABB.GetCenterPosition();

		node.m_TextureIndex
			= (int)( ( vCenterPos.x-root_min_x) / node_length )
			+ (int)( (-vCenterPos.z+root_max_z) / node_length ) * num_tex_edge_splits;
	}
	else
	{
		for( int i=0; i<2; i++ )
			SetTextureIndices_r( node.m_child[i], num_tex_edge_splits );
	}

}


//==================================================================
// CTerrainMeshGenerator
//==================================================================

CTerrainMeshGenerator::CTerrainMeshGenerator()
{
	m_TextureWidth = 1024;
//	m_TextureWidth = 256;

	// set default image format
	SetOutputTextureFormat( "jpg" );

//	m_pVertexBuffer
//		= shared_ptr<std::vector<CGeneral3DVertex>>( new std::vector<CGeneral3DVertex>() );
}


CTerrainMeshGenerator::~CTerrainMeshGenerator()
{
}


bool CTerrainMeshGenerator::SplitTexture( const string& src_tex_filename )
{
	LOG_FUNCTION_SCOPE();

//	g_Log.Print( "splitting texture image (src filename: %s)", src_tex_filename.c_str() );

	if( !fnop::file_exists(src_tex_filename) )
	{
		LOG_PRINT_ERROR( " - cannot find file: " + src_tex_filename );
		return false;
	}

	shared_ptr<CBitmapImage> pSrcImg = shared_ptr<CBitmapImage>( new CBitmapImage() );
	if( !pSrcImg->LoadFromFile( src_tex_filename ) )
	{
		LOG_PRINT_ERROR( " - cannot load file: " + src_tex_filename );
		return false;
	}

	const int src_tex_width  = pSrcImg->GetWidth();
	const int src_tex_height = pSrcImg->GetHeight();
	const int dest_tex_width = m_TextureWidth;

	int num_edge_splits = m_NumTexEdgeSplits = src_tex_width / dest_tex_width;

	// init tbb
	// - Should be done by the system
	tbb::task_scheduler_init init;

	CMGSplitImageFilepathPrinter filepath_printer( src_tex_filename, &m_MeshTree );
	CImageSplitter is(
		num_edge_splits,
		num_edge_splits,
		"",
		pSrcImg,
		&filepath_printer
		);

	is.SplitMT();

	LOG_PRINT( fmt_string( " - src_tex_width: %d, dest_tex_width: %d", src_tex_width, dest_tex_width ) );

//	g_Log.Print( "texture has been split into %d files", num_edge_splits*num_edge_splits );

	return true;
}


void CTerrainMeshGenerator::CreateMeshTree()
{
	LOG_FUNCTION_SCOPE();

//	int tree_depth = m_NumTexEdgeSplits + 1;
	int tree_depth = 2 * (int)log2( m_NumTexEdgeSplits ) + 1;

	m_MeshTree.Build( m_pSrcMesh, tree_depth );

	m_MeshTree.SetTextureIndices( m_NumTexEdgeSplits );
}


void CTerrainMeshGenerator::ScaleTextureCoordinates()
{
	LOG_FUNCTION_SCOPE();

	m_MeshTree.ScaleTexCoords();
}


bool CTerrainMeshGenerator::SaveToFiles()
{
	/// save to files
/*	size_t i, num_meshes = m_MeshTree.GetMeshArchive().size();
	for( i=0; i<num_meshes; i++ )
	{
		string filename = fmt_string( "dest/dest%02d.msh", i ); 
		m_MeshTree.GetMeshArchive()[i].SaveToFile( filename );

		filename = fmt_string( "dest/dest%02d.txt", i ); 
		m_MeshTree.GetMeshArchive()[i].WriteToTextFile( filename );
	}
*/
	return true;
}


void CTerrainMeshGenerator::SetOutputTextureFormat( const std::string& image_ext )
{
	m_MeshTree.SetOutputTextureImageFormat( image_ext );
}


bool CTerrainMeshGenerator::BuildTerrainMesh( boost::shared_ptr<CGeneral3DMesh> pSrcMesh )
{
	LOG_FUNCTION_SCOPE();

	m_pSrcMesh = pSrcMesh;

	if( m_pSrcMesh->GetNumMaterials() == 0 )
	{
		LOG_PRINT_ERROR( "The source mesh has no material" );
		return false;
	}

	CMMA_Material& src_mat = m_pSrcMesh->GetMaterialBuffer()[m_MeshTree.GetTargetMaterialIndex()];

	if( src_mat.vecTexture.size() == 0 )
	{
		LOG_PRINT_WARNING( " - No texture to split" );
		return false;
	}

	LOG_PRINT( " - splitting src texture" );

	bool res = SplitTexture( src_mat.vecTexture[0].strFilename );

	if( !res )
		return false;

	LOG_PRINT( fmt_string(" - src texture split (%d edge splits)", m_NumTexEdgeSplits) );

//	m_MeshTree.SetBaseTextureFilename( src_mat.SurfaceTexture.strFilename );
//	m_MeshTree.SetBaseTextureFilename( m_OutputTextureRelativePath + fnop::get_nopath(src_mat.vecTexture[0].strFilename) );

//	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	// copy all the triangles creates a space partitioning tree and link
	// the polygons to its leaf nodes.
	// This process splits the polygons and add new ones to the polygon buffer if necessary.
	// Vertices are also added when the polygons are split.
	CreateMeshTree();

//	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	ScaleTextureCoordinates();

	/// triangulate polygons
//	Triangulate( dest_polygon_buffer, m_vecPolygonBuffer );

	/// create mesh archives
	m_MeshTree.MakeMesh();

	LOG_PRINT( fmt_string( " Created a mesh tree. num nodes: %d / dest mesh - num materials: %d",
		m_MeshTree.GetNumNodes(),
		m_MeshTree.GetDestMesh() ? m_MeshTree.GetDestMesh()->GetNumMaterials() : -1 ) );

	return true;
}
