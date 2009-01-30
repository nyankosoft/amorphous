#ifndef  __2DTEXRECT_H__
#define  __2DTEXRECT_H__


#include "Graphics/Direct3D9.hpp"
#include "Graphics/FVF_TLVertex.h"
#include "Graphics/TextureCoord.hpp"
#include "Graphics/Rect.hpp"
using namespace::Graphics;

#include "3DMath/Vector2.hpp"
#include "3DMath/Matrix22.hpp"
#include "3DMath/Vector3.hpp"


/**
 * hold a 2D rectangle
 * - coord are set as left, top, right, bottom if 4 values are used as arguments without any notice
 * - vertex indexing is done in clockwise order: [0-3] > top-left, top-right, bottom-right, bottom-left
 */
class C2DTexRect
{
	TLVERTEX2 m_avRectVertex[4];

	DWORD m_DestAlphaBlend;

	inline void SetRenderStates();

	inline void DrawRect();

public:

	inline C2DTexRect() { SetDefault(); }

	inline C2DTexRect( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax, DWORD color = 0xFF000000 );

	inline C2DTexRect( int min_x, int min_y, int max_x, int max_y, DWORD color = 0xFF000000 );

	inline C2DTexRect( float min_x, float min_y, float max_x, float max_y, DWORD color = 0xFF000000 );

	inline C2DTexRect( const SRect& rect, DWORD color = 0xFF000000 );

	~C2DTexRect();

	void SetDefault();

	void Release();

	void Draw();
	void Draw( const LPDIRECT3DTEXTURE9 pTexture );
	void Draw( const LPDIRECT3DTEXTURE9 pTexture0, const LPDIRECT3DTEXTURE9 pTexture1 );

	void DrawWireframe();

	/// draw rect without any render state changes
	inline void draw() const;

	inline D3DXVECTOR2 GetPosition2D( int vert_index ) const;

	inline void SetPosition( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax);

	inline void SetPosition( int vert_index, const D3DXVECTOR2& rvPosition );

	/// set values for left, top, right and bottom of the rectangle
	inline void SetPositionLTRB( float l, float t, float r, float b );

	inline void SetPositionLTRB( int l, int t, int r, int b );

	inline void SetPositionLTWH( float l, float t, float w, float h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	inline void SetPositionLTWH( int l, int t, int w, int h ) { SetPositionLTRB(l,t,l+w-1,t+h-1); }

	void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax);

	void SetTextureUV( const D3DXVECTOR2& rvMin, const D3DXVECTOR2& rvMax )
	{
		SetTextureUV( TEXCOORD2( rvMin.x, rvMin.y ), TEXCOORD2( rvMax.x, rvMax.y ) );
	}

	void SetColor(DWORD dwColor, int iVertexNum = 4);	//'dwColor' is ARGB format

	void SetZDepth(float fZValue);

	void ScalePosition( float fScale );

	inline void Translate( float dx, float dy );
	inline void Translate( int dx, int dy );

	inline void Rotate( float angle );

	inline void Rotate( const Matrix22& matOrient );

	inline void SetDestAlphaBlendMode(DWORD DestBlend) { m_DestAlphaBlend = DestBlend; }

//	D3DXVECTOR2 GetPosition( int iVertex ) { return m_avRectVertex[iVertex]; }
};


// =================================== inline implementations =================================== 


inline C2DTexRect::C2DTexRect( int min_x, int min_y, int max_x, int max_y, DWORD color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2((float)min_x,(float)min_y), D3DXVECTOR2((float)max_x,(float)max_y) );
	SetColor( color );
}


inline C2DTexRect::C2DTexRect( float min_x, float min_y, float max_x, float max_y, DWORD color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2(min_x,min_y), D3DXVECTOR2(max_x,max_y) );
	SetColor( color );
}


inline C2DTexRect::C2DTexRect( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax, DWORD color )
{
	SetDefault();
	SetPosition( vMin, vMax );
	SetColor( color );
}


inline C2DTexRect::C2DTexRect( const SRect& rect, DWORD color )
{
	SetDefault();
	SetPosition( D3DXVECTOR2((float)rect.left,(float)rect.top), D3DXVECTOR2((float)rect.right,(float)rect.bottom) );
	SetColor( color );
}


inline void C2DTexRect::draw() const
{
	DIRECT3D9.GetDevice()->SetFVF( TLVERTEX2::FVF );
	DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectVertex, sizeof(TLVERTEX2) );
}

	
inline void C2DTexRect::DrawRect()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetFVF( TLVERTEX2::FVF );
	HRESULT hr;
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avRectVertex, sizeof(TLVERTEX2) );

	if( FAILED(hr) )
		MessageBox(NULL, "DrawPrimUP failed.", "Error", MB_OK);
}


inline void C2DTexRect::SetRenderStates()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
//		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

}


inline D3DXVECTOR2 C2DTexRect::GetPosition2D( int vert_index ) const
{
	const D3DXVECTOR3& pos = m_avRectVertex[vert_index].vPosition;
	return D3DXVECTOR2( pos.x, pos.y );
}


inline void C2DTexRect::SetPosition( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax )
{
	TLVERTEX2* pVert = m_avRectVertex;

	pVert[0].vPosition.x = vMin.x;		// top-left corner of the rectangle
	pVert[0].vPosition.y = vMin.y;

	pVert[1].vPosition.x = vMax.x;
	pVert[1].vPosition.y = vMin.y;

	pVert[2].vPosition.x = vMax.x;		// bottom-right corner of the rectangle
	pVert[2].vPosition.y = vMax.y;

	pVert[3].vPosition.x = vMin.x;
	pVert[3].vPosition.y = vMax.y;
}


inline void C2DTexRect::SetPosition( int vert_index, const D3DXVECTOR2& rvPosition )
{

#ifdef _DEBUG
	if( vert_index < 0 || 4 <= vert_index )
		return;
#endif

	m_avRectVertex[vert_index].vPosition.x = rvPosition.x;
	m_avRectVertex[vert_index].vPosition.y = rvPosition.y;
}


inline void C2DTexRect::SetPositionLTRB( float l, float t, float r, float b )
{
	SetPosition( D3DXVECTOR2(l,t), D3DXVECTOR2(r,b) );
}


inline void C2DTexRect::SetPositionLTRB( int l, int t, int r, int b )
{
	SetPositionLTRB( (float)l, (float)t, (float)r, (float)b );
}


inline void C2DTexRect::Translate( float dx, float dy )
{
	m_avRectVertex[0].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[1].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[2].vPosition += D3DXVECTOR3( dx, dy, 0 );
	m_avRectVertex[3].vPosition += D3DXVECTOR3( dx, dy, 0 );
}


inline void C2DTexRect::Translate( int dx, int dy )
{
	Translate( (float)dx, (float)dy );
}


inline void C2DTexRect::Rotate( float angle )
{
	Rotate( Matrix22Rotation(angle) );
}


inline void C2DTexRect::Rotate( const Matrix22& matOrient )
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



#endif		/*  __2DTEXRECT_H__  */
