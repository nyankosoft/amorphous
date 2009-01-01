#include "BE_StaticParticleSet.h"

#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "ScreenEffectManager.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/RectTriListIndex.h"
#include "Graphics/Shader/ShaderManager.h"
#include "GameCommon/MTRand.h"

#include "Support/memory_helpers.h"
#include "Support/msgbox.h"
#include "Support/Macro.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Log/DefaultLog.h"

using namespace std;


// num particles
// num_vertices  = num_particles * 4
// num_triangles = num_particles * 2
// num_indices   = num_triangles * 3 = num_particles * 6

inline static float& IndexOffset(CCopyEntity* pCopyEnt) { return pCopyEnt->f1; }
inline static float& NumParticles(CCopyEntity* pCopyEnt) { return pCopyEnt->f2; }


CBE_StaticParticleSet::CBE_StaticParticleSet()
:
m_pIB(NULL),
m_pVB(NULL)
{

	m_pParticleVertexDeclaration = NULL;

	m_aShaderTechHandle[0].SetTechniqueName( "StaticBillboard" );
}


CBE_StaticParticleSet::~CBE_StaticParticleSet()
{
	for( size_t i=0; i<m_vecpVB.size(); i++ )
        SAFE_RELEASE(m_vecpVB[i]);
	m_vecpVB.resize(0);

	SAFE_RELEASE( m_pVB );

	SAFE_RELEASE( m_pIB );

	SAFE_RELEASE( m_pParticleVertexDeclaration );
}


HRESULT CBE_StaticParticleSet::InitIndexBuffer()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	SAFE_RELEASE(m_pIB);

	// create the index buffer
	if( FAILED( pd3dDev->CreateIndexBuffer( sizeof(WORD) * NUM_MAX_PARTICLES_PER_VB * 6,
                                            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                            D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
	{
		MsgBox( "CBE_StaticParticleSet - cannot create index buffer" );
		return E_FAIL;
	}

	// set the index data. these indices remains constant during the game
	WORD awIndex[NUM_MAX_PARTICLES_PER_VB * 6];
	int i, iIndexOffset = 0;
	for( i=0; i<NUM_MAX_PARTICLES_PER_VB; i++ )	{
		// left upper triangle
		awIndex[i*6  ] = iIndexOffset;
		awIndex[i*6+1] = iIndexOffset + 1;
		awIndex[i*6+2] = iIndexOffset + 3;

		// right lower triangle
		awIndex[i*6+3] = iIndexOffset + 1;
		awIndex[i*6+4] = iIndexOffset + 2;
		awIndex[i*6+5] = iIndexOffset + 3;
		
		iIndexOffset += 4;
	}

	// get access to the index buffer
	VOID* pIndices;
	if( FAILED( m_pIB->Lock( 0,                 // Fill from start of the buffer
							 sizeof(WORD) * NUM_MAX_PARTICLES_PER_VB * 6, // Size of the data to load
							 &pIndices,  // Returned index data		// has to be (BYTE**)&pIndices?
							 0 ) ) )             // Send default flags to the lock
	{
		MsgBox( "CBE_StaticParticleSet - cannot lock index buffer" );
		SAFE_RELEASE(m_pIB);
		return E_FAIL;
	}


	// fill the index buffer
	memcpy( pIndices, awIndex, sizeof(awIndex) );

	m_pIB->Unlock();

    return S_OK;
}


void CBE_StaticParticleSet::Init()
{
	CBE_ParticleSet::Init();

	InitIndexBuffer();
	
	DIRECT3D9.GetDevice()->CreateVertexDeclaration( STATICBILLBOARDVERTEX_DECLARATION, &m_pParticleVertexDeclaration );
}


void CBE_StaticParticleSet::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	IndexOffset(pCopyEnt) = (float)( m_vecParticleVertex.size() / 4 * 6 );
	D3DXVECTOR3 vCenterPos = pCopyEnt->Position();

	STATICBILLBOARDVERTEX vert[4];
	float x,y,z;
	int i, j, num_particls_per_set = (int)(m_MaxNumParticlesPerSet * RangedRand(0.75f,0.95f));
//	rParticleSet.iNumParticles = num_particls_per_particles;
	float r = m_fParticleRadius;//m_fAverageParticleRadius;
	int num_tex_segs = m_iNumTextureSegments;
	int num_tex_patterns = num_tex_segs * num_tex_segs;
	float fTex = 1.0f / (float)num_tex_segs;	// TODO: random texture coord for particles
	float u,v;
	float factor_x, factor_y;
	int offset;
	AABB3 aabb;
	aabb.Nullify();

//	ONCE( MsgBoxFmt( "creating static particle set at %s(radius: %f)", to_string(vCenterPos).c_str(), r ) )

	for( i=0; i<num_particls_per_set; i++ )
	{
/*		vert[0].offset = D3DXVECTOR2(-r, r );
		vert[1].offset = D3DXVECTOR2( r, r );
		vert[2].offset = D3DXVECTOR2( r,-r );
		vert[3].offset = D3DXVECTOR2(-r,-r );*/
		vert[0].local_offset.x = -r;
		vert[0].local_offset.y =  r;
		vert[1].local_offset.x =  r;
		vert[1].local_offset.y =  r;
		vert[2].local_offset.x =  r;
		vert[2].local_offset.y = -r;
		vert[3].local_offset.x = -r;
		vert[3].local_offset.y = -r;
		factor_x = RangedRand( 0.8f, 1.6f );
		factor_y = RangedRand( 0.6f, 1.2f );

		offset = RangedRand( 0, num_tex_patterns - 1 );
		u = (float)(offset % num_tex_segs) * fTex;
		v = (float)(offset / num_tex_segs) * fTex;

		vert[0].tex = TEXCOORD2(u,        v       );
		vert[1].tex = TEXCOORD2(u + fTex, v       );
		vert[2].tex = TEXCOORD2(u + fTex, v + fTex);
		vert[3].tex = TEXCOORD2(u,        v + fTex);

		GaussianRand( x, z );
		y = RangedRand( -1.0f, 1.0f );	//GaussianRand( y, w );
		for( j=0; j<4; j++ )
		{
			vert[j].vPosition = vCenterPos + D3DXVECTOR3( x * 200.0f, y * 50.0f, z * 200.0f );

//			vert[j].offset.x *= factor_x;
//			vert[j].offset.y *= factor_y;
			vert[j].local_offset.x *= factor_x;
			vert[j].local_offset.y *= factor_y;
			vert[j].color = 0xFFFFFFFF;

			m_vecParticleVertex.push_back( vert[j] );
		}

		aabb.MergeAABB( AABB3(
			vert[0].vPosition + Vector3(-r,-r,-r) * 2.0f,
			vert[0].vPosition + Vector3( r, r, r) * 2.0f ) );
	}

//	pCopyEnt->world_aabb = aabb;

	NumParticles(pCopyEnt) = num_particls_per_set;

	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;
}


void CBE_StaticParticleSet::Act(CCopyEntity* pCopyEnt)
{
}	//behavior in in one frame


void CBE_StaticParticleSet::Draw(CCopyEntity* pCopyEnt)
{
	if( m_vecpVB.size() == 0 )
		return;

//	PERIODICAL( 2000, MsgBoxFmt( "CBE_StaticParticleSet::Draw()" ) )

	int index_offset = (int)IndexOffset(pCopyEnt);
	int num_particles = (int)NumParticles(pCopyEnt);

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
	Matrix33 matRot;
	D3DXMATRIX matWorld;
	m_pStage->GetBillboardRotationMatrix( matRot );
	matRot.GetRowMajorMatrix44( (Scalar *)&matWorld );
	matWorld._44 = 1;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;
	hr = pd3dDev->SetFVF( STATICBILLBOARDVERTEX::FVF );
	pd3dDev->SetVertexDeclaration( m_pParticleVertexDeclaration );

	if( FAILED(hr) ) MsgBox( "CBE_StaticParticleSet - invalid FVF" );

	// bind the vertex buffer to the device data stream.
	pd3dDev->SetStreamSource( 0, m_vecpVB[0], 0, sizeof(STATICBILLBOARDVERTEX) );
	
	// set index data
	hr = pd3dDev->SetIndices(m_pIB);
	if( FAILED(hr) ) MsgBox( "CBE_StaticParticleSet - cannot set index buffer" );


	// don't wirte to z-buffer so that smoke should be painted on one another
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	CShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	LPD3DXEFFECT pEffect = NULL;
	UINT pass, cPasses;
	if( pShaderManager &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		pEffect->SetMatrix( "WorldRot", &matWorld );

		D3DXMatrixIdentity( &matWorld );
		pShaderManager->SetWorldTransform( matWorld );
		pShaderManager->SetTexture( 0, m_BillboardTexture.GetTexture() );

		if( m_pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PseudoNightVision )
		{
			pShaderManager->SetTechnique( m_aShaderTechHandle[0] );
		}
		else
		{
			pShaderManager->SetTechnique( m_aShaderTechHandle[0] );
		}
	
		pEffect->CommitChanges();

		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//		pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//		pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//		pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA /*m_DestAlphaBlendMode*/ );

		hr = pEffect->Begin( &cPasses, 0 );

		for( pass=0; pass<cPasses; pass++ )
		{
			HRESULT hr;
			hr = pEffect->BeginPass( pass );

//			ONCE( MsgBoxFmt( "rendering %d particles", num_particles ) )

			// draw particles
			hr = pd3dDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
											0,
											index_offset / 3 * 2,
											num_particles * 4,
											index_offset,
											num_particles * 2 );

			pEffect->EndPass();

//			if( hr == S_OK )
//				ONCE( MsgBoxFmt( "%d particles rendered", num_particles ) )
//				PERIODICAL( 2500, MsgBoxFmt( "%d particles rendered", num_particles ) )

			if( 0 < m_vecpVB.size() )
			{
				void *pBuffer;
				m_vecpVB.back()->Lock( 0, 0, &pBuffer, 0 );
				STATICBILLBOARDVERTEX *pVert = (STATICBILLBOARDVERTEX *)pBuffer;
				ONCE( g_Log.Print( "particle local offset(x,y) = (%f,%f)", pVert[0].local_offset.x, pVert[0].local_offset.y ) )
				ONCE( g_Log.Print( "particle local offset(x,y) = (%f,%f)", pVert[1].local_offset.x, pVert[1].local_offset.y ) )
				ONCE( g_Log.Print( "particle local offset(x,y) = (%f,%f)", pVert[2].local_offset.x, pVert[2].local_offset.y ) )
				ONCE( g_Log.Print( "particle local offset(x,y) = (%f,%f)", pVert[3].local_offset.x, pVert[3].local_offset.y ) )
				m_vecpVB.back()->Unlock();
			}
		}

		pEffect->End();
	}

	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}


void CBE_StaticParticleSet::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
}


void CBE_StaticParticleSet::ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself )
{
}


void CBE_StaticParticleSet::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}


bool CBE_StaticParticleSet::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_ParticleSet::LoadSpecificPropertiesFromFile( scanner );

	return false;
}


void CBE_StaticParticleSet::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );

//	ar & ;
}


void CBE_StaticParticleSet::CommitStaticParticles()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	SAFE_RELEASE(m_pVB);

	size_t num_vertices = m_vecParticleVertex.size();
	if( NUM_MAX_PARTICLES_PER_VB < num_vertices )
		num_vertices = NUM_MAX_PARTICLES_PER_VB;

	m_vecpVB.push_back( NULL );
    // just create the empty vertex buffer.
	if( FAILED( pd3dDev->CreateVertexBuffer( sizeof(STATICBILLBOARDVERTEX) * num_vertices,
		                                     0, STATICBILLBOARDVERTEX::FVF,
											 D3DPOOL_DEFAULT, &m_vecpVB.back(), NULL ) ) )
	{
		MsgBox( "CBE_StaticParticleSet - cannot create vertex buffer" );
		return;// E_FAIL;
	}

	// get access to the index buffer
	VOID* pVertices;
	if( FAILED( m_vecpVB.back()->Lock( 0, 0, &pVertices, 0 ) ) )
	{
 		MsgBox( "CBE_StaticParticleSet - cannot lock vertex buffer" );
		SAFE_RELEASE(m_vecpVB.back());
		return;// E_FAIL;
	}

	// fill the vertex buffer
	memcpy( pVertices, &m_vecParticleVertex[0], sizeof(STATICBILLBOARDVERTEX) * num_vertices );

	m_vecpVB.back()->Unlock();

	STATICBILLBOARDVERTEX vert = m_vecParticleVertex[0];
	for( int i=0; i<8; i++ )
	{
//		g_Log.Print( "v[%d] - pos%s, offset(%f,%f), color: 0x%lx",
//			i, to_string(vert.vPosition).c_str(), vert.offset.x, vert.offset.y, vert.color );
	}
}
