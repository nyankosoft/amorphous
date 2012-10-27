#include "MeshGenerators.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "MeshModel/General3DMesh.hpp"
#include "MeshModel/3DMeshModelBuilder.hpp"

using std::vector;
using boost::shared_ptr;


inline std::string to_string( const SFloatRGBAColor& c, int precision = 3, int num_zfills = 0 )
{
	std::string fmt_float = "%" + to_string(num_zfills) + "." + to_string(precision) + "f";
	std::string fmt_buffer = "( " + fmt_float + ", " + fmt_float + ", " + fmt_float + ", " + fmt_float + " )";

	char buffer[80];
	memset( buffer, 0, sizeof(buffer) );
	_snprintf( buffer, numof(buffer)-1, fmt_buffer.c_str(), c.fRed, c.fGreen, c.fBlue, c.fAlpha );

	return std::string(buffer);
}



void CMeshGenerator::SetMiscMeshAttributes()
{
	SFloatRGBAColor diffuse_color = m_DiffuseColor;
	CMMA_VertexSet& vert_set = m_MeshArchive.GetVertexSet();
	if( vert_set.m_VertexFormatFlag & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		const int num_vertices = (int)vert_set.vecDiffuseColor.size();
		for( int i=0; i<num_vertices; i++ )
			vert_set.vecDiffuseColor[i] = diffuse_color;
	}

	vector<CMMA_Material>& material_buffer = m_MeshArchive.GetMaterial();
	if( 0 < material_buffer.size() )
	{
		material_buffer[0].vecTexture.resize( 1 );
		CMMA_Texture& tex0 = material_buffer[0].vecTexture[0];

		if( 0 < m_TexturePath.length() )
		{
			tex0.strFilename = m_TexturePath;
		}
		else
		{
			tex0.type = CMMA_Texture::SINGLECOLOR;
			tex0.vecfTexelData.resize( 1, 1, SFloatRGBAColor::White() );
		}
	}

	if( m_PolygonDirection == MeshPolygonDirection::INWARD )
		m_MeshArchive.FlipTriangles();
}


void CMeshGenerator::GenerateTextureCoords( CGeneral3DMesh& mesh )
{
	AABB3 aabb;
	Result::Name res = CalculateAABB( mesh, aabb );
	if( res != Result::SUCCESS )
		return;

	vector<CGeneral3DVertex>& vert_buffer = *(mesh.GetVertexBuffer().get());

	const float margin = 0.01f;
	if( m_TexCoordStyleFlags & (TexCoordStyle::LINEAR_SHIFT_Y | TexCoordStyle::LINEAR_SHIFT_INV_Y) )
	{
		const AABB3& mesh_aabb = aabb;
		float len_y = mesh_aabb.vMax.y - mesh_aabb.vMin.y;
		float min_y = mesh_aabb.vMin.y;
//		float offset_v = mesh_aabb.vMin.y / len_y;

		const size_t num_verts = vert_buffer.size();
		for( size_t i=0; i<num_verts; i++ )
		{
			vert_buffer[i].m_TextureCoord[0].v = (1.0f - 2.0f * margin) * (vert_buffer[i].m_vPosition.y - min_y) / len_y + margin;
		}

		if( m_TexCoordStyleFlags & TexCoordStyle::LINEAR_SHIFT_INV_Y )
		{
			for( size_t i=0; i<num_verts; i++ )
				vert_buffer[i].m_TextureCoord[0].v = 1.0f - vert_buffer[i].m_TextureCoord[0].v;
		}
	}
}



//====================================================================================
// CBoxMeshGenerator
//====================================================================================

CBoxMeshGenerator::CBoxMeshGenerator()
:
m_vEdgeLengths(Vector3(1,1,1))
{
}


Result::Name CBoxMeshGenerator::Generate()
{
	return Generate( m_vEdgeLengths,
		m_RequestedVertexFormatFlags,
		m_DiffuseColor,
		m_PolygonDirection );
}


Result::Name CBoxMeshGenerator::Generate( Vector3 vLengths, U32 vertex_flags, const SFloatRGBAColor& diffuse_color, MeshPolygonDirection::Type polygon_direction )
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

	vector<Vector3> vecvNormal;
	vecvNormal.resize( num_faces );

	int i;//, iAxis;

	const float x = vLengths.x * 0.5f;
	const float y = vLengths.y * 0.5f;
	const float z = vLengths.z * 0.5f;

	// top
	vecVertex[ 0].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[ 1].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[ 2].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[ 3].m_vPosition = Vector3( -x,  y, -z );
	vecvNormal[0] = Vector3(0,1,0);

	// bottom
	vecVertex[ 4].m_vPosition = Vector3(  x, -y,  z );
	vecVertex[ 5].m_vPosition = Vector3( -x, -y,  z );
	vecVertex[ 6].m_vPosition = Vector3( -x, -y, -z );
	vecVertex[ 7].m_vPosition = Vector3(  x, -y, -z );
	vecvNormal[1] = Vector3(0,-1,0);

	// near
	vecVertex[ 8].m_vPosition = Vector3( -x,  y, -z );
	vecVertex[ 9].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[10].m_vPosition = Vector3(  x, -y, -z );
	vecVertex[11].m_vPosition = Vector3( -x, -y, -z );
	vecvNormal[2] = Vector3(0,0,-1);

	// far
	vecVertex[12].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[13].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[14].m_vPosition = Vector3( -x, -y,  z );
	vecVertex[15].m_vPosition = Vector3(  x, -y,  z );
	vecvNormal[3] = Vector3(0,0,1);

	// right
	vecVertex[16].m_vPosition = Vector3(  x,  y, -z );
	vecVertex[17].m_vPosition = Vector3(  x,  y,  z );
	vecVertex[18].m_vPosition = Vector3(  x, -y,  z );
	vecVertex[19].m_vPosition = Vector3(  x, -y, -z );
	vecvNormal[4] = Vector3(1,0,0);

	// left
	vecVertex[20].m_vPosition = Vector3( -x,  y,  z );
	vecVertex[21].m_vPosition = Vector3( -x,  y, -z );
	vecVertex[22].m_vPosition = Vector3( -x, -y, -z );
	vecVertex[23].m_vPosition = Vector3( -x, -y,  z );
	vecvNormal[5] = Vector3(-1,0,0);

	// normal

/*	for( iAxis=0; iAxis<3; iAxis++ )
	{
		Vector3 vConstAxis = Vector3(0,0,0);
		vConstAxis[iAxis] = 1;
		for(i=0; i<4; i++)
		{
			vecVertex[iAxis*8 + i    ].m_vNormal = vConstAxis;// * 10.0f;
			vecVertex[iAxis*8 + i + 4].m_vNormal = vConstAxis * (-1);
		}
	}*/

	for( i=0; i<num_faces; i++ )
	{
		for( int j=0; j<4; j++ )
			vecVertex[i*4+j].m_vNormal = vecvNormal[i];
	}

	// indices
	vector<CIndexedPolygon>& polygon_buffer = pMesh->GetPolygonBuffer();
	polygon_buffer.resize( num_faces );
	for( i=0; i<num_faces; i++ )
	{
		polygon_buffer[i].m_index.resize( 4 );
		for( int j=0; j<4; j++ )
		{
			int local_index = j;
			polygon_buffer[i].m_index[j] = i*4+local_index;
		}
	}


	//
	// additional vertex attributes
	//

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

		GenerateTextureCoords( *(pMesh.get()) );
	}

	pMesh->GetMaterialBuffer().resize( 1 );

	// Create mesh archive from the mesh
	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModelArchive( pMesh );
	m_MeshArchive = mesh_builder.GetArchive();

	SetMiscMeshAttributes();

	// create a string that reprents all the properties of the mesh
	m_ResourceIDString = "<Mesh>";
	m_ResourceIDString += "dim=" + to_string(vLengths) + ", ";
	m_ResourceIDString += "vflags=" + to_string(vertex_flags) + ", ";
	m_ResourceIDString += "color=" + to_string(diffuse_color);

	return Result::SUCCESS;
}


Result::Name CConeMeshGenerator::Generate()
{
	CreateConeMeshArchive( m_Desc, m_MeshArchive );

	SetMiscMeshAttributes();

	return Result::SUCCESS;
}



Result::Name CCylinderMeshGenerator::Generate()
{
//	Result::Name res = CreateCylinderMeshArchive( m_Desc, m_MeshArchive );

	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateCylinderMesh( m_Desc, *pMesh );
	GenerateTextureCoords( *pMesh );

	Result::Name res = ::CreateArchiveFromGeneral3DMesh( pMesh, m_MeshArchive );

	SetMiscMeshAttributes(); // set texture filename if it is specified.
	return res;
}


Result::Name CSphereMeshGenerator::Generate()
{
	Result::Name res = CreateSphereMeshArchive( m_Desc, m_MeshArchive );
	SetMiscMeshAttributes(); // set texture filename if it is specified.
	return res;
}


Result::Name CCapsuleMeshGenerator::Generate()
{
	Result::Name res = CreateCapsuleMeshArchive( m_Desc, m_MeshArchive );
	SetMiscMeshAttributes(); // set texture filename if it is specified.
	return res;
}
