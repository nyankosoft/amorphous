#ifndef __BASEENTITYBLAST_H__
#define __BASEENTITYBLAST_H__

#include "BaseEntity.h"


class CUnitCube;

class CBE_Blast : public CBaseEntity
{
	float m_fBaseDamage;
	float m_fMaxBlastRadius;	///< how large the blast expands. blast is a spherical volume
	float m_fBlastDuration;		///< how long the blast lasts. longer duration means that the blast expands more slowly

	/// impulse given to the entity hit by the blast
	/// Impulse is calculated every frame as m_vImpulse * frametime
	/// and applied to the entity if it overlaps with the blast volume
	float m_fImpulse;

	CUnitCube *m_pUnitCube;	// draw boundary for debug

public:
	CBE_Blast();
	~CBE_Blast();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Draw(CCopyEntity* pCopyEnt);
	//void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	//void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_BLAST; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};


#endif	/*  __BASEENTITYBLAST_H__  */
