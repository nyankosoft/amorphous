
#include "TerrainMeshGenerator.h"


#include <stdio.h>
#include "ImageStone.h"

#include "Support/StringAux.h"
#include "Support/fnop.h"
#include "Support/Log/DefaultLog.h"
//#include "Support/msgbox.h"

using namespace std;
using namespace boost;

/**
 *
 * \param axis altenates 0(x) & 2(z)
 */
void TerrainMeshTree::MakeMeshNodes_r( TerrainMeshNode& node, int axis, int depth )
{
	vector<CIndexedPolygon>& vecPolygonBuffer = *m_pvecPolygonBuffer;

	// calculate a plane that splits the subspace of this node
	SPlane split_plane;
	split_plane.normal = (axis == 0) ? Vector3(1,0,0) : Vector3(0,0,1);
	split_plane.dist = ( node.m_AABB.vMax[axis] + node.m_AABB.vMin[axis] ) * 0.5f;

	node.m_child.resize(2);

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
			vecPolygonBuffer.push_back( front );
			vecPolygonBuffer.push_back( back );
			node.m_child[1].m_vecPolygonIndex.push_back( vecPolygonBuffer.size() - 2 );
			node.m_child[0].m_vecPolygonIndex.push_back( vecPolygonBuffer.size() - 1 );

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


/**
 * Add triangles to a mesh archive as a triangle set.
 * All polygons in the source polygon buffer must be triangulated in advance.
 */
void TerrainMeshTree::AddToMeshArchive( TerrainMeshNode& node,
//									    vector<CIndexedPolygon>& polygon_buffer,
										C3DMeshModelArchive& dest_mesh )
{
	CMMA_VertexSet& vertex_set = dest_mesh.GetVertexSet();
	vector<unsigned int>& dest_index_buffer = dest_mesh.GetVertexIndex();

	int vert_offset = vertex_set.GetNumVertices();
	int index_offset = dest_index_buffer.size();

	size_t i, j, num_triangles = node.m_vecPolygonIndex.size();

	vector<IndexMap> reg;
	reg.reserve( 1024 );
	int min_index = 999999999, max_index = -999999999;

	AABB3 aabb;
	aabb.Nullify();

	for( i=0; i<num_triangles; i++ )
	{
		CIndexedPolygon& triangle = (*m_pvecPolygonBuffer)[node.m_vecPolygonIndex[i]];

		for( j=0; j<3; j++ )
		{
			int new_index = FindRegisteredIndex( triangle.m_index[j], reg );

			if( new_index == -1 )
			{
				// the vertex has not been registered yet
				// 1. register the mapping from old index to the new index
				new_index = vertex_set.GetNumVertices();
				reg.push_back( IndexMap( triangle.m_index[j], new_index ) );

				// 2. crate a new vertex in the dest vertex buffer
//				CGeneral3DVertex& src_vertex = m_pvecVertexBuffer[triangle.m_index[i]];
				const CGeneral3DVertex& src_vertex = triangle.GetVertex(j);
				vertex_set.vecPosition.push_back( src_vertex.m_vPosition );
				vertex_set.vecNormal.push_back( src_vertex.m_vNormal );
				vertex_set.vecDiffuseColor.push_back( src_vertex.m_DiffuseColor );
				vertex_set.vecTex[0].push_back( src_vertex.m_TextureCoord[0] );

				aabb.AddPoint( src_vertex.m_vPosition );
			}

			// add vertex index to the dest index buffer
			dest_index_buffer.push_back( new_index );
	
			min_index = min( min_index, new_index );
			max_index = max( max_index, new_index );
		}
	}

	// register a new triangle set to the dest mesh archive
	CMMA_TriangleSet triangle_set;
	triangle_set.m_iStartIndex = index_offset;
	triangle_set.m_iNumTriangles = num_triangles;
	triangle_set.m_iMinIndex = min_index;
	triangle_set.m_iNumVertexBlocksToCover = max_index - min_index + 1;
	dest_mesh.GetTriangleSet().push_back( triangle_set );

	
	// set the material
	CMMA_Texture texture;
	texture.type = CMMA_Texture::FILENAME;

	texture.strFilename = m_BaseTextureFilename;
	fnop::append_to_body( texture.strFilename, fmt_string("%02d",node.m_TextureIndex) );
	fnop::change_ext( texture.strFilename, m_OutputTextureImageFormat );

//	string pak_filename = 
//	texture.strFilename = "Texture/.ptx:"
 
//	( base_tex_filename.change_ext()

	CMMA_Material mat;
	mat.vecTexture.resize( 1 );
	mat.vecTexture[0] = texture;

	dest_mesh.GetMaterial().push_back( mat );
}

/*
void CreateTriangleSets( vector<CIndexedPolygon>& rvecSrcIndexedPolygon,
						 vector<int>& rvecSrcTriangleIndex,
						 vector<CMMA_TriangleSet>& rvecDestTriangleSet )
{
}
*/

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
				m_vecMeshArchive.push_back( C3DMeshModelArchive() );

				// set the vertex format - texture coord buffer is set 
				m_vecMeshArchive.back().GetVertexSet().SetVertexFormat( CMMA_VertexSet::VF_TEXTUREVERTEX );
				m_vecMeshArchive.back().GetVertexSet().vecTex.resize(1);

				num_current_nodes = 0;
				num_current_vertices = 0;
				num_current_triangles= 0;
			}

			AddToMeshArchive( node.m_child[i],
//				m_pvecPolygonBuffer,
				m_vecMeshArchive.back() );

			num_current_vertices = m_vecMeshArchive.back().GetVertexSet().GetNumVertices();
			num_current_triangles = m_vecMeshArchive.back().GetVertexIndex().size() / 3;
			num_current_nodes++;
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


void TerrainMeshTree::MakeMesh()
{
	m_vecMeshArchive.push_back( C3DMeshModelArchive() );
	m_vecMeshArchive.back().GetVertexSet().SetVertexFormat( CMMA_VertexSet::VF_TEXTUREVERTEX );
	m_vecMeshArchive.back().GetVertexSet().vecTex.resize(1);

	int num_nodes = 0, num_vertices = 0, num_triangles = 0;
	if( !m_RootNode.IsLeaf() )
        MakeMesh_r( m_RootNode, num_nodes, num_vertices, num_triangles );	// create terrain mesh from sub-meshes
	else
		AddToMeshArchive( m_RootNode, m_vecMeshArchive[0] );	// create terrain mesh from only one mesh archive

	for( size_t i=0; i<m_vecMeshArchive.size(); i++ )
		m_vecMeshArchive[i].UpdateAABBs();
}


TerrainMeshTree::TerrainMeshTree()
{
	m_NumMaxTrianglesPerMesh = 20000;

	m_NumMaxVerticesPerMesh = 50000;

	m_NumMaxBatchNodes = 4;

	m_vecMeshArchive.reserve( 8 );

	m_TexCoordShiftU = -0.0078757f;
	m_TexCoordShiftV =  0.0078757f;
}


void TerrainMeshTree::Triangulate_r( TerrainMeshNode& node,
									 vector<CIndexedPolygon>& dest_triangle_buffer )
{
	if( node.IsLeaf() )
	{
		// triangulate all the polygons linked to this node
		vector<int> dest_index_buffer;
		dest_index_buffer.reserve( node.m_vecPolygonIndex.size() );

		size_t i, num_pols = node.m_vecPolygonIndex.size();
		for( i=0; i<num_pols; i++ )
		{
			const CIndexedPolygon& polygon = (*m_pvecPolygonBuffer)[node.m_vecPolygonIndex[i]];
			polygon.Triangulate( dest_triangle_buffer );

			size_t j, num_tris = polygon.m_index.size() - 2;

			for( j=0; j<num_tris; j++ )
				dest_index_buffer.push_back( dest_triangle_buffer.size() - num_tris + j );

		}
		node.m_vecPolygonIndex = dest_index_buffer;
	}
	else
	{
		Triangulate_r( node.m_child[0], dest_triangle_buffer );
		Triangulate_r( node.m_child[1], dest_triangle_buffer );
	}
}



bool TerrainMeshTree::Build( boost::shared_ptr<std::vector<CGeneral3DVertex>> pVertexBuffer,
							 vector<CIndexedPolygon>& vecPolygonBuffer,
							 int target_depth )
{
	g_Log.Print( "TerrainMeshTree::Build() - target_depth: %d", target_depth );

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
/*	else
	{
		// no need to split mesh into submeshes
		// but, then, is it really necessary to use TerrainMeshGenerator in the first place?
	}*/

	// triangulate polygons at each leaf node
	vector<CIndexedPolygon> dest_triangle_buffer;
	dest_triangle_buffer.reserve( (size_t)(m_pvecPolygonBuffer->size() * 1.2f) );
	Triangulate_r( m_RootNode, dest_triangle_buffer );

	(*m_pvecPolygonBuffer) = dest_triangle_buffer;

	LOG_PRINT( " Leaving." );

	return true;
}


vector<int> s_processed;


void TerrainMeshTree::ScaleTexCoords_r( TerrainMeshNode& node )
{
	LOG_FUNCTION_SCOPE();

	if( 0 < node.m_child.size() )
	{
		ScaleTexCoords_r( node.m_child[0] );
		ScaleTexCoords_r( node.m_child[1] );
	}
	else
	{
		CScopeLog sl( " Scaling texture coords..." );

		// leaf node
		double min_u = 0.000007, max_u = 0.999996;		// taken from arctic2/test30.07.lwo
		double tex_scale_u = 1.0 / ( max_u - min_u );

		AABB3 &root_aabb = m_RootNode.m_AABB;
		const double mesh_side_length = root_aabb.vMax.x - root_aabb.vMin.x;
		const double num_segs_x = (double)(m_TargetDepth - 1);
		size_t i, num_pols = node.m_vecPolygonIndex.size();
		size_t j, num_verts;
		for( i=0; i<num_pols; i++ )
		{
			CIndexedPolygon& polygon = (*m_pvecPolygonBuffer)[node.m_vecPolygonIndex[i]];
			num_verts = polygon.m_index.size();
			for( j=0; j<num_verts; j++ )
			{
				if( s_processed[polygon.m_index[j]] == 1 )
					continue;	// already done

				CGeneral3DVertex& vert = polygon.Vertex(j);//m_index[j] )m_pVertexBuffer[node.m_vecPolygonIndex];
				TEXCOORD2& tex = vert.m_TextureCoord[0];

				double u = tex.u, v = tex.v;

//				u = ( u - min_u ) * tex_scale_u;	// original tex coord is slightly shifted inward - reverse the offset

				u = ( ( u - (  node.m_AABB.vMin.x / mesh_side_length + 0.5 ) ) * num_segs_x );
				v = ( ( v - (- node.m_AABB.vMax.z / mesh_side_length + 0.5 ) ) * num_segs_x );

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

	LOG_PRINT( fmt_string(" Resizing an array of flags for processed vertices: %x", (unsigned int)(m_pVertexBuffer.get())) );

	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	LOG_PRINT( "Vertices: " + to_string( (int)m_pVertexBuffer->size() ) );

	s_processed.resize( m_pVertexBuffer->size(), 0 );

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
	SetOutputTextureFormat( "bmp" );

	m_pVertexBuffer
		= shared_ptr<std::vector<CGeneral3DVertex>>( new std::vector<CGeneral3DVertex>() );
}


CTerrainMeshGenerator::~CTerrainMeshGenerator()
{
}


bool CTerrainMeshGenerator::SplitTexture( const string& src_tex_filename )
{
/*
#ifdef TEXTURE_SPLIT_TEST
	string src_tex_filename = "test.bmp";
#else
	string src_tex_filename = "";
#endif
*/
//	g_Log.Print( "splitting texture image (src filename: %s)", src_tex_filename.c_str() );

	if( !fnop::file_exists(src_tex_filename) )
	{
		LOG_PRINT_ERROR( " - cannot find file: " + src_tex_filename );
		return false;
	}

	FCObjImage src_img;
	if (!src_img.Load (src_tex_filename.c_str()))
	{
		LOG_PRINT_ERROR( " - cannot load file: " + src_tex_filename );
		return false;
	}

	const int src_tex_width = src_img.Width();
	const int dest_tex_width = m_TextureWidth;

	int num_edge_splits = m_NumTexEdgeSplits = src_tex_width / dest_tex_width;

	LOG_PRINT( fmt_string( " - src_tex_width: %d, dest_tex_width: %d", src_tex_width, dest_tex_width ) );

	string image_format = m_OutputTextureImageFormat;

	string dest_filename;
	FCObjImage dest_img;

	const int num_color_bits = 32;

	dest_img.Create( dest_tex_width, dest_tex_width, num_color_bits );
	int i,x,y,offset_x,offset_y;
	for( i=0; i<num_edge_splits*num_edge_splits; i++ )
	{
		offset_x = (i % num_edge_splits) * dest_tex_width;
		offset_y = (i / num_edge_splits) * dest_tex_width;
		for( y=0; y<dest_tex_width; y++ )
		{
			for( x=0; x<dest_tex_width; x++ )
			{
				dest_img.SetPixelData( x, y, src_img.GetPixelData( offset_x + x, offset_y + y ) );
			}
		}

		dest_filename = m_TextureOutputDirectory + fnop::get_nopath(src_tex_filename);
//		dest_filename = ".\\temp" + fnop::get_nopath(src_tex_filename);

		fnop::append_to_body( dest_filename, fmt_string("%02d",i) );

		fnop::change_ext( dest_filename, image_format );

		g_Log.Print( "saving image: %s", dest_filename.c_str() );

		// save the split texture into file
//		sprintf( dest_filename, "%s%02d.bmp", "dest/dest", i );
		dest_img.Save(dest_filename.c_str());

		// save output texture filename
//		m_vecOutputTextureFilename.push_back( dest_filename );
	}

//	g_Log.Print( "texture has been split into %d files", num_edge_splits*num_edge_splits );

	return true;
}


void CTerrainMeshGenerator::CreateMeshTree()
{
	LOG_FUNCTION_SCOPE();

	int tree_depth = m_NumTexEdgeSplits + 1;
	m_MeshTree.Build( m_pVertexBuffer, m_vecPolygonBuffer, tree_depth );

	m_MeshTree.SetTextureIndices( m_NumTexEdgeSplits );
}


void CTerrainMeshGenerator::ScaleTextureCoordinates()
{
	LOG_FUNCTION_SCOPE();

	m_MeshTree.ScaleTexCoords();
}


void CTerrainMeshGenerator::CopyVerticesAndTriangles( C3DMeshModelArchive& src_mesh )
{
	// copy vertices
	src_mesh.GetVertexSet().GetVertices( *m_pVertexBuffer.get() );

	LOG_PRINT( "Vertices: " + to_string( (int)m_pVertexBuffer->size() ) );

	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	// copy triangles
	const vector<unsigned int>& src_index = src_mesh.GetVertexIndex();
	size_t i, num_tris = src_index.size() / 3;
	m_vecPolygonBuffer.resize( num_tris );
	for( i=0; i<num_tris; i++ )
	{
		m_vecPolygonBuffer[i]
		= CIndexedPolygon( m_pVertexBuffer,
			src_index[i*3],
			src_index[i*3+1],
			src_index[i*3+2]
			);
	}
}


bool CTerrainMeshGenerator::SaveToFiles()
{
	/// save to files
	size_t i, num_meshes = m_MeshTree.GetMeshArchive().size();
	for( i=0; i<num_meshes; i++ )
	{
		string filename = fmt_string( "dest/dest%02d.msh", i ); 
		m_MeshTree.GetMeshArchive()[i].SaveToFile( filename );

		filename = fmt_string( "dest/dest%02d.txt", i ); 
		m_MeshTree.GetMeshArchive()[i].WriteToTextFile( filename );
	}

	return true;
}


void CTerrainMeshGenerator::SetTextureOutputDirectory( const std::string& tex_output_dir )
{
	m_TextureOutputDirectory = tex_output_dir;

	size_t len = m_TextureOutputDirectory.length();

	char last_char = m_TextureOutputDirectory[len-1];

	if( last_char != '/' && last_char != '\\' )
		m_TextureOutputDirectory += "/";
}


void CTerrainMeshGenerator::SetOutputTextureFormat( const std::string& image_ext )
{
	if( image_ext != "bmp"
	 && image_ext != "jpg"
	 && image_ext != "tga" )
	{
		g_Log.Print( "CTerrainMeshGenerator::SetOutputTextureFormat() - unsupported image format: " + image_ext );
	}

	m_OutputTextureImageFormat = image_ext;
	m_MeshTree.SetOutputTextureImageFormat( image_ext );
}


bool CTerrainMeshGenerator::BuildTerrainMesh( C3DMeshModelArchive& src_mesh )
{
	CMMA_Material& src_mat = src_mesh.GetMaterial()[0];

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

//	MsgBox( "creating terrain mesh..." );

	// copy all the vertices & triangles in the source mesh archive
	// to a polygon buffer
	CopyVerticesAndTriangles( src_mesh );

//	m_MeshTree.SetBaseTextureFilename( src_mat.SurfaceTexture.strFilename );
	m_MeshTree.SetBaseTextureFilename( m_OutputTextureRelativePath + fnop::get_nopath(src_mat.vecTexture[0].strFilename) );

	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	// copy all the triangles
	// creates a space partitioning tree and link
	// the polygons to its leaf nodes.
	// This process splits the polygons and add new ones to the buffer
	// if necessary
	CreateMeshTree();

	LOG_PRINT( "vertex buffer ref counts: " + to_string( (int)m_pVertexBuffer.use_count() ) );

	ScaleTextureCoordinates();

	/// triangulate polygons
//	Triangulate( dest_polygon_buffer, m_vecPolygonBuffer );

	/// create mesh archives
	m_MeshTree.MakeMesh();
	
//	MsgBox( "terrain mesh created." );

	return true;
}
