#ifndef	__BASEENTITYDOOR_H__
#define __BASEENTITYDOOR_H__


#include "BE_PhysicsBaseEntity.hpp"


namespace amorphous
{


class CBE_Door : public CBE_PhysicsBaseEntity
{
	float m_fOpenLength;			// how much the door slides when it opens
	Vector3 m_vLocalOpenDirection;	// the direction in which the door slides

//	CSoundHandle m_OpenStartSound;
//	CSoundHandle m_OpenEndSound;
//	CSoundHandle m_CloseStartSound;
//	CSoundHandle m_CloseEndSound;

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
	void MessageProcedure(GameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
	void Draw(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_DOOR; }

	virtual void Serialize( IArchive& ar, const unsigned int version );
};
} // namespace amorphous



#endif /*  __BASEENTITYDOOR_H__  */
