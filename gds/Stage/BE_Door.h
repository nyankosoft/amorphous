
#ifndef	__BASEENTITYDOOR_H__
#define __BASEENTITYDOOR_H__


#include "BE_PhysicsBaseEntity.h"

#include "3DMath/Vector3.h"


class CBE_Door : public CBE_PhysicsBaseEntity
{
	float m_fOpenLength;			// how much the door slides when it opens
	Vector3 m_vLocalOpenDirection;	// the direction in which the door slides

//	char m_acStartSound[32];
//	char m_acStopSound[32];

	float m_fSpring;
	float m_fDamper;
	float m_fOpenDuration;

public:

	enum DoorState { DOOR_CLOSED, DOOR_OPEN, DOOR_OPENING, DOOR_CLOSING };

	CBE_Door();
	~CBE_Door();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
	void Draw(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_DOOR; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};

#endif /*  __BASEENTITYDOOR_H__  */
