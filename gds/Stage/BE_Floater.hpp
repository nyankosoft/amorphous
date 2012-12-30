#ifndef __BE_Floater_H__
#define __BE_Floater_H__

#include "BE_Enemy.hpp"


namespace amorphous
{

class CBE_Floater : public CBE_Enemy
{
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
} // namespace amorphous



#endif  /*  __BE_Floater_H__  */
