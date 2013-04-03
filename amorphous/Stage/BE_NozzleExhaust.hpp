#ifndef __BE_NOZZLEEXHAUST_H__
#define __BE_NOZZLEEXHAUST_H__

#include "BE_ParticleSet.hpp"

#include "BaseEntityHandle.hpp"


namespace amorphous
{

#define CBE_MUZZLEFLASH_NUM_RAND_POSITIONS	32
#define CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS	32

class CBE_NozzleExhaust : public CBE_ParticleSet
{
private:

//	float m_fMuzzleFlash_MaxRadius;

	Vector3 m_avRandDir_Circle[CBE_MUZZLEFLASH_NUM_RAND_DIRECTIONS];

	float m_afRandPosisionTable[CBE_MUZZLEFLASH_NUM_RAND_POSITIONS];

	BaseEntityHandle m_MuzzleFlashLight;

	void UpdateNozzleExhaust( CCopyEntity* pCopyEnt, SBE_ParticleSetExtraData& rParticleSet, float dt );

    void CreateNewParticle( CCopyEntity* pCopyEnt, SBE_ParticleSetExtraData& rParticleSet, int index );

protected:
		
	/**
	* f1 - local nozzle flame start position
	* f3 - nozzle flame length
	* f4 - nozzle flame radius at the root
	* NOTE: When you change mappings of these variables,
	* update desc value settings in CBEC_AircraftNozzleFlames::CreateNozzleFlames()
	*/
	inline float& NozzleFlameStartPos( CCopyEntity* pCopyEnt )	{ return pCopyEnt->f1; }
	inline float& NozzleFlameLength( CCopyEntity* pCopyEnt )	{ return pCopyEnt->f3; }
	inline float& NozzleFlameRadius( CCopyEntity* pCopyEnt )	{ return pCopyEnt->f4; }
	inline float& OverlapTime( CCopyEntity* pCopyEnt )			{ return pCopyEnt->f5; }

public:

	CBE_NozzleExhaust();
	virtual ~CBE_NozzleExhaust()
	{
	}

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	// behavior in one frame
	void Draw(CCopyEntity* pCopyEnt);
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_NOZZLEEXHAUST; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

};
} // namespace amorphous



#endif  /*  __BE_NOZZLEEXHAUST_H__  */
