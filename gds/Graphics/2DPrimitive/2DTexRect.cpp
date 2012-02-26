#include "2DTexRect.hpp"


void C2DTexRect::SetDefault()
{
	ZeroMemory(m_avRectVertex, sizeof(TLVERTEX2) * 4);
	for(int i=0; i<4; i++)
	{
		m_avRectVertex[i].rhw = 1.0f;
		m_avRectVertex[i].color = 0xff000000;		//opaque by default
	}

	m_DestAlphaBlend = D3DBLEND_INVSRCALPHA;

	for(int i=0; i<2; i++)
	{
		TLVERTEX2 *pVert = m_avRectVertex;
		pVert->tex[i] = TEXCOORD2(0,0);
		pVert->tex[i] = TEXCOORD2(1,0);
		pVert->tex[i] = TEXCOORD2(1,1);
		pVert->tex[i] = TEXCOORD2(0,1);
	}
	
}


C2DTexRect::~C2DTexRect()
{
}


void C2DTexRect::Release()
{
}


/// draws the rect with render state settings
/// if the rect has a texture, or is handed one as the argument
/// it will be rendered with the texture
/// alpha blend is enabled. The user can set the blend mode
/// by calling SetDestAlphaBlendMode()
/// the default blend mode is D3DBLEND_INVSRCALPHA
void C2DTexRect::Draw( const LPDIRECT3DTEXTURE9 pTexture0,
					   const LPDIRECT3DTEXTURE9 pTexture1 )
{
	SetRenderStates();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTexture( 0, pTexture0 );
	pd3dDev->SetTexture( 1, pTexture1 );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLORARG2,	D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 2, D3DTSS_COLOROP,	D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAARG1,	D3DTA_CURRENT );
	pd3dDev->SetTextureStageState( 2, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

	DrawRect();
}


void C2DTexRect::Draw( const LPDIRECT3DTEXTURE9 pTexture )
{
	SetRenderStates();

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetTexture( 0, pTexture );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
//		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	DrawRect();
}


void C2DTexRect::Draw()
{
	SetRenderStates();
	DrawRect();
}


void C2DTexRect::SetTextureUV( const TEXCOORD2& rvMin, const TEXCOORD2& rvMax)
{
	TLVERTEX2* pav2DTexRect = m_avRectVertex;

	for( int i=0; i<2; i++ )
	{
		pav2DTexRect[0].tex[i].u = rvMin.u;		// top-left corner of the rectangle
		pav2DTexRect[0].tex[i].v = rvMin.v;

		pav2DTexRect[1].tex[i].u = rvMax.u;
		pav2DTexRect[1].tex[i].v = rvMin.v;

		pav2DTexRect[2].tex[i].u = rvMax.u;		// bottom-right corner of the rectangle
		pav2DTexRect[2].tex[i].v = rvMax.v;

		pav2DTexRect[3].tex[i].u = rvMin.u;
		pav2DTexRect[3].tex[i].v = rvMax.v;
	}
}


void C2DTexRect::SetZDepth(float fZValue)
{
	m_avRectVertex[0].vPosition.z = fZValue;
	m_avRectVertex[1].vPosition.z = fZValue;
	m_avRectVertex[2].vPosition.z = fZValue;
	m_avRectVertex[3].vPosition.z = fZValue;
}


void C2DTexRect::SetColor(DWORD dwColor, int iVertexNum)
{
	if( iVertexNum == 4 )
	{	// set the color for all the 4 vertices at once
		for(int i=0; i<4; i++)
			m_avRectVertex[i].color   = dwColor;
	}
	else if( 0 <= iVertexNum && iVertexNum < 4 )
	{	// set the color for only one vertex specified by 'iVertexNum'
		m_avRectVertex[iVertexNum].color   = dwColor;
	}
}


void C2DTexRect::ScalePosition( float fScale )
{
	int i;
	for(i=0; i<4; i++)
		m_avRectVertex[i].vPosition *= fScale;

}
