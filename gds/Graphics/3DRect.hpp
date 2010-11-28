#ifndef  __3DRect_HPP__
#define  __3DRect_HPP__


#include "../base.hpp"
#include "../3DMath/Vector3.hpp"
#include "General3DVertex.hpp"
#include "Direct3D/Direct3D9.hpp"
#include "Direct3D/Conversions.hpp"
#include "FVF_NormalVertex.h"


class C3DRect
{
	CGeneral3DVertex m_RectVertices[4];

//	U32 m_VertexFlags;

public:

	inline C3DRect() {}//: m_VertexFlags(0) {}

	inline ~C3DRect() {}

//	void Release();

//	void LoadTextureFromFile(char* pTextureFilename);

	inline void SetPositions( Vector3* pavVertex );

	inline void SetTextureUV( TEXCOORD2& rvMin, TEXCOORD2& rvMax );

	inline void SetColor( SFloatRGBAColor color, int vert_index = 4 );	//'dwColor' is ARGB format

	inline void SetPosition( int iVertex, const Vector3& rvPosition );

	inline void ScalePosition( float fScale );

	inline void SetNormal( const Vector3& vNormal );

	inline void SetNormals( Vector3* pavNormal );

	// Draw the rectangle with the current shader settings
	// - Does not switch to fixed function pipeline.
	void draw() const;

	void Draw() const;

//	void DrawWireframe();
};


inline void C3DRect::SetPositions( Vector3* pavVertex )
{
	CGeneral3DVertex *pav3DRect = m_RectVertices;

	for( int i=0; i<4; i++ )
		pav3DRect[i].m_vPosition = pavVertex[i];
}


inline void C3DRect::SetColor( SFloatRGBAColor color, int vert_index )
{
	if( vert_index == 4 )
	{	// set the color for all the 4 vertices at once
		for(int i=0; i<4; i++)
			m_RectVertices[i].m_DiffuseColor = color;
	}
	else if( 0 <= vert_index && vert_index < 4 )
	{	// set the color for only one vertex specified by 'iVertexNum'
		m_RectVertices[vert_index].m_DiffuseColor = color;
	}
}


inline void C3DRect::SetPosition( int vert_index, const Vector3& rvPosition )
{
	if( vert_index < 0 || 4 <= vert_index )
		return;

	m_RectVertices[vert_index].m_vPosition = rvPosition;
}


inline void C3DRect::ScalePosition( float fScale )
{
	m_RectVertices[0].m_vPosition *= fScale;
	m_RectVertices[1].m_vPosition *= fScale;
	m_RectVertices[2].m_vPosition *= fScale;
	m_RectVertices[3].m_vPosition *= fScale;
}


inline void C3DRect::SetTextureUV(TEXCOORD2& rvMin, TEXCOORD2& rvMax)
{
	CGeneral3DVertex *pav3DRect = m_RectVertices;

	pav3DRect[0].m_TextureCoord[0].u = rvMin.u;		// top-left corner of the rectangle
	pav3DRect[0].m_TextureCoord[0].v = rvMin.v;

	pav3DRect[1].m_TextureCoord[0].u = rvMax.u;
	pav3DRect[1].m_TextureCoord[0].v = rvMin.v;

	pav3DRect[2].m_TextureCoord[0].u = rvMax.u;		// bottom-right corner of the rectangle
	pav3DRect[2].m_TextureCoord[0].v = rvMax.v;

	pav3DRect[3].m_TextureCoord[0].u = rvMin.u;
	pav3DRect[3].m_TextureCoord[0].v = rvMax.v;
}


inline void C3DRect::SetNormal( const Vector3& vNormal )
{
	CGeneral3DVertex *pav3DRect = m_RectVertices;

	for( int i=0; i<4; i++ )
		pav3DRect[i].m_vNormal = vNormal;
}


inline void C3DRect::SetNormals( Vector3* pavNormal )
{
	CGeneral3DVertex *pav3DRect = m_RectVertices;

	for( int i=0; i<4; i++ )
		pav3DRect[i].m_vNormal = pavNormal[i];
}


inline void C3DRect::draw() const
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
//	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//  pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//  pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_iDestAlphaBlend );

//    pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	//pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

/*
	// set the texture if loaded
	if(m_pTexture)
	{
		pd3dDev->SetTexture( 0, m_pTexture );

		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	}
	else
	{	// use only the vertex color & alpha
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	}
*/
	// draw a rectangle
	HRESULT hr = S_OK;

	NORMALVERTEX verts[4];
	for( int i=0; i<4; i++ )
	{
		verts[i].vPosition = ToD3DXVECTOR3( m_RectVertices[i].m_vPosition );
		verts[i].vNormal   = ToD3DXVECTOR3( m_RectVertices[i].m_vNormal );
		verts[i].color     = m_RectVertices[i].m_DiffuseColor.GetARGB32();
		verts[i].tex       = m_RectVertices[i].m_TextureCoord[0];
	}

	hr = pd3dDev->SetFVF( NORMALVERTEX::FVF );

	hr = pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_RectVertices, sizeof(NORMALVERTEX) );

	if( FAILED(hr) )
		MessageBox( NULL, "DrawPrimUP failed.", "Error", MB_OK );
}


inline void C3DRect::Draw() const
{
	DIRECT3D9.GetDevice()->SetVertexShader( NULL );
	DIRECT3D9.GetDevice()->SetPixelShader( NULL );

	draw();
}


#endif		/*  __3DRect_HPP__  */
