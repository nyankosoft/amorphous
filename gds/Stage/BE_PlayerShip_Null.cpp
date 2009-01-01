
#include "BE_PlayerShip.h"
#include "PlayerInfo.h"
#include "EntitySet.h"
#include "CopyEntityDesc.h"
#include "stage.h"
#include "Input/InputHandler_PlayerShip.h"
#include "Item/WeaponSystem.h"
#include "Stage/ScreenEffectManager.h"
#include "GameMessage.h"

#include "3DMath/Vector3.h"

#include "GameCommon/BasicGameMath.h"
#include "GameCommon/MTRand.h"
#include "GameCommon/RandomDirectionTable.h"
#include "GameCommon/ShockWaveCameraEffect.h"

#include "Stage/BE_LaserDot.h"

// added for laser dot casting test
#include "Graphics/Direct3D9.h"
#include "Graphics/D3DXMeshObject.h"
#include "Support/memory_helpers.h"
#include "trace.h"

#include "JigLib/JL_PhysicsActor.h"



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
