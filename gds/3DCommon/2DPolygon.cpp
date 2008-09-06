#include "2DPolygon.h"


void C2DRegularPolygon::MakeRegularPolygon( int num_polygon_vertices, const Vector2& vCenter, int radius )
{
	m_vCenterPosition = vCenter;

	m_Radius = radius;

	m_NumPolygonVertices = num_polygon_vertices;

	CalculateLocalVertexPositions();
	UpdateVertexPositions();
}


void C2DRegularPolygon::CalculateLocalVertexPositions()
{
	if( m_vecRectVertex.size() == 0 )
		ResizeBuffer();

	const Vector3 vCenterPos = m_vCenterPosition;

	m_vecLocalVertexPosition[0] = Vector2(0,0);

	Style style = VERTEX_AT_TOP;

	const float radius = (float)m_Radius;
	const int num_polygon_vertices = m_NumPolygonVertices;
	float angle_per_vertex = 2.0f * (float)PI / (float)num_polygon_vertices;
	float angle_offset = (style == EDGE_AT_TOP) ? angle_per_vertex * 0.5f : 0.0f;
	for( int i=0; i<num_polygon_vertices; i++ )
	{
		float angle = angle_offset + angle_per_vertex * (float)i;
		m_vecLocalVertexPosition[i+1].x =  cos(angle) * radius;
		m_vecLocalVertexPosition[i+1].y = -sin(angle) * radius;
	}
}


void C2DRegularPolygon::UpdateVertexPositions()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const int num_buffer_vertices = m_NumPolygonVertices + 1;
	const Vector2 vCenterPos = m_vCenterPosition;
	Vector2 vGlobalPos;
	for( int i=0; i<num_buffer_vertices; i++ )
	{
		vGlobalPos = vCenterPos + m_vecLocalVertexPosition[i];
		m_vecRectVertex[i].vPosition.x = vGlobalPos.x;
		m_vecRectVertex[i].vPosition.y = vGlobalPos.y;
	}
}
