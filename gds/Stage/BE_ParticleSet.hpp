#ifndef __BE_PARTICLESET_H__
#define __BE_PARTICLESET_H__


#include <boost/thread/thread.hpp>

#include "BaseEntity.hpp"
#include "BEC_Billboard.hpp"
#include "Stage.hpp"

#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/FloatRGBColor.hpp"
#include "Support/FloatLookUpTable.hpp"
#include "Support/SafeDelete.hpp"


// comment out the next line to draw particles without using vertex buffer and index buffer
//#define BE_PARTICLESET_USE_VB_AND_IB

class ParticleSetFlag
{
public:
	enum eFlag
	{
		POSITION      = (1 << 0 ),
		VELOCITY      = (1 << 1 ),
		ORIG_DIR      = (1 << 2 ),
		ANIM_TIME     = (1 << 3 ),
		ANIM_DURATION = (1 << 4 ),
		PATTERN       = (1 << 5 ),
		FADE_VEL      = (1 << 6 ),
		ALL = POSITION | VELOCITY | ORIG_DIR | ANIM_TIME | ANIM_DURATION | PATTERN | FADE_VEL
	};
};


struct SBE_ParticleSetExtraData
{
	int iNumParticles;
	
	/// avPosition[i] - position of i-th particle (in world coord)
	Vector3 *pavPosition;

	/// avVelocity[i] - velocity of i-th particle (in world coord)
	Vector3 *pavVelocity;

	/// use this to save the initial direction of particles
	Vector3 *pavOrigDirection;

	/// current animation time of particles
	float *pafAnimationTime;

	/// total animation time of particles
	float *pafAnimDuration;

	/// pattern ID of the i-th particle
	short *pasPattern;

	/// for smooth critical damping
	float *pafFadeVel;

	AABB3 aabb;


	SBE_ParticleSetExtraData()
		:
	iNumParticles(0),
	pavPosition(NULL),
	pavVelocity(NULL),
	pavOrigDirection(NULL),
	pafAnimationTime(NULL),
	pafAnimDuration(NULL),
	pasPattern(NULL),
	pafFadeVel(NULL)
	{
		aabb.Nullify();
	}

	~SBE_ParticleSetExtraData()
	{
		SafeDeleteArray( pavPosition );
		SafeDeleteArray( pavVelocity );
		SafeDeleteArray( pavOrigDirection );
		SafeDeleteArray( pafAnimationTime );
		SafeDeleteArray( pafAnimDuration );
		SafeDeleteArray( pasPattern );
		SafeDeleteArray( pafFadeVel );
	}

	void Init( int flag, int num_particles )
	{
		iNumParticles = 0;
		aabb.Nullify();

		if( flag & ParticleSetFlag::POSITION )      pavPosition = new Vector3 [num_particles];
		if( flag & ParticleSetFlag::VELOCITY )      pavVelocity = new Vector3 [num_particles];
		if( flag & ParticleSetFlag::ORIG_DIR )      pavOrigDirection = new Vector3 [num_particles];
		if( flag & ParticleSetFlag::ANIM_TIME )     pafAnimationTime = new float [num_particles];
		if( flag & ParticleSetFlag::ANIM_DURATION ) pafAnimDuration = new float [num_particles];
		if( flag & ParticleSetFlag::PATTERN )       pasPattern = new short [num_particles];
		if( flag & ParticleSetFlag::FADE_VEL )      pafFadeVel = new float [num_particles];
	}

//		iNumParticles = 0;
//		aabb.Nullify();
//		int i;
//		for( i=0; i<NUM_MAX_PARTICLES_PER_SET; i++ )
//		{
//			afAnimationTime[i] = 0.0f;
//			avVelocity[i] = Vector3(0,0,0);
//			asPattern[i] = 0;
//			afFadeVel[i] = 0;
//		}


	void Init()
	{
	}
};

#define PTCL_NUM_MAX_TEXTURE_SEGMENTS	8


class CParticleThreadStarter;


class CBE_ParticleSet : public CBaseEntity, public CBEC_Billboard
{
public:

	enum eParams
	{
		NUM_DEFAULT_PARTICLESETS = 64,
		NUM_DEFAULT_PARTICLES_PER_SET = 128,
		PTCL_NUM_VELOCITY_SAMPLES = 32
	};

	enum eParticleType
	{
		TYPE_SMOKE_NORMAL = 1,
		TYPE_SMOKE_DIRECTIONAL = 2,
		TYPE_SMOKE_SHOOTING = 3
	};

	enum eVertexBufferType
	{
		VBT_SHARED_VERTEX_BUFFER,
		VBT_SEPARATE_VERTEX_BUFFERS,
		NUM_VERTEX_BUFFER_TYPES
	};

protected:

	int m_MaxNumParticleSets;

	int m_MaxNumParticlesPerSet;

//	SBE_ParticleSetExtraData m_paParticleSet[NUM_MAX_PARTICLESETS];
	SBE_ParticleSetExtraData *m_paParticleSet;

	int m_VertexBufferType;

	int m_iNumTextureSegments;

	TEXCOORD2 m_avRandomTexCoordTable[PTCL_NUM_MAX_TEXTURE_SEGMENTS * PTCL_NUM_MAX_TEXTURE_SEGMENTS * 4];

	TCFloatLookUpTable<PTCL_NUM_VELOCITY_SAMPLES> m_FadeTable;

	int m_ParticleType;

	float m_fParticleRadius;

//	int m_iNumParticles;

	/// life time of the particle
	float m_fDuration;

	/// variations in animation times
	float m_fAnimTimeOffsetMin, m_fAnimTimeOffsetMax;

	float m_fRandomVelocity_XZ;
	float m_fRandomVelocity_Y;

	/// how much each particle expands
	float m_fExpansionFactor;

	/// how much particles will be influenced by the gravity
	float m_fGravityInfluenceFactor;

	int m_iParticleSetCurrentIndex;

	bool m_bWorldOffset;

	SFloatRGBColor m_VertexColor;

protected:

	void InitBillboardRects();

	void InitParticles();

//	HRESULT InitVertexBufferAndIndexBuffer();


	inline void UpdateParticles( SBE_ParticleSetExtraData& rParticleSet, float dt, AABB3& aabb );

	inline void UpdateParticlePositions( SBE_ParticleSetExtraData& rParticleSet, float dt, AABB3& aabb );

	inline int GetNewExtraDataID();

	inline float& CurrentTime(CCopyEntity *pCopyEnt) { return pCopyEnt->f2; }

	void ParticleThreadMain();

protected:

	bool m_bMinimumParticleUpdates;

	bool m_bCreateParticleThread;

	/// thread for particles (experimental)
	/// used when m_bCreateParticleThread == true

	float m_fFrameTimeLeft;

	bool m_TerminateParticleThread;

	boost::thread_group m_ParticleThreadGroup;

public:

	CBE_ParticleSet();

	virtual ~CBE_ParticleSet();

	virtual void Init();
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

	virtual void Act(CCopyEntity* pCopyEnt);	//behavior in in one frame
	virtual void Draw(CCopyEntity* pCopyEnt); 
	// virtual void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other) {}
	// virtual void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );
	// virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self) {}

	virtual void UpdateBaseEntity( float frametime ) { m_fFrameTimeLeft += frametime; }

	virtual bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_PARTICLESET; }

	virtual void Serialize( GameLib1::Serialization::IArchive& ar, const unsigned int version );

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

	void UpdateVertexBuffer(CCopyEntity* pCopyEnt);

	void DrawParticles(CCopyEntity* pCopyEnt);

	inline SBE_ParticleSetExtraData& GetExtraData(int iExtraDataIndex);

	// friend class
	friend class CParticleThreadStarter; ///< calls ParticleThreadMain() to start particle thread
};


/**
 * updates position, velocity, and animation time of particles
 *  - rParticleSet.aabb is also updated
 *
 */
inline void CBE_ParticleSet::UpdateParticles( SBE_ParticleSetExtraData& rParticleSet, float dt, AABB3& aabb )
{
//	Vector3 vGravityAccel = m_pStage->GetGravityAccel();
//	float fGravityInfluenceFactor = m_fGravityInfluenceFactor;
	Vector3 vGravityAccel = m_pStage->GetGravityAccel() * m_fGravityInfluenceFactor;

//	AABB3 aabb;
	aabb.Nullify();

	int i, num_particles = rParticleSet.iNumParticles;
	for(i=0; i<num_particles; i++)
	{
		// update velocity
//		rParticleSet.pavVelocity[i] += (vGravityAccel * fGravityInfluenceFactor) * dt;
		rParticleSet.pavVelocity[i] += vGravityAccel * dt;

		// update position
		rParticleSet.pavPosition[i] += rParticleSet.pavVelocity[i] * dt;

		// update animation time
		rParticleSet.pafAnimationTime[i] += dt;

		aabb.AddPoint( rParticleSet.pavPosition[i] );
	}

	const float r = m_fParticleRadius;
	aabb.vMin -= Vector3(r,r,r);
	aabb.vMax += Vector3(r,r,r);

	rParticleSet.aabb = aabb;
}


/**
 * updates position, velocity, and animation time of particles
 *  - rParticleSet.aabb is also updated
 *
 */
inline void CBE_ParticleSet::UpdateParticlePositions( SBE_ParticleSetExtraData& rParticleSet, float dt, AABB3& aabb )
{
//	AABB3 aabb;
	aabb.Nullify();

	int i, num_particles = rParticleSet.iNumParticles;
	for(i=0; i<num_particles; i++)
	{
		// update position
		rParticleSet.pavPosition[i] += rParticleSet.pavVelocity[i] * dt;

		// update animation time
		rParticleSet.pafAnimationTime[i] += dt;

		aabb.AddPoint( rParticleSet.pavPosition[i] );
	}

	const float r = m_fParticleRadius;
	aabb.vMin -= Vector3(r,r,r);
	aabb.vMax += Vector3(r,r,r);

	rParticleSet.aabb = aabb;
}


inline int CBE_ParticleSet::GetNewExtraDataID()
{
	if( m_iParticleSetCurrentIndex < m_MaxNumParticleSets )
		return m_iParticleSetCurrentIndex++;
	else
	{
		m_iParticleSetCurrentIndex = 0;
		return m_iParticleSetCurrentIndex++;
	}
}


inline SBE_ParticleSetExtraData& CBE_ParticleSet::GetExtraData(int iExtraDataIndex)
{
	return m_paParticleSet[iExtraDataIndex];
}


#endif  /*  __BE_PARTICLESET_H__  */
