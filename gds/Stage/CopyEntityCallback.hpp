#ifndef  __CCOPYENTITYCALLBACK_H__
#define  __CCOPYENTITYCALLBACK_H__


#include "gds/Physics/fwd.hpp"


class CCopyEntity;


class CCopyEntityCallbackBase
{
public:

	CCopyEntityCallbackBase() {}

	virtual ~CCopyEntityCallbackBase() {}

	virtual void OnCopyEntityDestroyed( CCopyEntity *pEntity ) {}

	virtual void OnPhysicsTrigger( physics::CShape& my_shape, CCopyEntity& other_entity, physics::CShape& other_shape, U32 trigger_flags ) {}

	virtual void OnPhysicsContact( physics::CContactPair& pair, CCopyEntity& other_entity ) {}

//	virtual void OnCopyEntityReceivedMessage( CCopyEntity* pEntity, const SGameMessage& msg );

//	virtual void OnCopyEntityHitByAnother( CCopyEntity* pSelf, CCopyEntity* pOther );

};


#endif  /*  __CCOPYENTITYCALLBACK_H__  */