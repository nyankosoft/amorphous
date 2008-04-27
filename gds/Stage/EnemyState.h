#ifndef  __ENEMYSTATE_H__
#define  __ENEMYSTATE_H__

class CCopyEntity;
class CBE_Enemy;

class CEnemyState
{
public:

	enum eEnemyState
	{
		STATE_SEARCH = 0,
		STATE_ALERT,
		STATE_ATTACK,
		STATE_UNDERATTACK,
		NUM_STATES,
	};

	CEnemyState();
	
	virtual ~CEnemyState();

	virtual void Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity ) = 0;
};


class CES_Search : public CEnemyState
{
public:
	void Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity );
};


class CES_Attack : public CEnemyState
{
public:
	void Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity );
};


class CES_UnderAttack : public CEnemyState
{
public:
	void Act( CCopyEntity& rEntity, CBE_Enemy& rBaseEntity );
};


#endif		/*  __ENEMYSTATE_H__  */