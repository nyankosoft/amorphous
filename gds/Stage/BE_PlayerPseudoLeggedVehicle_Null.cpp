
#include "BE_PlayerPseudoLeggedVehicle.hpp"
#include "PlayerInfo.hpp"
#include "EntitySet.hpp"
#include "CopyEntityDesc.hpp"
#include "stage.h"
#include "Graphics/fps.h"
#include "Input/InputHandler_PlayerShip.hpp"
#include "Item/WeaponSystem.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "GameMessage.hpp"

#include "3DMath/Vector3.hpp"

#include "GameCommon/BasicGameMath.hpp"
#include "GameCommon/GameMathMisc.hpp"
#include "GameCommon/MTRand.hpp"
#include "GameCommon/RandomDirectionTable.hpp"
#include "GameCommon/ShockWaveCameraEffect.hpp"

//#include "Stage/bsptree.hpp"
#include "Stage/BE_LaserDot.hpp"

// added for laser dot casting test
#include "Graphics/3DGameMath.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Support/memory_helpers.hpp"
#include "trace.hpp"

#include "JigLib/JL_PhysicsActor.hpp"
#include "JigLib/JL_LineSegment.hpp"


//================================================================================
// CVehicleLeg
//================================================================================

CVehicleLeg::CVehicleLeg( Vector3 vLocalPos, Vector3 vLocalDir, float fLength )
: m_vLocalPos(vLocalPos), m_vLocalDir(vLocalDir), m_fLength(fLength)
{
}


CBE_PlayerPseudoLeggedVehicle::~CBE_PlayerPseudoLeggedVehicle()
{
}


void CVehicleLeg::Update( CCopyEntity *pEntity, float dt )
{
}



//================================================================================
// CBE_PlayerPseudoLeggedVehicle
//================================================================================

CBE_PlayerPseudoLeggedVehicle::CBE_PlayerPseudoLeggedVehicle()
{
}

void CBE_PlayerPseudoLeggedVehicle::Init()
{
	CBE_Player::Init();
}

void CBE_PlayerPseudoLeggedVehicle::LimitOrientation( CCopyEntity *pCopyEnt )
{
}

void CBE_PlayerPseudoLeggedVehicle::Move( CCopyEntity *pCopyEnt )
{
}

void CBE_PlayerPseudoLeggedVehicle::UpdateCamera(CCopyEntity* pCopyEnt)
{
}

void CBE_PlayerPseudoLeggedVehicle::UpdatePhysics( CCopyEntity *pCopyEnt, float dt )
{
}

bool CBE_PlayerPseudoLeggedVehicle::HandleInput( SPlayerEntityAction& input )
{
	return false;
}

void CBE_PlayerPseudoLeggedVehicle::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Player::Serialize( ar, version );
}
