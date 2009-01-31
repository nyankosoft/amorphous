#include "BE_PlayerShip.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "stage.h"
#include "Input/InputHandler_PlayerShip.hpp"
#include "Item/WeaponSystem.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameMessage.hpp"

#include "3DMath/Vector3.hpp"

#include "GameCommon/MTRand.hpp"
#include "GameCommon/RandomDirectionTable.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"

#include "Stage/BE_LaserDot.hpp"

// added for laser dot casting test
#include "Graphics/Direct3D9.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Support/memory_helpers.hpp"
#include "trace.hpp"

#include "JigLib/JL_PhysicsActor.hpp"



//#define APPLY_PHYSICS_TO_PLAYER_SHIP


CBE_PlayerShip::CBE_PlayerShip()
{
}


CBE_PlayerShip::~CBE_PlayerShip()
{
}


void CBE_PlayerShip::Init()
{
	CBE_Player::Init();
}


void CBE_PlayerShip::InitCopyEntity(CCopyEntity* pCopyEnt)
{
}


void CBE_PlayerShip::Move( CCopyEntity *pCopyEnt )
{
}


bool CBE_PlayerShip::HandleInput( SPlayerEntityAction& input )
{
	return false;
}


void CBE_PlayerShip::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );
}
