#ifndef  __2DFRAMERECT_H__
#define  __2DFRAMERECT_H__


#include "2DPrimitive.hpp"


class C2DFrameRect : public C2DPrimitive
{
	TLVERTEX m_avRectVertex[10];

	/// width of the rect border in pixels
	int m_BorderWidth;

public:

	inline C2DFrameRect();

	inline C2DFrameRect( const D3DXVECTOR2& rvMin, const D3DXVECTOR2& rvMax, U32 color = 0xFF000000, int border_width = 1 );

	inline C2DFrameRect( int min_x, int min_y, int max_x, int max_y, U32 color = 0xFF000000, int border_width = 1 );

	inline C2DFrameRect( float min_x, float min_y, float max_x, float max_y, U32 color = 0xFF000000, float border_width = 1.0f );

	inline C2DFrameRect( const SRect& rect, U32 color = 0xFF000000, float border_width = 1.0f );

	~C2DFrameRect() {}

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_FRAMERECT; }

	inline void SetDefault();

	inline void SetBorderWidth( int border_width );

	/// draw framerect without any render state changes
	inline virtual void draw();

//	virtual void Draw( const CTextureHandle& texture );
//	virtual void Draw() { C2DPrimitive::Draw(); }

//	inline Vector2 GetCornerPos2D( int vert_index ) const;

	inline virtual Vector2 GetPosition2D( int vert_index ) const;

	int GetBorderWidth() const { return m_BorderWidth; }

	/// Sets an outer boundary of the frame rect
	inline void SetPosition( const Vector2& rvMin, const Vector2& rvMax);

	inline virtual void SetVertexPosition( int vert_index, const Vector2& rvPosition );

	inline void SetPositionLTRB( float left, float top, float right, float bottom );

	inline void SetPositionLTWH( float left, float top, float width, float height );

//	void SetTextureUV( const D3DXVECTOR2& rvMin, const D3DXVECTOR2& rvMax);

	void SetColor( U32 color ) { C2DPrimitive::SetColor(color); }

	void SetCornerColor( int corner, U32 color ) { C2DPrimitive::SetCornerColor(corner,color); }

	inline virtual void SetColor( const SFloatRGBAColor& color );

	inline virtual void SetCornerColor( int corner, const SFloatRGBAColor& color );

	virtual void SetZDepth(float fZValue);

	void ScalePosition( float fScale );

//	D3DXVECTOR2 GetPosition( int iVertex ) { return m_avRectVertex[iVertex]; }
};


// =================================== inline implementations =================================== 

inline C2DFrameRect::C2DFrameRect()
: m_BorderWidth(1)
{
	SetDefault();
}


inline C2DFrameRect::C2DFrameRect( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax, U32 color, int border_width )
: m_BorderWidth(border_width)
{
	SetDefault();
	SetBorderWidth( border_width );
	SetPosition( vMin, vMax );
	SetColor( color );
}


inline C2DFrameRect::C2DFrameRect( int min_x, int min_y, int max_x, int max_y, U32 color, int border_width )
{
	SetDefault();
	SetBorderWidth( border_width );
	SetPosition( D3DXVECTOR2((float)min_x,(float)min_y), D3DXVECTOR2((float)max_x,(float)max_y) );
	SetColor( color );
}


inline C2DFrameRect::C2DFrameRect( float min_x, float min_y, float max_x, float max_y, U32 color, float border_width )
{
	SetDefault();
	SetBorderWidth( (int)border_width );
	SetPosition( D3DXVECTOR2(min_x,min_y), D3DXVECTOR2(max_x,max_y) );
	SetColor( color );
}


inline C2DFrameRect::C2DFrameRect( const SRect& rect, U32 color, float border_width )
{
	SetDefault();
	SetBorderWidth( (int)border_width );
	SetPosition( D3DXVECTOR2((float)rect.left,(float)rect.top), D3DXVECTOR2((float)rect.right,(float)rect.bottom) );
	SetColor( color );
}


inline void C2DFrameRect::SetDefault()
{
	ZeroMemory(m_avRectVertex, sizeof(TLVERTEX) * 10);

	for(int i=0; i<10; i++)
	{
		m_avRectVertex[i].rhw = 1.0f;
		m_avRectVertex[i].color = 0xff000000;		// opaque by default
	}

	m_DestAlphaBlend = D3DBLEND_INVSRCALPHA;
}


inline void C2DFrameRect::draw()
{
	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 8, m_avRectVertex, sizeof(TLVERTEX) );
}

/*
inline D3DXVECTOR2 C2DFrameRect::GetCornerPos2D( int vert_index ) const
{
	GetPosition2D(vert_index);
}
*/

inline Vector2 C2DFrameRect::GetPosition2D( int vert_index ) const
{
	const D3DXVECTOR3& vPos = m_avRectVertex[ vert_index * 2 + 1 ].vPosition;
	return Vector2( vPos.x, vPos.y );
}


inline void C2DFrameRect::SetPosition( const Vector2& rvMin, const Vector2& rvMax )
{
	float bw = (float)m_BorderWidth;
	float z = m_avRectVertex[0].vPosition.z;

	TLVERTEX* pVert = m_avRectVertex;

	// top-left corner
	pVert[0].vPosition = D3DXVECTOR3( rvMin.x, rvMin.y, z ) + D3DXVECTOR3( bw, bw, 0); // inner
	pVert[1].vPosition = D3DXVECTOR3( rvMin.x, rvMin.y, z );                           // outer

	// top-right corner
	pVert[2].vPosition = D3DXVECTOR3( rvMax.x, rvMin.y, z ) + D3DXVECTOR3(-bw, bw, 0); // inner
	pVert[3].vPosition = D3DXVECTOR3( rvMax.x, rvMin.y, z );						   // outer

	// bottom-right corner
	pVert[4].vPosition = D3DXVECTOR3( rvMax.x, rvMax.y, z ) + D3DXVECTOR3(-bw,-bw, 0); // inner
	pVert[5].vPosition = D3DXVECTOR3( rvMax.x, rvMax.y, z );						   // outer

	// bottom-left corner
	pVert[6].vPosition = D3DXVECTOR3( rvMin.x, rvMax.y, z ) + D3DXVECTOR3( bw,-bw, 0); // inner
	pVert[7].vPosition = D3DXVECTOR3( rvMin.x, rvMax.y, z );						   // outer

	// wrapping at the top-left corner (same as pVert[0] & pVert[1])
	pVert[8].vPosition = D3DXVECTOR3( rvMin.x, rvMin.y, z ) + D3DXVECTOR3( bw, bw, 0); // inner
	pVert[9].vPosition = D3DXVECTOR3( rvMin.x, rvMin.y, z );						   // outer

	// set default texture coord
	// - set u to
	//   - 0.0 for vertices on the outer side of the frame
	//   - 1.0 for vertices on the inner side of the frame
	for( int i=0; i<5; i++ )
	{
		// inner 
		pVert[i*2].tu = 1.0f;
		pVert[i*2].tv = 0.0f;

		// outer
		pVert[i*2+1].tu = 0.0f;
		pVert[i*2+1].tv = 0.0f;
	}
}


inline void C2DFrameRect::SetVertexPosition( int vert_index, const Vector2& rvPosition )
{
	const float bw = (float)m_BorderWidth;
	const float z = m_avRectVertex[0].vPosition.z;

	TLVERTEX* pVert = m_avRectVertex;

	const float x = rvPosition.x;
	const float y = rvPosition.y;
	switch( vert_index )
	{
	case 0:
		// top-left corner
		pVert[0].vPosition = D3DXVECTOR3( x, y, z ) + D3DXVECTOR3( bw, bw, 0); // inner
		pVert[1].vPosition = D3DXVECTOR3( x, y, z );                           // outer
		// wrapping at the top-left corner (same as pVert[0] & pVert[1])
		pVert[8].vPosition = D3DXVECTOR3( x, y, z ) + D3DXVECTOR3( bw, bw, 0); // inner
		pVert[9].vPosition = D3DXVECTOR3( x, y, z );						   // outer
		break;

	case 1:
		// top-right corner
		pVert[2].vPosition = D3DXVECTOR3( x, y, z ) + D3DXVECTOR3(-bw, bw, 0); // inner
		pVert[3].vPosition = D3DXVECTOR3( x, y, z );						   // outer
		break;

	case 2:
		// bottom-right corner
		pVert[4].vPosition = D3DXVECTOR3( x, y, z ) + D3DXVECTOR3(-bw,-bw, 0); // inner
		pVert[5].vPosition = D3DXVECTOR3( x, y, z );						   // outer
		break;

	case 3:
		// bottom-left corner
		pVert[6].vPosition = D3DXVECTOR3( x, y, z ) + D3DXVECTOR3( bw,-bw, 0); // inner
		pVert[7].vPosition = D3DXVECTOR3( x, y, z );						   // outer
		break;

	default:
		break;
	}
}


inline void C2DFrameRect::SetPositionLTRB( float left, float top, float right, float bottom )
{
	SetPosition( D3DXVECTOR2(left, top), D3DXVECTOR2(right, bottom) );
}


inline void C2DFrameRect::SetPositionLTWH( float left, float top, float width, float height )
{
	SetPosition( D3DXVECTOR2(left, top), D3DXVECTOR2(left + width - 1.0f, top + height - 1.0f) );
}


inline void C2DFrameRect::SetColor( const SFloatRGBAColor& color )
{
	U32 c = color.GetARGB32();
	for( int i=0; i<10; i++ )
	{
		m_avRectVertex[i].color = c;
	}
}


inline void C2DFrameRect::SetCornerColor( int corner, const SFloatRGBAColor& color )
{
	U32 c = color.GetARGB32();
	m_avRectVertex[corner*2].color   = c;
	m_avRectVertex[corner*2+1].color = c;

	if( corner == 0 )
	{
		m_avRectVertex[8].color = c;
		m_avRectVertex[9].color = c;
	}
}


void C2DFrameRect::SetBorderWidth( int border_width )
{
	m_BorderWidth = border_width;

	TLVERTEX* pVert = m_avRectVertex;
	SetPosition(
		D3DXVECTOR2( pVert[1].vPosition.x, pVert[1].vPosition.y ),
		D3DXVECTOR2( pVert[5].vPosition.x, pVert[5].vPosition.y ) );
}



#endif		/*  __2DFRAMERECT_H__  */
