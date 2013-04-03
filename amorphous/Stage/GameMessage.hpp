#ifndef  __GAMEMESSAGE_H__
#define  __GAMEMESSAGE_H__

#include "amorphous/3DMath/Vector3.hpp"
#include "CopyEntity.hpp"


namespace amorphous
{


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


struct GameMessage
{
	int effect;
	std::string effect_name;
	short s1;
	float fParam1;
	float fParam2;
	Vector3 vParam;
	char *pcStrParam;
	CCopyEntity* pEntity0;
	EntityHandle<> sender;

	void *pUserData;

	inline GameMessage( int _effect = GM_INVALID )
		:
	effect(_effect),
	s1(0),
	fParam1(0),
	fParam2(0),
	vParam(Vector3(0,0,0))
	{}

	inline ~GameMessage() {}
};


//extern void SendGameMessageTo(GameMessage& rGameMessage, CCopyEntity* pCopyEnt);

/// \param target [in] recipient of the message
inline void SendGameMessageTo(GameMessage& rGameMessage, EntityHandle<> target )
{
	boost::shared_ptr<CCopyEntity> pEntity = target.Get();

	if( pEntity )
		pEntity->GetBaseEntity()->MessageProcedure( rGameMessage, pEntity.get() );
}


/// This function is deprecated.
/// \param pTarget [in] recipient of the message
inline void SendGameMessageTo(GameMessage& rGameMessage, CCopyEntity* pTarget )
{
	if( !IsValidEntity(pTarget) )
		return;

	EntityHandle<> target( pTarget->Self() );

	SendGameMessageTo( rGameMessage, target );
}


} // namespace amorphous



#endif		/*  __GAMEMESSAGE_H__  */
