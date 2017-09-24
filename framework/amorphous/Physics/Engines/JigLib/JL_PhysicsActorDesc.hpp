#ifndef  __JL_PHYSICSACTORDESC_H__
#define  __JL_PHYSICSACTORDESC_H__


#include <vector>
#include "amorphous/3DMath/Matrix33.hpp"
#include "amorphous/Support/Serialization/Serialization.hpp"
#include "amorphous/Support/Serialization/Serialization_3DMath.hpp"


namespace amorphous
{
using namespace serialization;


//#define JL_NUM_MAX_SHAPES_PER_ACTOR	8

class CJL_ShapeDesc;


class CJL_PhysicsActorDesc : public IArchiveObjectBase
{
	Matrix33 m_matLocalInertia;

public:
	Vector3 vPosition;	// global position
	Matrix33 matOrient;	// global orientation

	Vector3 vVelocity;	// initial velocity
	Vector3 vAngularVelocity;	// initial angular velocity

	Scalar fMass;

	unsigned int ActorFlag;

	bool bAllowFreezing;

	int iCollisionGroup;

//	CJL_BodyDesc *pBodyDesc;

	std::vector<CJL_ShapeDesc *>	vecpShapeDesc;
//	TCFixedPointerVector<CJL_ShapeDesc, JL_NUM_MAX_SHAPES_PER_ACTOR>	vecpShapeDesc;

	void *pUserPtr;

public:
	CJL_PhysicsActorDesc() { SetDefault(); }
	~CJL_PhysicsActorDesc() {}

	inline void SetDefault()
	{
		vPosition = Vector3(0,0,0);
		vVelocity = Vector3(0,0,0);
		vAngularVelocity = Vector3(0,0,0);
		matOrient = Matrix33Identity();
		fMass = 0;
		ActorFlag = 0;
		bAllowFreezing = true;
		iCollisionGroup = 0;
	}

	// calculate inertia tensor. call this function after all the shapes are set.
	void CalculateMassProperties();

	virtual void Serialize( IArchive& ar, const unsigned int version );
};

} // namespace amorphous



#endif		/*  __JL_PHYSICSACTORDESC_H__  */