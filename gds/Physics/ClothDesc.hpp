#ifndef  __PhysClothDesc_HPP__
#define  __PhysClothDesc_HPP__


#include <vector>

#include "3DMath/Matrix34.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;

#include "fwd.hpp"
#include "MeshData.hpp"


namespace physics
{


class CClothDesc : public IArchiveObjectBase
{
public:

	CClothMesh *pClothMesh;

	Matrix34 WorldPose;

	Scalar Thickness;

	Scalar Density;

	Scalar BendingStiffness;
	Scalar StretchingStiffness;
	Scalar Friction;
	Scalar Pressure;
	Scalar TearFactor;
	Scalar CollisionResponseCoefficient;
	Scalar AttachmentResponseCoefficient;
	Scalar AttachmentTearFactor;
	Scalar ToFluidResponseCoefficient;
	Scalar FromFluidResponseCoefficient;
	Scalar MinAdhereVelocity;
	U32    SolverIterations;
	Vector3 ExternalAcceleration;
	Vector3 WindAcceleration;
	Scalar WakeUpCounter;
	Scalar SleepLinearVelocity;
	CMeshData MeshData;
//	NxCollisionGroup collisionGroup;
//	NxGroupsMask GroupsMask;
	U16     ForceFieldMaterial;
	AABB3 ValidBounds;
	Scalar    RelativeGridSpacing;
	U32       Flags;
//	void* userData;
//	const char* name;
//	NxCompartment *	compartment;

public:

	CClothDesc() { SetDefault(); }

	virtual ~CClothDesc() {}

	inline void SetDefault()
	{
		pClothMesh = NULL;
	}

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
//		ar & ;
//		ar & ;
	}

	inline bool IsValid() const
	{
		return true;
	}
};


} // namespace physics


#endif		/*  __PhysClothDesc_HPP__  */
