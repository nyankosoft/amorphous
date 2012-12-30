#ifndef  __PhysActorDesc_H__
#define  __PhysActorDesc_H__


#include <vector>

#include "fwd.hpp"
#include "BodyDesc.hpp"

#include "ShapeDescFactory.hpp"


namespace amorphous
{


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
		fDensity = 1.0f;
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

	inline bool IsValid() const
	{
		return ( 0 < vecpShapeDesc.size() );
	}
};


} // namespace physics

} // namespace amorphous



#endif		/*  __PhysActorDesc_H__  */
