#include "3DRect.hpp"
#include "Graphics/Direct3D9.hpp"


namespace amorphous
{


/*
C3DRect::C3DRect()
{
	m_pTexture = NULL;
	ZeroMemory(m_avRectVertex, sizeof(TLVERTEX) * 4);
	ZeroMemory(m_av3DRectVertex, sizeof(TEXTUREVERTEX) * 4);
	for(int i=0; i<4; i++)
	{
		m_avRectVertex[i].rhw = 1.0f;
		m_avRectVertex[i].color = 0xff000000;		//opaque by default

		m_av3DRectVertex[i].color = 0xff000000;		//opaque by default
	}

	m_iRectType = RECTTYPE_2D;
	m_iDestAlphaBlend = D3DBLEND_INVSRCALPHA;
}*/

/*
C3DRect::~C3DRect()
{
	SAFE_RELEASE( m_pTexture )
}

void C3DRect::Release()
{
	SAFE_RELEASE( m_pTexture )
}




void C3DRect::DrawWireframe()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// enable alpha blending
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//  pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_iDestAlphaBlend );

    pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	//pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	// use only the vertex color & alpha
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

	// draw a rectangle
	pd3dDev->SetVertexShader( NULL );

	pd3dDev->SetFVF( D3DFVF_TLVERTEX );

	// draw the first three lines
	pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3,
		                           m_avRectVertex, sizeof(TLVERTEX) );

	// draw the last fourth line
	TLVERTEX avLine[2] = { m_avRectVertex[3], m_avRectVertex[0] };
	pd3dDev->DrawPrimitiveUP( D3DPT_LINESTRIP, 1,
		                           avLine, sizeof(TLVERTEX) );
}*/

/*
void C3DRect::LoadTextureFromFile(char* pTextureFilename)
{
	// Create the texture
	HRESULT hr;
	hr = D3DXCreateTextureFromFile( DIRECT3D9.GetDevice(), pTextureFilename, &m_pTexture );
    if( FAILED( hr ) )
        m_pTexture = NULL;
}*/


/*
void C3DRect::SetPosition(D3DXVECTOR3* pvRectPosition,
						  D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj)
{

	for(int i=0; i<4; i++)
	{
		m_avRectVertex[i].vPosition = pvRectPosition[i];
		//m_avRectVertex[i].rhw = ???;

		if(pMatView)
			D3DXVec3TransformCoord(&m_avRectVertex[i].vPosition,
			&m_avRectVertex[i].vPosition, pMatView);

		if(pMatProj)
		{
			D3DXVec3TransformCoord(&m_avRectVertex[i].vPosition,
			&m_avRectVertex[i].vPosition, pMatProj);
			int screen_w = 800, screen_h = 600;
			(m_avRectVertex[i].vPosition.x)
			=(  m_avRectVertex[i].vPosition.x + 1.0f ) * (float)(screen_w/2);
			(m_avRectVertex[i].vPosition.y)
			=( -m_avRectVertex[i].vPosition.y + 1.0f ) * (float)(screen_h/2);

			m_avRectVertex[i].vPosition.z = 0.0f;

		}

	}

}*/

} // amorphous
