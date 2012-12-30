#ifndef  __2DRECT_H__
#define  __2DRECT_H__


#include "2DPrimitive.hpp"
#include "2DPrimitiveRenderer.hpp"


namespace amorphous
{


/**
 hold a 2D rectangle
 - coord are set as left, top, right, bottom if 4 values are used as arguments without any notice
 - vertex indexing is done in clockwise order: [0,3] > top-left, top-right, bottom-right, bottom-left
 */
class C2DRect : public C2DPrimitive
{
	CGeneral2DVertex m_avRectVertex[4];

public:

	inline C2DRect() { SetDefault(); }

	inline C2DRect( const Vector2& vMin, const Vector2& vMax, U32 color = 0xFF000000 );

	inline C2DRect( const Vector2& vMin, const Vector2& vMax, const SFloatRGBAColor& color );

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

	void DrawWireframe();

	inline virtual Vector2 GetPosition2D( int vert_index ) const;

	/// Added to let 2DPrimitiveRender access individual vertices and render them with shader
	const CGeneral2DVertex& GetVertex( int vert_index ) const { return m_avRectVertex[vert_index]; }

	inline void SetPosition( const Vector2& vMin, const Vector2& vMax);

	inline void SetPosition( int vert_index, const Vector2& rvPosition );

	virtual void SetVertexPosition( int vert_index, const Vector2& rvPosition ) { SetPosition( vert_index, rvPosition ); }

	/// set values for left, top, right and bottom of the rectangle
	inline void SetPositionLTRB( float l, float t, float r, float b );

	inline void SetPositionLTRB( int l, int t, int r, int b );

	inline void SetPositionLTWH( float l, float t, float w, float h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	inline void SetPositionLTWH( int l, int t, int w, int h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	virtual void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax );

	inline void SetTextureCoord( int vert_index, int tex_coord_index, const TEXCOORD2& t );

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
	SetPosition( Vector2((float)min_x,(float)min_y), Vector2((float)max_x,(float)max_y) );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( float min_x, float min_y, float max_x, float max_y, U32 color )
{
	SetDefault();
	SetPosition( Vector2(min_x,min_y), Vector2(max_x,max_y) );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( const Vector2& vMin, const Vector2& vMax, U32 color )
{
	SetDefault();
	SetPosition( vMin, vMax );
	C2DPrimitive::SetColor( color );
}


inline C2DRect::C2DRect( const Vector2& vMin, const Vector2& vMax, const SFloatRGBAColor& color )
{
	SetDefault();
	SetPosition( vMin, vMax );
	SetColor( color );
}


inline C2DRect::C2DRect( const SRect& rect, U32 color )
{
	SetDefault();
	SetPosition( Vector2((float)rect.left,(float)rect.top), Vector2((float)rect.right,(float)rect.bottom) );
	C2DPrimitive::SetColor( color );
}


inline void C2DRect::SetDefault()
{
	memset( m_avRectVertex, 0, sizeof(m_avRectVertex) );

	for(int i=0; i<4; i++)
	{
		m_avRectVertex[i].m_fRHW = 1.0f;
		m_avRectVertex[i].m_DiffuseColor.SetToBlack(); // opaque by default
	}

	m_DestAlphaBlend = AlphaBlend::InvSrcAlpha;
}


inline Vector2 C2DRect::GetPosition2D( int vert_index ) const
{
	const Vector3& pos = m_avRectVertex[vert_index].m_vPosition;
	return Vector2( pos.x, pos.y );
}


/// Draws the rect without any render state changes
inline void C2DRect::draw()
{
//	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
//	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectVertex, sizeof(TLVERTEX) );

	PrimitiveRenderer().Render( m_avRectVertex, 4, PrimitiveType::TRIANGLE_FAN );
}


inline void C2DRect::SetPosition( const Vector2& vMin, const Vector2& vMax )
{
	CGeneral2DVertex* pVert = m_avRectVertex;

	pVert[0].m_vPosition.x = vMin.x;		// top-left corner of the rectangle
	pVert[0].m_vPosition.y = vMin.y;

	pVert[1].m_vPosition.x = vMax.x;
	pVert[1].m_vPosition.y = vMin.y;

	pVert[2].m_vPosition.x = vMax.x;		// bottom-right corner of the rectangle
	pVert[2].m_vPosition.y = vMax.y;

	pVert[3].m_vPosition.x = vMin.x;
	pVert[3].m_vPosition.y = vMax.y;
}


inline void C2DRect::SetPosition( int vert_index, const Vector2& rvPosition )
{
	m_avRectVertex[vert_index].m_vPosition.x = rvPosition.x;
	m_avRectVertex[vert_index].m_vPosition.y = rvPosition.y;
}


inline void C2DRect::SetPositionLTRB( float l, float t, float r, float b )
{
	SetPosition( Vector2(l,t), Vector2(r,b) );
}


inline void C2DRect::SetPositionLTRB( int l, int t, int r, int b )
{
	SetPositionLTRB( (float)l, (float)t, (float)r, (float)b );
}


inline void C2DRect::SetTextureCoord( int vert_index, int tex_coord_index, const TEXCOORD2& t )
{
	m_avRectVertex[vert_index].m_TextureCoord[tex_coord_index] = t;
}


inline void C2DRect::SetColor( const SFloatRGBAColor& color )
{
//	U32 c = color.GetARGB32();
	for(int i=0; i<4; i++)
//		m_avRectVertex[i].color   = c;
		m_avRectVertex[i].m_DiffuseColor   = color;
}


inline void C2DRect::SetCornerColor( int corner, const SFloatRGBAColor& color )
{
//	U32 c = color.GetARGB32();
	if( 0 <= corner && corner < 4 )
	{
		// set the color for only one vertex specified by 'iVertexNum'
//		m_avRectVertex[corner].color = c;
		m_avRectVertex[corner].m_DiffuseColor = color;
	}
}


inline void C2DRect::Translate( float dx, float dy )
{
	m_avRectVertex[0].m_vPosition += Vector3( dx, dy, 0 );
	m_avRectVertex[1].m_vPosition += Vector3( dx, dy, 0 );
	m_avRectVertex[2].m_vPosition += Vector3( dx, dy, 0 );
	m_avRectVertex[3].m_vPosition += Vector3( dx, dy, 0 );
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
		Vector3& vert_pos = m_avRectVertex[i].m_vPosition;
		v = matOrient * Vector2(vert_pos.x, vert_pos.y);

		vert_pos.x = v.x;
		vert_pos.y = v.y;
	}
}


} // namespace amorphous



#endif		/*  __2DRECT_H__  */
