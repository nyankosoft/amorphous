
#include "Skybox.h"

CSkybox::CSkybox()
{
	m_iNumPolygons = 0;
}

CSkybox::~CSkybox()
{
}


void CSkybox::Render( D3DXVECTOR3& rvOffset )
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	matWorld._41 = rvOffset.x;
	matWorld._42 = rvOffset.y;
	matWorld._43 = rvOffset.z;
	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	for(int i=0; i<m_iNumPolygons; i++)
		m_aSkyboxPolygon[i].Draw( pd3dDev );
}


/*
void CSkybox::Render_Effect( D3DXVECTOR3& rvOffset )
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	matWorld._41 = rvOffset.x;
	matWorld._42 = rvOffset.y;
	matWorld._43 = rvOffset.z;
	CDIRECTX9.m_pEffect->SetMatrix( "World", &matWorld );

	for(int i=0; i<m_iNumPolygons; i++)
	{
		SPolygon& polygon = m_aSkyboxPolygon[i];
		// texture setting
//	  if(0 <= sTextureID && s_paTexture[ sTextureID ].pTexture)
//	  {
			CDIRECTX9.m_pEffect->SetTexture( "Texture0", SPolygon::s_paTexture[ polygon.sTextureID ].pTexture );
//	  }

		DIRECT3D9.GetDevice()->DrawPrimitive(D3DPT_TRIANGLEFAN, polygon.sIndexOffset, polygon.sNumVertices - 2);
		
	}
}
*/


void CSkybox::SetSkyboxPolygons( CSG_Polygon* paPolygon, int iNumPolygons )
{
	if( MAX_SKYBOXPOLYGONS < iNumPolygons )
		iNumPolygons = MAX_SKYBOXPOLYGONS;

	memcpy( this->m_aSkyboxPolygon, paPolygon, sizeof(CSG_Polygon) * iNumPolygons );
	this->m_iNumPolygons = iNumPolygons;
}
