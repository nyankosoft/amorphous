#ifndef  __CCOPYENTITYCALLBACK_H__
#define  __CCOPYENTITYCALLBACK_H__


class CCopyEntity;


class CCopyEntityCallbackBase
{
public:

	CCopyEntityCallbackBase() {}

	virtual ~CCopyEntityCallbackBase() {}

	virtual void OnCopyEntityDestroyed( CCopyEntity *pEntity ) {}

//	virtual void OnCopyEntityReceivedMessage( CCopyEntity* pEntity, const SGameMessage& msg );

//	virtual void OnCopyEntityHitByAnother( CCopyEntity* pSelf, CCopyEntity* pOther );

};


#endif  /*  __CCOPYENTITYCALLBACK_H__  */