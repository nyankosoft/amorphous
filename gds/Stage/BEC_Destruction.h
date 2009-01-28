#ifndef  __BEC_Destruction_H__
#define  __BEC_Destruction_H__


#include "Support/Serialization/Archive.h"
#include "Support/Serialization/Serialization_3DMath.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "GameCommon/RangedSet.h"
#include "Support/TextFileScanner.h"
#include "Stage/BaseEntityHandle.h"
#include "Stage/Serialization_BaseEntityHandle.h"
#include "Stage/fwd.h"


/**
 stores information that is used when an entity is destroyed


*/
class CBEC_Destruction : public IArchiveObjectBase
{
	/// debris that are generated when the entity is destroyed
	class Fragment : public IArchiveObjectBase
	{
	public:

		CBaseEntityHandle BaseEntity;
		Vector3 vLocalPosition;
		RangedSet<float> InitSpeed;

		virtual void Serialize( IArchive& ar, const unsigned int version )
		{
			ar & BaseEntity & vLocalPosition & InitSpeed.min, & InitSpeed.max;
		}
	};

	std::vector<Fragment> m_vecFragment;

	std::vector<Fragment> m_vecTrail;

	CBaseEntityHandle m_Explosion;

public:

	enum eParams
	{
		NUM_DEFAULT_MAX_FRAGMENTS = 256,
	};

	CBEC_Destruction() {}
	
	~CBEC_Destruction() {}

	/// create an explosion entity and fragment entities
	void CreateExplosion( CCopyEntity& entity, CStage *pStage );

	/// create fragment entities from m_vecFragment
	void CreateFragments( CCopyEntity& entity, CStage *pStage, int num_max_fragments = NUM_DEFAULT_MAX_FRAGMENTS );

	/// create trail entities that follows 'entity'
	/// - created from m_vecTrail
	void CreateTrails( CCopyEntity& entity, CStage *pStage );

	inline void Serialize( IArchive& ar, const unsigned int version );

	bool LoadFromFile( CTextFileScanner& scanner );

};


//============================= inline implementations =============================

inline void CBEC_Destruction::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_vecFragment;
	ar & m_vecTrail;
	ar & m_Explosion;
}


//============================= global functions =============================

/// creates explosion entity and fragment entities
inline void OnEntityDestroyed( CBEC_Destruction& destruction,
							   CCopyEntity& entity,
							   CStage *pStage,
							   int num_max_fragments = CBEC_Destruction::NUM_DEFAULT_MAX_FRAGMENTS )
{
	destruction.CreateExplosion( entity, pStage );
	destruction.CreateFragments( entity, pStage, num_max_fragments );
}


#endif		/*  __BEC_Destruction_H__  */
