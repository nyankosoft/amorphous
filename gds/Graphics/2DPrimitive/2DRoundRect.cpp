#include "2DRoundRect.hpp"


namespace amorphous
{


C2DRoundRect::~C2DRoundRect()
{
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


void C2DRoundRect::UpdateVertexPositions()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const float r = (float)m_CornerRadius;
	Vector2 avCornerCenterPos[4] =
	{
		// push vertices inward to save the room for the corner radius
		Vector2( m_AABB.vMin.x + r, m_AABB.vMin.y + r ),
		Vector2( m_AABB.vMax.x - r, m_AABB.vMin.y + r ),
		Vector2( m_AABB.vMax.x - r, m_AABB.vMax.y - r ),
		Vector2( m_AABB.vMin.x + r, m_AABB.vMax.y - r )
	};

	const int num_segs_per_corner = m_NumSegmentsPerCorner;
	int vert_index = 0;
	Vector2 global_pos;
	for( int i=0; i<4; i++ )
	{
		for( int j=0; j<=num_segs_per_corner; j++, vert_index++ )
		{
			Vector2 global_pos = avCornerCenterPos[i] + m_vecLocalVertexPosition[vert_index];
			m_vecRectVertex[vert_index].m_vPosition.x = global_pos.x;
			m_vecRectVertex[vert_index].m_vPosition.y = global_pos.y;
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
		CGeneral2DVertex& v = m_vecRectVertex[i];
		TEXCOORD2& tex = v.m_TextureCoord[0];

		tex.u = ( vLocalPos.x + outer_radius ) / outer_diameter;
		tex.v = ( vLocalPos.y + outer_radius ) / outer_diameter;

		// (tex.u,tex.v): [(0.0),(1.1)]

		tex.u = rvMin.u + tex.u * vExtent.u;
		tex.v = rvMin.v + tex.v * vExtent.v;

		// (tex.u,tex.v): [rMin,rMax]

		tex.u = tex.u + uv_margin / ( 1.0f + uv_margin * 2.0f );
		tex.v = tex.v + uv_margin / ( 1.0f + uv_margin * 2.0f );
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
		m_vecRectVertex[i].m_vPosition.z   = fZValue;
}


void C2DRoundRect::ScalePosition( float fScale )
{
	if( m_vecRectVertex.size() == 0 )
		return;

	const int num_vertices = GetNumVertices();
	for(int i=0; i<num_vertices; i++)
		m_vecRectVertex[i].m_vPosition *= fScale;
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
			CGeneral2DVertex& v_inner = m_vecRectVertex[vert_index];
			CGeneral2DVertex& v_outer = m_vecRectVertex[vert_index+1];
			vert_index += 2;
			float angle = ( 2.0f * (float)PI ) * ( seg_offset + j ) / (float)num_total_segments - (float)PI;
			angle *= -1;

			pos = Vector2( cos( angle ), -sin( angle ) );
			inner_pos = ( pos * margin          + Vector2(1,1) ) * 0.5f;
			outer_pos = ( pos * (1.0f - margin) + Vector2(1,1) ) * 0.5f;

			// pos: [(0.0),(1.1)]

			v_inner.m_TextureCoord[0].u = inner_pos.x;
			v_inner.m_TextureCoord[0].v = inner_pos.y;
			v_outer.m_TextureCoord[0].u = outer_pos.x;
			v_outer.m_TextureCoord[0].v = outer_pos.y;
		}
	}

	m_vecRectVertex[vert_index].m_TextureCoord[0].u = m_vecRectVertex[0].m_TextureCoord[0].u;
	m_vecRectVertex[vert_index].m_TextureCoord[0].v = m_vecRectVertex[0].m_TextureCoord[0].v;
	vert_index++;

	m_vecRectVertex[vert_index].m_TextureCoord[0].u = m_vecRectVertex[1].m_TextureCoord[0].u;
	m_vecRectVertex[vert_index].m_TextureCoord[0].v = m_vecRectVertex[1].m_TextureCoord[0].v;
}


} // namespace amorphous
