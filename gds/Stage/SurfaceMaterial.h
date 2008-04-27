#ifndef  __SURFACEMATERIAL_H__
#define  __SURFACEMATERIAL_H__


#include <vector>
#include <string>
using namespace std;


#include "Sound/SoundHandle.h"
#include "BaseEntityHandle.h"
#include "Support/FixedVector.h"
#include "GameCommon/MTRand.h"

#include "Support/Serialization/Serialization.h"
using namespace GameLib1::Serialization;


class CTextFileScanner;


//===========================================================================
// CPhysicsMaterial
//===========================================================================

class CPhysicsMaterial : public IArchiveObjectBase
{
public:

	float fStaticFriction;
	float fDynamicFriction;
	float fElasticity;

	inline CPhysicsMaterial();

	void Serialize( IArchive& ar, const unsigned int version );
};

//============================== inline implementations ==============================


inline CPhysicsMaterial::CPhysicsMaterial()
{
	fStaticFriction = 1.0f;
	fDynamicFriction = 1.0f;
	fElasticity = 0;
}



//===========================================================================
// CSurfaceMaterial
//===========================================================================


#define CSM_NUM_MAX_REACTION_ENTITIES	8

/**
 * holds material properties of surface & objects
 */
class CSurfaceMaterial : public IArchiveObjectBase
{
public:

	enum eReactionType
	{
		REACTION_TO_HIT,		 ///< sounds / entity to make when a bullt hit the surface
		REACTION_TO_BULLET_HIT,  ///< sounds / entity to make when an object hit the surface
		REACTION_TO_BULLET_OUT,	 ///< sounds / entity to make when an object exit the surface after penetrating it
		REACTION_TO_EXPLOSION_S, ///< sounds / entity to make when a small explosion occurred on the surface
		REACTION_TO_EXPLOSION_M, ///< sounds / entity to make when a medium level of explosion occurred on the surface
		REACTION_TO_EXPLOSION_L, ///< sounds / entity to make when a large explosion occurred on the surface
		NUM_REACTION_TYPES
	};

private:

	std::string m_strName;

	/// penetration resistance represents how difficult it is for bullets to penetrate the material
	float m_fPenetrationResistance;

	CPhysicsMaterial m_PhysicsMaterial;

	/// entities that are created when objects hit the material
//	TCFixedVector<CBaseEntityHandle, CSM_NUM_MAX_REACTION_ENTITIES> m_vecReactionEntity[NUM_REACTION_TYPES]; ///< causes error. why?
	vector<CBaseEntityHandle> m_vecReactionEntity[NUM_REACTION_TYPES];

	/// sound that are played when objects hit the material
	/// sounds are usually chosen randomly
	vector<CSoundHandle> m_vecSound[NUM_REACTION_TYPES];

	bool m_bValid;

	static CSurfaceMaterial sm_Null;

private:

	void LoadReactionEntities( CTextFileScanner& scanner );

	void LoadReactionSounds( CTextFileScanner& scanner );

public:

	CSurfaceMaterial();

	/// used only to craete null surface material which is returned by
	/// CSurfaceMaterialManager::GetSurfaceMaterial() if the argument handle / index is invalid
	CSurfaceMaterial( bool valid );

	~CSurfaceMaterial();

	const std::string& GetName() const { return m_strName; }

	inline float GetPenetrationResistance() const { return m_fPenetrationResistance; }

	inline const CPhysicsMaterial& GetPhysicsMaterial() const { return m_PhysicsMaterial; }

	inline int GetNumReactionEntities( int ReactionType ) const { return (int)m_vecReactionEntity[ReactionType].size(); }

	inline CBaseEntityHandle& GetReactionEntity( int ReactionType, int index )
	{
		return m_vecReactionEntity[ReactionType][index];
	}

	/// returns true if sound should be made when something interacts with the surface
	inline bool HasSoundOf( int ReactionType ) const;

	inline CSoundHandle& GetSound_Random( int ReactionType );

//	void Load( CSurfaceMaterialData& src_material );

	void LoadFromTextFile( CTextFileScanner& scanner );

	void Serialize( IArchive& ar, const unsigned int version );

	bool IsValid() const { return m_bValid; }

//	void SetValid( bool valid ) { m_bValid = valid; }

	static CSurfaceMaterial& Null() { return sm_Null; }
};



//============================== inline implementations ==============================


inline bool CSurfaceMaterial::HasSoundOf( int ReactionType ) const
{
	if( 0 < m_vecSound[ReactionType].size() )
		return true;
	else
		return false;
}


inline CSoundHandle& CSurfaceMaterial::GetSound_Random( int ReactionType )
{
	size_t i = RangedRand( 0, (int)(m_vecSound[ReactionType].size() - 1) );

//	if( m_vecSound[ReactionType].size() <= i )
//		i = m_vecSound[ReactionType].size() - 1;

	return m_vecSound[ReactionType][i];
}


#endif		/*  __SURFACEMATERIAL_H__  */
