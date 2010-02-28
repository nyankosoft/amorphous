#include "PrimitiveShapeMeshes.hpp"
#include "3DMath/Matrix34.hpp"

using namespace std;
using namespace boost;


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


void CreateSphereMesh( const CSphereDesc& desc, CGeneral3DMesh& mesh )
{
	if( !desc.IsValid() )
		return;

	mesh.SetVertexFormatFlags(
		 CMMA_VertexSet::VF_POSITION
		|CMMA_VertexSet::VF_NORMAL
		|CMMA_VertexSet::VF_DIFFUSE_COLOR
		|CMMA_VertexSet::VF_2D_TEXCOORD0 );

	// vertices

	shared_ptr< vector<CGeneral3DVertex> > pVertexBuffer = mesh.GetVertexBuffer();
	vector<CGeneral3DVertex>& vecVertex = *pVertexBuffer;

	const int num_vertices = desc.num_sides * (desc.num_segments - 1) + 2;
	vecVertex.resize( num_vertices );

	float y = desc.radii[1];
	vecVertex[0].m_vPosition = Vector3( 0.0f,  y, 0.0f );
	vecVertex[1].m_vPosition = Vector3( 0.0f, -y, 0.0f );
/*
	for( int i=0; i<desc.num_sides; i++ )
	{
		for( int j=0; j<desc.num_segments; j++ )
		{
			vecVertex[??? + 1].m_vPosition = ???
		}
	}
*/
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
