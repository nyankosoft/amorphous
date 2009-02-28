#include "GameItemObjectFactory.hpp"

#include "GameItem.hpp"
#include "GI_Weapon.hpp"
#include "GI_Ammunition.hpp"
#include "GI_GravityGun.hpp"
#include "GI_Aircraft.hpp"
#include "GI_MissileLauncher.hpp"


CGameItem *CGameItemObjectFactory::CreateGameItem( const unsigned int id )
{
	switch( id )
	{
	case CGameItem::ID_GRAVITY_GUN:		return new CGI_GravityGun;
	case CGameItem::ID_BINOCULAR:		return new CGI_Binocular;
	case CGameItem::ID_NIGHT_VISION:	return new CGI_NightVision;
	case CGameItem::ID_AMMUNITION:		return new CGI_Ammunition;
	case CGameItem::ID_FIREARMS:		return new CGI_Weapon;
	case CGameItem::ID_KEY:				return new CGI_Key;
	case CGameItem::ID_SUPPRESSOR:		return new CGI_Suppressor;
	case CGameItem::ID_CAMFLOUGE_DEVICE:return new CGI_CamouflageDevice;
	case CGameItem::ID_AIRCRAFT:		return new CGI_Aircraft;
	case CGameItem::ID_MISSILELAUNCHER:	return new CGI_MissileLauncher;
//	case CGameItem::ID_:				return new CGI_;
	default:	return NULL;
	}
}

IArchiveObjectBase *CGameItemObjectFactory::CreateObject( const unsigned int id )
{
	return CreateGameItem( id );
}
