#include "MeshGenerators.hpp"

using namespace std;
using namespace boost;


void CMeshGenerator::SetMiscMeshAttributes()
{
	if( 0 < m_TexturePath.length() )
	{
		vector<CMMA_Material>& material_buffer = m_MeshArchive.GetMaterial();
		if( 0 < material_buffer.size() )
		{
			material_buffer[0].vecTexture.resize( 1 );
			material_buffer[0].vecTexture[0].strFilename = m_TexturePath;
		}
	}
}


Result::Name CBoxMeshGenerator::Generate()
{
	return Generate( m_vEdgeLengths,
		m_RequestedVertexFormatFlags,
		m_DiffuseColor );
}


Result::Name CBoxMeshGenerator::Generate( Vector3 vLengths, U32 vertex_flags, const SFloatRGBAColor& diffuse_color )
{
	const int num_vertices = 24;
	const int num_faces = 6;

//	CMMA_VertexSet& vertex_set = m_MeshArchive.GetVertexSet();

	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	pMesh->SetVertexFormatFlags( vertex_flags );

	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = pMesh->GetVertexBuffer();

	pVertexBuffer->resize( num_vertices );
	vector<CGeneral3DVertex>& vecVertex = (*pVertexBuffer.get());

//	vertex_set.Resize( num_vertices );
//	vector<Vector3>& vecPosition = vertex_set.vecPosition;

	int iAxis, i;

	const float x = vLengths.x * 0.5f;
	const float y = vLengths.y * 0.5f;
	const float z = vLengths.z * 0.5f;

	// top
	vecVertex[ 0].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[ 1].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[ 2].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[ 3].m_vPosition = Vector3( -x,  y, -z );

	// bottom
	vecVertex[ 4].m_vPosition = Vector3(  x, -y,  z );
	vecVertex[ 5].m_vPosition = Vector3( -x, -y,  z );
	vecVertex[ 6].m_vPosition = Vector3( -x, -y,  z );
	vecVertex[ 7].m_vPosition = Vector3(  x, -y,  z );

	// near
	vecVertex[ 8].m_vPosition = Vector3( -x,  y, -z );
	vecVertex[ 9].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[10].m_vPosition = Vector3(  x, -y, -z );
	vecVertex[11].m_vPosition = Vector3( -x, -y, -z );

	// far
	vecVertex[12].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[13].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[14].m_vPosition = Vector3( -x, -y,  z );
	vecVertex[15].m_vPosition = Vector3(  x, -y,  z );

	// right
	vecVertex[16].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[17].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[18].m_vPosition = Vector3(  x, -y,  z );
	vecVertex[19].m_vPosition = Vector3(  x, -y, -z );

	// left
	vecVertex[20].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[21].m_vPosition = Vector3( -x,  y, -z );
	vecVertex[22].m_vPosition = Vector3( -x, -y, -z );
	vecVertex[23].m_vPosition = Vector3( -x, -y,  z );

	for( iAxis=0; iAxis<3; iAxis++ )
	{
		Vector3 vConstAxis = Vector3(0,0,0);
		vConstAxis[iAxis] = 1;
		for(i=0; i<4; i++)
		{
			vecVertex[iAxis*8 + i    ].m_vNormal = vConstAxis;// * 10.0f;
			vecVertex[iAxis*8 + i + 4].m_vNormal = vConstAxis * (-1);
		}
	}

	// indices
	vector<CIndexedPolygon>& polygon_buffer = pMesh->GetPolygonBuffer();
	polygon_buffer.resize( num_faces );
	for( i=0; i<num_faces; i++ )
	{
		polygon_buffer[i].m_index.resize( 4 );
		for( int j=0; j<4; j++ )
			polygon_buffer[i].m_index[j] = i*4+j;
	}


	//
	// additional vertex attributes
	//

	if( vertex_flags & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		for(i=0; i<num_vertices; i++)
			vecVertex[i].m_DiffuseColor = diffuse_color;
	}

	if( vertex_flags & CMMA_VertexSet::VF_2D_TEXCOORD0 )
	{
//		vertex_set.vecTex.resize( 1 );

//	if( m_TexturePath.length() )
//	{
//		vector<TEXCOORD2>& vecTex = vertex_set.vecTex[0];
		// 1 tex uv for each vertex
		for( int i=0; i<num_vertices; i++ )
			vecVertex[i].m_TextureCoord.resize( 1 );

		for( int i=0; i<num_faces; i++ )
		{
			vecVertex[i*4+0].m_TextureCoord[0] = TEXCOORD2(0,0);
			vecVertex[i*4+1].m_TextureCoord[0] = TEXCOORD2(1,0);
			vecVertex[i*4+2].m_TextureCoord[0] = TEXCOORD2(1,1);
			vecVertex[i*4+3].m_TextureCoord[0] = TEXCOORD2(0,1);
		}
	}

	pMesh->GetMaterialBuffer().resize( 1 );

	// Create mesh archive from the mesh
	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModelArchive( pMesh );
	m_MeshArchive = mesh_builder.GetArchive();

	SetMiscMeshAttributes();

	return Result::SUCCESS;
}


Result::Name CConeMeshGenerator::Generate()
{
	CreateConeMeshArchive( m_Desc, m_MeshArchive );

	SetMiscMeshAttributes();

	return Result::SUCCESS;
}
