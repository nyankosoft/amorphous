#ifndef  __2DTRIANGLE_H__
#define  __2DTRIANGLE_H__


#include "2DPrimitive.hpp"
#include "2DPrimitiveRenderer.hpp"


namespace amorphous
{


class C2DTriangle : public C2DPrimitive
{
	General2DVertex m_avVertex[3];

public:

	enum Direction
	{
		DIR_UP,
		DIR_DOWN,
		DIR_RIGHT,
		DIR_LEFT,
		NUM_DIRECTIONS
	};

	inline C2DTriangle() { SetDefault(); }

//	inline C2DTriangle( const Vector2& pos0, const Vector2& pos1, const Vector2& pos2 );
//	inline C2DTriangle( const Vector2& vMin, const Vector2& vMax );
//	inline C2DTriangle( int min_x, int min_y, int max_x, int max_y );

	inline C2DTriangle( Direction dir,  const Vector3& vMin, const Vector3& vMax, U32 color = 0xFF000000 ); 

	inline C2DTriangle( Direction dir,  const SRect& rect, U32 color = 0xFF000000 ); 

	virtual ~C2DTriangle() {}

	virtual int unsigned GetPrimitiveType() const { return C2DPrimitive::TYPE_TRIANGLE; }

	inline void SetDefault();

	inline virtual void draw();

	inline Vector2 GetPosition2D( int vert_index ) const;

	/// Performs scaling of the triangle without changing the current orientation
	void SetPosition( const Vector2& vMin, const Vector2& vMax );

	inline void SetPosition( int vert_index, const Vector2& rvPosition );

	inline void SetPosition( Direction dir, const SRect& rect );

	/// set values for the 3 vertices in the clockwise order starting with index 0
	inline void SetPositionCC( float x0, float y0, float x1, float y1, float x2, float y2  );

	inline void SetPositionCC( int x0, int y0, int x1, int y1, int x2, int y2 );

//	void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax);

	/// \param dwColor 32-bit, ARGB format color
	inline void SetVertexColor( U32 color, int iVertexNum );

	void SetColor( U32 color ) { C2DPrimitive::SetColor(color); }

	/// sets a single color to all the 3 vertices
	inline virtual void SetColor( const SFloatRGBAColor& color );

	inline void SetZDepth(float fZValue);

	inline void ScalePosition( float fScale );

	inline void Rotate( float angle );

	inline void Rotate( const Matrix22& matOrient );
};


class C2DFrameTriangle : public C2DPrimitive
{
public:
};


class C2DRoundFrameTriangle : public C2DPrimitive
{
public:
};



// =================================== inline implementations =================================== 

/*
inline C2DTriangle::C2DTriangle( int min_x, int min_y, int max_x, int max_y )
{
	SetDefault();
	SetPosition( Vector2((float)min_x,(float)min_y), Vector2((float)max_x,(float)max_y) );
}
*/

inline C2DTriangle::C2DTriangle( Direction dir,  const Vector3& vMin, const Vector3& vMax, U32 color )
{
	SetDefault();

	SetPosition( dir, RectLTRB( (int)vMin.x, (int)vMin.y, (int)vMax.x, (int)vMax.y ) );

	SetColor( color );
}


inline C2DTriangle::C2DTriangle( Direction dir,  const SRect& rect, U32 color )
{
	SetDefault();

	SetPosition( dir, rect );

	SetColor( color );
}


inline void C2DTriangle::SetDefault()
{
	for(int i=0; i<3; i++)
	{
		m_avVertex[i].m_vPosition = Vector3(0,0,0);
		m_avVertex[i].m_fRHW = 1.0f;
		m_avVertex[i].m_DiffuseColor.SetToBlack(); // opaque by default
		for( int j=0; j<General2DVertex::NUM_MAX_TEXTURECOORD_SETS; j++ )
			m_avVertex[i].m_TextureCoord[j] = TEXCOORD2(0,0);
	}

	m_DestAlphaBlend = AlphaBlend::InvSrcAlpha;
}


/*
inline void C2DTriangle::Draw( const TextureHandle& texture )
{
	Draw( texture.GetTexture() );
}
*/


inline void C2DTriangle::draw()
{
//	DIRECT3D9.GetDevice()->SetFVF( D3DFVF_TLVERTEX );
//	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 1, m_avVertex, sizeof(TLVERTEX) );
	Get2DPrimitiveRenderer().Render( m_avVertex, 3, PrimitiveType::TRIANGLE_FAN );
}


inline Vector2 C2DTriangle::GetPosition2D( int vert_index ) const
{
	const Vector3& pos = m_avVertex[vert_index].m_vPosition;

	return Vector2( pos.x, pos.y );
}


inline void C2DTriangle::SetPosition( int vert_index, const Vector2& rvPosition )
{
	m_avVertex[vert_index].m_vPosition.x = rvPosition.x;
	m_avVertex[vert_index].m_vPosition.y = rvPosition.y;
}


inline void C2DTriangle::SetPosition( Direction dir, const SRect& rect )
{
	const float cx = (float)( rect.left + rect.right  ) * 0.5f;//( vMax.x + vMin.x ) * 0.5f;
	const float cy = (float)( rect.top  + rect.bottom ) * 0.5f;//( vMax.y + vMin.y ) * 0.5f;
	const float sx = (float)rect.left;  //vMin.x;
	const float sy = (float)rect.top;   //vMin.y;
	const float ex = (float)rect.right; //vMax.x;
	const float ey = (float)rect.bottom;//vMax.y;

	switch( dir )
	{
	case DIR_UP:
	default:
		SetPositionCC( cx, sy,  ex, ey,  sx, ey ); break;
	case DIR_DOWN:
		SetPositionCC( cx, ey,  sx, sy,  ex, sy ); break;
	case DIR_RIGHT:
		SetPositionCC( sx, sy,  ex, cy,  sx, ey ); break;
	case DIR_LEFT:
		SetPositionCC( sx, cy,  ex, sy,  ex, ey ); break;
	}
}


inline void C2DTriangle::SetPositionCC( float x0, float y0, float x1, float y1, float x2, float y2 )
{
	m_avVertex[0].m_vPosition.x = x0;
	m_avVertex[0].m_vPosition.y = y0;
	m_avVertex[1].m_vPosition.x = x1;
	m_avVertex[1].m_vPosition.y = y1;
	m_avVertex[2].m_vPosition.x = x2;
	m_avVertex[2].m_vPosition.y = y2;
}


inline void C2DTriangle::SetPositionCC( int x0, int y0, int x1, int y1, int x2, int y2 )
{
	SetPositionCC( (float)x0, (float)y0, (float)x1, (float)y1, (float)x2, (float)y2 );
}


void C2DTriangle::SetVertexColor( U32 color, int vert_num )
{
	if( 0 <= vert_num && vert_num < 3 )
	{
		// set the color for only one vertex specified by 'iVertexNum'
		m_avVertex[vert_num].m_DiffuseColor.SetARGB32( color );
	}
}


void C2DTriangle::SetColor( const SFloatRGBAColor& color )
{
	for(int i=0; i<3; i++)
		m_avVertex[i].m_DiffuseColor = color;
}


inline void C2DTriangle::SetZDepth(float fZValue)
{
	m_avVertex[0].m_vPosition.z = fZValue;
	m_avVertex[1].m_vPosition.z = fZValue;
	m_avVertex[2].m_vPosition.z = fZValue;
}


inline void C2DTriangle::ScalePosition( float fScale )
{
	for(int i=0; i<3; i++)
		m_avVertex[i].m_vPosition *= fScale;
}


inline void C2DTriangle::Rotate( float angle )
{
	Rotate( Matrix22Rotation(angle) );
}


inline void C2DTriangle::Rotate( const Matrix22& matOrient )
{
	Vector2 v;

	for( int i=0; i<3; i++ )
	{
		Vector3& vert_pos = m_avVertex[i].m_vPosition;
		v = matOrient * Vector2(vert_pos.x, vert_pos.y);

		vert_pos.x = v.x;
		vert_pos.y = v.y;
	}
}


} // namespace amorphous



#endif		/*  __2DTRIANGLE_H__  */
