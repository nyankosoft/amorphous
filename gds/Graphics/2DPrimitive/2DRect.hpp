#ifndef  __2DRECT_H__
#define  __2DRECT_H__


#include "2DPrimitive.hpp"


/**
 hold a 2D rectangle
 - coord are set as left, top, right, bottom if 4 values are used as arguments without any notice
 - vertex indexing is done in clockwise order: [0,3] > top-left, top-right, bottom-right, bottom-left
 */
class C2DRect : public C2DPrimitive
{
	TLVERTEX m_avRectVertex[4];

public:

	inline C2DRect() { SetDefault(); }

	inline C2DRect( const Vector2& vMin, const Vector2& vMax, U32 color = 0xFF000000 );

	inline C2DRect( int min_x, int min_y, int max_x, int max_y, U32 color = 0xFF000000 );

	inline C2DRect( float min_x, float min_y, float max_x, float max_y, U32 color = 0xFF000000 );

	inline C2DRect( const SRect& rect, U32 color = 0xFF000000 );

	~C2DRect();

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_RECT; }

	inline void SetDefault();

	void Release();

	/// Draws the rect with the current render state settings.
	/// Check the following render states before calling this function
	/// - z-buffer
	/// - texture
	/// - alpha blend
	inline virtual void draw();

	void Draw() { C2DPrimitive::Draw(); }

	void Draw( const CTextureHandle& texture ) { C2DPrimitive::Draw( texture ); }

	void Draw( const LPDIRECT3DTEXTURE9 pTexture );

	void DrawWireframe();

	inline virtual Vector2 GetPosition2D( int vert_index ) const;

	inline void SetPosition( const Vector2& vMin, const Vector2& vMax);

	inline void SetPosition( int vert_index, const D3DXVECTOR2& rvPosition );

	/// set values for left, top, right and bottom of the rectangle
	inline void SetPositionLTRB( float l, float t, float r, float b );

	inline void SetPositionLTRB( int l, int t, int r, int b );

	inline void SetPositionLTWH( float l, float t, float w, float h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	inline void SetPositionLTWH( int l, int t, int w, int h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	virtual void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax );

	void SetColor( U32 color ) { C2DPrimitive::SetColor(color); }

	void SetCornerColor( int corner, U32 color ) { C2DPrimitive::SetCornerColor(corner,color); }

	inline virtual void SetColor( const SFloatRGBAColor& color );

	inline virtual void SetCornerColor( int corner, const SFloatRGBAColor& color );

	virtual void SetZDepth(float fZValue);

	void ScalePosition( float fScale );

	inline void Translate( float dx, float dy );
	inline void Translate( int dx, int dy );

	inline void Rotate( float angle );

	inline void Rotate( const Matrix22& matOrient );
};


// =================================== inline implementations =================================== 


inline C2DRect::C2DRect( int min_x, int min_y, int max_x, int max_y, U32 color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2((float)min_x,(float)min_y), D3DXVECTOR2((float)max_x,(float)max_y) );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( float min_x, float min_y, float max_x, float max_y, U32 color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2(min_x,min_y), D3DXVECTOR2(max_x,max_y) );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax, U32 color )
{
	SetDefault();
	SetPosition( vMin, vMax );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( const SRect& rect, U32 color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2((float)rect.left,(float)rect.top), D3DXVECTOR2((float)rect.right,(float)rect.bottom) );
	C2DPrimitive::SetColor( color );
}


inline void C2DRect::SetDefault()
{
	ZeroMemory(m_avRectVertex, sizeof(TLVERTEX) * 4);
	for(int i=0; i<4; i++)
	{
		m_avRectVertex[i].rhw = 1.0f;
		m_avRectVertex[i].color = 0xff000000;		//opaque by default
	}

	m_DestAlphaBlend = D3DBLEND_INVSRCALPHA;
}


inline Vector2 C2DRect::GetPosition2D( int vert_index ) const
{
	const D3DXVECTOR3& pos = m_avRectVertex[vert_index].vPosition;
	return Vector2( pos.x, pos.y );
}


/// Draws the rect without any render state changes
inline void C2DRect::draw()
{
	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectVertex, sizeof(TLVERTEX) );
}


inline void C2DRect::SetPosition( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax )
{
	TLVERTEX* pVert = m_avRectVertex;

	pVert[0].vPosition.x = vMin.x;		// top-left corner of the rectangle
	pVert[0].vPosition.y = vMin.y;

	pVert[1].vPosition.x = vMax.x;
	pVert[1].vPosition.y = vMin.y;

	pVert[2].vPosition.x = vMax.x;		// bottom-right corner of the rectangle
	pVert[2].vPosition.y = vMax.y;

	pVert[3].vPosition.x = vMin.x;
	pVert[3].vPosition.y = vMax.y;
}


inline void C2DRect::SetPosition( int vert_index, const D3DXVECTOR2& rvPosition )
{
	m_avRectVertex[vert_index].vPosition.x = rvPosition.x;
	m_avRectVertex[vert_index].vPosition.y = rvPosition.y;
}


inline void C2DRect::SetPositionLTRB( float l, float t, float r, float b )
{
	SetPosition( D3DXVECTOR2(l,t), D3DXVECTOR2(r,b) );
}


inline void C2DRect::SetPositionLTRB( int l, int t, int r, int b )
{
	SetPositionLTRB( (float)l, (float)t, (float)r, (float)b );
}


inline void C2DRect::SetColor( const SFloatRGBAColor& color )
{
	U32 c = color.GetARGB32();
	for(int i=0; i<4; i++)
		m_avRectVertex[i].color   = c;
}


inline void C2DRect::SetCornerColor( int corner, const SFloatRGBAColor& color )
{
	U32 c = color.GetARGB32();
	if( 0 <= corner && corner < 4 )
	{
		// set the color for only one vertex specified by 'iVertexNum'
		m_avRectVertex[corner].color = c;
	}
}


inline void C2DRect::Translate( float dx, float dy )
{
	m_avRectVertex[0].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[1].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[2].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[3].vPosition += D3DXVECTOR3( dx, dy, 0 );
}


inline void C2DRect::Translate( int dx, int dy )
{
	Translate( (float)dx, (float)dy );
}


inline void C2DRect::Rotate( float angle )
{
	Rotate( Matrix22Rotation(angle) );
}


inline void C2DRect::Rotate( const Matrix22& matOrient )
{
	Vector2 v;

	for( int i=0; i<4; i++ )
	{
		Vector3& vert_pos = m_avRectVertex[i].vPosition;
		v = matOrient * Vector2(vert_pos.x, vert_pos.y);

		vert_pos.x = v.x;
		vert_pos.y = v.y;
	}
}



#endif		/*  __2DRECT_H__  */
