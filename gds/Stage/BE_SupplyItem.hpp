#ifndef	__BASEENTITYSUPPLYITEM_H__
#define __BASEENTITYSUPPLYITEM_H__

#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "Sound/SoundHandle.hpp"


class CBE_SupplyItem : public CBaseEntity
{
	/// used for 'iEffect' in SGameMessage structure
	int m_iEffect;

	std::string m_strItemName;

	float m_fAmount;

	float m_fRotationSpeed;

	/// played when player gets this item
	CSoundHandle m_Sound;

	CBaseEntityHandle m_PseudoGlare;	// base entity for pseudo glare effect put together with items

public:

	CBE_SupplyItem();

	~CBE_SupplyItem() {}

	void Init();

	void InitCopyEntity( CCopyEntity* pCopyEnt );

//	void Act(CCopyEntity* pCopyEnt);

	void Draw(CCopyEntity* pCopyEnt);

	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);

	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_SUPPLYITEM; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};



#endif /*  __BASEENTITYSUPPLYITEM_H__  */
