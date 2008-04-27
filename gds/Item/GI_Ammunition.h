
#ifndef __GAMEITEM_AMMUNITION_H__
#define __GAMEITEM_AMMUNITION_H__

#include "GameItem.h"

#include "Stage/BaseEntityHandle.h"
#include "Stage/Serialization_BaseEntityHandle.h"


//======================================================================================
// CGI_Ammunition
//======================================================================================

class CGI_Ammunition;
typedef boost::shared_ptr<CGI_Ammunition> CGI_AmmunitionSharedPtr;

/**
 ammunition data
 - bullets
 - missiles
*/
class CGI_Ammunition : public CGameItem
{
	enum params { MAX_AMMOTYPE_NAME_LENGTH = 16 };

	char m_acAmmoType[MAX_AMMOTYPE_NAME_LENGTH];	///< caliber

	float m_fMuzzleSpeed;
	float m_fPower;
	int m_iNumPellets;		///< for shotguns

	float m_fRange;

	CBaseEntityHandle m_AmmoBaseEntity;		///< base entity for this ammo
	CBaseEntityHandle m_MuzzleFlashBaseEntity;

public:

	inline CGI_Ammunition();

	void SetData( char* pcAmmoType, float fMuzzleSpeed, float fPower, int iNumPellets, char* pcBaseEntityName );

	inline const char* GetAmmoType() const { return m_acAmmoType; }
	inline float GetMuzzleSpeed() const { return m_fMuzzleSpeed; }
	inline float GetPower() const { return m_fPower; }
	inline int GetNumPellets() const { return m_iNumPellets; }

	inline float GetRange() const { return m_fRange; }
	inline float GetRangeSq() const { return m_fRange*m_fRange; }

	/// return handle to a base entity used as ammunition
	inline CBaseEntityHandle& GetBaseEntityHandle() { return m_AmmoBaseEntity; }

	/// return handle to a base entity is used as muzzle flash of this ammo
	inline CBaseEntityHandle& GetMuzzleFlashHandle() { return m_MuzzleFlashBaseEntity; }

	void OnOwnerLeftStage();

	unsigned int GetArchiveObjectID() const { return ID_AMMUNITION; }

	inline void Serialize( IArchive& ar, const unsigned int version );

	friend class CItemDatabaseBuilder;

};


inline CGI_Ammunition::CGI_Ammunition()
{
	memset( m_acAmmoType, 0, sizeof(m_acAmmoType) );
	m_MuzzleFlashBaseEntity.SetBaseEntityName( "mflsh" );
	m_TypeFlag |= (TYPE_AMMO);
	m_iNumPellets = 1;
	m_fRange = 100000.0f;
}


inline void CGI_Ammunition::Serialize( IArchive& ar, const unsigned int version )
{
	CGameItem::Serialize( ar, version );

	for( int i=0; i<MAX_AMMOTYPE_NAME_LENGTH; i++ )
		ar & m_acAmmoType[i];

	ar & m_fMuzzleSpeed;
	ar & m_fPower;
	ar & m_iNumPellets;

	ar & m_fRange;

	ar & m_AmmoBaseEntity;
	ar & m_MuzzleFlashBaseEntity;
}


#endif  /*  __GAMEITEM_AMMUNITION_H__  */
