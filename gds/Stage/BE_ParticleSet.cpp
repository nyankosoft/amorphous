#include "BE_ParticleSet.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "ScreenEffectManager.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/BillboardArrayMesh.hpp"

#include "GameCommon/MTRand.hpp"
#include "GameCommon/CriticalDamping.hpp"

#include "3DMath/MathMisc.hpp"

#include "Support/SafeDelete.hpp"
#include "Support/Profile.hpp"
#include "Support/Serialization/Serialization_Color.hpp"


class CParticleThreadStarter
{
	CBE_ParticleSet *m_ParticleSetBaseEntity;

public:

	CParticleThreadStarter( CBE_ParticleSet *pBaseEntity ) : m_ParticleSetBaseEntity(pBaseEntity) {}

	void operator()()
	{
		// start particle thread
		m_ParticleSetBaseEntity->ParticleThreadMain();
	}
};


void CBE_ParticleSet::ParticleThreadMain()
{
//	const float dt = m_fParticleUpdateTimestep;
	const float dt = 0.03f;
	const int num_particle_sets = m_MaxNumParticleSets;
	while( !m_TerminateParticleThread )
	{
		Sleep(10);

		if( m_fFrameTimeLeft <= 0 )
			continue;

		m_fFrameTimeLeft -= dt;

		for( int i=0; i<num_particle_sets; i++ )
		{
			SBE_ParticleSetExtraData& rParticleSet = m_paParticleSet[i];
			AABB3 aabb;

			if( 0 < rParticleSet.iNumParticles )
//				UpdateParticles( rParticleSet, dt, aabb );
				UpdateParticlePositions( rParticleSet, dt, aabb );
		}

	}
}


CBE_ParticleSet::CBE_ParticleSet()
{
	m_iParticleSetCurrentIndex = 0;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_iNumTextureSegments = 1;

//	m_iNumParticles				= 8;
	m_fDuration					= 1.0f;
	m_fRandomVelocity_XZ		= 0.0f;
	m_fRandomVelocity_Y			= 0.0f;
	m_fExpansionFactor			= 1.0f;
	m_fGravityInfluenceFactor	= 1.0f;

	m_ParticleType = TYPE_SMOKE_NORMAL;

	m_bWorldOffset = false;

	m_VertexColor.SetRGB( 1.0f, 1.0f, 1.0f );

	m_bMinimumParticleUpdates = false;

	// particle threads (experimental)

	m_bCreateParticleThread = false;

	m_fFrameTimeLeft = 0;

	m_TerminateParticleThread = false;

	m_MaxNumParticleSets = NUM_DEFAULT_PARTICLESETS;

	m_paParticleSet = NULL;

    m_MaxNumParticlesPerSet = NUM_DEFAULT_PARTICLES_PER_SET;

	m_VertexBufferType = VBT_SHARED_VERTEX_BUFFER;
}


CBE_ParticleSet::~CBE_ParticleSet()
{
	ReleaseGraphicsResources();

	if( m_bCreateParticleThread )
	{
		m_TerminateParticleThread = true;

		// wait until all the particle threads are released
		m_ParticleThreadGroup.join_all();
	}

	SafeDeleteArray( m_paParticleSet );
}


void CBE_ParticleSet::Init()
{
	// allocate buffer for particle sets
	m_paParticleSet = new SBE_ParticleSetExtraData [m_MaxNumParticleSets];

	int i;
	for( i=0; i<m_MaxNumParticleSets; i++ )
	{
		m_paParticleSet[i].Init( ParticleSetFlag::ALL, m_MaxNumParticlesPerSet );
	}

	// load billboard texture
	CBEC_Billboard::Init();

	InitParticles();

//	m_Type = TYPE_RECT_ARRAY_AND_INDICES;

//	HRESULT hr;
	switch( m_Type )
	{
	case TYPE_BILLBOARDARRAYMESH:
	case TYPE_BILLBOARDARRAYMESH_SHARED:
		{
			LoadBillboardArrayMesh( m_fParticleRadius, m_MaxNumParticleSets, m_MaxNumParticlesPerSet, m_iNumTextureSegments );
		}
		break;

	case TYPE_RECT_ARRAY_AND_INDICES:
		{
			InitBillboardRects();
		}
		break;

	default:
		break;
	}

	// start particle thread
	if( m_bCreateParticleThread )
	{
/*		m_pThreadPooler = new CThreadPooler();

		m_pThreadPooler->StartThread( 1 );

		smart_ptr< function_callback > fn( new function_callback_vm0<CBE_ParticleSet>(ParticleThreadMain,this) );
//		m_fn = smart_ptr< function_callback >( new function_callback_vm0<CBE_ParticleSet>(ParticleThreadMain,this), false );

		// let the particle thread do its job
		m_pThreadPooler->Invoke( fn );*/

		for( i=0; i<1; i++ )
			m_ParticleThreadGroup.add_thread( new boost::thread(CParticleThreadStarter(this)) );
	}
}


void CBE_ParticleSet::InitParticles()
{
	int i, j;
	for(i=0; i<m_MaxNumParticleSets; i++)
	{
		for(j=0; j<m_MaxNumParticlesPerSet; j++)
		{
			m_paParticleSet[i].pafAnimDuration[j] = m_fDuration + RangedRand( -0.1f, 0.2f );
		}
	}
}


void CBE_ParticleSet::InitBillboardRects()
{
	// set particle properties
	float fPRadius = m_fParticleRadius;

	int i;
	int num_tex_segs = m_iNumTextureSegments;
	int num_tex_patterns = num_tex_segs * num_tex_segs;
	float fTex = 1.0f / (float)m_iNumTextureSegments;	// TODO: random texture coord for particles
	int offset;
	for(i=0; i<m_MaxNumParticlesPerSet; i++)
	{
		// set random texture patterns for each particle in advance
		offset = RangedRand( 0, num_tex_patterns - 1 );
		float u,v;
		u = (float)(offset % num_tex_segs) * fTex;
		v = (float)(offset / num_tex_segs) * fTex;

//		m_avBillboardRect[i*4+0].tex = TEXCOORD2(u,        v       );
//		m_avBillboardRect[i*4+1].tex = TEXCOORD2(u + fTex, v       );
//		m_avBillboardRect[i*4+2].tex = TEXCOORD2(u + fTex, v + fTex);
//		m_avBillboardRect[i*4+3].tex = TEXCOORD2(u,        v + fTex);
		SetTexCoord( i, 0, TEXCOORD2(u,        v       ) );
		SetTexCoord( i, 1, TEXCOORD2(u + fTex, v       ) );
		SetTexCoord( i, 2, TEXCOORD2(u + fTex, v + fTex) );
		SetTexCoord( i, 3, TEXCOORD2(u,        v + fTex) );
	}

	// set rect vertex positions in local space
	// these values are not changed during particle rendering
	const float r = m_fParticleRadius;
	for(i=0; i<m_MaxNumParticlesPerSet; i++)
	{
		m_avBillboardRect_S[i*4+0].local_offset = D3DXVECTOR2(-r, r);
		m_avBillboardRect_S[i*4+1].local_offset = D3DXVECTOR2( r, r);
		m_avBillboardRect_S[i*4+2].local_offset = D3DXVECTOR2( r,-r);
		m_avBillboardRect_S[i*4+3].local_offset = D3DXVECTOR2(-r,-r);
	}


/*
#ifdef BE_PARTICLESET_USE_VB_AND_IB

	InitVertexBufferAndIndexBuffer();

#endif
*/

	m_FadeTable.SetQuadraticFadeout( 1.0f, 0.0f, 1.2f, 1.0f, 0.0f );

}


void CBE_ParticleSet::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime = 0.0f;

	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	float fRandX, fRandY, fRandZ;
	float fRandVelocity_XZ = m_fRandomVelocity_XZ;
	float fRandVelocity_Y  = m_fRandomVelocity_Y;
	int i, num_particles = m_MaxNumParticlesPerSet;
	for(i=0; i<num_particles; i++)
	{
		rParticleSet.pavPosition[i] = pCopyEnt->Position();
		fRandX = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_XZ;
		fRandZ = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_XZ;
		fRandY = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_Y;
		rParticleSet.pavVelocity[i] = pCopyEnt->Velocity() + Vector3(fRandX, fRandY, fRandZ);

		rParticleSet.pafAnimationTime[i] = 0.0f;
		rParticleSet.pasPattern[i] = 0;
	}
	rParticleSet.iNumParticles = num_particles;

	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;
//	pCopyEnt->bUseZSort = true;
}


void CBE_ParticleSet::Act(CCopyEntity* pCopyEnt)
{
	float fFrameTime = m_pStage->GetFrameTime();
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime += fFrameTime;
	if( m_fDuration <= rfCurrentTime )
	{
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	// update the positions of particles

	Vector3 vGravityAccel = m_pStage->GetGravityAccel();
	float fGravityInfluenceFactor = m_fGravityInfluenceFactor;

//	DWORD dwColor = D3DCOLOR_ARGB( ((int)((m_fDuration - rfCurrentTime) / m_fDuration * 255.0f)), 255, 255, 255 );

	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );
	int i, num_particles = m_MaxNumParticlesPerSet;

	if( m_ParticleType == TYPE_SMOKE_SHOOTING )
	{
		for(i=0; i<num_particles; i++)
		{
		  if( 0.01f < Vec3Dot( rParticleSet.pavVelocity[i], rParticleSet.pavOrigDirection[i] ) )
		  {

			// update velocity
			rParticleSet.pavVelocity[i] -= rParticleSet.pavOrigDirection[i] * 28.0f * fFrameTime * RangedRand( 0.9f, 1.1f );
//			rParticleSet.pavVelocity[i]
//				-= rParticleSet.pavOrigDirection[i]
//				* Vec3Dot( rParticleSet.pavVelocity[i], rParticleSet.pavOrigDirection[i] ) * 1.0f * fFrameTime;

			if( Vec3Dot( rParticleSet.pavVelocity[i], rParticleSet.pavOrigDirection[i] ) < 0 )
				rParticleSet.pavVelocity[i] = Vector3(0,0,0);
		  }


			rParticleSet.pavVelocity[i] += (vGravityAccel * fGravityInfluenceFactor) * fFrameTime;

			// update position
			rParticleSet.pavPosition[i] += rParticleSet.pavVelocity[i] * fFrameTime;

			// update animation time
			rParticleSet.pafAnimationTime[i] += fFrameTime;
		}
	}
	else
	{
		AABB3 aabb;
		aabb.Nullify();
		UpdateParticles( rParticleSet, fFrameTime, aabb );

		pCopyEnt->world_aabb.MergeAABB( aabb );
	}

	if( m_Type == CBEC_Billboard::TYPE_BILLBOARDARRAYMESH )
	{
		// each copy entity (i.e. particle group) has its own paticles vertices
		UpdateVertexBuffer( pCopyEnt );
	}
}


void CBE_ParticleSet::Draw(CCopyEntity* pCopyEnt)
{
	DrawParticles(pCopyEnt);
}


void CBE_ParticleSet::UpdateVertexBuffer(CCopyEntity* pCopyEnt)
{
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

///	float fRadius, fBaseRadius = m_fParticleRadius;
	float fFraction, fTotalAnimationTime = m_fDuration;
	float fExpansionFactor = m_fExpansionFactor;
	int i, num_particles = rParticleSet.iNumParticles;
//	short sPatternOffset;

	// vertex color - alpha is set later for each particle to make fading effects
	const DWORD vert_color = 0x00FFFFFF & m_VertexColor.GetARGB32();

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
	D3DXMATRIX matWorld;
	Matrix34 billboard_pose;
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	billboard_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );

	D3DXVECTOR3 avBasePos[4];

	// set transformed particle data to dest buffer 'm_avDestParticle'
	int vert_offset;
	float factor;

	float particle_set_anim_time;
	if( m_bMinimumParticleUpdates )
		particle_set_anim_time = CurrentTime(pCopyEnt);
	else
		particle_set_anim_time = 0;


	ProfileBegin( "CBE_ParticleSet::UpdateVB() - lock VB" );

//	TEXTUREVERTEX *pParticleVertex = m_avBillboardRect;
	BILLBOARDVERTEX *pParticleVertex;// = m_avBillboardRect_S;
	switch( m_Type )
	{
	case TYPE_BILLBOARDARRAYMESH:
		{
			LPD3DXMESH pMesh = m_pBillboardArrayMesh->GetMesh();
			void *pBuffer;
//			pMesh->LockVertexBuffer( 0, &pBuffer );
			pMesh->LockVertexBuffer( D3DLOCK_NOSYSLOCK, &pBuffer );
			pParticleVertex = (BILLBOARDVERTEX *)pBuffer;
			pParticleVertex = pParticleVertex + pCopyEnt->iExtraDataIndex * m_MaxNumParticlesPerSet;
		}
		break;
	case TYPE_BILLBOARDARRAYMESH_SHARED:
		{
			LPD3DXMESH pMesh = m_pBillboardArrayMesh->GetMesh();
			void *pBuffer;
//			pMesh->LockVertexBuffer( 0, &pBuffer );
			pMesh->LockVertexBuffer( D3DLOCK_NOSYSLOCK, &pBuffer );
			pParticleVertex = (BILLBOARDVERTEX *)pBuffer;
		}
		break;
	case TYPE_RECT_ARRAY_AND_INDICES:
		pParticleVertex = m_avBillboardRect_S;
		break;
	default:
		pParticleVertex = NULL;
		break;
	}

	ProfileEnd( "CBE_ParticleSet::UpdateVB() - lock VB" );


	ProfileBegin( "CBE_ParticleSet::UpdateVB() - VB setup" );

	for(i=0; i<num_particles; i++)
	{
		// change anim duration for each particle
		fTotalAnimationTime = rParticleSet.pafAnimDuration[i];

		vert_offset = i * 4;
		// set alpha value - smoke particles fade away as time passes
		float fCurrentTime = particle_set_anim_time + rParticleSet.pafAnimationTime[i];

		if( fCurrentTime < 0 || fTotalAnimationTime <= fCurrentTime )
		{
			pParticleVertex[vert_offset+0].color = 0;
			pParticleVertex[vert_offset+1].color = 0;
			pParticleVertex[vert_offset+2].color = 0;
			pParticleVertex[vert_offset+3].color = 0;
			continue;
		}

		fFraction = fCurrentTime / fTotalAnimationTime;

//		fRadius = fBaseRadius * (1.0f + fFraction * 1.5f);	// expand particle up to 2.5 times
///		fRadius = fBaseRadius * (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
		factor = (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
		pParticleVertex[vert_offset+0].factor = factor;
		pParticleVertex[vert_offset+1].factor = factor;
		pParticleVertex[vert_offset+2].factor = factor;
		pParticleVertex[vert_offset+3].factor = factor;

		const D3DXVECTOR3 vBasePos = rParticleSet.pavPosition[i];
		pParticleVertex[vert_offset+0].vPosition = vBasePos;
		pParticleVertex[vert_offset+1].vPosition = vBasePos;
		pParticleVertex[vert_offset+2].vPosition = vBasePos;
		pParticleVertex[vert_offset+3].vPosition = vBasePos;

		// calc color and alpha value of the particle
//		DWORD dwColor = 0x00FFFFFF | ( ((int)((m_FadeTable.GetValue(fFraction)) * 255.0f)) << 24 );
//		DWORD dwColor = 0x00FFFFFF | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );
		DWORD dwColor = vert_color | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );

//		float& rfFadeVel = rParticleSet.pafFadeVel[i];
//		DWORD dwColor = 0x00FFFFFF | (int)( SmoothCD( fFraction, 0.0f, rfFadeVel, fTotalAnimationTime, dt ) );

//		dwColor = 0xFFFFFFFF;

		pParticleVertex[vert_offset+0].color = dwColor;
		pParticleVertex[vert_offset+1].color = dwColor;
		pParticleVertex[vert_offset+2].color = dwColor;
		pParticleVertex[vert_offset+3].color = dwColor;
	}

	ProfileEnd( "CBE_ParticleSet::UpdateVB() - VB setup" );

/*	char acStr[256];
	sprintf( acStr, "texture: %d", m_BillboardTexture.GetTexture() );
    MessageBox( NULL, acStr, "check", MB_OK|MB_ICONWARNING );*/

	if( m_bWorldOffset )
	{
		Matrix34 world_pose = pCopyEnt->GetWorldPose();
		for( i=0; i<num_particles * 4; i++ )
		{
			world_pose.Transform( pParticleVertex[i].vPosition, pParticleVertex[i].vPosition );
		}
	}

	if( m_Type == TYPE_BILLBOARDARRAYMESH
	 || m_Type == TYPE_BILLBOARDARRAYMESH_SHARED )
	{
		LPD3DXMESH pMesh = m_pBillboardArrayMesh->GetMesh();
		pMesh->UnlockVertexBuffer();
	}
}


/// draw particles through billboard array mesh or rect array
void CBE_ParticleSet::DrawParticles( CCopyEntity* pCopyEnt )
{
	ProfileBegin( "CBE_ParticleSet::DrawParticles()" );

	if( m_Type != TYPE_BILLBOARDARRAYMESH )
	{
		// particle vertices are shared by copy entities of this base entity
		// to save memory - need update VB in rendering routine
		UpdateVertexBuffer( pCopyEnt );
	}

	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );
	const int num_particles = rParticleSet.iNumParticles;

	ProfileBegin( "DrawParticles(): pEffect->SetMatrix(), etc." );

	Matrix33 matRot;
	D3DXMATRIX matBillboard;
	m_pStage->GetBillboardRotationMatrix( matRot );
	matRot.GetRowMajorMatrix44( (Scalar *)&matBillboard );
	CShaderManager *pShaderManager = NULL;
	LPD3DXEFFECT pEffect = NULL;
	if( (pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager()) &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		pEffect->SetMatrix( "ParticleWorldRot", &matBillboard );
	}

	ProfileEnd( "DrawParticles(): pEffect->SetMatrix(), etc." );

	// draw particles
	if( m_Type == TYPE_BILLBOARDARRAYMESH )
		DrawBillboards( num_particles, pCopyEnt->iExtraDataIndex, m_MaxNumParticlesPerSet, m_pStage );
	else
        DrawBillboards( num_particles, 0, 0, m_pStage );

	ProfileEnd( "CBE_ParticleSet::DrawParticles()" );
}


bool CBE_ParticleSet::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBaseEntity::LoadSpecificPropertiesFromFile( scanner );
	CBEC_Billboard::LoadSpecificPropertiesFromFile( scanner );

	string smoke_type;

	if( scanner.TryScanLine( "PTCL_RADIUS",     m_fParticleRadius ) ) return true;
	if( scanner.TryScanLine( "DURATION",        m_fDuration ) ) return true;
	if( scanner.TryScanLine( "RANDVEL_XZ",      m_fRandomVelocity_XZ ) ) return true;
	if( scanner.TryScanLine( "RANDVEL_Y",       m_fRandomVelocity_Y ) ) return true;

	if( scanner.TryScanLine( "ANIMTIME_OFFSET", m_fAnimTimeOffsetMin, m_fAnimTimeOffsetMax ) ) return true;

	if( scanner.TryScanLine( "MAX_NUM_PARTICLE_GROUPS",     m_MaxNumParticleSets ) ) return true;
	if( scanner.TryScanLine( "MAX_NUM_PARTICLES_PER_GROUP", m_MaxNumParticlesPerSet ) ) return true;
//	if( scanner.TryScanLine( "NUM_PTCLS", m_iNumParticles ) )
//	{
//		Limit( m_iNumParticles, 1, m_MaxNumParticlesPerSet );
//		return true;
//	}

	SFloatRGBColor& color = m_VertexColor;
	if( scanner.TryScanLine( "VERT_COLOR",	color.fRed, color.fGreen, color.fBlue ) ) return true;

	if( scanner.TryScanLine( "NUM_TEXSEGS", m_iNumTextureSegments ) )
	{
		if( PTCL_NUM_MAX_TEXTURE_SEGMENTS < m_iNumTextureSegments )
			m_iNumTextureSegments = PTCL_NUM_MAX_TEXTURE_SEGMENTS;
		return true;
	}

	if( scanner.TryScanLine( "EXPANSION", m_fExpansionFactor ) ) return true;
	if( scanner.TryScanLine( "GRAVITY_INFLUENCE", m_fGravityInfluenceFactor ) ) return true;


	if( scanner.TryScanLine( "SMOKE_TYPE", smoke_type ) ) 
	{
		if( smoke_type == "TYPE_NORMAL" )			m_ParticleType = TYPE_SMOKE_NORMAL;
		else if( smoke_type == "TYPE_DIRECTIONAL" )	m_ParticleType = TYPE_SMOKE_DIRECTIONAL;
		else if( smoke_type == "TYPE_SHOOTING" )	m_ParticleType = TYPE_SMOKE_SHOOTING;
		else return false;

		return true;
	}

	return false;
}


void CBE_ParticleSet::ReleaseGraphicsResources()
{
	CBEC_Billboard::ReleaseGraphicsComponentResources();
}


void CBE_ParticleSet::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	// reload texture
	CBEC_Billboard::LoadGraphicsComponentResources( rParam );

//	InitVertexBufferAndIndexBuffer();
}


void CBE_ParticleSet::Serialize( GameLib1::Serialization::IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );
	SerializeBillboardProperty( ar, version );

	ar & m_ParticleType;
	ar & m_fParticleRadius;
//	ar & m_iNumParticles;
	ar & m_iNumTextureSegments;
	ar & m_fDuration;
	ar & m_fAnimTimeOffsetMin & m_fAnimTimeOffsetMax;
	ar & m_fRandomVelocity_XZ & m_fRandomVelocity_Y;
	ar & m_fExpansionFactor;
	ar & m_fGravityInfluenceFactor;
	ar & m_VertexColor;
	ar & m_bWorldOffset;
	ar & m_bCreateParticleThread;
	ar & m_bMinimumParticleUpdates;
	ar & m_MaxNumParticleSets;
    ar & m_MaxNumParticlesPerSet;
	ar & m_VertexBufferType;
}





/*
HRESULT CBE_ParticleSet::InitVertexBufferAndIndexBuffer()
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	SAFE_RELEASE(m_pVB);

    // just create the empty vertex buffer.
    if( FAILED( pd3dDev->CreateVertexBuffer( sizeof(TEXTUREVERTEX) * m_MaxNumParticlesPerSet * 4,
                                                  0, D3DFVF_TEXTUREVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

	// Vertex buffer will be filled every time the particles are drawn.

	SAFE_RELEASE(m_pIB);

	// create the index buffer
	if( FAILED( pd3dDev->CreateIndexBuffer( sizeof(WORD) * m_MaxNumParticlesPerSet * 6,
                                            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
                                            D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// set the index data. these indices remains constant during the game
	WORD awIndex[m_MaxNumParticlesPerSet * 6];
	int i, iIndexOffset = 0;
	for( i=0; i<m_MaxNumParticlesPerSet; i++ )	{
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

	// establish access to the index buffer
	VOID* pIndices;
	if( FAILED( m_pIB->Lock( 0,                 // Fill from start of the buffer
							 sizeof(WORD) * m_MaxNumParticlesPerSet * 6, // Size of the data to load
							 &pIndices,  // Returned index data		// has to be (BYTE**)&pIndices?
							 0 ) ) )             // Send default flags to the lock
	{
		SAFE_RELEASE(m_pIB);
		return E_FAIL;
	}

	// fill the index buffer
	memcpy( pIndices, awIndex, sizeof(awIndex) );

	m_pIB->Unlock();

    return S_OK;
}*/
