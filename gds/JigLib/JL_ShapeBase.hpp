#ifndef  __JL_SHAPEBASE_H__
#define  __JL_SHAPEBASE_H__


#include "3DMath/Matrix34.hpp"
#include "3DMath/AABB3.hpp"

#include "fwd.hpp"
#include "JL_ShapeDesc.hpp"


class CJL_ShapeBase
{
protected:

	short m_sShape;
	short m_sMaterialIndex;

	Matrix34 m_LocalPose;	///< pose in actor space

	/// world pose is not updated by the base shape class
	/// each shape is responsible for updating its own world pose in UpdateWorldProperties()
	Matrix34 m_WorldPose;	///< pose in world space

	Matrix34 m_OldWorldPose;

	AABB3 m_WorldAABB;

	CJL_PhysicsActor *m_pPhysicsActor;	///< owner of this shape

public:

	CJL_ShapeBase();
	virtual ~CJL_ShapeBase();

	inline void Init( CJL_ShapeDesc& rShapeDesc );
	virtual void InitSpecific( CJL_ShapeDesc& rShapeDesc ) = 0;

	AABB3& GetWorldAABB() { return m_WorldAABB; }

	/// must be called every frame
	virtual void UpdateWorldProperties() {}

	/// save the current world pose
	inline void CopyCurrentWorldPoseToOld() { m_OldWorldPose = m_WorldPose; }

	void SetShape( CJL_ShapeDesc& rDesc );

	short GetMaterialIndex() { return m_sMaterialIndex; }

	inline short GetShapeType() const { return m_sShape; }

	inline CJL_PhysicsActor *GetPhysicsActor()
	{ 
		if( this == NULL )
			return NULL;

		return m_pPhysicsActor;
	}

	inline Matrix34& GetWorldPose() { return m_WorldPose; }
	inline const Matrix34& GetWorldPose() const { return m_WorldPose; }
	inline void GetWorldPose( Matrix34& rWorldPose ) { rWorldPose = m_WorldPose; }
	inline Vector3& GetWorldPosition() { return m_WorldPose.vPosition; }
	inline void GetWorldOrient( Matrix33& matWorldOrient ) { matWorldOrient = m_WorldPose.matOrient; }
	inline Matrix33& GetWorldOrient() { return m_WorldPose.matOrient; }

	inline const Matrix34& GetOldWorldPose() { return m_OldWorldPose; }
	// reutrn pose in actor space
	inline Matrix34& GetLocalPose() { return m_LocalPose; }
	inline void GetLocalPose( Matrix34& local_pose ) { local_pose = m_LocalPose; }

	// reutrn pose in world space
//	inline Matrix34& GetWorldPose() { return m_WorldPose; }
//	inline void GetWorldPose( Matrix34& world_pose ) { world_pose = m_WorldPose; }

//	inline virtual void Serialize( IArchive& ar, const unsigned int version );

	friend CJL_PhysicsActor;
};


inline void CJL_ShapeBase::Init( CJL_ShapeDesc& rShapeDesc )
{
	m_pPhysicsActor = rShapeDesc.pPhysicsActor;

	m_sShape = rShapeDesc.sShape;
	m_sMaterialIndex = rShapeDesc.sMaterialIndex;

	m_LocalPose.vPosition = rShapeDesc.vLocalPos;
	m_LocalPose.matOrient = rShapeDesc.matLocalOrient;

	// additional initializations for each shape
	InitSpecific( rShapeDesc );
}

/*
inline void Serialize( IArchive& ar, const unsigned int version )
{
}
*/


#endif		/*  __JL_SHAPEBASE_H__  */
