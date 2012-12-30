#ifndef __GAMEITEM_AMMUNITION_H__
#define __GAMEITEM_AMMUNITION_H__

#include "GameItem.hpp"
#include "../Stage/BaseEntityHandle.hpp"
#include "../Stage/Serialization_BaseEntityHandle.hpp"


namespace amorphous
{


//======================================================================================
// CGI_Ammunition
//======================================================================================

/**
 ammunition data
 - firearms cartridges are created as instances of CCartridge class, a derived class of CGI_Ammunition
 - missiles
*/
class CGI_Ammunition : public CGameItem
{
protected:

	std::string m_strAmmoType;

	float m_fMuzzleSpeed;
	float m_fPower;

	float m_fRange;

	CBaseEntityHandle m_AmmoBaseEntity;		///< base entity for this ammo
	CBaseEntityHandle m_MuzzleFlashBaseEntity;

	///< the number of this cartridge item currently loaded in chambers and magazines.
	///< - m_NumLoaded <= CGameItem::m_iCurrentQuantity
	int m_NumLoaded;

public:

	CGI_Ammunition();

	void SetData( char* pcAmmoType, float fMuzzleSpeed, float fPower, int iNumPellets, char* pcBaseEntityName );

	inline const std::string& GetAmmoType() const { return m_strAmmoType; }
	inline float GetMuzzleSpeed() const { return m_fMuzzleSpeed; }
	inline float GetPower() const { return m_fPower; }

	inline float GetRange() const { return m_fRange; }
	inline float GetRangeSq() const { return m_fRange*m_fRange; }

	/// return handle to a base entity used as ammunition
	inline CBaseEntityHandle& GetBaseEntityHandle() { return m_AmmoBaseEntity; }

	/// return handle to a base entity is used as muzzle flash of this ammo
	inline CBaseEntityHandle& GetMuzzleFlashHandle() { return m_MuzzleFlashBaseEntity; }

	void OnOwnerLeftStage();

	int IncreaseLoadedQuantity( int quantity )
	{
		int num_available = m_iCurrentQuantity - m_NumLoaded;

		if( num_available < quantity )
		{
			m_NumLoaded += num_available;
			return num_available;
		}
		else
		{
			m_NumLoaded += quantity;
			return quantity;
		}
	}

	int ReduceLoadedQuantity( int quantity )
	{
		int num_max_quantity_to_reduce = take_min( quantity, m_NumLoaded, m_iCurrentQuantity );
		m_NumLoaded -= num_max_quantity_to_reduce;
		m_iCurrentQuantity -= num_max_quantity_to_reduce;
		return num_max_quantity_to_reduce;
	}

	virtual unsigned int GetArchiveObjectID() const { return ID_AMMUNITION; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	virtual void LoadFromXMLNode( CXMLNodeReader& reader );

	friend class CItemDatabaseBuilder;

};

} // namespace amorphous



#endif  /*  __GAMEITEM_AMMUNITION_H__  */
