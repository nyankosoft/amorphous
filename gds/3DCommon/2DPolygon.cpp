#include "2DPolygon.h"


void C2DRegularPolygon::MakeRegularPolygon( int num_polygon_vertices, const Vector2& vCenter, int radius, CRegularPolygonStyle::Name style )
{
	m_vCenterPosition = vCenter;

	const float r = (float)radius;
	m_AABB.vMin = vCenter - Vector2(r,r);
	m_AABB.vMax = vCenter + Vector2(r,r);

	m_Radius = radius;

	m_NumPolygonVertices = num_polygon_vertices;

	m_InitStyle = style;

	CalculateLocalVertexPositions();
	UpdateVertexPositions();
}


void C2DRegularPolygon::CalculateLocalVertexPositions()
{
	if( m_vecRectVertex.size() == 0 )
		ResizeBuffer();

	m_vecLocalVertexPosition[0] = Vector2(0,0);

	CRegularPolygonStyle::Name style = m_InitStyle;

	// Set the vertices in the clockwise order
	const float radius = (float)m_Radius;
	const int num_polygon_vertices = m_NumPolygonVertices;
	float angle_per_vertex = 2.0f * (float)PI / (float)num_polygon_vertices;
	float angle_offset = (float)PI / 2.0f + (float)PI * 2.0f; // Start at the vertex at the twelve o'clock position
	angle_offset += (style == CRegularPolygonStyle::EDGE_AT_TOP) ? angle_per_vertex * 0.5f : 0.0f;
	for( int i=0; i<num_polygon_vertices; i++ )
	{
		float angle = angle_offset - angle_per_vertex * (float)i;
		m_vecLocalVertexPosition[i+1].x =  cos(angle) * radius;
		m_vecLocalVertexPosition[i+1].y = -sin(angle) * radius;
	}

	// wrapping at the start position
	// - need this because the polygon has a vertex at the center
	m_vecLocalVertexPosition[num_polygon_vertices + 1] = m_vecLocalVertexPosition[1];
}


void C2DRegularPolygon::UpdateVertexPositions()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const int num_buffer_vertices = GetNumVertices();
	const Vector2 vCenterPos = m_AABB.GetCenterPosition();//m_vCenterPosition;
	Vector2 vGlobalPos;
	for( int i=0; i<num_buffer_vertices; i++ )
	{
		vGlobalPos = vCenterPos + m_vecLocalVertexPosition[i];
		m_vecRectVertex[i].vPosition.x = vGlobalPos.x;
		m_vecRectVertex[i].vPosition.y = vGlobalPos.y;
	}
}
