#ifndef  __PhysActorDesc_H__
#define  __PhysActorDesc_H__


#include <vector>

#include "3DMath/Matrix34.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/Serialization_3DMath.h"
using namespace GameLib1::Serialization;

#include "fwd.h"
#include "BodyDesc.h"

#include "ShapeDescFactory.h"


namespace physics
{


class CActorDesc : public IArchiveObjectBase
{
public:

	Matrix34 WorldPose;

	Scalar fDensity;

	CBodyDesc BodyDesc;

	unsigned int ActorFlags;

//	bool bAllowFreezing;

	int CollisionGroup;

//	std::string Name;

	// DominanceGroup

	// Compartment

public:

	/// borrowed reference?
	std::vector<CShapeDesc *>	vecpShapeDesc;

	void *pUserPtr;

public:

	CActorDesc() { SetDefault(); }

	virtual ~CActorDesc() {}

	inline void SetDefault()
	{
		WorldPose.Identity();
		fDensity = 0;
//		BodyDesc = ???;
		ActorFlags = 0;
//		bAllowFreezing = true;
		CollisionGroup = 0;

		vecpShapeDesc.resize( 0 );
	}

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & WorldPose;
		ar & fDensity;
		ar & BodyDesc;
		ar & ActorFlags;
//		ar & bAllowFreezing;
		ar & CollisionGroup;

		// serialize shape descs

		CShapeDescFactory shapedesc_factory;

		ar.Polymorphic( vecpShapeDesc, shapedesc_factory );
	}
};


} // namespace physics


#endif		/*  __PhysActorDesc_H__  */
