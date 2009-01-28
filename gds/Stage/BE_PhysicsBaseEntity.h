#ifndef __BE_PHYSICSBASEENTITY_H__
#define __BE_PHYSICSBASEENTITY_H__

#include "BaseEntity.h"
#include "Physics/ActorDesc.h"


class CBE_PhysicsBaseEntity : public CBaseEntity
{
protected:

	// the following variables are used in rigid body simulation
	physics::CActorDesc m_ActorDesc;

public:

	CBE_PhysicsBaseEntity();

	virtual ~CBE_PhysicsBaseEntity();

	const physics::CActorDesc& GetPhysicsActorDesc() { return m_ActorDesc; }

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_PHYSICSBASEENTITY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CEntitySet;
};


#endif  /*  __BE_PHYSICSBASEENTITY_H__  */
