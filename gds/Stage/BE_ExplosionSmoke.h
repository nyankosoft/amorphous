#ifndef __BE_EXPLOSIONSMOKE_H__
#define __BE_EXPLOSIONSMOKE_H__

#include "BE_ParticleSet.h"

#define TYPE_SMOKE_NORMAL		1
#define TYPE_SMOKE_DIRECTIONAL	2
#define TYPE_SMOKE_SHOOTING		3


class CBE_ExplosionSmoke : public CBE_ParticleSet
{
private:

	float m_fRandPositionMin, m_fRandPositionMax;
	float m_fRandVelocityMin, m_fRandVelocityMax;

public:

	CBE_ExplosionSmoke();
	virtual ~CBE_ExplosionSmoke()
	{
	}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

//	void Act(CCopyEntity* pCopyEnt);	// behavior in one frame
	void Draw(CCopyEntity* pCopyEnt);
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_EXPLOSIONSMOKE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};

#endif  /*  __BE_EXPLOSIONSMOKE_H__  */
