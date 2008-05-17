#include "2DRoundRect.h"


C2DRoundRect::~C2DRoundRect()
{
	Release();
}


void C2DRoundRect::CalculateLocalVertexPositions()
{
	const int num_segments_per_corner = m_NumSegmentsPerCorner;
	const int num_total_segments = num_segments_per_corner * 4;
	int i,j;
	float radius = (float)m_CornerRadius;
	int vert_index = 0;
	Vector3 avCornerBasePos[4];

	if( m_vecRectVertex.size() == 0 )
		ResizeBuffer();

	// calculate vertex positions at each corner and cache them
	// - used every time the rect gets resized
	// - clockwise order
	// 0 top-left
	// 1 top-right
	// 2 bottom-right
	// 3 bottom-left
	for( i=0; i<4; i++ )
	{
		int seg_offset = i * num_segments_per_corner;
		for( j=0; j<=num_segments_per_corner; j++ )
		{
			float angle = ( 2.0f * (float)PI ) * ( seg_offset + j ) / (float)num_total_segments - (float)PI;
			angle *= -1;
			m_vecLocalVertexPosition[vert_index++] = Vector2( cos( angle ), -sin( angle ) ) * radius;
		}
	}

}


/// draws the rect with render state settings
/// if the rect has a texture, or is handed one as the argument
/// it will be rendered with the texture
/// alpha blend is enabled. The user can set the blend mode
/// by calling SetDestAlphaBlendMode()
/// the default blend mode is D3DBLEND_INVSRCALPHA
void C2DRoundRect::Draw()
{
	if( m_vecRectVertex.size() == 0 )
		return;

	SetBasicRenderStates();

	SetStdRenderStates();

	draw();
}


void C2DRoundRect::Draw( const CTextureHandle& texture )
{
	if( m_vecRectVertex.size() == 0 )
		return;

	SetBasicRenderStates();

	SetStdRenderStatesForTexture( texture );

	draw();
}


/// Set min & max texture coordinates in the rectangular region
void C2DRoundRect::SetTextureCoords( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax )
{
	const float outer_radius = (float)m_CornerRadius;
	const float outer_diameter = outer_radius * 2.0f;
	const float uv_margin = 0.01f;
	const int num_vertices = GetNumVertices();

	if( m_vecRectVertex.size() == 0 )
		CalculateLocalVertexPositions();

	TEXCOORD2 vExtent = rvMax - rvMin;

	// Note that the last 2 vertices are wrapping of the border at the top-left corner
	for(int i=0; i<num_vertices; i++)
	{
		Vector2 vLocalPos = m_vecLocalVertexPosition[i];
		TLVERTEX& v = m_vecRectVertex[i];

		v.tu = ( vLocalPos.x + outer_radius ) / outer_diameter;
		v.tv = ( vLocalPos.y + outer_radius ) / outer_diameter;

		// (v.tu,v.tv): [(0.0),(1.1)]

		v.tu = rvMin.u + v.tu * vExtent.u;
		v.tv = rvMin.v + v.tv * vExtent.v;

		// (v.tu,v.tv): [rMin,rMax]

		v.tu = v.tu + uv_margin / ( 1.0f + uv_margin * 2.0f );
		v.tv = v.tv + uv_margin / ( 1.0f + uv_margin * 2.0f );
	}
}


void C2DRoundRect::SetZDepth(float fZValue)
{
	if( m_vecRectVertex.size() == 0 )
	{
		CalculateLocalVertexPositions();
		UpdateVertexPositions();
	}

	const int num_vertices = GetNumVertices();
	for(int i=0; i<num_vertices; i++)
		m_vecRectVertex[i].vPosition.z   = fZValue;
}


void C2DRoundRect::ScalePosition( float fScale )
{
	if( m_vecRectVertex.size() == 0 )
		return;

	const int num_vertices = GetNumVertices();
	for(int i=0; i<num_vertices; i++)
		m_vecRectVertex[i].vPosition *= fScale;
}



//=============================================================================
// C2DRoundFrameRect
//=============================================================================

void C2DRoundFrameRect::CalculateLocalVertexPositions()
{
	int num_segments_per_corner = m_NumSegmentsPerCorner;
	int num_total_segments = num_segments_per_corner * 4;
	int i,j;
	const float outer_radius = (float)m_CornerRadius;
	const float inner_radius = outer_radius - (float)m_BorderWidth;
	int vert_index = 0;
	Vector3 avCornerBasePos[4];

	if( m_vecRectVertex.size() == 0 )
		ResizeBuffer();

	// calculate vertex positions at each corner and cache them
	// - used every time the rect gets resized
	// - clockwise order
	// 0 top-left
	// 1 top-right
	// 2 bottom-right
	// 3 bottom-left
	for( i=0; i<4; i++ )
	{
		int seg_offset = i * num_segments_per_corner;
		for( j=0; j<=num_segments_per_corner; j++ )
		{
			float angle = ( 2.0f * (float)PI ) * ( seg_offset + j ) / (float)num_total_segments - (float)PI;
			angle *= -1;

			m_vecLocalVertexPosition[vert_index++] = Vector2( cos( angle ), -sin( angle ) ) * inner_radius;

			m_vecLocalVertexPosition[vert_index++] = Vector2( cos( angle ), -sin( angle ) ) * outer_radius;
		}
	}

	// not used in C2DRoundFrameRect::UpdateVertexPositions(),
	// but used in C2DRoundRect::SetTextureCoords()
	m_vecLocalVertexPosition[vert_index++] = m_vecLocalVertexPosition[0];
	m_vecLocalVertexPosition[vert_index++] = m_vecLocalVertexPosition[1];
}


void C2DRoundFrameRect::Set2DCircularBorderTextureCoords( float margin )
{
	int num_segments_per_corner = m_NumSegmentsPerCorner;
	int num_total_segments = num_segments_per_corner * 4;
	int i,j;
	const float outer_radius = (float)m_CornerRadius;
	const float inner_radius = outer_radius - (float)m_BorderWidth;
	int vert_index = 0;

	if( m_vecRectVertex.size() == 0 )
		CalculateLocalVertexPositions();

	// calculate vertex positions at each corner and cache them
	// - used every time the rect gets resized
	// - clockwise order
	// [ 0, 1, 2, 3 ]
	// [ top-left, top-right, bottom-right, bottom-left ]
	Vector2 pos, inner_pos, outer_pos;
	for( i=0; i<4; i++ )
	{
		int seg_offset = i * num_segments_per_corner;
		for( j=0; j<=num_segments_per_corner; j++ )
		{
			TLVERTEX& v_inner = m_vecRectVertex[vert_index];
			TLVERTEX& v_outer = m_vecRectVertex[vert_index+1];
			vert_index += 2;
			float angle = ( 2.0f * (float)PI ) * ( seg_offset + j ) / (float)num_total_segments - (float)PI;
			angle *= -1;

			pos = Vector2( cos( angle ), -sin( angle ) );
			inner_pos = ( pos * margin          + Vector2(1,1) ) * 0.5f;
			outer_pos = ( pos * (1.0f - margin) + Vector2(1,1) ) * 0.5f;

			// pos: [(0.0),(1.1)]

			v_inner.tu = inner_pos.x;
			v_inner.tv = inner_pos.y;
			v_outer.tu = outer_pos.x;
			v_outer.tv = outer_pos.y;
		}
	}

	m_vecRectVertex[vert_index].tu = m_vecRectVertex[0].tu;
	m_vecRectVertex[vert_index].tv = m_vecRectVertex[0].tv;
	vert_index++;

	m_vecRectVertex[vert_index].tu = m_vecRectVertex[1].tu;
	m_vecRectVertex[vert_index].tv = m_vecRectVertex[1].tv;
}
