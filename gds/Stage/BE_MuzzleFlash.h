#ifndef __BE_MUZZLEFLASH_H__
#define __BE_MUZZLEFLASH_H__

#include "BE_ParticleSet.h"

#include "BaseEntityHandle.h"

#define CBE_MUZZLEFLASH_NUM_RAND_POSITIONS	32
#define CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS	32

class CBE_MuzzleFlash : public CBE_ParticleSet
{
private:

	float m_fMuzzleFlash_MaxRadius;

	Vector3 m_avRandDir_Circle[CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS];

	float m_afRandPosisionTable[CBE_MUZZLEFLASH_NUM_RAND_POSITIONS];

	CBaseEntityHandle m_MuzzleFlashLight;

public:

	CBE_MuzzleFlash();
	virtual ~CBE_MuzzleFlash()
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

	virtual unsigned int GetArchiveObjectID() const { return BE_MUZZLEFLASH; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};

#endif  /*  __BE_MUZZLEFLASH_H__  */
