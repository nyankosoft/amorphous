
#include "BE_PlayerPseudoAircraft.h"
#include "PlayerInfo.h"
#include "EntitySet.h"
#include "CopyEntityDesc.h"
#include "stage.h"
#include "Input/InputHandler_PlayerPAC.h"
#include "Item/WeaponSystem.h"
#include "Stage/ScreenEffectManager.h"
#include "GameMessage.h"

#include "3DMath/Vector3.h"

#include "GameCommon/BasicGameMath.h"
#include "GameCommon/GameMathMisc.h"
#include "GameCommon/MTRand.h"
#include "GameCommon/RandomDirectionTable.h"
#include "GameCommon/ShockWaveCameraEffect.h"
#include "GameCommon/PseudoAircraftSimulator.h"

#include "Graphics/3DGameMath.h"
#include "Graphics/Direct3D9.h"
#include "Graphics/D3DXMeshObject.h"
#include "Support/memory_helpers.h"
#include "trace.h"

#include "JigLib/JL_PhysicsActor.h"
#include "JigLib/JL_LineSegment.h"



//================================================================================
// CBE_PlayerPseudoAircraft
//================================================================================

CBE_PlayerPseudoAircraft::CBE_PlayerPseudoAircraft()
{
}


CBE_PlayerPseudoAircraft::~CBE_PlayerPseudoAircraft()
{
}


void CBE_PlayerPseudoAircraft::Init()
{
	CBE_Player::Init();
}


void CBE_PlayerPseudoAircraft::InitCopyEntity( CCopyEntity *pCopyEnt )
{
}


void CBE_PlayerPseudoAircraft::Act( CCopyEntity *pCopyEnt )
{
}


void CBE_PlayerPseudoAircraft::Draw( CCopyEntity *pCopyEnt )
{
}


void CBE_PlayerPseudoAircraft::Move( CCopyEntity *pCopyEnt )
{
}


void CBE_PlayerPseudoAircraft::UpdateCamera(CCopyEntity* pCopyEnt)
{
}


void CBE_PlayerPseudoAircraft::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
}


bool CBE_PlayerPseudoAircraft::HandleInput( SPlayerEntityAction& input )
{
	return false;
}


void CBE_PlayerPseudoAircraft::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
}


void CBE_PlayerPseudoAircraft::ReleaseGraphicsResources()
{
}


void CBE_PlayerPseudoAircraft::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );
}
