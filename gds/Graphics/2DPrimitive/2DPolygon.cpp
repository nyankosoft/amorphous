#include "2DPolygon.hpp"


const float gs_fDefaultRadius = 100.0f;


void C2DRegularPolygon::ResizeBuffer()
{
	C2DRoundRect::ResizeBuffer();

	m_vecRadius.resize( GetNumVertices(), 0 );
}


void C2DRegularPolygon::CalculateLocalVertexPositions()
{
	if( m_vecRectVertex.size() == 0 )
		ResizeBuffer();

	m_vecLocalVertexPosition[0] = Vector2(0,0);

	CRegularPolygonStyle::Name style = m_InitStyle;

	// Set the vertices in the clockwise order
//	const float radius = (float)m_Radius;
	const int num_polygon_vertices = m_NumPolygonVertices;
	float angle_per_vertex = 2.0f * (float)PI / (float)num_polygon_vertices;
	float angle_offset = (float)PI / 2.0f + (float)PI * 2.0f; // Start at the vertex at the twelve o'clock position
	angle_offset += (style == CRegularPolygonStyle::EDGE_AT_TOP) ? angle_per_vertex * 0.5f : 0.0f;
	for( int i=0; i<num_polygon_vertices; i++ )
	{
		float angle = angle_offset - angle_per_vertex * (float)i;
		m_vecLocalVertexPosition[i+1].x =  cos(angle) * gs_fDefaultRadius;//radius;
		m_vecLocalVertexPosition[i+1].y = -sin(angle) * gs_fDefaultRadius;//radius;
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
		vGlobalPos = vCenterPos + m_vecLocalVertexPosition[i] * (float)m_vecRadius[i] / gs_fDefaultRadius;
		m_vecRectVertex[i].m_vPosition.x = vGlobalPos.x;
		m_vecRectVertex[i].m_vPosition.y = vGlobalPos.y;
	}
}


void C2DRegularPolygon::MakeRegularPolygon( int num_polygon_vertices, const Vector2& vCenter, int radius, CRegularPolygonStyle::Name style )
{
	m_vCenterPosition = vCenter;

	const float r = (float)radius;
	m_AABB.vMin = vCenter - Vector2(r,r);
	m_AABB.vMax = vCenter + Vector2(r,r);

	m_NumPolygonVertices = num_polygon_vertices;

	m_InitStyle = style;

	CalculateLocalVertexPositions();

	SetRadius( radius );

	UpdateVertexPositions();
}


void C2DRegularPolygon::SetRadius( int radius )
{
	if( m_NumPolygonVertices <= 0 )
		return;

	m_vecRadius.resize( GetNumVertices() );
	
	m_vecRadius[0] = 0; // center

	for( size_t i=1; i<m_vecRadius.size(); i++ )
		m_vecRadius[i] = radius;
}


void C2DRegularPolygon::SetRadius( int vertex, int radius )
{
	if( m_vecRadius.size() == 0 )
		ResizeBuffer();

	if( vertex < 0 || m_NumPolygonVertices <= vertex )
		return;

	m_vecRadius[vertex+1] = radius;

	// The first vertex uses two vertices in the buffer for wrapping
	if( vertex == 0 )
		m_vecRadius[m_NumPolygonVertices + 1] = radius;

	UpdateVertexPositions();
}


Vector2 C2DRegularPolygon::GetVertexPos( int vertex )
{
	if( m_vecRectVertex.size() == 0 )
		return Vector2(0,0);

	if( vertex < 0 || m_NumPolygonVertices <= vertex )
		return Vector2(0,0);

	const Vector3& vPos = m_vecRectVertex[vertex+1].m_vPosition;

	return Vector2(vPos.x,vPos.y);
}


void C2DRegularPolygon::SetVertexColor( int vertex, const SFloatRGBAColor& color )
{
	if( m_vecRectVertex.size() == 0 )
		return;

	if( vertex < 0 || m_NumPolygonVertices <= vertex )
		return;

	m_vecRectVertex[vertex+1].m_DiffuseColor = color;
}

