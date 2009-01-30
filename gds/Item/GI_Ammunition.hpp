#ifndef __GAMEITEM_AMMUNITION_H__
#define __GAMEITEM_AMMUNITION_H__

#include "GameItem.hpp"

#include "Stage/BaseEntityHandle.hpp"
#include "Stage/Serialization_BaseEntityHandle.hpp"


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
	std::string m_strAmmoType;

	float m_fMuzzleSpeed;
	float m_fPower;
	int m_iNumPellets;		///< for shotguns

	float m_fRange;

	CBaseEntityHandle m_AmmoBaseEntity;		///< base entity for this ammo
	CBaseEntityHandle m_MuzzleFlashBaseEntity;

public:

	CGI_Ammunition();

	void SetData( char* pcAmmoType, float fMuzzleSpeed, float fPower, int iNumPellets, char* pcBaseEntityName );

	inline const std::string& GetAmmoType() const { return m_strAmmoType; }
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

	void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class CItemDatabaseBuilder;

};


#endif  /*  __GAMEITEM_AMMUNITION_H__  */
