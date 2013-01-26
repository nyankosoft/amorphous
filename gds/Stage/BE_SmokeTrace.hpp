#ifndef __BE_SMOKETRACE_H__
#define __BE_SMOKETRACE_H__


#include "BE_ParticleSet.hpp"


namespace amorphous
{


class CBE_SmokeTrace : public CBE_ParticleSet
{
private:

	/// distance between each billboard
	/// used when eSmokeType ==ST_PARTICLES_PER_DISTANCE 
	float m_fParticleInterval;

	/// number of particle billboards generated every second
	/// used when eSmokeType == ST_PARTICLES_PER_TIME
	int m_iNumParticlesPerSec;

	/// handle to myself - used to create smoke object one after another
	CBaseEntityHandle m_SmokeTrace;

	/// decides the condition on which new particles are created
	unsigned int m_SmokeType;

	/// the amount of random position shifts for the newly created particle.
	/// applied to each component of xyz
    float m_fSmokeTraceParticleRand;

	enum eSmokeType
	{
		ST_PARTICLES_PER_DISTANCE,
		ST_PARTICLES_PER_TIME,
	};


	void MakeSmoke( CCopyEntity* pCopyEnt );

	void MakeSmokeTrace( CCopyEntity* pCopyEnt );

public:

	CBE_SmokeTrace();
	~CBE_SmokeTrace();

	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );

	void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	void Draw(CCopyEntity* pCopyEnt);
	// void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SMOKETRACE; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	void LoadGraphicsResources( const GraphicsParameters& rParam );
//	void ReleaseGraphicsResources();


/*	int GetNewExtraDataID()
	{
		if( m_iParticleSetCurrentIndex < NUM_MAX_PARTICLESETS )
			return m_iParticleSetCurrentIndex++;
		else
		{
			m_iParticleSetCurrentIndex = 0;
			return m_iParticleSetCurrentIndex++;
		}
	}

	SBE_ParticleSetExtraData& GetExtraData(int iExtraDataIndex)
	{
		return m_aParticleSet[iExtraDataIndex];
	}*/

};
} // namespace amorphous



#endif  /*  __BE_SMOKETRACE_H__  */
