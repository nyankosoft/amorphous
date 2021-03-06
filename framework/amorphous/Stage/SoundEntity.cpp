#include "SoundEntity.hpp"
#include "Stage.hpp"
#include "GameMessage.hpp"
#include "trace.hpp"
#include "../Sound/SoundManager.hpp"


namespace amorphous
{

using namespace std;

float sg_fSpeedOfSoundAtSeaLevel = 340.29f;


SoundEntity::SoundEntity()
:
m_fMaxRadius(100.0f),
m_fCurrentRadius(0.0f)
{
}


SoundEntity::~SoundEntity()
{
}


void SoundEntity::Update( float dt )
{
	m_fCurrentRadius += dt * sg_fSpeedOfSoundAtSeaLevel;
	clamp( m_fCurrentRadius, 0.0f, m_fMaxRadius );

	bool sound_attenuation = false;
	if( !sound_attenuation )
	{
		const Camera *pCamera = GetStage()->GetCurrentCamera();
		if( pCamera )
		{
			const Camera& camera = *pCamera;
			const float dist_to_camera = Vec3Length( camera.GetPosition() - GetWorldPosition() );
			if( dist_to_camera <= m_fCurrentRadius )
			{
				float ref_dist = 100.0f;
				GetSoundManager().PlayAt( "resource_path", GetWorldPosition(), m_fMaxRadius, ref_dist );
			}
		}
	}

	// check if there is any entity which is overlapping the bounding sphere
	// of the current blast (rough estimate)
	CTrace tr;
	STrace tr2;
	static vector<CCopyEntity *> s_vecpEntityBuffer;

	s_vecpEntityBuffer.resize( 0 );
	tr.SetTouchEntityBuffer( &s_vecpEntityBuffer );
	tr.SetSphere( GetWorldPosition(), m_fCurrentRadius );
	tr.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );	// skip checking the no-clip entities

	GetStage()->CheckCollision( tr );

	GameMessage msg;
	msg.sender = this->Self();	// sender is the blast;
	msg.effect = GM_SOUND;
//	msg.s1 = DMG_BLAST;
	const int iNumOvelappingEntities = (int)tr.GetNumTouchEntities();
	CCopyEntity* pTouchedEntity = NULL;

	for(int i=0; i<iNumOvelappingEntities; i++)
	{
		if( tr.GetTouchEntity(i) == this )
			continue;	// myself

		CCopyEntity *pTouchedEntity = tr.GetTouchEntity(i);

		const float attenuation_factor = 1.0f;//GetSoundAttenuation( *pTouchedEntity );

		if( fabs(attenuation_factor) < 0.001 )
			continue;

		SendGameMessageTo( msg, pTouchedEntity );
	}

	if( abs( m_fMaxRadius - m_fCurrentRadius ) < 0.001 )
	{
		Terminate();
	}
}


void SoundEntity::Draw()
{
//	CCopyEntity *pParent = m_pParent;
//	if( !pParent )
//		return;

	// copy parent light info
//	m_vecLight = pParent->m_vecLight;

//	this->pBaseEntity->Draw3DModel( this );
}


void SoundEntity::HandleMessage( GameMessage& msg )
{
}


void SoundEntity::TerminateDerived()
{
//	shared_ptr<CCopyEntity> pSelf = this->Self().lock();
//	m_pPool->release( pSelf ); // pSelf is CCopyEntity type pointer!!!
}


} // namespace amorphous
