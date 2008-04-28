#ifndef __FG_AIAircraftBaseEntity_H__
#define __FG_AIAircraftBaseEntity_H__


#include <vector>
#include <string>
#include "Stage/fwd.h"
#include "Stage/CopyEntity.h"
#include "Stage/EntityGroupHandle.h"
#include "Stage/BE_IndividualEntity.h"
#include "Stage/BEC_Destruction.h"


class CFG_BaseEntityID
{
public:
	enum BaseEntityID
	{
		BE_AIAIRCRAFTBASEENTITY = CBaseEntity::USER_BASE_ENTITY_ID_OFFSET,
		NUM_FG_BASEENTITY_IDS
	};
};


class CFG_AIAircraftBaseEntity : public CBE_IndividualEntity
{
	std::string m_strAircraftItemName;

	std::vector<std::string> m_vecWeaponItemName;

	std::vector<std::string> m_vecAmmoItemName;

	CEntityGroupHandle m_ProjectileEntityGroup;

	CEntityGroupHandle m_TargetFilter;

	/// determines behaviors when entities are destroyed
	CBEC_Destruction m_Destruction;

public:

	CFG_AIAircraftBaseEntity();

	virtual ~CFG_AIAircraftBaseEntity();

	/// initialization of the base entity
	virtual void Init();

	/// initialization of each copy entity
	virtual void InitCopyEntity( CCopyEntity* pCopyEnt );

//	virtual void Act(CCopyEntity* pCopyEnt);

//	virtual void Draw(CCopyEntity* pCopyEnt);

	// void ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself );

	virtual void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);

//	void ReleaseGraphicsResources();
//	void LoadGraphicsResources( const CGraphicsParameters& rParam );

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return CFG_BaseEntityID::BE_AIAIRCRAFTBASEENTITY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


#endif  /*  __FG_AIAircraftBaseEntity_H__  */
