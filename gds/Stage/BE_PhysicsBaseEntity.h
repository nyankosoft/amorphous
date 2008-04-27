#ifndef __BE_PHYSICSBASEENTITY_H__
#define __BE_PHYSICSBASEENTITY_H__

#include "BaseEntity.h"
#include "JigLib/JL_PhysicsActorDesc.h"


class CBE_PhysicsBaseEntity : public CBaseEntity
{
protected:

	// the following variables are used in rigid body simulation
	CJL_PhysicsActorDesc m_ActorDesc;

public:
	CBE_PhysicsBaseEntity();

	virtual ~CBE_PhysicsBaseEntity();

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_PHYSICSBASEENTITY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CEntitySet;

};

#endif  /*  __BE_PHYSICSBASEENTITY_H__  */