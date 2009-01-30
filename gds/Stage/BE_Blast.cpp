
#include "BE_Blast.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"

#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/Direct3D9.hpp"


#include "Graphics/UnitCube.hpp"
#include "Support/SafeDelete.hpp"


float& CurrentBlastTime( CCopyEntity *pEntity ) { return pEntity->f2; }

CBE_Blast::CBE_Blast()
{
	m_bNoClip = true;
	m_BoundingVolumeType = BVTYPE_DOT;
	m_fRadius = 0;
	m_aabb.SetMaxAndMin( Vector3(0,0,0), Vector3(0,0,0) );
//	m_sGlareType = CEGT_NOGLARE;

	m_fImpulse = 100.0f;

	m_pUnitCube = NULL;
}


CBE_Blast::~CBE_Blast()
{
	SafeDelete( m_pUnitCube );
}


void CBE_Blast::Init()
{
	m_pUnitCube = new CUnitCube;
	m_pUnitCube->Init();
//	m_pUnitCube->SetRenderMode( CUnitCube::RS_WIREFRAME );
}


void CBE_Blast::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	CurrentBlastTime(pCopyEnt) = 0;
//	memset( &pCopyEnt->GetWorldInvInertia(), 0, sizeof(float) * 16 );
//	pCopyEnt->GetWorldInvInertia()._44 = 1.0f;

//	pCopyEnt->fMass = 5.0f;
//	pCopyEnt->fMass_inv = 1.0f / pCopyEnt->fMass;
}


void CBE_Blast::Act(CCopyEntity* pCopyEnt)
{
	float& rfCurrentBlastTime = CurrentBlastTime(pCopyEnt);
	float fBlastFrameTime;

	float frametime = m_pStage->GetFrameTime();
	if( rfCurrentBlastTime + frametime < m_fBlastDuration )
	{
		fBlastFrameTime = frametime;
		rfCurrentBlastTime += frametime;
	}
	else
	{
		fBlastFrameTime = m_fBlastDuration - rfCurrentBlastTime;
		rfCurrentBlastTime = m_fBlastDuration;
	}

	float fCurrentBlastRadius
		= m_fMaxBlastRadius * rfCurrentBlastTime / m_fBlastDuration;

	// save blast radius
	pCopyEnt->f3 = fCurrentBlastRadius;

	// check if there is any entity which is overlapping the bounding sphere
	// of the current blast (rough estimate)
	CTrace tr;
	STrace tr2;
	static vector<CCopyEntity *> s_vecpEntityBuffer;
	
	s_vecpEntityBuffer.resize( 0 );
	tr.SetTouchEntityBuffer( &s_vecpEntityBuffer );
	tr.SetSphere( pCopyEnt->Position(), fCurrentBlastRadius );
	tr.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );	// skip checking the no-clip entities
	m_pStage->CheckCollision( tr );

	// check if the candidates are really taking the blast
	SGameMessage msg;
	msg.pSenderEntity = pCopyEnt;	// sender is the blast;
	msg.iEffect = GM_DAMAGE;
	msg.s1 = DMG_BLAST;
	msg.fParam1 = m_fBaseDamage * fBlastFrameTime;	// amount of damage
	size_t iNumOvelappingEntities = tr.GetNumTouchEntities();
	CCopyEntity* pTouchedEntity;


//	tr2.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );
	tr2.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr2.bvType = BVTYPE_DOT;

	for(int i=0; i<iNumOvelappingEntities; i++)
	{
//		Release Mode: At least reached here
//		MessageBox( NULL, "found entity in blast range", "Blast::Act()", MB_OK );

		if( tr.GetTouchEntity(i) == pCopyEnt )
			continue;	// myself

		tr2.fFraction = 1.0f;
		tr2.iNumTouches = 0;
		tr2.pSourceEntity = pCopyEnt;
		tr2.pTouchedEntity = NULL;
		tr2.pvStart = &pCopyEnt->Position();
		tr2.pvGoal = &tr.GetTouchEntity(i)->Position();


		// check if there is any obstacle between the blast center and the candidate
		m_pStage->ClipTrace( tr2 );

		if( tr2.pTouchedEntity == 0 )
			continue;

///		MessageBox( NULL, "entity in valid blast range", "Blast::Act()", MB_OK );

		pTouchedEntity = tr2.pTouchedEntity;

		if( pTouchedEntity != tr.GetTouchEntity(i) )
			continue;	// candidate is protected by another entity

		if( pTouchedEntity->bNoClip )
			continue;	// blast gives no damage to no-clip entities

///		MessageBox( NULL, "sending blast damge", "Blast::Act()", MB_OK );

		// no oblstacle covering the entity from the blast
		Vector3 vBlastCenterToTarget = pTouchedEntity->Position() - pCopyEnt->Position();
		Vec3Normalize( vBlastCenterToTarget, vBlastCenterToTarget );
		msg.vParam = vBlastCenterToTarget;

//		if( !(pTouchedEntity->EntityFlag & BETYPE_RIGIDBODY) )	// avoid damaging rigid bodies to see the effect of blast 
			SendGameMessageTo( msg, pTouchedEntity );


		if( pTouchedEntity->EntityFlag & BETYPE_RIGIDBODY )
		{
//			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * 10.0f, tr2.GetEndPosition() );
//			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * 10.0f, tr2.vEnd );
			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * m_fImpulse * fBlastFrameTime, tr2.vEnd );
		}
		else if( pTouchedEntity->EntityFlag & BETYPE_PLAYER )
		{
//			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * msg.fParam1 * 0.1f, tr2.GetEndPosition() );
			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * msg.fParam1 * 0.1f, tr2.vEnd );
		}
	}

	pCopyEnt->Velocity() = Vector3(0,0,0);

	if( rfCurrentBlastTime == m_fBlastDuration )
		m_pStage->TerminateEntity( pCopyEnt );	// blast has been expanded to its maximum size
}


void CBE_Blast::Draw(CCopyEntity* pCopyEnt)
{
/*	float fCurrentBlastRadius = pCopyEnt->f3;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	memcpy( &matWorld._41, &pCopyEnt->Position(), sizeof(D3DXVECTOR3) );

	// show the center position of the blast ( in black )
	matWorld._11 = matWorld._22 = matWorld._33 = 0.2f;

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	m_pUnitCube->SetUniformColor( 0.0f, 0.0f, 0.0f, 0.9f );
	m_pUnitCube->Draw();

	// show the blast range ( in red )
	matWorld._11 = matWorld._22 = matWorld._33 = pCopyEnt->f3 * 2.0f;

	DIRECT3D9.GetDevice()->SetTransform( D3DTS_WORLD, &matWorld );

	m_pUnitCube->SetUniformColor( 1.0f, 0.0f, 0.0f, 0.6f );
	m_pUnitCube->Draw();*/
}


bool CBE_Blast::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( scanner.TryScanLine( "BASEDAMAGE",	m_fBaseDamage ) ) return true;
	if( scanner.TryScanLine( "MAX_RADIUS",	m_fMaxBlastRadius ) ) return true;
	if( scanner.TryScanLine( "DURATION",	m_fBlastDuration ) ) return true;
	if( scanner.TryScanLine( "IMPULSE",		m_fImpulse ) ) return true;

	return false;
}


void CBE_Blast::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_fBaseDamage;
	ar & m_fMaxBlastRadius;
	ar & m_fBlastDuration;
	ar & m_fImpulse;
}
