
#include "BEC_Billboard.h"

#include "Stage.h"
#include "ScreenEffectManager.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/RectTriListIndex.h"

#include "Support/memory_helpers.h"
#include "Support/Profile.h"

#include <string>
using namespace std;


CBEC_Billboard::CBEC_Billboard()
:
m_DestAlphaBlendMode(D3DBLEND_INVSRCALPHA),
m_Type(TYPE_BILLBOARDARRAYMESH),
m_pBillboardArrayMesh(NULL)
{
	m_aShaderTechHandle[0].SetTechniqueName( "Particle" );
	m_aShaderTechHandle[1].SetTechniqueName( "Particle_AdditiveAlpha" );
}


CBEC_Billboard::~CBEC_Billboard()
{
	ReleaseGraphicsComponentResources();
}


void CBEC_Billboard::LoadBillboardArrayMesh( float billboard_radius,
											 int num_groups,
											 int num_billboards_par_groups,
											 int num_tex_divisions )
{
	SafeDelete( m_pBillboardArrayMesh );

    CBillboardArrayMeshGenerator mesh_generator;

	if( m_Type == TYPE_BILLBOARDARRAYMESH )
        mesh_generator.GenerateForMultipleParticleGroups( num_billboards_par_groups, num_groups, billboard_radius, num_tex_divisions );
	else if( m_Type == TYPE_BILLBOARDARRAYMESH_SHARED )
        mesh_generator.Generate( num_billboards_par_groups * num_groups, billboard_radius, num_tex_divisions );
	else
		return;

	m_BillboardArrayMeshArchive = mesh_generator.GetMeshArchive();

	m_pBillboardArrayMesh = new CBillboardArrayMesh();
	bool loaded = m_pBillboardArrayMesh->LoadFromArchive( m_BillboardArrayMeshArchive, "BillboardArrayMesh", 0 );
}


void CBEC_Billboard::Init()
{
	bool b = m_BillboardTexture.Load();

//	if( b )
//		MessageBox( NULL, m_strTextureFilename.c_str(), "texture loaded", MB_OK|MB_ICONWARNING );

/*	HRESULT hr;
	switch( m_Type )
	{
	case TYPE_BILLBOARDARRAYMESH:
	case TYPE_BILLBOARDARRAYMESH_SHARED:
		{
		}
		break;

	case TYPE_RECT_ARRAY_AND_INDICES:
		{
		}
		break;

	default:
		break;
	}*/
}


void CBEC_Billboard::Release()
{
//	ReleaseGraphicsResources();
}


inline void CBEC_Billboard::DrawPrimitives( int num_rects, int group, int num_rects_per_group )
{
	switch( m_Type )
	{
	case TYPE_BILLBOARDARRAYMESH:
		{
			LPD3DXMESH pMesh = m_pBillboardArrayMesh->GetMesh();
//			D3DXATTRIBUTERANGE attrib_range;
//			attrib_range.AttribId    = group;
//			attrib_range.FaceStart   = group * num_rects_per_group * 2;
//			attrib_range.FaceCount   = num_rects * 2;
//			attrib_range.VertexStart = group * num_rects_per_group * 4;
//			attrib_range.VertexCount = num_rects * 4;
//			HRESULT hr = pMesh->SetAttributeTable( &attrib_range, 1 );

			ProfileBegin( "CBEC_Billboard::DrawPrims() - pMesh->DrawSubset()" );

			pMesh->DrawSubset( group );

			ProfileEnd( "CBEC_Billboard::DrawPrims() - pMesh->DrawSubset()" );
		}
		break;

	case TYPE_BILLBOARDARRAYMESH_SHARED:
		{
			LPD3DXMESH pMesh = m_pBillboardArrayMesh->GetMesh();
			D3DXATTRIBUTERANGE attrib_range;
			attrib_range.AttribId    = 0;
			attrib_range.FaceStart   = 0;
			attrib_range.FaceCount   = num_rects * 2;
			attrib_range.VertexStart = 0;
			attrib_range.VertexCount = num_rects * 4;
			HRESULT hr = pMesh->SetAttributeTable( &attrib_range, 1 );

			pMesh->DrawSubset( 0 );
		}
		break;

	case TYPE_RECT_ARRAY_AND_INDICES:
		// draw particles
		DIRECT3D9.GetDevice()->DrawIndexedPrimitiveUP(
										D3DPT_TRIANGLELIST,
										0,
										num_rects * 4,
										num_rects * 2,
										s_RectTriListIndex,
	//									m_awIndex,
										D3DFMT_INDEX16,
	//									m_avBillboardRect,
										m_avBillboardRect_S,
	//									sizeof(TEXTUREVERTEX) );
										sizeof(BILLBOARDVERTEX)
										);
        break;

	default:
		break;
	}
}


void CBEC_Billboard::DrawBillboards( int num_rects, int group, int num_rects_per_group, CStage *pStage )
{
	ProfileBegin( "CBEC_Billboard::DrawBillboards()" );

	if( NUM_MAX_BILLBOARDS_PER_ENTITY < num_rects )
	{
		MessageBox( NULL, "too many billboards", "error", MB_OK|MB_ICONWARNING );
		return;
	}

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	pd3dDev->SetFVF( TEXTUREVERTEX::FVF );
	pd3dDev->SetFVF( BILLBOARDVERTEX::FVF );

	// don't wirte to z-buffer so that smoke should be painted on one another
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );

	CShaderManager *pShaderManager = NULL;
	LPD3DXEFFECT pEffect = NULL;
	UINT pass, cPasses;
	if( (pShaderManager = CShader::Get()->GetCurrentShaderManager()) &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		pShaderManager->SetWorldTransform( matWorld );
		pShaderManager->SetTexture( 0, m_BillboardTexture.GetTexture() );

		int tech;
		switch( m_DestAlphaBlendMode )
		{
		case D3DBLEND_INVSRCALPHA:	tech = 0;	break;
		case D3DBLEND_ONE:			tech = 1;	break;
		default:					tech = 1;	break;
		}

		if( pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PseudoNightVision )
		{
//			pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD_PNV );
			pShaderManager->SetTechnique( m_aShaderTechHandle[tech] );
			pEffect->CommitChanges();
		}
		else
		{
//			pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD );
			pShaderManager->SetTechnique( m_aShaderTechHandle[tech] );
			pEffect->CommitChanges();
		}

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_DestAlphaBlendMode );

		pEffect->Begin( &cPasses, 0 );

		for( pass=0; pass<cPasses; pass++ )
		{
			pEffect->BeginPass( pass );

			DrawPrimitives( num_rects, group, num_rects_per_group );

			pEffect->EndPass();
		}

		pEffect->End();
	}
	else
	{
		pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );
		pd3dDev->SetVertexShader( NULL );
		pd3dDev->SetTexture( 0, m_BillboardTexture.GetTexture() );

		// use the texture color only
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_DestAlphaBlendMode );

		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
		pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		// draw particles
		pd3dDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST,
										0,
										num_rects * 4,
										num_rects * 2,
										s_RectTriListIndex,
//										m_awIndex,
										D3DFMT_INDEX16,
										m_avBillboardRect,
										sizeof(TEXTUREVERTEX) );
	}

	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	ProfileEnd( "CBEC_Billboard::DrawBillboards()" );
}


bool CBEC_Billboard::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string blend_mode, type;

	if( scanner.TryScanLine( "TEXTURE", m_BillboardTexture.filename ) ) return true;

	if( scanner.TryScanLine( "BILLBOARD_TYPE", type ) )
	{
		if( type == "SEPARATE_VERTEX_STORAGE" ) { m_Type = CBEC_Billboard::TYPE_BILLBOARDARRAYMESH; return true; }
		else if( type == "SHARED_VERTEX_STORAGE" ) { m_Type = CBEC_Billboard::TYPE_BILLBOARDARRAYMESH_SHARED; return true; }
		else if( type == "SHARED_VERTEX_STORAGE_WITHOUT_VB" ) { m_Type = CBEC_Billboard::TYPE_RECT_ARRAY_AND_INDICES; return true; }
		else return false;
	}

	if( scanner.TryScanLine( "ALPHABLEND", blend_mode ) )
	{
		if( blend_mode == "INVSRCALPHA" )	m_DestAlphaBlendMode = D3DBLEND_INVSRCALPHA;
		else if( blend_mode == "ONE")		m_DestAlphaBlendMode = D3DBLEND_ONE;
		else								return false;

		return true;
	}

	return false;
}


void CBEC_Billboard::SerializeBillboardProperty( IArchive& ar, const unsigned int version )
{
	ar & m_Type;
	ar & m_BillboardTexture.filename;
	ar & m_DestAlphaBlendMode;
}



void CBEC_Billboard::LoadGraphicsComponentResources( const CGraphicsParameters& rParam )
{
	m_BillboardTexture.Load();

	m_pBillboardArrayMesh = new CBillboardArrayMesh();
	bool loaded = m_pBillboardArrayMesh->LoadFromArchive( m_BillboardArrayMeshArchive, "BillboardArrayMesh", 0 );
}


void CBEC_Billboard::ReleaseGraphicsComponentResources()
{
	m_BillboardTexture.Release();

	SafeDelete( m_pBillboardArrayMesh );
}
