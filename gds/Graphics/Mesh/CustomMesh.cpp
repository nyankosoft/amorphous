#include "CustomMesh.hpp"
#include "CustomMeshRenderer.hpp"
#include "gds/Graphics.hpp"

using namespace std;
//using namespace boost;


CCustomMesh::VertexColorFormat CCustomMesh::ms_DefaultVertexDiffuseColorFormat = CCustomMesh::VCF_FRGBA;


CCustomMesh::CCustomMesh()
:
m_VertexFlags(0),
m_VertexSize(0),
m_NumUpdatedVertices(0),
m_NumUpdatedIndices(0)
{
	memset( m_ElementOffsets, 0, sizeof(m_ElementOffsets) );
}


void CCustomMesh::SetDiffuseColors( const std::vector<SFloatRGBAColor>& diffuse_colors )
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


void CCustomMesh::InitVertexBuffer( int num_vertices, U32 vertex_format_flags )
{
	m_VertexFlags = vertex_format_flags;
	uint vert_size = 0;

	U32 vec3_element_flags[] =
	{
		VFF::POSITION,
		VFF::NORMAL,
		VFF::BINORMAL,
		VFF::TANGENT
	};

	for( int i=0; i<numof(vec3_element_flags); i++ )
	{
		if( vertex_format_flags & vec3_element_flags[i] )
		{
			m_ElementOffsets[VEE::POSITION + i] = vert_size;
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


bool CCustomMesh::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	const CMMA_VertexSet& vs = archive.GetVertexSet();
	const int num_verts = vs.GetNumVertices();

	U32 flags = ToVFF(vs.GetVertexFormat());

	InitVertexBuffer( num_verts, flags );

	if( flags & VFF::POSITION )
		SetPositions( vs.vecPosition );

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
	InitIndexBuffer( num_indices, sizeof(U16) );

	SetIndices( archive.GetVertexIndex() );

	LoadMaterialsFromArchive( archive, option_flags );

	m_TriangleSets = archive.GetTriangleSet();

	return true;
}


bool CCustomMesh::LoadFromFile( const std::string& mesh_archive_filepath )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( mesh_archive_filepath );
	if( !loaded )
		return false;

	m_strFilename = mesh_archive_filepath;

	return LoadFromArchive( archive );
}


void CCustomMesh::Render()
{
	GetCustomMeshRenderer().RenderMesh( *this );
}


void CCustomMesh::Render( CShaderManager& rShaderMgr )
{
	GetCustomMeshRenderer().RenderMesh( *this, rShaderMgr );
}
