#include "BE_StaticGeometry.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"
#include "Support/Profile.hpp"

#include "StaticGeometryBase.hpp"
#include "ScreenEffectManager.hpp"


namespace amorphous
{

using namespace std;


CBE_StaticGeometry::CBE_StaticGeometry()
{
	m_aabb = AABB3( Vector3( -1000000, -1000000, -1000000 ),
		            Vector3(  1000000,  1000000,  1000000 ) );

	m_fRadius = 1500000;

	m_BoundingVolumeType = BVTYPE_AABB;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	SetLighting( false );
}


CBE_StaticGeometry::~CBE_StaticGeometry()
{
}


void CBE_StaticGeometry::Init()
{
}


void CBE_StaticGeometry::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->iExtraDataIndex = (int)m_vecpStaticGeometry.size();//GetNewExtraDataID();

	m_vecpStaticGeometry.push_back( (CStaticGeometryBase *)(pCopyEnt->pUserData) );

	Vector3 vCenterPos = pCopyEnt->GetWorldPosition();

	// static geometry has load been loaded yet.
	// - called from CStaticGeometry::LoadFromFile()
//	m_vecpStaticGeometry.back()->CreateCollisionGeometry( *(m_pStage->GetPhysicsScene()) );
}


void CBE_StaticGeometry::Act(CCopyEntity* pCopyEnt)
{
}	//behavior in in one frame


void CBE_StaticGeometry::Draw(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	CStaticGeometryBase* pStaticGeometry = m_vecpStaticGeometry[pCopyEnt->iExtraDataIndex];
	
	Camera* pCamera = m_pStage->GetCurrentCamera();
	if( !pCamera )
	{
		ONCE( LOG_PRINT( " - cannot find a camera for the stage" ) );
		return;
	}

	pStaticGeometry->Render( *pCamera, m_pStage->GetScreenEffectManager()->GetEffectFlag() );
}


void CBE_StaticGeometry::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
}


void CBE_StaticGeometry::ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself )
{
}


void CBE_StaticGeometry::MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}


CStaticGeometryBase *CBE_StaticGeometry::GetStaticGeometry( CCopyEntity* pEntity )
{
	if( !pEntity )
		return NULL;

	int index = pEntity->iExtraDataIndex;
	if( 0 <= index && index < (int)m_vecpStaticGeometry.size() )
	{
		return m_vecpStaticGeometry[pEntity->iExtraDataIndex];
	}
	else
		return NULL;
}


bool CBE_StaticGeometry::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_StaticGeometry::Serialize( IArchive& ar, const unsigned int version )
{
	BaseEntity::Serialize( ar, version );
}


} // namespace amorphous
