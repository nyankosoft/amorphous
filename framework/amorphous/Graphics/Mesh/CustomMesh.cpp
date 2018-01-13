#include "CustomMesh.hpp"
#include "CustomMeshRenderer.hpp"
#include "../PrimitiveRenderer.hpp"
#include "../Shader/ShaderManager.hpp"
#include "amorphous/Support/Vec3_StringAux.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Graphics/Graphics_StringAux.hpp"


namespace amorphous
{

using namespace std;
//

template<typename IndexType>
inline static void copy_indices(
	const vector<uchar>& src_index_buffer,
	const uint num_src_indices,
	vector<unsigned int>& dest
	)
{
	dest.resize( 0 );
	dest.resize( num_src_indices, 0 );

	vector<IndexType> copied_indices;
	copied_indices.resize( num_src_indices, 0 );
	memcpy( &(copied_indices[0]), &(src_index_buffer[0]), sizeof(U16) * num_src_indices );

	for( uint i=0; i<num_src_indices; i++ )
		dest[i] = (uint)copied_indices[i];
}


CustomMesh::VertexColorFormat CustomMesh::ms_DefaultVertexDiffuseColorFormat = CustomMesh::VCF_FRGBA;


CustomMesh::CustomMesh()
:
m_VertexFlags(0),
m_VertexSize(0),
m_IndexSize(2),
m_NumUpdatedVertices(0),
m_NumUpdatedIndices(0)
{
	memset( m_ElementOffsets, 0, sizeof(m_ElementOffsets) );
}


void CustomMesh::SetNormals( const Vector3& normal )
{
	const int num_vertices = GetNumVertices();
	const int offset = m_ElementOffsets[VEE::NORMAL];
	const Vector3 n = normal;

	for( int i=0; i<num_vertices; i++ )
		memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &n, sizeof(Vector3) );
}


void CustomMesh::SetDiffuseColors( const std::vector<SFloatRGBAColor>& diffuse_colors )
{
	const int num = (int)diffuse_colors.size();
	const int offset = m_ElementOffsets[VEE::DIFFUSE_COLOR];

	if( ms_DefaultVertexDiffuseColorFormat == VCF_ARGB32 )
	{
		for( int i=0; i<num; i++ )
		{
			U32 argb32 = diffuse_colors[i].GetARGB32();
			memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &(argb32), sizeof(U32) );
		}
	}
	else if( ms_DefaultVertexDiffuseColorFormat == VCF_FRGBA )
	{
		for( int i=0; i<num; i++ )
			memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &(diffuse_colors[i]), sizeof(SFloatRGBAColor) );
	}
}


void CustomMesh::SetDiffuseColors( const SFloatRGBAColor& diffuse_color )
{
	const int num_vertices = GetNumVertices();
	const int offset = m_ElementOffsets[VEE::DIFFUSE_COLOR];

	if( ms_DefaultVertexDiffuseColorFormat == VCF_ARGB32 )
	{
		U32 argb32 = diffuse_color.GetARGB32();
		for( int i=0; i<num_vertices; i++ )
			memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &(argb32), sizeof(U32) );
	}
	else if( ms_DefaultVertexDiffuseColorFormat == VCF_FRGBA )
	{
		const SFloatRGBAColor rgba_color( diffuse_color );
		for( int i=0; i<num_vertices; i++ )
			memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &rgba_color, sizeof(SFloatRGBAColor) );
	}
}


void CustomMesh::SetBlendWeights( const std::vector< TCFixedVector<float,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> >& vecfMatrixWeight )
{
	const int offset = m_ElementOffsets[VEE::BLEND_WEIGHTS];
	float zeros[CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX];

	for( int i=0; i<CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX; i++ )
		zeros[i] = 0;

	int num_blend_weights_in_vertex = take_min( 4, (int)CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX );

	for( size_t i=0; i<vecfMatrixWeight.size(); i++ )
	{
		// Claculate the destination address
		void *dest = &(m_VertexBuffer[0]) + m_VertexSize * i + offset;

		// Initialize the area with 0s
		memcpy( dest, zeros, sizeof(float) * num_blend_weights_in_vertex );

		// Copy the weights
		if( 0 < vecfMatrixWeight[i].size() )
		{
			memcpy(
				dest,
				&(vecfMatrixWeight[i][0]),
				sizeof(float) * take_min( num_blend_weights_in_vertex, (int)(vecfMatrixWeight[i].size()) )
				);
		}
	}
}


void CustomMesh::SetBlendIndices( const std::vector< TCFixedVector<int,CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX> >& veciMatrixIndex )
{
	const int offset = m_ElementOffsets[VEE::BLEND_INDICES];
	for( size_t i=0; i<veciMatrixIndex.size(); i++ )
	{
		U32 indices
			= ((U8)veciMatrixIndex[i][0])
			| ((U8)veciMatrixIndex[i][1] << 8)
			| ((U8)veciMatrixIndex[i][2] << 16)
			| ((U8)veciMatrixIndex[i][3] << 24);

		memcpy( &(m_VertexBuffer[0]) + m_VertexSize * i + offset, &indices, sizeof(U32) );
	}
}


void CustomMesh::InitVertexBuffer( int num_vertices, U32 vertex_format_flags )
{
	m_VertexFlags = vertex_format_flags;
	uint vert_size = 0;

	if( vertex_format_flags & VFF::POSITION )
	{
		m_ElementOffsets[VEE::POSITION] = vert_size;
		vert_size += sizeof(Vector3);
	}

	// vertex blend weights and indices are placed right after vertex positions
	// because flexible vertex format flags of Direct3D require them to be so.
	// See D3DFVF_XYZB5 and D3DFVF_LASTBETA_UBYTE4 for more details.

	if( vertex_format_flags & VFF::BLEND_WEIGHTS )
	{
		m_ElementOffsets[VEE::BLEND_WEIGHTS] = vert_size;
		vert_size += sizeof(float) * 4;//CMMA_VertexSet::NUM_MAX_BLEND_MATRICES_PER_VERTEX;
	}

	if( vertex_format_flags & VFF::BLEND_INDICES )
	{
		m_ElementOffsets[VEE::BLEND_INDICES] = vert_size;
		vert_size += sizeof(U32);
	}

	pair<int,U32> vec3_element_and_flags[] =
	{
		pair<int,U32>( VEE::NORMAL,   VFF::NORMAL ),
		pair<int,U32>( VEE::BINORMAL, VFF::BINORMAL ),
		pair<int,U32>( VEE::TANGENT,  VFF::TANGENT )
	};

	for( int i=0; i<numof(vec3_element_and_flags); i++ )
	{
		if( vertex_format_flags & vec3_element_and_flags[i].second )
		{
			m_ElementOffsets[ vec3_element_and_flags[i].first ] = vert_size;
			vert_size += sizeof(Vector3);
		}
	}

	// The diffuse color element must be defined before texture coords elements
	if( vertex_format_flags & VFF::DIFFUSE_COLOR )
	{
		if( ms_DefaultVertexDiffuseColorFormat == VCF_ARGB32 )
		{
			m_ElementOffsets[VEE::DIFFUSE_COLOR] = vert_size;
			vert_size += 4;
		}
		else if( ms_DefaultVertexDiffuseColorFormat == VCF_FRGBA )
		{
			m_ElementOffsets[VEE::DIFFUSE_COLOR] = vert_size;
			vert_size += sizeof(float) * 4;
		}
	}

	U32 texcoord2_element_flags[] =
	{
		VFF::TEXCOORD2_0,
		VFF::TEXCOORD2_1,
		VFF::TEXCOORD2_2,
		VFF::TEXCOORD2_3
	};

	for( int i=0; i<numof(texcoord2_element_flags); i++ )
	{
		if( vertex_format_flags & texcoord2_element_flags[i] )
		{
			m_ElementOffsets[VEE::TEXCOORD2_0 + i] = vert_size;
			vert_size += sizeof(TEXCOORD2);
		}
	}

	m_VertexBuffer.resize( vert_size * num_vertices );

	m_VertexSize = vert_size;
}


void CustomMesh::GetVertexIndices( std::vector<unsigned int>& dest )
{
	if( m_IndexBuffer.empty() )
		return;

	const uint num_indices = GetNumIndices();

	if( m_IndexSize == sizeof(U16) )
	{
		copy_indices<U16>( m_IndexBuffer, num_indices, dest );
	}
	else if( m_IndexSize == sizeof(U16) )
	{
		copy_indices<U32>( m_IndexBuffer, num_indices, dest );
	}
	else
	{
		// LOG_PRINT_ERROR( "An unsupported index size: " + to_string(m_IndexSize) );
	}
}


bool CustomMesh::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	// The reason to save the mesh file pathname: to find if the mesh is a file or an asset when loading texture.
	// 
	// The mesh class loads textures as resources of the same type, e.g. if the mesh is an asset,
	// the mesh class assumes that textures of the mesh are also assets, and it attempts to load
	// the textures as assets.
	// Since the resource types are encoded in the file path, we need to save the path so that the mesh class
	// can later finds its own resource type when it loads texture images.
	m_strFilename = filename;

	const CMMA_VertexSet& vs = archive.GetVertexSet();
	const int num_verts = vs.GetNumVertices();

	LOG_PRINTF(( "file: %s, num_verts: %d", filename.c_str(), num_verts ));

	U32 flags = ToVFF(vs.GetVertexFormat());

	InitVertexBuffer( num_verts, flags );

	if( flags & VFF::POSITION )
		SetPositions( vs.vecPosition );

	if( flags & VFF::BLEND_WEIGHTS )
		SetBlendWeights( vs.vecfMatrixWeight );

	if( flags & VFF::BLEND_INDICES )
		SetBlendIndices( vs.veciMatrixIndex );

	if( flags & VFF::NORMAL )
		SetNormals( vs.vecNormal );

	if( flags & VFF::TANGENT )
		SetTangents( vs.vecTangent );

	if( flags & VFF::BINORMAL )
		SetBinormals( vs.vecBinormal );

	U32 tex2_flags[] = { VFF::TEXCOORD2_0, VFF::TEXCOORD2_1, VFF::TEXCOORD2_2, VFF::TEXCOORD2_3 };
	for( int i=0; i<4; i++ )
	{
		if( flags & tex2_flags[i] )
			Set2DTexCoords( vs.vecTex[i], i );
	}

	if( flags & VFF::DIFFUSE_COLOR )
		SetDiffuseColors( vs.vecDiffuseColor );

//	InitVertexBuffer( num_vertices, VFF::POSITION|VFF::NORMAL );

	const int num_indices = (int)archive.GetVertexIndex().size();
	if( num_indices < 0xFFFF )
		m_IndexSize = sizeof(U16);
	else
		m_IndexSize = sizeof(U32);

//	InitIndexBuffer( num_indices, m_IndexSize );

//	SetIndices( archive.GetVertexIndex() );
	SetIndices( archive.GetVertexIndex(), m_IndexSize );

	LoadMaterialsFromArchive( archive, option_flags );

	m_TriangleSets = archive.GetTriangleSet();

	return true;
}


void CustomMesh::UpdateAABBs()
{
	m_vecAABB.resize( m_TriangleSets.size() );
	m_AABB.Nullify();

	for( size_t i=0; i<m_TriangleSets.size(); i++ )
	{
		AABB3 aabb;
		aabb.Nullify();
		int start = m_TriangleSets[i].m_iStartIndex;
		int end = m_TriangleSets[i].m_iStartIndex + m_TriangleSets[i].m_iNumTriangles;
		for( int j=start; j<=end; j++ )
			aabb.AddPoint( GetPosition(j) );

		m_TriangleSets[i].m_AABB = aabb;
		m_vecAABB[i] = aabb;
		m_AABB.MergeAABB( aabb );
	}
}


bool CustomMesh::LoadFromFile( const std::string& mesh_archive_filepath )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( mesh_archive_filepath );
	if( !loaded )
		return false;

	m_strFilename = mesh_archive_filepath;

	return LoadFromArchive( archive );
}


void CustomMesh::Render()
{
	GetCustomMeshRenderer().RenderMesh( *this );
}


void CustomMesh::Render( ShaderManager& rShaderMgr )
{
	GetCustomMeshRenderer().RenderMesh( *this, rShaderMgr );
}


static bool rear_to_front( const std::pair<float,int>& lhs, const std::pair<float,int>& rhs )
{
	return (rhs.first < lhs.first);
}


void CustomMesh::RenderZSorted( ShaderManager& rShaderMgr )
{
	if( GetNumVertices() == 0
	 || GetNumIndices() == 0 )
	{
		return;
	}

	const uint num_tris  = GetNumIndices() / 3;
	const uint num_verts = GetNumVertices();

	Vector3 cam_near_plane_n = Vector3(0,0,1);

	const Matrix34 camera_pose = GetCameraPoseFromCameraMatrix( rShaderMgr.GetViewTransform() );
	Vector3 cam_fwd_dir_in_world_space = camera_pose.matOrient.GetColumn(2);
	Matrix34 inv_world_pose( Matrix34Identity() );
	ToMatrix34( rShaderMgr.GetWorldTransform().GetInverse(), inv_world_pose );
	Vector3 cam_fwd_dir = inv_world_pose * cam_fwd_dir_in_world_space;

	static std::vector< std::pair<float,int> > z_and_tri_pairs;
	z_and_tri_pairs.resize( 0 );
	z_and_tri_pairs.reserve( num_tris );

	// sort
	for( uint i=0; i<num_tris; i++ )
	{
		const Vector3 v0 = GetPosition( (int)GetIndex(i*3+0) );
		const Vector3 v1 = GetPosition( (int)GetIndex(i*3+1) );
		const Vector3 v2 = GetPosition( (int)GetIndex(i*3+2) );

		const Vector3 c = (v0 + v1 + v2) / 3.0f;

		float z = Vec3Dot( cam_fwd_dir, c );// - cam_near_plane_d;

		z_and_tri_pairs.push_back( std::pair<float,int>(z,(int)i) );
	}

	std::sort( z_and_tri_pairs.begin(), z_and_tri_pairs.end(), rear_to_front );

	if( GetNumMaterials() == 0 )
		return;

	for( size_t i=0; i<GetMaterial(0).Texture.size(); i++ )
	{
		rShaderMgr.SetTexture( (int)i, GetMaterial(0).Texture[i] );
	}

	General3DVertex verts[3];

	vector<U16> indices;
	indices.resize( 0 );
	indices.resize( GetNumIndices() );

	for( uint i=0; i<num_tris; i++ )
	{
		const int tri_index = z_and_tri_pairs[i].second;
		for( int j=0; j<3; j++ )
			indices[i*3+j] = GetIndex( tri_index * 3 + j );
//			indices[i*3+j] = GetIndex( i*3+j );
	}

	const size_t index_data_size = sizeof(U16) * indices.size();
	m_ZSortedIndexBuffer.resize( index_data_size );
	memcpy( &m_ZSortedIndexBuffer[0], &indices[0], index_data_size );

	GetCustomMeshRenderer().RenderZSortedMesh( *this, rShaderMgr );
}


void CustomMesh::RenderSubset( ShaderManager& rShaderMgr, int material_index )
{
	GetCustomMeshRenderer().RenderSubset( *this, rShaderMgr, material_index );
}


void CustomMesh::DumpToTextFile( const std::string& output_pathname )
{
	FILE *fp = fopen( output_pathname.c_str(), "w" );
	if( !fp )
		return;

	const uint num_vertices = GetNumVertices();

	fprintf( fp, "num vertices: %u\n", num_vertices );
	fprintf( fp, "vertex size: %u\n", GetVertexSize() );

	U32 vf_flags = GetVertexFormatFlags();
	string flags;
	if( vf_flags & VFF::POSITION )      flags += "POSITION";
	if( vf_flags & VFF::NORMAL )        flags += " | NORMAL";
	if( vf_flags & VFF::DIFFUSE_COLOR ) flags += " | DIFFUSE_COLOR";
	if( vf_flags & VFF::TEXCOORD2_0 )   flags += " | TEXCOORD2_0";
	if( vf_flags & VFF::TEXCOORD2_1 )   flags += " | TEXCOORD2_1";
	if( vf_flags & VFF::TEXCOORD2_2 )   flags += " | TEXCOORD2_2";
	if( vf_flags & VFF::TEXCOORD2_3 )   flags += " | TEXCOORD2_3";

	fprintf( fp, "vertex format flags: %s\n\n", flags.c_str() );

	const uint num_indices = GetNumIndices();
	fprintf( fp, "num indices: %u\n", num_indices );
	fprintf( fp, "index size: %u\n\n", GetIndexSize() );

	const size_t num_triangle_sets = m_TriangleSets.size();
	fprintf( fp, "num triangle sets: %zd\n\n", num_triangle_sets );
	for( size_t i=0; i<num_triangle_sets; i++ )
	{
		fprintf( fp, "triangle set [%d]\n", (int)i );

		string buffer;
		m_TriangleSets[i].DumpToText( buffer );
		fprintf( fp, "%s", buffer.c_str() );
	}

	uint i=0;

	vector<Vector3> positions;
	GetVertexPositions( positions );
	fprintf( fp, "\npositions:\n" );
	for( i=0; i<num_vertices; i++ )
		fprintf( fp, "[%06u] %s\n", (int)i, to_string(positions[i]).c_str() );

	if( vf_flags & VFF::NORMAL )
	{
		fprintf( fp, "\nnormals:\n" );
		for( i=0; i<num_vertices; i++ )
			fprintf( fp, "[%06u] %s\n", (int)i, to_string(GetNormal(i)).c_str() );
	}

	U32 tex2_flags[] = { VFF::TEXCOORD2_0, VFF::TEXCOORD2_1, VFF::TEXCOORD2_2, VFF::TEXCOORD2_3 };
	for( int t=0; t<4; t++ )
	{
		if( vf_flags & tex2_flags[t] )
		{
			fprintf( fp, "\ntexture coordinates[%d]:\n", t );
			for( i=0; i<num_vertices; i++ )
				fprintf( fp, "[%06u] %s\n", (int)i, to_string(Get2DTexCoord(i,t)).c_str() );
		}
	}

	if( vf_flags & VFF::DIFFUSE_COLOR )
	{
		fprintf( fp, "\ndiffuse colors:\n" );
		for( i=0; i<num_vertices; i++ )
			fprintf( fp, "[%06u] %s\n", i, to_string(GetDiffuseColor(i)).c_str() );
	}

	fprintf( fp, "\ntriangle indices:\n" );
	vector<uint> indices;
	GetVertexIndices( indices );
	uint num_triangles = num_indices / 3;
	for( i=0; i<num_triangles; i++ )
	{
		fprintf( fp, "[%06u] %u %u %u\n",
			i,
			indices[i * 3 + 0],
			indices[i * 3 + 1],
			indices[i * 3 + 2]
		);
	}

	int ret = fclose( fp );
}


} // namespace amorphous
