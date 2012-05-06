#include "PrimitiveShapeMeshes.hpp"
#include "3DMath/Matrix34.hpp"
#include "3DMath/PrimitivePolygonModelMaker.hpp"
#include "General3DMesh.hpp"
#include "3DMeshModelArchive.hpp"
#include "3DMeshModelBuilder.hpp"

using std::vector;
using namespace boost;


void SetDefaultMeshAttributes( C3DMeshModelArchive& mesh_archive )
{
	vector<CMMA_Material>& material_buffer = mesh_archive.GetMaterial();
	if( 0 < material_buffer.size() )
	{
		material_buffer[0].vecTexture.resize( 1 );
		CMMA_Texture& tex0 = material_buffer[0].vecTexture[0];

//		if( 0 < m_TexturePath.length() )
//		{
//			tex0.type = CMMA_Texture::FILENAME;
//			tex0.strFilename = m_TexturePath;
//		}
//		else
//		{
			tex0.type = CMMA_Texture::SINGLECOLOR;
			tex0.vecfTexelData.resize( 1, 1, SFloatRGBAColor::White() );
//		}
	}
}


Result::Name CreateArchiveFromGeneral3DMesh( boost::shared_ptr<CGeneral3DMesh>& pSrcMesh, C3DMeshModelArchive& dest_mesh_archive )
{
	C3DMeshModelBuilder mesh_builder;
	mesh_builder.BuildMeshModelArchive( pSrcMesh );
	dest_mesh_archive = mesh_builder.GetArchive();

	SetDefaultMeshAttributes( dest_mesh_archive );

	return Result::SUCCESS;
}


static Matrix33 GetRotationMatrixToAlignToAxis( AxisAndDirection::Name axis )
{
	// rotate
	switch(axis)
	{
	case AxisAndDirection::POS_X: return Matrix33RotationY( (Scalar)( PI * 0.5) );
	case AxisAndDirection::NEG_X: return Matrix33RotationY( (Scalar)(-PI * 0.5) );
	case AxisAndDirection::POS_Y: return Matrix33RotationX( (Scalar)(-PI * 0.5) );
	case AxisAndDirection::NEG_Y: return Matrix33RotationX( (Scalar)( PI * 0.5) );
	case AxisAndDirection::POS_Z: return Matrix33Identity();
	case AxisAndDirection::NEG_Z: return Matrix33RotationX( (Scalar)( PI * 1.0) );
	default:
		return Matrix33Identity();
	}
}


static Matrix33 GetRotationMatrixToAlignToAxisForCylinder( AxisAndDirection::Name axis )
{
	// rotate
	switch(axis)
	{
	case AxisAndDirection::POS_X: return Matrix33RotationZ( (Scalar)(-PI * 0.5) );
	case AxisAndDirection::NEG_X: return Matrix33RotationZ( (Scalar)( PI * 0.5) );
	case AxisAndDirection::POS_Y: return Matrix33Identity();
	case AxisAndDirection::NEG_Y: return Matrix33RotationX( (Scalar)( PI * 1.0) );
	case AxisAndDirection::POS_Z: return Matrix33RotationX( (Scalar)( PI * 0.5) );
	case AxisAndDirection::NEG_Z: return Matrix33RotationX( (Scalar)(-PI * 0.5) );
	default:
		return Matrix33Identity();
	}
}


void CreateCylinderMesh( const CCylinderDesc& desc, CGeneral3DMesh& mesh )
{
	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	vector<Vector3> vertices;
	vector<Vector3> normals;
	vector< vector<int> > polygons;

	PrimitiveModelStyle::Name style
		= (desc.style_flags & CCylinderMeshStyleFlags::WELD_VERTICES) ?
		 PrimitiveModelStyle::EDGE_VERTICES_WELDED
		:PrimitiveModelStyle::EDGE_VERTICES_UNWELDED;

	CreateCylinder(
		desc.height,
		desc.radii,
		desc.num_sides,
		style,//desc.edge_option, // [in]
		desc.style_flags & CCylinderMeshStyleFlags::TOP_POLYGONS,
		desc.style_flags & CCylinderMeshStyleFlags::BOTTOM_POLYGONS,
		vertices, normals, polygons // [out]
		);

	if( desc.style == PrimitivePlacingStyle::PLACE_ON_PLANE )
	{
		LOG_PRINT_ERROR( " Not implemented - does not work when the cylinder's axis is not (0,1,0)." );
		const float half_height = desc.height * 0.5f;
		const int num_points = (int)vertices.size();
		for( int i=0; i<num_points; i++ )
		{
			vertices[i].y += half_height;
		}
	}

	Matrix33 rotation = GetRotationMatrixToAlignToAxisForCylinder( desc.axis );

	const size_t num_vertices = vertices.size();
	vector<CGeneral3DVertex>& vert_buffer = *(mesh.GetVertexBuffer());
	vert_buffer.resize( num_vertices, CGeneral3DVertex() );
	for( size_t i=0; i<num_vertices; i++ )
	{
		vert_buffer[i].m_vPosition = rotation * vertices[i];
		vert_buffer[i].m_vNormal   = rotation * normals[i];
		vert_buffer[i].m_TextureCoord.resize( 1 );
	}

	const int num_polygons = (int)polygons.size();
	vector<CIndexedPolygon>& polygon_buffer = mesh.GetPolygonBuffer();
	polygon_buffer.resize( num_polygons );
	for( int i=0; i<num_polygons; i++ )
	{
		polygon_buffer[i].m_index = polygons[i];
	}

//	UpdateAABBs( polygon_buffer );

	std::vector<CMMA_Material>& material_buffer = mesh.GetMaterialBuffer();
	material_buffer.resize( 1 );
}


void CreateConeMesh( const CConeDesc& desc, CGeneral3DMesh& mesh )
{
	int i,j;
	int vert_index = 0;
//	vector<Vector3> vecVertex;

	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );


//	CGeneral3DMesh mesh;
	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = mesh.GetVertexBuffer();
	vector<CGeneral3DVertex>& vecVertex = *(pVertexBuffer.get());

	const int num_vertices = (desc.num_sides + 1) * (desc.num_segments + 1);
	vecVertex.resize( num_vertices );

	float segment_height = desc.body_height / (float)desc.num_segments;


	const Vector3 vBaseNormal = Vec3GetNormalized( Vector3( 0.0f, desc.cone_height, desc.radius ) );

	// calculate the cone along the positive direction of the z-axis
	// - vertices in clockwise order
	for( i=0; i<2; i++ )
	{
		float z = (float)i * desc.body_height;
		float r = desc.radius * ( desc.cone_height - i * segment_height ) / desc.cone_height; // radius : cone_height == r : cone_height - i * segment_height

		float span_v  = r / desc.radius; // r : radius = x : 0.5
		float delta_v = span_v / (float)desc.num_sides;

		for( j=0; j<=desc.num_sides; j++ )
		{
			float angle = - ( 2.0f * (float)PI ) * (float)j / (float)desc.num_sides + (float)PI * 0.5f;
//			angle *= -1;
			vecVertex[vert_index].m_vPosition = Vector3( cos( angle ) * r, sin( angle ) * r, z );
//			vecVertex[vert_index] =

			vecVertex[vert_index].m_vNormal = Matrix33RotationZ( -angle + (float)PI * 0.5f ) * vBaseNormal;

			vecVertex[vert_index].m_TextureCoord.resize( 1 );
			vecVertex[vert_index].m_TextureCoord[0].u = (float)i / (float)desc.num_segments;
			vecVertex[vert_index].m_TextureCoord[0].v = 0.5f - span_v * 0.5f + delta_v * (float)i;

			vert_index++;
		}
	}

	Matrix33 matRot = GetRotationMatrixToAlignToAxis( desc.axis );
	for( i=0; i<num_vertices; i++ )
		vecVertex[i].m_vPosition = matRot * vecVertex[i].m_vPosition;

	// calculate indices
	vector<uint> vecIndex;
	const int num_rects = desc.num_sides * desc.num_segments;
	const int num_triangles = 2 * num_rects;
//	const int num_indices = num_triangles * 3;
//	vecIndex.resize( num_indices );
//	int index = 0;
	vert_index = 0;

	vector<CIndexedPolygon>& polygon_buffer = mesh.GetPolygonBuffer();
	polygon_buffer.resize( num_rects, CIndexedPolygon( pVertexBuffer ) );

	for( i=0; i<num_rects; i++ )
	{
		CIndexedPolygon& polygon = polygon_buffer[i];
		polygon.m_index.resize( 4 );
		polygon.m_index[0] = vert_index + desc.num_sides + 1;
		polygon.m_index[1] = vert_index + desc.num_sides + 2;
		polygon.m_index[2] = vert_index + 1;
		polygon.m_index[3] = vert_index;
		vert_index++;

		// triangle at top-left in the first rectangle of the first(=bottom) segment
/*		vecIndex[index++] = vert_index + (num_sides+1);
		vecIndex[index++] = vert_index + (num_sides+1) + 1;
		vecIndex[index++] = vert_index;

		// triangle at bottom-right in the first rectangle of the first(=bottom) segment
		vecIndex[index++] = vert_index + (num_sides+1);
		vecIndex[index++] = vert_index + 1;
		vecIndex[index++] = vert_index;

		vert_index++;*/
	}

//	CMeshModelArchive archive;

	std::vector<CMMA_Material>& material_buffer = mesh.GetMaterialBuffer();
	material_buffer.resize( 1 );

//	mesh.Create3DMeshModelArchive( archive );

/*
	archive.GetTriangleSet().resize( 1 );

	CMMA_TriangleSet& triangle_set = archive.GetTriangleSet()[0];
	triangle_set.m_iStartIndex = 0;
	triangle_set.m_iMinIndex = 0;
	triangle_set.m_iNumVertexBlocksToCover = 0;
	triangle_set.m_iNumTriangles = num_triangles;
*/	

}


static void RotateVertices( vector<CGeneral3DVertex>& vecVertex, Matrix33 matRotation )
{
	const int num_vertices = (int)vecVertex.size();
	for( int i=0; i<num_vertices; i++ )
	{
		vecVertex[i].m_vPosition = matRotation * vecVertex[i].m_vPosition;
		vecVertex[i].m_vNormal   = matRotation * vecVertex[i].m_vNormal;
	}
}


void CreateSphereMesh( const CSphereDesc& desc, CGeneral3DMesh& mesh )
{
	if( !desc.IsValid() )
		return;

	const int num_segs  = desc.num_segments;
	const int num_sides = desc.num_sides;

	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	// vertices

	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = mesh.GetVertexBuffer();
	vector<CGeneral3DVertex>& vecVertex = *pVertexBuffer;

	const int num_vertices = num_sides * (num_segs - 1) + 2;
	vecVertex.resize( num_vertices );

	vecVertex[0].m_vPosition     = Vector3( 0, 0,  1 );
	vecVertex[0].m_vNormal       = Vector3( 0, 0,  1 );
	vecVertex.back().m_vPosition = Vector3( 0, 0, -1 );
	vecVertex.back().m_vNormal   = Vector3( 0, 0, -1 );

	int vert_index = 1;
	float seg_delta = (float)PI / (float)num_segs;
	float side_delta = 2.0f * (float)PI / (float)num_sides;
	for( int i=0; i<num_segs-1; i++ )
	{
		for( int j=0; j<num_sides; j++ )
		{
			Vector3 vDir
				= Matrix33RotationZ( side_delta * (float)j )
				* Matrix33RotationY( seg_delta * (float)(i+1) )
				* Vector3(0,0,1);

			vecVertex[vert_index].m_vPosition = vDir;

			vecVertex[vert_index].m_vNormal = vDir;

			vert_index += 1;
		}
	}

	// simple tex coords for Nx1 gradation texture used with skysphere
	if( mesh.GetVertexFormatFlags() & CMMA_VertexSet::VF_2D_TEXCOORD0 )
	{
		for( int i=0; i<num_vertices; i++ )
		{
			vecVertex[i].m_TextureCoord.resize( 1 );
			TEXCOORD2& tex = vecVertex[i].m_TextureCoord[0];
			tex.u = 0;
			tex.v = ( vecVertex[i].m_vPosition.z + 1.0f ) * 0.5f;
			tex.v = tex.v * (1.0f - 0.001f * 2.0f) + 0.001f;
		}
	}

	if( desc.poly_dir == MeshPolygonDirection::INWARD )
	{
		for( int i=0; i<num_vertices; i++ )
			vecVertex[i].m_vNormal *= (-1.0f);
	}

	switch( desc.axis )
	{
	case 0:
		RotateVertices( vecVertex, Matrix33RotationY((float)PI * 0.5f) );
		break;
	case 1:
		RotateVertices( vecVertex, Matrix33RotationX((float)PI * 0.5f) );
		break;
	case 2:
	default:
		break;
	}

	for( int i=0; i<num_vertices; i++ )
	{
		Vector3& vPos = vecVertex[i].m_vPosition;
		vPos.x *= desc.radii[0];
		vPos.y *= desc.radii[1];
		vPos.z *= desc.radii[2];
	}

	// polygons
	vector<CIndexedPolygon>& polygon_buffer = mesh.GetPolygonBuffer();
	polygon_buffer.resize( num_sides * num_segs );
	int poly_index = 0;

	// top/bottom row - triangles
	for( int i=0; i<2; i++ )
	{
		int center_vert_index = (i==0) ? 0 : num_vertices-1;//1 + num_sides * (num_segs-1);
		int vert_index_offset = (i==0) ? 1 : 1 + num_sides * (num_segs-2);
		int poly_index_offset = (i==0) ? 0 : num_sides * (num_segs-1);
		int i1=i+1,i2=2-i;
		for( int j=0; j<num_sides; j++ )
		{
			CIndexedPolygon& poly = polygon_buffer[poly_index_offset+j];
			poly.m_index.resize( 3 );
			poly.m_index[0]  = center_vert_index;
			poly.m_index[i1] = vert_index_offset + j;
			poly.m_index[i2] = vert_index_offset + (j+1) % num_sides;
		}
	}

	int mid_pols_index_offset = num_sides;
	for( int i=0; i<num_segs-2; i++ )
	{
		int index_offset_u = 1 + num_sides * i;
		int index_offset_l = 1 + num_sides * (i+1);
		for( int j=0; j<num_sides; j++ )
		{
			CIndexedPolygon& poly = polygon_buffer[mid_pols_index_offset + i*num_sides + j];
			poly.m_index.resize( 4 );
			poly.m_index[0] = index_offset_u + (j+1) % num_sides;
			poly.m_index[1] = index_offset_u + j;
			poly.m_index[2] = index_offset_l + j;
			poly.m_index[3] = index_offset_l + (j+1) % num_sides;
		}
	}

	if( desc.poly_dir == MeshPolygonDirection::INWARD )
	{
		mesh.FlipPolygons();
	}

	std::vector<CMMA_Material>& material_buffer = mesh.GetMaterialBuffer();
	material_buffer.resize( 1 );
}


void CreateCapsuleMesh( const CCapsuleDesc& desc, CGeneral3DMesh& mesh )
{
	if( !desc.IsValid() )
		return;

	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	const int num_segs  = desc.num_segments;
	const int num_sides = desc.num_sides;

	// vertices

	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = mesh.GetVertexBuffer();
	vector<CGeneral3DVertex>& vecVertex = *pVertexBuffer;

	const int num_vertices = desc.num_sides * (desc.num_segments * 2) + 2;
	vecVertex.resize( num_vertices );

//	float z = desc.length * 0.5f;
//	vecVertex[0].m_vPosition = Vector3( 0.0f, 0.0f,  z );
//	vecVertex[1].m_vPosition = Vector3( 0.0f, 0.0f, -z );

	float x=0,y=0,z=0;
	const float radius = desc.radius;
	float inclination_delta = (float)PI * 0.5f / (float)num_segs;
	float azimuth_delta = 2.0f * (float)PI / (float)num_sides;
	for( int n=0; n<2; n++ )
	{
		float sign = (n==0) ? 1.0f : -1.0f;
		int offset = (n==0) ? 0 : 1 + num_segs * num_sides;
		z = desc.length * 0.5f * sign;
		vecVertex[offset].m_vPosition = Vector3( 0.0f, 0.0f, z );
		vecVertex[offset].m_vNormal   = Vector3( 0.0f, 0.0f, sign );
		Vector3 vCenter = Vector3( 0.0f, 0.0f, z - radius * sign );
		for( int i=0; i<num_segs; i++ )
		{
			const float inclination = inclination_delta * (float)(i+1);
			z = desc.length * 0.5f - radius + (float)cos( inclination ) * radius;
			z *= sign;
			for( int j=0; j<num_sides; j++ )
			{
				CGeneral3DVertex& vert = vecVertex[num_sides * i + j + offset + 1];

				float inclination_sin = (float)sin( inclination );
				x = (float)cos( azimuth_delta * (float)j ) * radius * inclination_sin;
				y = (float)sin( azimuth_delta * (float)j ) * radius * inclination_sin;
				vert.m_vPosition = Vector3(x,y,z);
				vert.m_vNormal   = Vec3GetNormalized( Vector3(x,y,z) - vCenter );
			}
		}
	}

	// polygons
	vector<CIndexedPolygon>& polygon_buffer = mesh.GetPolygonBuffer();

	int i1=0,i2=0,i3=0;
	for( int n=0; n<2; n++ )
	{
		int offset = (n==0) ? 0 : 1 + num_segs * num_sides;

		i1 = (n==0) ? 1 : 2;
		i2 = (n==0) ? 2 : 1;
		// top row - triangles
		for( int i=0; i<num_sides; i++ )
		{
			polygon_buffer.push_back( CIndexedPolygon() );
			CIndexedPolygon& polygon = polygon_buffer.back();
			polygon.m_index.resize( 3 );
			polygon.m_index[0]  = offset;
			polygon.m_index[i1] = offset + 1 + i;
			polygon.m_index[i2] = offset + 1 + (i + 1) % num_sides;
		}

		i1 = (n==0) ? 1 : 3;
		i3 = (n==0) ? 3 : 1;
		// the rest of the rows - quads
		for( int i=0; i<num_segs-1; i++ )
		{
			for( int j=0; j<num_sides; j++ )
			{
				int start = offset + 1 + i * num_sides;
				polygon_buffer.push_back( CIndexedPolygon() );
				CIndexedPolygon& polygon = polygon_buffer.back();
				polygon.m_index.resize( 4, 0 );
				polygon.m_index[0]  = start + (j + 1) % num_sides;
				polygon.m_index[i1] = start + j;
				polygon.m_index[2]  = start + num_sides + j;
				polygon.m_index[i3] = start + num_sides + (j + 1) % num_sides;
			}
		}
	}

	// side
	int offset_0 = num_segs * num_sides + 1 - num_sides;
	int offset_1 = (num_segs * num_sides + 1) * 2 - num_sides;
	for( int i=0; i<num_sides; i++ )
	{
		polygon_buffer.push_back( CIndexedPolygon() );
		CIndexedPolygon& polygon = polygon_buffer.back();
		polygon.m_index.resize( 4 );
		polygon.m_index[0] = offset_0 + (i + 1) % num_sides;
		polygon.m_index[1] = offset_0 + i;
		polygon.m_index[2] = offset_1 + i;
		polygon.m_index[3] = offset_1 + (i + 1) % num_sides;
	}

	std::vector<CMMA_Material>& material_buffer = mesh.GetMaterialBuffer();
	material_buffer.resize( 1 );
}


Result::Name CreateCylinderMeshArchive( const CCylinderDesc& desc, C3DMeshModelArchive& mesh_archive )
{
	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateCylinderMesh( desc, *pMesh );
	return CreateArchiveFromGeneral3DMesh( pMesh, mesh_archive );
}


Result::Name CreateConeMeshArchive( const CConeDesc& desc, C3DMeshModelArchive& mesh_archive )
{
//	CGeneral3DMesh mesh;
//	CreateConeMesh( desc, mesh );
//	mesh.Create3DMeshModelArchive( mesh_archive );

	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateConeMesh( desc, *pMesh );
	return CreateArchiveFromGeneral3DMesh( pMesh, mesh_archive );
}

Result::Name CreateSphereMeshArchive( const CSphereDesc& desc,   C3DMeshModelArchive& mesh_archive )
{
	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateSphereMesh( desc, *pMesh );
	return CreateArchiveFromGeneral3DMesh( pMesh, mesh_archive );
}


Result::Name CreateCapsuleMeshArchive( const CCapsuleDesc& desc, C3DMeshModelArchive& mesh_archive )
{
	shared_ptr<CGeneral3DMesh> pMesh( new CGeneral3DMesh() );
	CreateCapsuleMesh( desc, *pMesh );
	return CreateArchiveFromGeneral3DMesh( pMesh, mesh_archive );
}
