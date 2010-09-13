#ifndef	__BE_PLATFORM_H__
#define __BE_PLATFORM_H__


#include "BaseEntity.hpp"
#include "BaseEntityHandle.hpp"
#include "BE_PhysicsBaseEntity.hpp"
#include "Sound/SoundHandle.hpp"


class CBE_Platform : public CBE_PhysicsBaseEntity
{
	CSoundHandle m_StartSound;
	CSoundHandle m_StopSound;

public:

	CBE_Platform();
	~CBE_Platform();
	void Init();
	void InitCopyEntity( CCopyEntity* pCopyEnt );
	void Act(CCopyEntity* pCopyEnt);
	void Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other);
	void MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self);
	void Draw(CCopyEntity* pCopyEnt);

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_PLATFORM; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	enum eState
	{
		STATE_MOVING,
		STATE_REST,
	};
};


#endif /*  __BE_PLATFORM_H__  */
