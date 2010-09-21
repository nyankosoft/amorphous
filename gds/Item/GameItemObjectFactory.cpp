#include "GameItemObjectFactory.hpp"

#include "GameItem.hpp"
#include "GI_Weapon.hpp"
#include "GI_Ammunition.hpp"
#include "GI_GravityGun.hpp"
#include "GI_Aircraft.hpp"
#include "GI_MissileLauncher.hpp"
#include "Radar.hpp"
#include "LandVehicle.hpp"
#include "RotatableTurret.hpp"
#include "Firearm.hpp"
#include "Clothing.hpp"


using namespace item;


CGameItem *CGameItemObjectFactory::CreateGameItem( const unsigned int id )
{
	switch( id )
	{
	case CGameItem::ID_GRAVITY_GUN:		 return new CGI_GravityGun;
	case CGameItem::ID_BINOCULAR:		 return new CGI_Binocular;
	case CGameItem::ID_NIGHT_VISION:	 return new CGI_NightVision;
	case CGameItem::ID_AMMUNITION:		 return new CGI_Ammunition;
	case CGameItem::ID_WEAPON:           return new CGI_Weapon;
	case CGameItem::ID_KEY:				 return new CGI_Key;
	case CGameItem::ID_SUPPRESSOR:		 return new CGI_Suppressor;
	case CGameItem::ID_CAMFLOUGE_DEVICE: return new CGI_CamouflageDevice;
	case CGameItem::ID_AIRCRAFT:		 return new CGI_Aircraft;
	case CGameItem::ID_MISSILELAUNCHER:	 return new CGI_MissileLauncher;
	case CGameItem::ID_RADAR:			 return new CRadar;
	case CGameItem::ID_ROTATABLE_TURRET: return new CRotatableTurret;
	case CGameItem::ID_LAND_VEHICLE:	 return new CLandVehicle;
	case CGameItem::ID_ARMED_VEHICLE:	 return new CArmedVehicle;
	case CGameItem::ID_FIREARM:          return new CFirearm;
	case CGameItem::ID_CLOTHING:         return new CClothing;
//	case CGameItem::ID_:				return new CGI_;
	default:	return NULL;
	}
}

IArchiveObjectBase *CGameItemObjectFactory::CreateObject( const unsigned int id )
{
	return CreateGameItem( id );
}
