#ifndef  __3DRECT_H__
#define  __3DRECT_H__

#include <d3d9.h>
#include <d3dx9.h>

#include "FVF_NormalVertex.h"

#include "Graphics/Direct3D9.h"


class C3DRect
{
	NORMALVERTEX m_av3DRectVertex[4];

public:

	inline C3DRect() {}

	inline ~C3DRect() {}

//	void Release();

//	void LoadTextureFromFile(char* pTextureFilename);

//	void SetPosition(D3DXVECTOR3* pvRectPosition,
//					 D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj);

	inline void SetPosition( D3DXVECTOR3* pavVertex );

	inline void SetTextureUV( D3DXVECTOR2& rvMin, D3DXVECTOR2& rvMax );

	inline void SetColor( DWORD dwColor, int vert_index = 4 );	//'dwColor' is ARGB format

	inline void SetPosition( int iVertex, const D3DXVECTOR3& rvPosition );

	inline void ScalePosition( float fScale );

	inline void SetNormal( D3DXVECTOR3* pavNormal );

//	D3DXVECTOR2 GetPosition( int iVertex ) { return m_avRectVertex[iVertex]; }

	void Draw();

//	void DrawWireframe();
};


inline void C3DRect::SetPosition( D3DXVECTOR3* pavVertex )
{
	NORMALVERTEX* pav3DRect = m_av3DRectVertex;

	for( int i=0; i<4; i++ )
		pav3DRect[i].vPosition = pavVertex[i];
}


inline void C3DRect::SetColor( DWORD dwColor, int vert_index )
{
	if( vert_index == 4 )
	{	// set the color for all the 4 vertices at once
		for(int i=0; i<4; i++)
			m_av3DRectVertex[i].color = dwColor;
	}
	else if( 0 <= vert_index && vert_index < 4 )
	{	// set the color for only one vertex specified by 'iVertexNum'
		m_av3DRectVertex[vert_index].color = dwColor;
	}
}


inline void C3DRect::SetPosition( int vert_index, const D3DXVECTOR3& rvPosition )
{
	if( vert_index < 0 || 4 <= vert_index )
		return;

	m_av3DRectVertex[vert_index].vPosition = rvPosition;
}


inline void C3DRect::ScalePosition( float fScale )
{
	m_av3DRectVertex[0].vPosition *= fScale;
	m_av3DRectVertex[1].vPosition *= fScale;
	m_av3DRectVertex[2].vPosition *= fScale;
	m_av3DRectVertex[3].vPosition *= fScale;
}


inline void C3DRect::SetTextureUV(D3DXVECTOR2& rvMin, D3DXVECTOR2& rvMax)
{
	NORMALVERTEX* pav3DRect = m_av3DRectVertex;

	pav3DRect[0].tex.u = rvMin.x;		// top-left corner of the rectangle
	pav3DRect[0].tex.v = rvMin.y;

	pav3DRect[1].tex.u = rvMax.x;
	pav3DRect[1].tex.v = rvMin.y;

	pav3DRect[2].tex.u = rvMax.x;		// bottom-right corner of the rectangle
	pav3DRect[2].tex.v = rvMax.y;

	pav3DRect[3].tex.u = rvMin.x;
	pav3DRect[3].tex.v = rvMax.y;
}


inline void C3DRect::SetNormal( D3DXVECTOR3* pavNormal )
{
	NORMALVERTEX* pav3DRect = m_av3DRectVertex;

	for( int i=0; i<4; i++ )
		pav3DRect[i].vNormal = pavNormal[i];
}


inline void C3DRect::Draw()
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
	HRESULT hr ;

	pd3dDev->SetFVF( NORMALVERTEX::FVF );

	hr = pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_av3DRectVertex, sizeof(NORMALVERTEX) );

	if( FAILED(hr) )
		MessageBox(NULL, "DrawPrimUP failed.", "Error", MB_OK);
}


#endif		/*  __3DRECT_H__  */
