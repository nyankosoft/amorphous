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

//	CClothMesh *pClothMesh;
	CTriangleMesh *pClothMesh;

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

		WorldPose = Matrix34Identity();

		Thickness = 0.01f;

		Density = 1.0f;

		BendingStiffness = 1.0f;
		StretchingStiffness = 1.0f;
		Friction = 0.5f;
		Pressure = 1.0f;
		TearFactor = 1.5f;
		CollisionResponseCoefficient = 0.2f;
		AttachmentResponseCoefficient = 0.2f;
		AttachmentTearFactor = 1.5f;
		ToFluidResponseCoefficient = 1.0f;
		FromFluidResponseCoefficient = 1.0f;
		MinAdhereVelocity = 1.0f;
		SolverIterations = 5;
		ExternalAcceleration = Vector3(0,0,0);
		WindAcceleration = Vector3(0,0,0);
		WakeUpCounter = 0;
		SleepLinearVelocity = 0;
//		MeshData;
//		NxCollisionGroup collisionGroup = 0;
//		NxGroupsMask GroupsMask = 0;
		ForceFieldMaterial = 0;
//		ValidBounds = AABB3(-Vector3(FLT_MAX,FLT_MAX,FLT_MAX),Vector3(FLT_MAX,FLT_MAX,FLT_MAX));
		ValidBounds.Nullify();
		RelativeGridSpacing = 0.25f;
		Flags = 0;
	}

	inline virtual void Serialize( IArchive& ar, const unsigned int version )
	{
/*		ar & WorldPose;
		ar & Thickness;
		ar & Density;
		ar & BendingStiffness;
		ar & StretchingStiffness;
		ar & Friction;
		ar & Pressure;
		ar & TearFactor;
		ar & CollisionResponseCoefficient;
		ar & AttachmentResponseCoefficient;
		ar & AttachmentTearFactor;
		ar & ToFluidResponseCoefficient;
		ar & FromFluidResponseCoefficient;
		ar & MinAdhereVelocity;
		ar & SolverIterations;
		ar & ExternalAcceleration;
		ar & WindAcceleration;
		ar & WakeUpCounter;
		ar & SleepLinearVelocity;*/
	}

	inline bool IsValid() const
	{
		return true;
	}
};


} // namespace physics


#endif		/*  __PhysClothDesc_HPP__  */
