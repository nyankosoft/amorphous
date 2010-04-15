#include "RectSetMesh.hpp"
#include "Graphics/RectTriListIndex.hpp"
#include "Support/Log/DefaultLog.hpp"

using namespace std;


Result::Name CRectSetMesh::Init( uint num_rects, U32 vertex_format_flags )
{
	InitVertexBuffer( num_rects * 4, vertex_format_flags );

	CRectTriListIndexBuffer::SetNumMaxRects( num_rects );
	SetIndices( CRectTriListIndexBuffer::GetIndexBuffer() );

	return Result::SUCCESS;
}


bool CRectSetMesh::LoadFromArchive( C3DMeshModelArchive& rArchive, const std::string& filename, U32 option_flags )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );

	return false;
}



void CRectSetMesh::LoadVertices( void*& pVBData, C3DMeshModelArchive& archive )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectPosition( int rect_index,
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



void CRectSetMesh::SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetVertexPosition( int vert_index, const float x, const float y )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



Vector2 CRectSetMesh::GetRectVertexPosition( int rect_index, int vert_index )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );

	return Vector2(0,0);
}



void CRectSetMesh::SetRectNormal( int rect_index, const Vector3& vNormal )
{
	int element = VEE::NORMAL;
	const uint vertex_size = m_VertexSize;
	const int element_offset = m_ElementOffsets[element];
	uchar *pOffset = &(m_VertexBuffer[0]) + m_VertexSize * (rect_index*4) + element_offset;

	const int num_verts = 4;
	for( int i=0; i<num_verts; i++ )
		memcpy( pOffset + vertex_size * i, &vNormal, sizeof(Vector3) );
}


/// CRectSetMesh::texture CRectSetMesh::coord

void CRectSetMesh::SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



void CRectSetMesh::SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax )
{
	int element = VEE::TEXCOORD2_0;
	const uint vertex_size = m_VertexSize;
	const int element_offset = m_ElementOffsets[element];
	TEXCOORD2 vTopRight(   TEXCOORD2(vMax.u,vMin.v) );
	TEXCOORD2 vBottomLeft( TEXCOORD2(vMin.u,vMax.v) );

	uchar *pOffset = &(m_VertexBuffer[0]) + m_VertexSize * (rect_index*4) + element_offset;
	memcpy( pOffset + vertex_size * 0, &vMin,         sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 1, &vTopRight,    sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 2, &vMax,         sizeof(TEXCOORD2) );
	memcpy( pOffset + vertex_size * 3, &vBottomLeft,  sizeof(TEXCOORD2) );
}



void CRectSetMesh::SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v )
{
	LOG_PRINT_ERROR( " NOT IMPLEMENTED" );
}



TEXCOORD2 CRectSetMesh::GetTopLeftTextureCoord( int rect_index )
{
	return TEXCOORD2(0,0);
}


TEXCOORD2 CRectSetMesh::GetBottomRightTextureCoord( int rect_index )
{
	return TEXCOORD2(0,0);
}
