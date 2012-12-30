#ifndef  __CopyEntityDesc_H__
#define  __CopyEntityDesc_H__


#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Physics/fwd.hpp"
#include "EntityGroupHandle.hpp"
#include "fwd.hpp"

#include <string>


namespace amorphous
{


class CCopyEntityTypeID
{
public:
	enum eTypeID
	{
		DEFAULT = 0,
		ALPHA_ENTITY,
		LIGHT_ENTITY,
		SOUND_ENTITY,
		SCRIPTED_CAMERA_ENTITY,

		/// id offset of the entity types which are defined by other module(s) of the GameStageFramework.
		/// - e.g., CItemEntity uses this offset
		/// - What if 2 or more modules define their entities?
		SYSTEM_ENTITY_ID_OFFSET,

		/// id offset of the entity types which are defined by the user.
		USER_ID_OFFSET = 0x0100
	};
};


class CCopyEntityDesc
{
public:

	CBaseEntityHandle* pBaseEntityHandle;

	/// name to identify the copy entity
	std::string strName;

	/// world position and orientation
	Matrix34 WorldPose;

	/// initial velocity
	Vector3 vVelocity;

	/// initial speed
	float fSpeed;

	/// parent copy entity
	CCopyEntity *pParent;

	short sGroupID;

	/// use this when copy entities managed by a same base entity
	/// need to have different mesh objects.
	CMeshObjectHandle MeshObjectHandle;

	// general purpose variables
	float f1, f2, f3, f4;
	short s1;
	Vector3 v1, v2;

	int iExtraDataIndex;

	void *pUserData;

	int TypeID;

	physics::CActorDesc *pPhysActorDesc;

public:

	CCopyEntityDesc()
	{
		SetDefault();
	}

	virtual ~CCopyEntityDesc() {}

	inline void SetDefault()
	{
		pBaseEntityHandle = NULL;

		f1 = 0.0f;
		f2 = 0.0f;
		f3 = 0.0f;
		f4 = 0.0f;
		s1 = 0;
		v1 = Vector3(0,0,0);
		v2 = Vector3(0,0,0);

		WorldPose.Identity();

		vVelocity  = Vector3(0,0,0);
		fSpeed = 0;

		pParent = NULL;

		sGroupID = ENTITY_GROUP_INVALID_ID;

		pUserData = NULL;

		iExtraDataIndex = 0;

		TypeID = CCopyEntityTypeID::DEFAULT;

		pPhysActorDesc = NULL;
	}

	// set orientation from 4x4 matrix
/*	inline void SetOrientation( D3DXMATRIX& rmatOrientation )
	{
		memcpy( &vRight,     &rmatOrientation._11, sizeof(float) * 3 );
		memcpy( &vUp,        &rmatOrientation._21, sizeof(float) * 3 );
		memcpy( &vDirection, &rmatOrientation._31, sizeof(float) * 3 );
	}*/

	inline void SetWorldPosition( const Vector3& vWorldPos )
	{
		WorldPose.vPosition = vWorldPos;
	}

	inline void SetWorldOrient( const Matrix33& rmatWorldOrient )
	{
		WorldPose.matOrient = rmatWorldOrient;
	}

	inline void SetWorldPose( const Matrix34& rWorldPose )
	{
		WorldPose = rWorldPose;
	}
};


} // namespace amorphous



#endif		/*  __CopyEntityDesc_H__  */
