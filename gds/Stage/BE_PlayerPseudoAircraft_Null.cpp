
#include "BE_PlayerPseudoAircraft.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "stage.h"
#include "Input/InputHandler_PlayerPAC.hpp"
#include "Item/WeaponSystem.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameMessage.hpp"

#include "3DMath/Vector3.hpp"

#include "GameCommon/BasicGameMath.hpp"
#include "GameCommon/GameMathMisc.hpp"
#include "GameCommon/MTRand.hpp"
#include "GameCommon/RandomDirectionTable.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"
#include "GameCommon/PseudoAircraftSimulator.hpp"

#include "Graphics/3DGameMath.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Support/memory_helpers.hpp"
#include "trace.hpp"

#include "JigLib/JL_PhysicsActor.hpp"
#include "JigLib/JL_LineSegment.hpp"



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
