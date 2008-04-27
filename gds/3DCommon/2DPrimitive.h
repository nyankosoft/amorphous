#ifndef  __2DPrimitive_H__
#define  __2DPrimitive_H__


#include "../base.h"
#include "Direct3D9.h"
#include "FVF_TLVertex.h"
#include "TextureCoord.h"
#include "AlphaBlend.h"
#include "TextureHandle.h"
#include "FloatRGBAColor.h"

#include "3DMath/Vector2.h"
#include "3DMath/Matrix22.h"
#include "3DMath/Vector3.h"

#include "Graphics/Rect.h"
using namespace::Graphics;


/**
 * hold a 2D primitive
 * - About corner indexing of rectangular primitives:
 *   - indexing for vertices is done in clockwise order: [0,3] > top-left, top-right, bottom-right, bottom-left
 */
class C2DPrimitive
{
protected:

	U32 m_DestAlphaBlend;

protected:

	inline void SetBasicRenderStates();

	/// set render states to draw without texture
	inline void SetStdRenderStates();

	/// set render states to draw with texture
	inline void SetStdRenderStatesForTexture( const CTextureHandle& texture );

public:

	inline C2DPrimitive() : m_DestAlphaBlend(D3DBLEND_INVSRCALPHA) { /*SetDefault();*/ }

	virtual ~C2DPrimitive() {}

	enum ePrimitiveType
	{
		TYPE_TRIANGLE,
		TYPE_RECT,
		TYPE_FRAMERECT,
		TYPE_ROUNDRECT,
		TYPE_ROUNDFRAMERECT,
		TYPE_RECTSET,
		NUM_PRIMITIVE_TYPES
	};

	virtual int unsigned GetPrimitiveType() const = 0;

//	virtual void SetDefault() {}

	/// draw rect without any render state changes
	inline virtual void draw() {}

	inline virtual void Draw();

	inline virtual void Draw( const CTextureHandle& texture );

	inline virtual Vector2 GetPosition2D( int vert_index ) const { return Vector2(0,0); }

	/// set the position of the rect through top-left and bottom-right corner positions
	virtual void SetPosition( const Vector2& vMin, const Vector2& vMax) {}

	virtual void SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax ) {}

	virtual void SetColor( const SFloatRGBAColor& color ) {}

	virtual void SetCornerColor( int corner, const SFloatRGBAColor& color ) {}

	/// \param color 32-bit ARGB color with 8bits for each component (NOTE: bit order. alpha component fist)
	inline void SetColor( U32 color );

	inline void SetCornerColor( int corner, U32 color );

	virtual void SetZDepth(float fZValue) {}

//	void ScalePosition( float fScale );

	virtual void Translate( float dx, float dy ) {}
	virtual void Translate( int dx, int dy ) {}

	virtual void Rotate( float angle ) {}

	virtual void Rotate( const Matrix22& matOrient ) {}

	inline void SetDestAlphaBlendMode( AlphaBlend::Mode dest_blend_mode ) { m_DestAlphaBlend = g_dwD3DBlendMode[dest_blend_mode]; }
};

// =================================== inline implementations =================================== 



inline void C2DPrimitive::SetBasicRenderStates()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_DestAlphaBlend );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
//	pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );
	pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );
}



inline void C2DPrimitive::SetStdRenderStates()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// no texture
	// - use only the vertex color & alpha
	pd3dDev->SetTexture( 0, NULL );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}


inline void C2DPrimitive::SetStdRenderStatesForTexture( const CTextureHandle& texture )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTexture( 0, texture.GetTexture() );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
}


inline void C2DPrimitive::Draw()
{
	SetBasicRenderStates();

	SetStdRenderStates();

	draw();
}


inline void C2DPrimitive::Draw( const CTextureHandle& texture )
{
	SetBasicRenderStates();

	SetStdRenderStatesForTexture( texture );

	draw();
}


inline void C2DPrimitive::SetColor( U32 color )
{
	SFloatRGBAColor c;
	c.SetARGB32(color);
	SetColor( c );
}


inline void C2DPrimitive::SetCornerColor( int corner, U32 color )
{
	SFloatRGBAColor c;
	c.SetARGB32(color);
	SetCornerColor( corner, c );
}


#endif		/*  __2DPrimitive_H__  */
