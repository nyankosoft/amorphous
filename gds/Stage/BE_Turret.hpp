#ifndef	__BASEENTITYTURRET_H__
#define __BASEENTITYTURRET_H__

#include "BE_Enemy.hpp"


class CBE_Turret : public CBE_Enemy
{

public:
	CBE_Turret();
	// ~CBE_Turret();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	// void Act(CCopyEntity* pCopyEnt);
	// void Draw(CCopyEntity* pCopyEnt);
	// void SearchPlayer(CCopyEntity* pCopyEnt);
	// void Fire(CCopyEntity* pCopyEnt);
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	// void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_TURRET; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};

#endif /*  __BASEENTITYTURRET_H__  */
