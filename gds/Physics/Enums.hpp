#ifndef  __PhysEnums_H__
#define  __PhysEnums_H__


#include "3DMath/Vector3.hpp"
#include "fwd.hpp"
#include "Ray.hpp"


namespace physics
{


class ForceMode
{
public:
	enum Mode
	{
		Force,
		Impulse,
		VelocityChange,
		SmoothImpulse,
		SmoothVelocityChange,
		Acceleration,
		NumModes
	};
};


class ShapeFlag
{
public:
	enum Name
	{
		TriggerOnEnter    = (1 << 0),
		TriggerOnLeave    = (1 << 1),
		TriggerOnStay     = (1 << 2),
		DisableCollision  = (1 << 3),
		DisableRaycasting = (1 << 4),
		TriggerEnable = (TriggerOnEnter | TriggerOnLeave | TriggerOnStay),
	};
};


class ActorFlag
{
public:
	enum Name
	{
		DISABLE_FREEZING = (1 << 0),
		ANOTHER_FLAG     = (1 << 1),
		YET_ANOTHER_FLAG = (1 << 2),
	};
};


class BodyFlag
{
public:
	enum Flag
	{
		Static         = ( 1 << 0 ),
		Kinematic      = ( 1 << 1 ),
		DisableGravity = ( 1 << 2 ),
	};
};

class Timestep
{
public:
	enum Method
	{
		Fixed,
		Variable,
		Inherit,
		NumMethods
	};
};


class SimulationStatus
{
public:
	enum Status
	{
		RigidBodyFinished,
		AllFinished,
		PrimaryFinished,
		NumStatus
	};
};


class PhysSceneStats
{
public:
};


} // namespace physics


#endif /* __PhysEnums_H__ */
