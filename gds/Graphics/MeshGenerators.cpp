#include "MeshGenerators.hpp"

using namespace std;
using namespace boost;


Result::Name CBoxMeshGenerator::Generate()
{
	return Generate( m_vEdgeLengths,
		m_RequestedVertexFormatFlags,
		m_DiffuseColor );
}


Result::Name CBoxMeshGenerator::Generate( Vector3 vLengths, U32 vertex_flags, const SFloatRGBAColor& diffuse_color )
{
	const int num_vertices = 24;
	CMMA_VertexSet& vertex_set = m_MeshArchive.GetVertexSet();

	vertex_set.Resize( num_vertices );

    vector<Vector3>& vecPosition = vertex_set.vecPosition;

	int iAxis, i;

	const float x = vLengths.x * 0.5f;
	const float y = vLengths.y * 0.5f;
	const float z = vLengths.z * 0.5f;

	// top
	vecPosition[ 0] = Vector3( -x,  y,  z );
	vecPosition[ 1] = Vector3(  x,  y,  z );
	vecPosition[ 2] = Vector3(  x,  y, -z );
	vecPosition[ 3] = Vector3( -x,  y, -z );

	// bottom
	vecPosition[ 4] = Vector3(  x, -y,  z );
	vecPosition[ 5] = Vector3( -x, -y,  z );
	vecPosition[ 6] = Vector3( -x, -y,  z );
	vecPosition[ 7] = Vector3(  x, -y,  z );

	// near
	vecPosition[ 8] = Vector3( -x,  y, -z );
	vecPosition[ 9] = Vector3(  x,  y, -z );
	vecPosition[10] = Vector3(  x, -y, -z );
	vecPosition[11] = Vector3( -x, -y, -z );

	// far
	vecPosition[12] = Vector3(  x,  y,  z );
	vecPosition[13] = Vector3( -x,  y,  z );
	vecPosition[14] = Vector3( -x, -y,  z );
	vecPosition[15] = Vector3(  x, -y,  z );

	// right
	vecPosition[16] = Vector3(  x,  y, -z );
	vecPosition[17] = Vector3(  x,  y,  z );
	vecPosition[18] = Vector3(  x, -y,  z );
	vecPosition[19] = Vector3(  x, -y, -z );

	// left
	vecPosition[20] = Vector3( -x,  y,  z );
	vecPosition[21] = Vector3( -x,  y, -z );
	vecPosition[22] = Vector3( -x, -y, -z );
	vecPosition[23] = Vector3( -x, -y,  z );

	for( iAxis=0; iAxis<3; iAxis++ )
	{
		Vector3 vConstAxis = Vector3(0,0,0);
		vConstAxis[iAxis] = 1;
		for(i=0; i<4; i++)
		{
			vertex_set.vecNormal[iAxis*8 + i    ] = vConstAxis;// * 10.0f;
			vertex_set.vecNormal[iAxis*8 + i + 4] = vConstAxis * (-1);
		}
	}

	if( vertex_flags & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		for(i=0; i<num_vertices; i++)
			vertex_set.vecDiffuseColor[i] = diffuse_color;
	}

	if( vertex_flags & CMMA_VertexSet::VF_2D_TEXCOORD0 )
	{
		vertex_set.vecTex.resize( 1 );
		vertex_set.vecTex[0].resize( num_vertices, TEXCOORD2(0,0) );
//		for(i=0; i<num_vertices; i++)
//			vertex_set.vecTex[0][i] = ???;
	}

	return Result::SUCCESS;
}
