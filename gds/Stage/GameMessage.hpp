#ifndef  __GAMEMESSAGE_H__
#define  __GAMEMESSAGE_H__

#include "gds/3DMath/Vector3.hpp"
#include "CopyEntity.hpp"


enum eGameMessageEffect
{
	GM_INVALID = -1,
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
	GM_SOUND,
	NUM_GAME_MESSAGES
};


enum eDamageType
{
	DMG_BULLET = 0,
	DMG_BLAST,
	DMG_XXX,
	NUM_DAMAGE_TYPES
};


struct SGameMessage
{
	int effect;
	std::string effect_name;
	short s1;
	float fParam1;
	float fParam2;
	Vector3 vParam;
	char *pcStrParam;
	CCopyEntity* pEntity0;
	CEntityHandle<> sender;

	void *pUserData;

	inline SGameMessage( int _effect = GM_INVALID )
		:
	effect(_effect),
	s1(0),
	fParam1(0),
	fParam2(0),
	vParam(Vector3(0,0,0))
	{}

	inline ~SGameMessage() {}
};


//extern void SendGameMessageTo(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt);

/// \param target [in] recipient of the message
inline void SendGameMessageTo(SGameMessage& rGameMessage, CEntityHandle<> target )
{
	boost::shared_ptr<CCopyEntity> pEntity = target.Get();

	if( pEntity )
		pEntity->GetBaseEntity()->MessageProcedure( rGameMessage, pEntity.get() );
}


/// This function is deprecated.
/// \param pTarget [in] recipient of the message
inline void SendGameMessageTo(SGameMessage& rGameMessage, CCopyEntity* pTarget )
{
	if( !IsValidEntity(pTarget) )
		return;

	CEntityHandle<> target( pTarget->Self() );

	SendGameMessageTo( rGameMessage, target );
}



#endif		/*  __GAMEMESSAGE_H__  */
