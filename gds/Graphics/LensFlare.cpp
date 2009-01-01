
#include "LensFlare.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/D3DMisc.h"
#include "Graphics/Shader/ShaderManager.h"

using namespace std;


CLensFlareGroup::CLensFlareGroup()
:
m_NumTextureSegmentsX(0),
m_NumTextureSegmentsY(0)
{
	m_RectGroup.SetDestAlphaBlendMode( AlphaBlend::One );
}



CLensFlare::CLensFlare()
:
m_vLightPosition( Vector3(1,1,1) )
{
}


CLensFlare::~CLensFlare()
{
	Release();
}


void CLensFlare::Release()
{
	m_vecLensFlareGroup.resize( 0 );
}


void CLensFlare::UpdateLensFlares()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// update rect positions
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply( &matViewProj, &m_matView, &m_matProj );

	D3DXVECTOR3 vLightPosSS;	// light position in screen space
	D3DXVec3TransformCoord( &vLightPosSS, &m_vLightPosition, &matViewProj );

	const Vector3 vCenter = Vector3(0,0,0);

	Vector3 vCenterToLight = vLightPosSS - vCenter;

	float fDistToLightSS = Vec3Length( vCenterToLight );

	Vector3 vDirToLight = vCenterToLight / fDistToLightSS;

	int vert_index = 0;
	float r, fDistFactor, fScaleFactor;
	const size_t num_groups = m_vecLensFlareGroup.size();

	float screen_width, screen_height;
//	screen_width  = (float)m_ScreenWidth;
//	screen_height = (float)m_ScreenHeight;
	GetViewportSize( screen_width, screen_height );

	for( size_t i=0; i<num_groups; i++ )
	{
		CLensFlareGroup& group = m_vecLensFlareGroup[i];
	
		const size_t num_flares = group.m_vecComponent.size();
		for( size_t j=0; j<num_flares; j++ )
		{
			const CLensFlareComponent& rFlare = group.m_vecComponent[j];

			fDistFactor  = rFlare.m_fDistFactor;
			fScaleFactor = rFlare.m_fScaleFactor;
			r = rFlare.m_fRadius * fScaleFactor;

			// adopt screen width to scale radius?
			float radius = r * screen_width * 0.5f;

			Vector2 proj_pos, pos;

			// convert pos in projection space to screen space
			// this will change the range of the value from [-1.0, 1.0] to [0.0, 1.0]
			proj_pos.x = vCenterToLight.x * fDistFactor;
			proj_pos.y = vCenterToLight.y * fDistFactor;
	//		proj_pos.x = vLightPosSS.x;
	//		proj_pos.y = vLightPosSS.y;
			pos.x = ( proj_pos.x + 1.0f) * 0.5f;
			pos.y = (-proj_pos.y + 1.0f) * 0.5f;

			Vector2 screen_pos;
			screen_pos.x = pos.x * screen_width;
			screen_pos.y = pos.y * screen_height;

			group.m_RectGroup.SetRectMinMax( 
				(int)j,
				screen_pos - Vector2(radius,radius),
				screen_pos + Vector2(radius,radius)
				);
		}
	}
}

/*
void CLensFlare::Render()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDev->SetFVF( TLVERTEX::FVF );

	const int num_flares = (int)m_vecFlareComponent.size();
	for( int i=0; i<num_flares; i++ )
	{
		int tex_index = m_vecFlareComponent[i].m_TexIndex;

		if( tex_index < 0 )
			continue;

		pd3dDev->SetTexture( 0, m_vecLensFlareGroup[tex_index].m_Texture.GetTexture() );
//		pd3dDev->SetTexture( 0, NULL );

		pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avVertex + i * 4, sizeof(TLVERTEX) );
	}

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}
*/


void CLensFlare::Render( CShaderManager& rShaderManager, int texture_stage )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pd3dDev->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetPixelShader( NULL );

	UINT pass, cPasses;

//	LPD3DXEFFECT pEffect = rShaderManager.GetEffect();

//	pEffect->Begin( &cPasses, 0 );

	const size_t num_groups = m_vecLensFlareGroup.size();
	for( size_t i=0; i<num_groups; i++ )
	{
		CLensFlareGroup& group = m_vecLensFlareGroup[i];

//		rShaderManager.SetTexture( texture_stage, group.m_Texture );

		group.m_RectGroup.Draw( group.m_Texture );

/*		for( pass=0; pass<cPasses; pass++ )
		{
			pEffect->BeginPass( pass );

//			group.m_RectGroup.Draw( group.m_Texture ); // Not rendered
			group.m_RectGroup.draw(); // Not rendered

			pEffect->EndPass();
		}*/
	}

//	pEffect->End();

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}


bool CLensFlare::AddTexture( const std::string& texture_filename, int group_index, int num_segments_x, int num_segments_y )
{
	while( (int)m_vecLensFlareGroup.size() <= group_index )
	{
		m_vecLensFlareGroup.push_back( CLensFlareGroup() );
	}

	CLensFlareGroup& rDestGroup = m_vecLensFlareGroup[group_index];

	rDestGroup.m_NumTextureSegmentsX = num_segments_x;
	rDestGroup.m_NumTextureSegmentsY = num_segments_y;

	return rDestGroup.m_Texture.Load( texture_filename );
}


void CLensFlare::AddLensFlareRect( float dim,
								   float scale_factor,
								   float dist_factor,
								   U32 color,
								   int group_index,
								   int tex_seg_index_x,
								   int tex_seg_index_y )
{
	while( (int)m_vecLensFlareGroup.size() <= group_index )
		m_vecLensFlareGroup.push_back( CLensFlareGroup() );

	CLensFlareGroup& rDestGroup = m_vecLensFlareGroup[group_index];

	CLensFlareComponent flare;

	flare.m_Color.SetARGB32( color );
	flare.m_fDistFactor	 = dist_factor;
	flare.m_fScaleFactor = scale_factor;
	flare.m_fRadius		 = dim;

	const int num_flares = (int)rDestGroup.m_vecComponent.size();

	rDestGroup.m_vecComponent.push_back( flare );

	rDestGroup.m_RectGroup.AddRects( 1 );

	// TODO: support packed texture
	rDestGroup.m_RectGroup.SetTextureCoordMinMax( num_flares, 0,0, 1,1 );

	// vertex colors
	rDestGroup.m_RectGroup.SetRectColor( num_flares, color );
}
