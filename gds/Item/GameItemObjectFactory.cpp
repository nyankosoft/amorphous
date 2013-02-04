#include "GameItemObjectFactory.hpp"

#include "GameItem.hpp"
#include "MiscGameItems.hpp"
#include "GI_Weapon.hpp"
#include "GI_Ammunition.hpp"
#include "GI_GravityGun.hpp"
#include "GI_Aircraft.hpp"
#include "GI_MissileLauncher.hpp"
#include "Radar.hpp"
#include "LandVehicle.hpp"
#include "RotatableTurret.hpp"
#include "Cartridge.hpp"
#include "Magazine.hpp"
#include "Firearm.hpp"
#include "Clothing.hpp"
#include "SkeletalCharacter.hpp"


namespace amorphous
{


using namespace item;


GameItem *GameItemObjectFactory::CreateGameItem( const unsigned int id )
{
	switch( id )
	{
	case GameItem::ID_GRAVITY_GUN:		  return new GravityGun;
	case GameItem::ID_BINOCULAR:		  return new Binocular;
	case GameItem::ID_NIGHT_VISION:	      return new CGI_NightVision;
	case GameItem::ID_AMMUNITION:		  return new CGI_Ammunition;
	case GameItem::ID_WEAPON:             return new CGI_Weapon;
	case GameItem::ID_KEY:				  return new CGI_Key;
	case GameItem::ID_SUPPRESSOR:		  return new CGI_Suppressor;
	case GameItem::ID_CAMFLOUGE_DEVICE:   return new CGI_CamouflageDevice;
	case GameItem::ID_AIRCRAFT:		      return new CGI_Aircraft;
	case GameItem::ID_MISSILELAUNCHER:	  return new MissileLauncher;
	case GameItem::ID_RADAR:			  return new Radar;
	case GameItem::ID_ROTATABLE_TURRET:   return new CRotatableTurret;
	case GameItem::ID_LAND_VEHICLE:	      return new LandVehicle;
	case GameItem::ID_ARMED_VEHICLE:	  return new ArmedVehicle;
	case GameItem::ID_CARTRIDGE:          return new Cartridge;
	case GameItem::ID_MAGAZINE:           return new Magazine;
	case GameItem::ID_FIREARM:            return new Firearm;
	case GameItem::ID_CLOTHING:           return new Clothing;
	case GameItem::ID_SKELETAL_CHARACTER: return new SkeletalCharacter;
//	case GameItem::ID_:				return new CGI_;
	default:	return NULL;
	}
}

IArchiveObjectBase *GameItemObjectFactory::CreateObject( const unsigned int id )
{
	return CreateGameItem( id );
}


} // namespace amorphous
