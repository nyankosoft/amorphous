#ifndef __BE_Floater_H__
#define __BE_Floater_H__

#include "BE_Enemy.h"

class CBE_Floater : public CBE_Enemy
{
private:
//	char m_acTurretName[32];	// name of the base entity which is used as a weapon for this floater

public:

	CBE_Floater();
	// ~CBE_Floater();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	// void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	// void Draw(CCopyEntity* pCopyEnt); 
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_FLOATER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	void SearchManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
	void AttackManeuver(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
//	void ManeuverUnderAttack(CCopyEntity* pCopyEnt, SBE_EnemyExtraData *pExtraData);
};

#endif  /*  __BE_Floater_H__  */