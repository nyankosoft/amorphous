#ifndef  __2DRectSetImpl_GL_HPP__
#define  __2DRectSetImpl_GL_HPP__


#include "../../base.hpp"
#include "../../2DPrimitive/2DRectSet.hpp"
#include "../../GraphicsDevice.hpp"

#include <vector>
#include "2DPrimitiveRenderer_GL.hpp"


//=============================================================================
// C2DRectSetImpl_GL
//=============================================================================

class C2DRectSetImpl_GL : public C2DRectSetImpl
{
	std::vector<CGeneral2DVertex> m_vecRectVertex;

public:

	/// position

	inline virtual void SetRectMinMax( int rect_index, const Vector2& vMin, const Vector2& vMax );

//	inline virtual void SetRectMinMax( int rect_index, const Vector3& vMin, const Vector3& vMax );

	inline virtual void SetRectMinMax( int rect_index, float min_x, float min_y, float max_x, float max_y );

	inline virtual void SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos );

	inline virtual void SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos );

	inline virtual void SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y );

	inline virtual void SetVertexPosition( int vert_index, const float x, const float y );

	inline virtual Vector2 GetRectVertexPosition( int rect_index, int vert_index );


	/// texture coord

	inline virtual void SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max );

	inline virtual void SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax );

	inline virtual void SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v );

	inline virtual TEXCOORD2 GetTopLeftTextureCoord( int rect_index );

	inline virtual TEXCOORD2 GetBottomRightTextureCoord( int rect_index );


	/// color

	inline virtual void SetColor( U32 color );

	inline virtual void SetRectColor( int rect_index, U32 color );

	inline virtual void SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color );


	/// FVF vertex

//	inline virtual void SetRectVertex( int rect_index, int vert_index, const TLVERTEX& vSrc );

//	inline virtual void SetVertex( int vert_index, const TLVERTEX& vSrc );

	inline virtual int GetNumRects() { return (int)m_vecRectVertex.size() / 4; }

	inline virtual void SetNumRects( int num_rects );

	inline virtual void AddRects( int num_rects );

	inline virtual void ClearRects() { m_vecRectVertex.resize(0); }

	inline virtual void draw( int start_rect_index, int num_rects );

	/// renders all the rects with the current render states
//	inline virtual void draw() { draw( 0, (int)m_vecRectVertex.size() / 4 ); }

	inline virtual void Draw( int start_rect_index, int num_rects );

	/// renders all the rects
//	inline virtual void Draw() { Draw( 0, (int)m_vecRectVertex.size() / 4 ); }

	inline virtual void Draw( int start_rect_index, int num_rects, const CTextureHandle& texture );

//	inline virtual void Draw( const CTextureHandle& texture ) { Draw( 0, (int)m_vecRectVertex.size() / 4, texture ); }
};


//================================/ inline implementations /================================//


inline void C2DRectSetImpl_GL::SetRectMinMax( int rect_index,
									   float min_x, float min_y,
									   float max_x, float max_y )
{
	const int offset = rect_index * 4;
	m_vecRectVertex[offset    ].m_vPosition.x = min_x;
	m_vecRectVertex[offset    ].m_vPosition.y = min_y;

	m_vecRectVertex[offset + 1].m_vPosition.x = max_x;
	m_vecRectVertex[offset + 1].m_vPosition.y = min_y;

	m_vecRectVertex[offset + 2].m_vPosition.x = max_x;
	m_vecRectVertex[offset + 2].m_vPosition.y = max_y;

	m_vecRectVertex[offset + 3].m_vPosition.x = min_x;
	m_vecRectVertex[offset + 3].m_vPosition.y = max_y;
}


inline void C2DRectSetImpl_GL::SetRectMinMax( int rect_index,
									   const Vector2& vMin,
									   const Vector2& vMax )
{
	SetRectMinMax( rect_index, vMin.x, vMin.y, vMax.x, vMax.y );
}


inline void C2DRectSetImpl_GL::SetRectVertexPosition( int rect_index, int vert_index, const Vector2& vPos )
{
	m_vecRectVertex[rect_index*4+vert_index].m_vPosition.x = vPos.x;
	m_vecRectVertex[rect_index*4+vert_index].m_vPosition.y = vPos.y;
}


inline void C2DRectSetImpl_GL::SetRectVertexPosition( int rect_index, int vert_index, const Vector3& vPos )
{
	m_vecRectVertex[rect_index*4+vert_index].m_vPosition = vPos;
}


inline void C2DRectSetImpl_GL::SetRectVertexPosition( int rect_index, int vert_index, const float x, const float y )
{
	m_vecRectVertex[rect_index*4+vert_index].m_vPosition.x = x;
	m_vecRectVertex[rect_index*4+vert_index].m_vPosition.y = y;
}


inline void C2DRectSetImpl_GL::SetVertexPosition( int vert_index, const float x, const float y )
{
	m_vecRectVertex[vert_index].m_vPosition.x = x;
	m_vecRectVertex[vert_index].m_vPosition.y = y;
}


inline Vector2 C2DRectSetImpl_GL::GetRectVertexPosition( int rect_index, int vert_index )
{
	Vector3 v = m_vecRectVertex[rect_index * 4 + vert_index].m_vPosition;
	return Vector2( v.x, v.y );
}


inline void C2DRectSetImpl_GL::SetTextureCoordMinMax( int rect_index, float u_min, float v_min, float u_max, float v_max )
{
	const int offset = rect_index * 4;
	m_vecRectVertex[offset    ].m_TextureCoord[0] = TEXCOORD2( u_min, v_min );
	m_vecRectVertex[offset + 1].m_TextureCoord[0] = TEXCOORD2( u_max, v_min );
	m_vecRectVertex[offset + 2].m_TextureCoord[0] = TEXCOORD2( u_max, v_max );
	m_vecRectVertex[offset + 3].m_TextureCoord[0] = TEXCOORD2( u_min, v_max );
}


inline void C2DRectSetImpl_GL::SetTextureCoordMinMax( int rect_index, const TEXCOORD2& vMin, const TEXCOORD2& vMax )
{
	SetTextureCoordMinMax( rect_index, vMin.u, vMin.v, vMax.u, vMax.v );
}


inline void C2DRectSetImpl_GL::SetRectVertexTextureCoord( int rect_index, int vert_index, float u, float v )
{
	const int index = rect_index*4 + vert_index;
	m_vecRectVertex[index].m_TextureCoord[0].u = u;
	m_vecRectVertex[index].m_TextureCoord[0].v = v;
}


inline TEXCOORD2 C2DRectSetImpl_GL::GetTopLeftTextureCoord( int rect_index )
{
	TEXCOORD2 tex = m_vecRectVertex[rect_index*4].m_TextureCoord[0];
	return TEXCOORD2( tex.u, tex.v );
}


inline TEXCOORD2 C2DRectSetImpl_GL::GetBottomRightTextureCoord( int rect_index )
{
	TEXCOORD2 tex = m_vecRectVertex[rect_index*4 + 2].m_TextureCoord[0]; // 3rd vertex is at the bottom right corner
	return TEXCOORD2( tex.u, tex.v );
}


inline void C2DRectSetImpl_GL::SetColor( U32 color )
{
	SFloatRGBAColor rgba_color;
	rgba_color.SetARGB32( color );

	const size_t num_vertices = m_vecRectVertex.size();
	for(size_t i=0; i<num_vertices; i++)
		m_vecRectVertex[i].m_DiffuseColor = rgba_color;
}


inline void C2DRectSetImpl_GL::SetRectColor( int rect_index, U32 color )
{
	SFloatRGBAColor rgba_color;
	rgba_color.SetARGB32( color );
	m_vecRectVertex[rect_index*4  ].m_DiffuseColor = rgba_color;
	m_vecRectVertex[rect_index*4+1].m_DiffuseColor = rgba_color;
	m_vecRectVertex[rect_index*4+2].m_DiffuseColor = rgba_color;
	m_vecRectVertex[rect_index*4+3].m_DiffuseColor = rgba_color;
}


inline void C2DRectSetImpl_GL::SetRectVertexColor( int rect_index, int vert_index, const SFloatRGBAColor& color )
{
	m_vecRectVertex[rect_index*4 + vert_index].m_DiffuseColor = color;
}


inline void C2DRectSetImpl_GL::SetNumRects( int num_rects )
{
	const int num_vertices = num_rects * 4;
/*
	TLVERTEX v;
	v.rhw = 1.0f;
	v.vPosition.z = 0.0f;
//	v.color = 0xFF000000;
*/
	m_vecRectVertex.resize( num_vertices, CGeneral2DVertex() );
}


inline void C2DRectSetImpl_GL::AddRects( int num_rects )
{
	const int num_vertices = num_rects * 4;
/*
	TLVERTEX v;
	v.rhw = 1.0f;
	v.vPosition.z = 0.0f;
//	v.color = 0xFF000000;
*/
	for( int i=0; i<num_vertices; i++ )
		m_vecRectVertex.push_back( CGeneral2DVertex() );
}


inline void C2DRectSetImpl_GL::Draw( int start_rect_index, int num_rects )
{
//	SetBasicRenderStates();

//	SetStdRenderStates();

	draw( start_rect_index, num_rects );
}


inline void C2DRectSetImpl_GL::Draw( int start_rect_index, int num_rects, const CTextureHandle& texture )
{
	GraphicsDevice().SetTexture( 0, texture );

	// draw rectangles
	draw( start_rect_index, num_rects );
}


inline void C2DRectSetImpl_GL::draw( int start_rect_index, int num_rects )
{
	if( m_vecRectVertex.size() == 0
	 || (int)m_vecRectVertex.size() < (start_rect_index + num_rects)*4 )
		return;

	PrimitiveRenderer_GL().RenderGL( &(m_vecRectVertex[start_rect_index*4]), num_rects * 4, GL_QUADS );
}



#endif  /*  __2DRectSetImpl_GL_HPP__  */
