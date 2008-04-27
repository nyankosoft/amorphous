#include "BE_StaticGeometry.h"

#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
//#include "3DCommon/Direct3D9.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"
#include "Support/Profile.h"

#include "StaticGeometryBase.h"
#include "ScreenEffectManager.h"

using namespace std;


CBE_StaticGeometry::CBE_StaticGeometry()
{
	m_aabb = AABB3( Vector3( -1000000, -1000000, -1000000 ),
		            Vector3(  1000000,  1000000,  1000000 ) );

	m_fRadius = 1500000;

	m_BoundingVolumeType = BVTYPE_AABB;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_bLighting = false;
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

	Vector3 vCenterPos = pCopyEnt->Position();

}


void CBE_StaticGeometry::Act(CCopyEntity* pCopyEnt)
{
}	//behavior in in one frame


void CBE_StaticGeometry::Draw(CCopyEntity* pCopyEnt)
{
	PROFILE_FUNCTION();

	CStaticGeometryBase* pStaticGeometry = m_vecpStaticGeometry[pCopyEnt->iExtraDataIndex];
	
	CCamera* pCamera = m_pStage->GetCurrentCamera();
	if( !pCamera )
	{
		ONCE( g_Log.Print( "CBE_StaticGeometry::Draw() - cannot find a camera for the stage" ) );
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


void CBE_StaticGeometry::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}


bool CBE_StaticGeometry::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_StaticGeometry::Serialize( IArchive& ar, const unsigned int version )
{
}
