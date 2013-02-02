#include "2DRect.hpp"


namespace amorphous
{


C2DRect::~C2DRect()
{
}


void C2DRect::Release()
{
}


void C2DRect::DrawWireframe()
{
/*	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//  pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_DestAlphaBlend );

    pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	//pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	// use only the vertex color & alpha
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

//	C2DPrimitiveRenderer_D3D renderer;
//	renderer.Render( m_avRectVertex, 4, PrimitiveType::LINE_STRIP );
*/

/*
	// draw a rectangle
//	pd3dDev->SetVertexShader( NULL );

	pd3dDev->SetFVF( D3DFVF_TLVERTEX );

	HRESULT hr;

	// draw the first three lines
	hr = pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, m_avRectVertex, sizeof(TLVERTEX) );

	if( FAILED(hr) )
		int iError = 1;


	// draw the last fourth line
	TLVERTEX avLine[2] = { m_avRectVertex[3], m_avRectVertex[0] };
	pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, avLine, sizeof(TLVERTEX) );
*/
}


void C2DRect::SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax/*, int index */)
{
	int index = 0;

	General2DVertex* paVertex= m_avRectVertex;

	paVertex[0].m_TextureCoord[index].u = rvMin.u;		// top-left corner of the rectangle
	paVertex[0].m_TextureCoord[index].v = rvMin.v;

	paVertex[1].m_TextureCoord[index].u = rvMax.u;
	paVertex[1].m_TextureCoord[index].v = rvMin.v;

	paVertex[2].m_TextureCoord[index].u = rvMax.u;		// bottom-right corner of the rectangle
	paVertex[2].m_TextureCoord[index].v = rvMax.v;

	paVertex[3].m_TextureCoord[index].u = rvMin.u;
	paVertex[3].m_TextureCoord[index].v = rvMax.v;
}


void C2DRect::SetZDepth( float fZValue )
{
	m_avRectVertex[0].m_vPosition.z = fZValue;
	m_avRectVertex[1].m_vPosition.z = fZValue;
	m_avRectVertex[2].m_vPosition.z = fZValue;
	m_avRectVertex[3].m_vPosition.z = fZValue;
}


void C2DRect::ScalePosition( float fScale )
{
	int i;
	for(i=0; i<4; i++)
		m_avRectVertex[i].m_vPosition *= fScale;

}


} // namespace amorphous
