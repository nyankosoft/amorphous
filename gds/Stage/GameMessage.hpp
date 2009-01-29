#ifndef  __GAMEMESSAGE_H__
#define  __GAMEMESSAGE_H__

#include "3DMath/Vector3.h"

#include "CopyEntity.h"


enum eGameMessageEffect
{
	GM_DAMAGE = 0,
	GM_DESTROYED,
	GM_IMPACT,
	GM_HEALING,
	GM_AMMOSUPPLY,
	GM_EFFECTACCEPTED,
	GM_DOORKEYITEM,
	GM_KEYCODE_REQUEST,
	GM_KEYCODE_INPUT,
	GM_DOOR_TOUCHED,
	GM_TERMINATE,
	GM_ENTITY_DETECTED_BY_SENSOR,
	GM_SET_MOTION_PATH,
	GM_SET_DEFAULT_CAMERA_PARAM,
	GM_SET_CAMERA_PARAM,
	GM_SET_SCRIPTCAMERAKEYFRAMES,
	GM_SET_TARGET,
	GM_MISSILE_TARGET,
	NUM_GAME_MESSAGES
};


enum eDamageType
{
	DMG_BULLET = 0,
	DMG_BLAST,
	DMG_XXX,
	NUM_DAMAGE_TYPES
};


class CCopyEntity;

struct SGameMessage
{
	int iEffect;
	short s1;
	float fParam1;
	float fParam2;
	Vector3 vParam;
	char *pcStrParam;
	CCopyEntity* pEntity0;
	CCopyEntity* pSenderEntity;
	void *pUserData;

	inline SGameMessage() {}
	inline SGameMessage( int effect ) : iEffect(effect) {}
	inline ~SGameMessage() {}
};


//extern void SendGameMessageTo(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt);

inline void SendGameMessageTo(SGameMessage& rGameMessage, CCopyEntity* pEntity )
{
	if( !IsValidEntity(pEntity) )
		return;

	pEntity->GetBaseEntity()->MessageProcedure( rGameMessage, pEntity );
}



#endif		/*  __GAMEMESSAGE_H__  */