#ifndef  __2DRoundRect_H__
#define  __2DRoundRect_H__


#include "2DPrimitive.h"

#include "3DMath/aabb2.h"
#include "Support/SafeDelete.h"

#include <vector>


/**
 * frame rectangle with rounded corners
 * - vertex indexing is done in clockwise order: [0,3] > top-left, top-right, bottom-right, bottom-left
 *
 */
class C2DRoundRect : public C2DPrimitive
{
protected:

	AABB2 m_AABB;

	std::vector<TLVERTEX> m_vecRectVertex;

	/// local vertex positions
	/// - x: [ -m_CornerRadius, m_CornerRadius ]
	/// - y: [ -m_CornerRadius, m_CornerRadius ]
	std::vector<Vector2> m_vecLocalVertexPosition;

	/// radius of the corner in pixels
	int m_CornerRadius;

	int m_NumSegmentsPerCorner;

	/// uniform color
	SFloatRGBAColor m_Color;

protected:

	// Allocate memory for vertex positions cache and vertices.
	// - m_vecRectVertex
	// - m_vecLocalVertexPosition
	// All the previous content will be cleared.
	// NOTE:
	// - Virtual function call. Must not be called from ctor.
	inline virtual void ResizeBuffer();

	virtual int GetNumVertices() const { return ( m_NumSegmentsPerCorner + 1 ) * 4; }

	virtual void CalculateLocalVertexPositions();

	virtual void UpdateVertexPositions();

	inline virtual void UpdateColor();

public:

	inline C2DRoundRect();

	inline C2DRoundRect( const Vector2& vMin, const Vector2& vMax, U32 color = 0xFF000000, int radius = 8, int num_segments_per_corner = 8 );

	inline C2DRoundRect( int min_x, int min_y, int max_x, int max_y, U32 color = 0xFF000000, int radius = 8, int num_segments_per_corner = 8 );

	inline C2DRoundRect( float min_x, float min_y, float max_x, float max_y, U32 color = 0xFF000000, int radius = 8, int num_segments_per_corner = 8 );

	inline C2DRoundRect( const SRect& rect, U32 color = 0xFF000000, int radius = 8, int num_segments_per_corner = 8 );

	virtual ~C2DRoundRect();

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_ROUNDRECT; }

	inline void SetDefault();

	/// draw rect without any render state changes
	inline virtual void draw();

	inline void Draw();

	inline void Draw( const CTextureHandle& texture );

	inline virtual Vector2 GetPosition2D( int vert_index ) const;

	inline void SetPosition( const Vector2& vMin, const Vector2& vMax);

	/// set values for left, top, right and bottom of the rectangle
	inline void SetPositionLTRB( float l, float t, float r, float b );

	inline void SetPositionLTRB( int l, int t, int r, int b );

	inline void SetPositionLTWH( float l, float t, float w, float h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	inline void SetPositionLTWH( int l, int t, int w, int h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	virtual void SetTextureCoords( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax );

	// TODO: replace all SetTextureUV() calls with SetTextureCoords()
	virtual void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax) { SetTextureCoords( rvMin, rvMax ); }

	/// store a single color to 'm_Color'
	/// - vertex colors are updated if vertex buffers are created
	/// - can be called from constructor
	inline virtual void SetColor( const SFloatRGBAColor& color );

	inline void SetColor( U32 color );

//	void SetColor(U32 dwColor, int iVertexNum = 4);	//'dwColor' is ARGB format

	/// calls ResizeBuffer() internally
	/// - must not be called from constructor
	virtual void SetZDepth(float fZValue);

	void ScalePosition( float fScale );

	inline void Translate( float dx, float dy );
	inline void Translate( int dx, int dy );

	inline void Rotate( float angle );

	inline void Rotate( const Matrix22& matOrient );

	// re-calculate vertex positions
	inline void SetCornerRadius( int radius );

	inline void SetNumSegmentsPerCorner( int num_segs_per_corner );
};


// =================================== inline implementations =================================== 

inline C2DRoundRect::C2DRoundRect()
:
m_CornerRadius(8),
m_NumSegmentsPerCorner(8)
{
//	CalculateLocalVertexPositions();
//	SetDefault();
	SetColor( 0xFFFFFFFF );
}


inline C2DRoundRect::C2DRoundRect( int min_x, int min_y, int max_x, int max_y, U32 color,
								   int radius, int num_segments_per_corner )
:
m_CornerRadius(radius),
m_NumSegmentsPerCorner(num_segments_per_corner)
{
	m_AABB.vMin = Vector2( (float)min_x, (float)min_y );
	m_AABB.vMax = Vector2( (float)max_x, (float)max_y );
//	CalculateLocalVertexPositions();
//	SetDefault();
	SetColor( color );
}


inline C2DRoundRect::C2DRoundRect( float min_x, float min_y, float max_x, float max_y, U32 color,
								   int radius, int num_segments_per_corner )
:
m_CornerRadius(radius),
m_NumSegmentsPerCorner(num_segments_per_corner)
{
	m_AABB.vMin = Vector2( min_x, min_y );
	m_AABB.vMax = Vector2( max_x, max_y );
//	CalculateLocalVertexPositions();
//	SetDefault();
	SetColor( color );
}


inline C2DRoundRect::C2DRoundRect( const Vector2& vMin, const Vector2& vMax, U32 color,
								   int radius, int num_segments_per_corner )
:
m_CornerRadius(radius),
m_NumSegmentsPerCorner(num_segments_per_corner)
{
	m_AABB.vMin = vMin;
	m_AABB.vMax = vMax;
//	CalculateLocalVertexPositions();
//	SetDefault();
	SetColor( color );
}


inline C2DRoundRect::C2DRoundRect( const SRect& rect, U32 color, int radius, int num_segments_per_corner )
:
m_CornerRadius(radius),
m_NumSegmentsPerCorner(num_segments_per_corner)
{
	m_AABB.vMin = Vector2( (float)rect.left,  (float)rect.top );
	m_AABB.vMax = Vector2( (float)rect.right, (float)rect.bottom );
//	SetPosition( Vector2((float)rect.left,(float)rect.top), Vector2((float)rect.right,(float)rect.bottom) );
//	CalculateLocalVertexPositions();
//	SetDefault();
	SetColor( color );
}


inline void C2DRoundRect::SetDefault()
{
	if( m_vecRectVertex.size() == 0 )
		return;

	int num_vertices = GetNumVertices();

	for(int i=0; i<num_vertices; i++)
	{
		m_vecRectVertex[i].rhw = 1.0f;
		m_vecRectVertex[i].color = 0xFF000000;		// opaque by default
	}

	m_DestAlphaBlend = D3DBLEND_INVSRCALPHA;
}


inline void C2DRoundRect::draw()
{
	if( m_vecRectVertex.size() == 0 )
	{
		CalculateLocalVertexPositions();
		UpdateVertexPositions();
	}

	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, GetNumVertices() - 2, &m_vecRectVertex[0], sizeof(TLVERTEX) );
}



inline Vector2 C2DRoundRect::GetPosition2D( int vert_index ) const
{
	switch(vert_index)
	{
	case 0: return m_AABB.vMin;
	case 1: return Vector2( m_AABB.vMax.x, m_AABB.vMin.y );
	case 2: return m_AABB.vMax;
	case 3: return Vector2( m_AABB.vMin.x, m_AABB.vMax.y );
	default: return Vector2(0,0);
	}

	return Vector2(0,0);
}


inline void C2DRoundRect::SetPosition( const Vector2& vMin, const Vector2& vMax )
{
	m_AABB.vMin = vMin;
	m_AABB.vMax = vMax;

	if( m_vecRectVertex.size() == 0 )
		CalculateLocalVertexPositions();

	UpdateVertexPositions();
}


inline void C2DRoundRect::SetPositionLTRB( float l, float t, float r, float b )
{
	SetPosition( Vector2( l, t ), Vector2( r, b ) );
}


inline void C2DRoundRect::SetPositionLTRB( int l, int t, int r, int b )
{
	SetPositionLTRB( (float)l, (float)t, (float)r, (float)b );
}


inline void C2DRoundRect::Translate( float dx, float dy )
{
	const int num_vertices = GetNumVertices();
	for( int i=0; i<num_vertices; i++ )
		m_vecRectVertex[i].vPosition += D3DXVECTOR3( dx, dy, 0 );
}


inline void C2DRoundRect::Translate( int dx, int dy )
{
	Translate( (float)dx, (float)dy );
}


inline void C2DRoundRect::Rotate( float angle )
{
	Rotate( Matrix22Rotation(angle) );
}


inline void C2DRoundRect::Rotate( const Matrix22& matOrient )
{
	Vector2 v;

	const int num_vertices = GetNumVertices();
	for( int i=0; i<num_vertices; i++ )
	{
		Vector3& vert_pos = m_vecRectVertex[i].vPosition;
		v = matOrient * Vector2(vert_pos.x, vert_pos.y);

		vert_pos.x = v.x;
		vert_pos.y = v.y;
	}
}


inline void C2DRoundRect::ResizeBuffer()
{
	const int num_vertices = GetNumVertices();

	if( num_vertices == 0 )
		return;

	// buffer for FVF vertices
	TLVERTEX vert;
	vert.rhw   = 1.0f;
	vert.color = 0xFF000000;		// opaque by default
	vert.tu    = 0.0f;
	vert.tv    = 0.0f;
	vert.vPosition = D3DXVECTOR3(0,0,0);

	m_vecRectVertex.resize( num_vertices, vert );

	// buffer for cache of vertex positions
	m_vecLocalVertexPosition.resize( num_vertices, Vector2(0,0) );

	UpdateColor();
}


inline void C2DRoundRect::UpdateColor()
{
	const U32 argb32 = m_Color.GetARGB32();
	const int num_vertices = GetNumVertices();
	for( int i=0; i<num_vertices; i++ )
		m_vecRectVertex[i].color = argb32;
}


inline void C2DRoundRect::SetColor( const SFloatRGBAColor& color )
{
	m_Color = color;

	if( 0 < m_vecRectVertex.size() ) // avoid calling this from ctor
		UpdateColor();
}


inline void C2DRoundRect::SetColor( U32 color )
{
	SFloatRGBAColor c;
	c.SetARGB32( color );
	SetColor( c );
}


inline void C2DRoundRect::SetCornerRadius( int radius )
{
	m_CornerRadius = radius;

	CalculateLocalVertexPositions();

	UpdateVertexPositions();
}


inline void C2DRoundRect::SetNumSegmentsPerCorner( int num_segs_per_corner )
{
	if( num_segs_per_corner != m_NumSegmentsPerCorner )
	{
		// need reallocation
		m_NumSegmentsPerCorner = num_segs_per_corner;
		ResizeBuffer();
	}
}





/**
 * frame rectangle with rounded corners
 *
 */
class C2DRoundFrameRect : public C2DRoundRect
{
	/// width of the border in pixels
	int m_BorderWidth;

	SFloatRGBAColor m_aCornerColor[4];

protected:

	virtual int GetNumVertices() const { return ( m_NumSegmentsPerCorner + 1 ) * 4 * 2 + 2; }

	virtual void CalculateLocalVertexPositions();

	virtual void UpdateVertexPositions();

	inline virtual void UpdateColor();

public:

	inline C2DRoundFrameRect() : m_BorderWidth(2) {}

	inline C2DRoundFrameRect( const Vector2& vMin, const Vector2& vMax, U32 color = 0xFF000000, int radius = 8, int border_width = 2, int num_segments_per_corner = 8 );

	inline C2DRoundFrameRect( int min_x, int min_y, int max_x, int max_y, U32 color = 0xFF000000, int radius = 8, int border_width = 2, int num_segments_per_corner = 8 );

	inline C2DRoundFrameRect( float min_x, float min_y, float max_x, float max_y, U32 color = 0xFF000000, int radius = 8, int border_width = 2, int num_segments_per_corner = 8 );

	inline C2DRoundFrameRect( const SRect& rect, U32 color = 0xFF000000, int radius = 8, int border_width = 2, int num_segments_per_corner = 8 );

	virtual ~C2DRoundFrameRect() {}

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_ROUNDFRAMERECT; }

	inline virtual void SetColor( const SFloatRGBAColor& color );

	virtual void SetCornerColor( int corner, const SFloatRGBAColor& color ) { m_aCornerColor[corner] = color; UpdateColor(); }

	virtual void SetCornerColor( int corner, U32 color ) { m_aCornerColor[corner].SetARGB32( color ); UpdateColor(); }

	inline virtual void Set1DBorderTextureCoords();

	virtual void Set2DCircularBorderTextureCoords( float margin = 0.05f );

	/// draw rect without any render state changes
	inline virtual void draw();

	inline void SetBorderWidth( int border_width );

//	inline void SetPosition( const Vector2& vMin, const Vector2& vMax);

//	inline void SetPosition( int vert_index, const D3DXVECTOR2& rvPosition );

	/// set values for left, top, right and bottom of the rectangle
//	inline void SetPositionLTRB( float l, float t, float r, float b );
//	inline void SetPositionLTRB( int l, int t, int r, int b );
//	inline void SetPositionLTWH( float l, float t, float w, float h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }
//	inline void SetPositionLTWH( int l, int t, int w, int h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

//	inline void Translate( float dx, float dy );
//	inline void Translate( int dx, int dy );

//	inline void Rotate( float angle );

//	inline void Rotate( const Matrix22& matOrient );
};


// =================================== inline implementations =================================== 

inline C2DRoundFrameRect::C2DRoundFrameRect( int min_x, int min_y, int max_x, int max_y, U32 color,
								   int radius, int border_width, int num_segments_per_corner )
:
C2DRoundRect( min_x, min_y, max_x, max_y, color, radius, num_segments_per_corner ),
m_BorderWidth(border_width)
{
	m_AABB.vMin = Vector2( (float)min_x, (float)min_y );
	m_AABB.vMax = Vector2( (float)max_x, (float)max_y );
//	CalculateLocalVertexPositions();
//	SetDefault();
	C2DPrimitive::SetColor( color );
}


inline C2DRoundFrameRect::C2DRoundFrameRect( float min_x, float min_y, float max_x, float max_y, U32 color,
								   int radius, int border_width, int num_segments_per_corner )
:
C2DRoundRect( min_x, min_y, max_x, max_y, color, radius, num_segments_per_corner ),
m_BorderWidth(border_width)
{
	m_AABB.vMin = Vector2( min_x, min_y );
	m_AABB.vMax = Vector2( max_x, max_y );
//	CalculateLocalVertexPositions();
//	SetDefault();
	C2DPrimitive::SetColor( color );
}


inline C2DRoundFrameRect::C2DRoundFrameRect( const Vector2& vMin, const Vector2& vMax, U32 color,
								   int radius, int border_width, int num_segments_per_corner )
:
C2DRoundRect( vMin, vMax, color, radius, num_segments_per_corner ),
m_BorderWidth(border_width)
{
	m_AABB.vMin = vMin;
	m_AABB.vMax = vMax;
//	CalculateLocalVertexPositions();
//	SetDefault();
	C2DPrimitive::SetColor( color );
}


inline C2DRoundFrameRect::C2DRoundFrameRect( const SRect& rect, U32 color, int radius, int border_width, int num_segments_per_corner )
:
C2DRoundRect( rect, color, radius, num_segments_per_corner ),
m_BorderWidth(border_width)
{
//	CalculateLocalVertexPositions();
//	SetDefault();
	C2DPrimitive::SetColor( color );
}


inline void C2DRoundFrameRect::SetColor( const SFloatRGBAColor& color )
{
	// set the color to all 4 corners
	for( int i=0; i<4; i++ )
		m_aCornerColor[i] = color;

	if( 0 <= m_vecRectVertex.size() ) // avoid calling this from ctor
		UpdateColor();
}


inline void C2DRoundFrameRect::Set1DBorderTextureCoords()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const int num_points = GetNumVertices() / 2;
	for( int i=0; i<num_points; i++ )
	{
		int j = i*2;
		m_vecRectVertex[j].tu   = 1.0f;
		m_vecRectVertex[j].tv   = 0.0f;

		m_vecRectVertex[j+1].tu = 0.0f;
		m_vecRectVertex[j+1].tv = 0.0f;
	}
}


inline void C2DRoundFrameRect::draw()
{
	if( m_vecRectVertex.size() == 0 )
	{
		CalculateLocalVertexPositions();
		UpdateVertexPositions();
	}

	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, GetNumVertices() - 2, &m_vecRectVertex[0], sizeof(TLVERTEX) );
}


inline void C2DRoundFrameRect::SetBorderWidth( int border_width )
{
	m_BorderWidth = border_width;

	CalculateLocalVertexPositions();

	UpdateVertexPositions();
}


inline void C2DRoundFrameRect::UpdateVertexPositions()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const float r = (float)m_CornerRadius;
	Vector2 avCornerCenterPos[4] =
	{
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
		for( int j=0; j<(num_segs_per_corner+1)*2; j++, vert_index++ )
		{
			global_pos = avCornerCenterPos[i] + m_vecLocalVertexPosition[vert_index];
			m_vecRectVertex[vert_index].vPosition.x = global_pos.x;
			m_vecRectVertex[vert_index].vPosition.y = global_pos.y;
		}
	}

	// need to wrap positions
	global_pos = avCornerCenterPos[0] + m_vecLocalVertexPosition[0];
	m_vecRectVertex[vert_index].vPosition.x = global_pos.x;
	m_vecRectVertex[vert_index].vPosition.y = global_pos.y;

	vert_index++;
	global_pos = avCornerCenterPos[0] + m_vecLocalVertexPosition[1];
	m_vecRectVertex[vert_index].vPosition.x = global_pos.x;
	m_vecRectVertex[vert_index].vPosition.y = global_pos.y;

	// set default texture coordinates for simple border textures
	Set1DBorderTextureCoords();
}


inline void C2DRoundFrameRect::UpdateColor()
{
	if( m_vecLocalVertexPosition.size() == 0 || GetNumVertices() == 0 )
		return;

	const int num_segs_per_corner = m_NumSegmentsPerCorner;
	int vert_index = 0;
	U32 color;
	for( int i=0; i<4; i++ )
	{
		color = m_aCornerColor[i].GetARGB32();
		for( int j=0; j<(num_segs_per_corner+1)*2; j++, vert_index++ )
			m_vecRectVertex[vert_index].color = color;
	}

	color = m_aCornerColor[0].GetARGB32();
	m_vecRectVertex[vert_index++].color = color;
	m_vecRectVertex[vert_index].color   = color;

}


class C2DCompositeRoundRect : public C2DPrimitive
{
	C2DRoundRect m_Rect;
	C2DRoundFrameRect m_FrameRect;

public:
};


#endif		/*  __2DRoundRect_H__  */
