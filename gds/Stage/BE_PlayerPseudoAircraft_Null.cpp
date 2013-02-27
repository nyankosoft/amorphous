#include "BE_PlayerPseudoAircraft.hpp"


namespace amorphous
{



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


} // amorphous
