#include "BE_PlayerShip.hpp"


namespace amorphous
{



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


} // amorphous
