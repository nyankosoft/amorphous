#include "BE_Blast.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
//#include "Graphics/UnitCube.hpp"
//#include "Support/SafeDelete.hpp"
#include "3DMath/MathMisc.hpp"
#include "3DMath/MatrixConversions.hpp"

using namespace std;
using namespace boost;


#define LOG_PRINTF(x) LOG_PRINT( string(" ") + fmt_string x )


float& CurrentBlastTime( CCopyEntity *pEntity ) { return pEntity->f2; }
float& CurrentBlastRadius( CCopyEntity *pEntity ) { return pEntity->f3; }


CBE_Blast::CBE_Blast()
{
	m_bNoClip = true;
	m_BoundingVolumeType = BVTYPE_DOT;
	m_fRadius = 0;
	m_aabb.SetMaxAndMin( Vector3(0,0,0), Vector3(0,0,0) );

	m_fImpulse = 100.0f;

	m_fBaseDamage = 1000.0f;

	m_fMaxBlastRadius = 1.0f;

	m_fBlastDuration = 0.1f;

	m_fLinearExpansionSpeed = 1000000.0f;
}


CBE_Blast::~CBE_Blast()
{
}


void CBE_Blast::Init()
{
//	m_pUnitCube = shared_ptr<CUnitCube>( new CUnitCube );
//	m_pUnitCube->Init();
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

//	float fCurrentBlastRadius
//		= m_fMaxBlastRadius * rfCurrentBlastTime / m_fBlastDuration;

	float fCurrentBlastRadius
		= m_fLinearExpansionSpeed * rfCurrentBlastTime;
	Limit( fCurrentBlastRadius, 0.0f, m_fMaxBlastRadius );

	// save blast radius
	pCopyEnt->f3 = fCurrentBlastRadius;

	// check if there is any entity which is overlapping the bounding sphere
	// of the current blast (rough estimate)
	CTrace tr;
	STrace tr2;
	static vector<CCopyEntity *> s_vecpEntityBuffer;

	s_vecpEntityBuffer.resize( 0 );
	tr.SetTouchEntityBuffer( &s_vecpEntityBuffer );
	tr.SetSphere( pCopyEnt->GetWorldPosition(), fCurrentBlastRadius );
	tr.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );	// skip checking the no-clip entities
	m_pStage->CheckCollision( tr );

	// check if the candidates are really taking the blast
	SGameMessage msg;
	msg.sender = pCopyEnt->Self();	// sender is the blast;
	msg.effect = GM_DAMAGE;
	msg.s1 = DMG_BLAST;
	msg.fParam1 = m_fBaseDamage * fBlastFrameTime;	// amount of damage
	int iNumOvelappingEntities = (int)tr.GetNumTouchEntities();
	CCopyEntity* pTouchedEntity;

//	LOG_PRINTF(( " entity id: %d - Checking overlapping entities (%d) ", pCopyEnt->GetID(), iNumOvelappingEntities ));

//	tr2.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );
	tr2.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr2.bvType = BVTYPE_DOT;
//	tr2.GroupIndex = pCopyEnt->GroupIndex;

	for(int i=0; i<iNumOvelappingEntities; i++)
	{
//		Release Build: At least reached here
//		LOG_PRINT( "Found an entity in blast range." );

		if( tr.GetTouchEntity(i) == pCopyEnt )
			continue;	// myself

		tr2.fFraction = 1.0f;
		tr2.iNumTouches = 0;
		tr2.pSourceEntity = pCopyEnt;
		tr2.pTouchedEntity = NULL;
		Vector3 vStart = pCopyEnt->GetWorldPosition();
		tr2.pvStart = &vStart;
		Vector3 vGoal = tr.GetTouchEntity(i)->GetWorldPosition();
		tr2.pvGoal = &vGoal;


		// check if there is any obstacle between the blast center and the candidate
		m_pStage->ClipTrace( tr2 );

		if( tr2.pTouchedEntity == 0 )
			continue;

//		LOG_PRINT( "entity in valid blast range" );

		pTouchedEntity = tr2.pTouchedEntity;

		if( pTouchedEntity != tr.GetTouchEntity(i) )
			continue;	// candidate is protected by another entity

		if( pTouchedEntity->bNoClip )
			continue;	// blast gives no damage to no-clip entities

//		LOG_PRINTF(( " Sending blast damge to an entity (name: %s)", pTouchedEntity->GetName().c_str() ));

		// no oblstacle covering the entity from the blast
		Vector3 vBlastCenterToTarget = pTouchedEntity->GetWorldPosition() - pCopyEnt->GetWorldPosition();
		Vec3Normalize( vBlastCenterToTarget, vBlastCenterToTarget );
		msg.vParam = vBlastCenterToTarget;

//		if( !(pTouchedEntity->EntityFlag & BETYPE_RIGIDBODY) )	// avoid damaging rigid bodies to see the effect of blast 
			SendGameMessageTo( msg, pTouchedEntity );


		if( pTouchedEntity->GetEntityFlags() & BETYPE_RIGIDBODY )
		{
//			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * 10.0f, tr2.GetEndPosition() );
//			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * 10.0f, tr2.vEnd );
			pTouchedEntity->ApplyWorldImpulse( vBlastCenterToTarget * m_fImpulse * fBlastFrameTime, tr2.vEnd );
		}
		else if( pTouchedEntity->GetEntityFlags() & BETYPE_PLAYER )
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
	Matrix44 matWorld = ToMatrix44( pCopyEnt->GetWorldPose() );

	// show the center position of the blast ( in black )
	FixedFunctionPipelineManager().SetWorldTransform( matWorld * Matrix44Scaling(0.2f,0.2f,0.2f) );

//	m_pUnitCube->SetUniformColor( 0.0f, 0.0f, 0.0f, 0.9f );
//	m_pUnitCube->Draw();

	// show the blast range ( in red )
	float s = CurrentBlastRadius(pCopyEnt) * 2.0f;
	FixedFunctionPipelineManager().SetWorldTransform( matWorld * Matrix44Scaling(s,s,s) );

//	m_pUnitCube->SetUniformColor( 1.0f, 0.0f, 0.0f, 0.6f );
//	m_pUnitCube->Draw();
}


bool CBE_Blast::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( scanner.TryScanLine( "BASEDAMAGE",      m_fBaseDamage ) ) return true;
	if( scanner.TryScanLine( "MAX_RADIUS",      m_fMaxBlastRadius ) ) return true;
	if( scanner.TryScanLine( "DURATION",        m_fBlastDuration ) ) return true;
	if( scanner.TryScanLine( "EXPANSION_SPEED",	m_fLinearExpansionSpeed) ) return true;
	if( scanner.TryScanLine( "IMPULSE",         m_fImpulse ) ) return true;

	return false;
}


void CBE_Blast::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_fBaseDamage;
	ar & m_fMaxBlastRadius;
	ar & m_fBlastDuration;
	ar & m_fLinearExpansionSpeed;
	ar & m_fImpulse;
}
