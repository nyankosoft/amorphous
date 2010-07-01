#ifndef	__BE_DOORCONTROLLER_H__
#define __BE_DOORCONTROLLER_H__


#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "Serialization_BaseEntityHandle.hpp"

#include "Support/FixedVector.hpp"
#include "3DMath/Matrix34.hpp"

#include "Sound/SoundHandle.hpp"


#define NUM_MAX_DOOR_COMPONENTS		8

class CBE_DoorController : public CBaseEntity
{
	struct SDoorComponentSet : public IArchiveObjectBase
	{
		CBaseEntityHandle entity;
		Matrix34 matLocalPose;	// offset position in the local space of door controller

		SDoorComponentSet();
        
		virtual void Serialize( IArchive& ar, const unsigned int version ) { ar & entity & matLocalPose; }
	};

	TCFixedVector<SDoorComponentSet, NUM_MAX_DOOR_COMPONENTS> m_vecDoorComponent;

	CSoundHandle m_OpenSound;
	CSoundHandle m_CloseSound;

	/// passward string to unlock the door
	std::string m_strKeyCode;

public:

	CBE_DoorController();
	~CBE_DoorController();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
//	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
//	void Draw(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_DOORCONTROLLER; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};


#endif /*  __BE_DOORCONTROLLER_H__  */
