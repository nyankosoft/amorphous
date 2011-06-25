#ifndef __BE_PHYSICSBASEENTITY_H__
#define __BE_PHYSICSBASEENTITY_H__

#include "BaseEntity.hpp"
#include "gds/Physics/ActorDesc.hpp"
#include "gds/Physics/TriangleMeshDesc.hpp"


class CBE_PhysicsBaseEntity : public CBaseEntity
{
protected:

	// the following variables are used in rigid body simulation
	physics::CActorDesc m_ActorDesc;

	// Used when one of the entity's shapes is a convex mesh
	// - Not serialized.
	physics::CConvexMesh *m_pConvexMesh;

	// Used when one of the entity's shapes is a convex/non-convex mesh
	// - Serialized.
	physics::CTriangleMeshDesc m_ConvexMeshDesc;

	std::string m_ShapeName;

private:

	void CreatePseudoCylinderDescFromBoxes( float radius, float height );

	void CreatePseudoCylinderMeshDesc( float radius, float height );

public:

	CBE_PhysicsBaseEntity();

	virtual ~CBE_PhysicsBaseEntity();

	void Init();

	void InitCopyEntity( CCopyEntity *pCopyEnt );

	const physics::CActorDesc& GetPhysicsActorDesc() { return m_ActorDesc; }

	bool LoadSpecificPropertiesFromFile( CTextFileScanner& scanner );

	virtual unsigned int GetArchiveObjectID() const { return BE_PHYSICSBASEENTITY; }

	virtual void Serialize( IArchive& ar, const unsigned int version );

	friend class CEntitySet;
};


#endif  /*  __BE_PHYSICSBASEENTITY_H__  */
