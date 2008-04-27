
#include "BE_PlayerPseudoLeggedVehicle.h"
#include "PlayerInfo.h"
#include "EntitySet.h"
#include "CopyEntityDesc.h"
#include "stage.h"
#include "3DCommon/fps.h"
#include "Input/InputHandler_PlayerShip.h"
#include "Item/WeaponSystem.h"
#include "Stage/ScreenEffectManager.h"
#include "GameMessage.h"

#include "3DMath/Vector3.h"

#include "GameCommon/BasicGameMath.h"
#include "GameCommon/GameMathMisc.h"
#include "GameCommon/MTRand.h"
#include "GameCommon/RandomDirectionTable.h"
#include "GameCommon/ShockWaveCameraEffect.h"

//#include "Stage/bsptree.h"
#include "Stage/BE_LaserDot.h"

// added for laser dot casting test
#include "3DCommon/3DGameMath.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DXMeshObject.h"
#include "Support/memory_helpers.h"
#include "trace.h"

#include "JigLib/JL_PhysicsActor.h"
#include "JigLib/JL_LineSegment.h"


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
