#include "RectSetMesh.hpp"
#include "Graphics/RectTriListIndex.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


RectSetMesh::RectSetMesh()
:
m_NumRects(0)
{}


Result::Name RectSetMesh::Init( uint num_rects, U32 vertex_format_flags )
{
	m_NumRects = num_rects;

	InitVertexBuffer( num_rects * 4, vertex_format_flags );

	CRectTriListIndexBuffer::SetNumMaxRects( num_rects );

	vector<U16> indices;
	const vector<U16>& src_indices = CRectTriListIndexBuffer::GetIndexBuffer();
	indices.assign( src_indices.begin(), src_indices.begin() + 6 ); //6 indices => 2 rects

	SetIndices( indices );

	m_vecMaterial.resize( 1 );

	vector<CMMA_TriangleSet> triangle_sets;
	triangle_sets.resize( 1 );
	triangle_sets[0].m_iStartIndex             = 0;
	triangle_sets[0].m_iMinIndex               = 0;
	triangle_sets[0].m_iNumVertexBlocksToCover = num_rects * 4;
	triangle_sets[0].m_iNumTriangles           = num_rects * 2;

	SetTriangleSets( triangle_sets );

	return Result::SUCCESS;
}


bool RectSetMesh::LoadFromArchive( C3DMeshModelArchive& rArchive, const std::string& filename, U32 option_flags )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );

	return false;
}



void RectSetMesh::LoadVertices( void*& pVBData, C3DMeshModelArchive& archive )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectPosition( int rect_index,
								    const Vector3& vTopLeft,
								    const Vector3& vTopRight,
								    const Vector3& vBottomRight,
								    const Vector3& vBottomLeft )
{
//	Vector3 src
	int element = VEE::POSITION;
	const uint vertex_size = m_VertexSize;
//	const int num_verts = 4;
	const int element_offset = m_ElementOffsets[element];
//	for( int i=0; i<num_verts; i++ )
//		memcpy( &(m_VertexBuffer[0]) + vertex_size * (rect_index*4+i) + element_offset, &(src[i]), sizeof(Vector3) );

	uchar *pOffset = &(m_VertexBuffer[0]) + m_VertexSize * (rect_index*4) + element_offset;
	memcpy( pOffset + vertex_size * 0, &vTopLeft,     sizeof(Vector3) );
	memcpy( pOffset + vertex_size * 1, &vTopRight,    sizeof(Vector3) );
	memcpy( pOffset + vertex_size * 2, &vBottomRight, sizeof(Vector3) );
	memcpy( pOffset + vertex_size * 3, &vBottomLeft,  sizeof(Vector3) );
}



void RectSetMesh::SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void RectSetMesh::SetVertexPosition( int vert_index, const float x, const float y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



Vector2 RectSetMesh::GetRectVertexPosition( int rect_index, int vert_index )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );

	return Vector2(0,0);
}



void RectSetMesh::SetRectNormal( int rect_index, const Vector3& vNormal )
{
	int element = VEE::NORMAL;
	const uint vertex_size = m_VertexSize;
	const int element_offset = m_ElementOffsets[element];
	uchar *pOffset = &(m_VertexBuffer[0]) + m_VertexSize * (rect_index*4) + element_offset;

	const int num_verts = 4;
	for( int i=0; i<num_verts; i++ )
		memcpy( pOffset + vertex_size * i, &vNormal, sizeof(Vector3) );
}


/// RectSetMesh::texture RectSetMesh::coord

void RectSetMesh::SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max )
{
	SetTextureCoordMinMax( rect_index, TEXCOORD2(u_min,v_min), TEXCOORD2(u_max,v_max) );
}



void RectSetMesh::SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax )
{
	int element = VEE::TEXCOORD2_0;
	const uint vertex_size = m_VertexSize;
	const int element_offset = m_ElementOffsets[element];
	TEXCOORD2 vTopRight(   TEXCOORD2(vMax.u,vMin.v) );
	TEXCOORD2 vBottomLeft( TEXCOORD2(vMin.u,vMax.v) );

	uchar *pOffset = &(m_VertexBuffer[0]) + m_VertexSize * (rect_index*4) + element_offset;
//	TEXCOORD2 test( 0.5f, 0.5f );
	memcpy( pOffset + vertex_size * 0, &vMin,        /*&test,*/ sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 1, &vTopRight,   /*&test,*/ sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 2, &vMax,        /*&test,*/ sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 3, &vBottomLeft, /*&test,*/ sizeof(TEXCOORD2) );
}



void RectSetMesh::SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



TEXCOORD2 RectSetMesh::GetTopLeftTextureCoord( int rect_index )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
	return TEXCOORD2(0,0);
}


TEXCOORD2 RectSetMesh::GetBottomRightTextureCoord( int rect_index )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
	return TEXCOORD2(0,0);
}


void RectSetMesh::SetColor( const SFloatRGBAColor& color )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}


void RectSetMesh::SetRectColor( int rect_index, const SFloatRGBAColor& color )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}


void RectSetMesh::SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}


void RectSetMesh::SetColorARGB32( U32 color )
{
	int element = VEE::DIFFUSE_COLOR;
	const uint vertex_size = m_VertexSize;
	const int element_offset = m_ElementOffsets[element];
	uchar *pFirstElementOffset = &(m_VertexBuffer[0]) + element_offset;
//	switch( m_DiffuseColorFormat )
//	{
//	case CF_ARGB32:
		for( uint i=0; i<m_NumRects; i++ )
		{
			uint rect_index = i;
			uchar *pOffset = pFirstElementOffset + m_VertexSize * (rect_index*4);
//			const U32 test = 0xFFFFFF00;
			memcpy( pOffset + vertex_size * 0, &color,/*&test,*/ sizeof(U32) );
			memcpy( pOffset + vertex_size * 1, &color,/*&test,*/ sizeof(U32) );
			memcpy( pOffset + vertex_size * 2, &color,/*&test,*/ sizeof(U32) );
			memcpy( pOffset + vertex_size * 3, &color,/*&test,*/ sizeof(U32) );
		}
//		break;
//	default:
//		break;
//	}
}


void RectSetMesh::SetRectColorARGB32( int rect_index, U32 color )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}


} // namespace amorphous
