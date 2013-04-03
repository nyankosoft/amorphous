#ifndef  __PhysSceneDesc_H__
#define  __PhysSceneDesc_H__


#include <float.h>
#include "fwd.hpp"
#include "Enums.hpp"
#include "../Support/Serialization/Serialization.hpp"
#include "../Support/Serialization/Serialization_3DMath.hpp"
#include "../Support/msgbox.hpp"


namespace amorphous
{
using namespace serialization;


namespace physics
{


class CSceneDesc : public IArchiveObjectBase
{
public:

	/// Gravity vector. 
	Vector3  Gravity;

	/// Possible notification callback. 
	// NxUserNotify *  userNotify 

	/// Possible notification callback for fluids. 
	// NxFluidUserNotify *  fluidUserNotify 

	/// Possible asynchronous callback for contact modification. 
	// NxUserContactModify *  userContactModify 

	/// Possible trigger callback. 
	// NxUserTriggerReport *  userTriggerReport 

	/// Possible contact callback. 
	CUserContactReport *pUserContactReport;

	/// Maximum substep size. 
	Scalar  MaxTimestep;

	/// Maximum number of substeps to take. 
	U32  MaxIter;

	/// Integration method. 
	Timestep::Method TimeStepMethod;

	/// Max scene bounds.
	AABB3 MaxBounds;

	/// Expected scene limits (or NULL). 
	// NxSceneLimits *  limits 

	/// Used to specify if the scene is a master hardware or software scene. 
	// NxSimulationType  simType 

	/// Enable/disable default ground plane. 
	bool  EnableDefaultGroundPlane;

	/// Enable/disable 6 planes around maxBounds (if available). 
	bool  EnableBoundsPlanes;

	/// Flags used to select scene options. 
	U32  Flags;

	/// Defines a custom scheduler. 
	//NxUserScheduler *  customScheduler 

	/// Allows the user to specify the stack size for the main simulation thread. 
	U32  SimThreadStackSize;

	/// Sets the thread priority of the main simulation thread. 
	//NxThreadPriority  simThreadPriority

	/// Allows the user to specify which (logical) processor to allocate the simulation thread to. 
	//U32  simThreadMask

	/// Sets the number of SDK managed worker threads used when running the simulation in parallel. 
	//U32  internalThreadCount

	/// Allows the user to specify the stack size for the worker threads created by the SDK. 
	//U32  workerThreadStackSize

	/// Sets the thread priority of the SDK created worker threads. 
	//NxThreadPriority  workerThreadPriority

	/// Allows the user to specify which (logical) processor to allocate SDK internal worker threads to. 
	//U32  threadMask

	/// Sets the number of SDK managed threads which will be processing background tasks. 
	//U32  backgroundThreadCount

	/// Allows the user to specify which (logical) processor to allocate SDK background threads. 
	//U32  backgroundThreadMask

	/// Defines the up axis for your world. This is used to accelerate scene queries like raycasting or sweep tests. Internally, a 2D structure is used instead of a 3D one whenever an up axis is defined. This saves memory and is usually faster. 
	//U32  upAxis

	/// Defines the subdivision level for acceleration structures used for scene queries. 
	//U32  subdivisionLevel

	/// Defines the structure used to store static objects. 
	//NxPruningStructure  staticStructure

	/// Defines the structure used to store dynamic objects. 
	//NxPruningStructure  dynamicStructure

	/// Will be copied to NxScene::userData. 
	//void *  userData

public:

	CSceneDesc()
	{
		SetDefault();
	}

	virtual ~CSceneDesc() {}

//	userNotify 
//	NxFluidUserNotify *  fluidUserNotify 
//	NxUserContactModify *  userContactModify 
//	NxUserTriggerReport *  userTriggerReport 
//	NxUserContactReport *  userContactReport 
	 
//	void SetMaxTimeStep( Scalar timestep );
//	void SetNumMaxIterations( Scalar num_mat_iters );
//	void SetTimeStepMethod( int method );
//	void SetSceneMaxBounds( const AABB3& aabb ); 
	 
//	NxSceneLimits *  limits 

	void SetDefault()
	{
		Gravity = Vector3(0.0f,-9.8f,0.0f);

		// NxUserNotify *  userNotify 
		// NxFluidUserNotify *  fluidUserNotify 
		// NxUserContactModify *  userContactModify 
		// NxUserTriggerReport *  userTriggerReport 
		// NxUserContactReport *  userContactReport 

		/// Maximum substep size. 
		MaxTimestep = 1.0f / 60.f;

		/// Maximum number of substeps to take. 
		MaxIter = 8;

		/// Integration method. 
		TimeStepMethod = Timestep::Fixed;

		/// Max scene bounds.
		MaxBounds = AABB3( Vector3(-FLT_MAX,-FLT_MAX,-FLT_MAX), Vector3(FLT_MAX,FLT_MAX,FLT_MAX) );

		// NxSceneLimits *  limits 
		// NxSimulationType  simType 

		/// Enable/disable default ground plane. 
//		EnableDefaultGroundPlane;

		/// Enable/disable 6 planes around maxBounds (if available). 
//		EnableBoundsPlanes;

		/// Flags used to select scene options. 
		Flags = 0;

		//NxUserScheduler *  customScheduler 

		/// Allows the user to specify the stack size for the main simulation thread. 
//		SimThreadStackSize;

		//NxThreadPriority  simThreadPriority
		//U32  simThreadMask
		//U32  internalThreadCount
		//U32  workerThreadStackSize
		//NxThreadPriority  workerThreadPriority
		//U32  threadMask
		//U32  backgroundThreadCount
		//U32  backgroundThreadMask

		//U32  upAxis

		//U32  subdivisionLevel

		//NxPruningStructure  staticStructure
		//NxPruningStructure  dynamicStructure
	}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & Gravity;

		// NxUserNotify *  userNotify 
		// NxFluidUserNotify *  fluidUserNotify 
		// NxUserContactModify *  userContactModify 
		// NxUserTriggerReport *  userTriggerReport 
		// NxUserContactReport *  userContactReport 

		/// Maximum substep size. 
		ar & MaxTimestep;

		/// Maximum number of substeps to take. 
		ar & MaxIter;

		/// Integration method.
//		MsgBox( "serializing enum type variable..." );
		ar & (unsigned int&)TimeStepMethod;

		/// Max scene bounds.
		ar & MaxBounds;

		// NxSceneLimits *  limits 
		// NxSimulationType  simType 

		/// Enable/disable default ground plane. 
		ar & EnableDefaultGroundPlane;

		/// Enable/disable 6 planes around maxBounds (if available). 
		ar & EnableBoundsPlanes;

		/// Flags used to select scene options. 
		ar & Flags;

		//NxUserScheduler *  customScheduler 

		/// Allows the user to specify the stack size for the main simulation thread. 
		//ar & SimThreadStackSize;

		//NxThreadPriority  simThreadPriority
		//U32  simThreadMask
		//U32  internalThreadCount
		//U32  workerThreadStackSize
		//NxThreadPriority  workerThreadPriority
		//U32  threadMask
		//U32  backgroundThreadCount
		//U32  backgroundThreadMask

		//U32  upAxis

		//U32  subdivisionLevel

		//NxPruningStructure  staticStructure
		//NxPruningStructure  dynamicStructure
	}
};


} // namespace physics

} // namespace amorphous



#endif		/*  __PhysSceneDesc_H__  */
