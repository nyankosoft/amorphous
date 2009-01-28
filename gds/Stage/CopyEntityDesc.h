#ifndef  __CopyEntityDesc_H__
#define  __CopyEntityDesc_H__


#include "3DMath/Matrix34.h"
#include "Graphics/MeshObjectHandle.h"
#include "EntityGroupHandle.h"
#include "fwd.h"

#include <string>


class CCopyEntityTypeID
{
public:
	enum eTypeID
	{
		DEFAULT = 0,
		ALPHA_ENTITY,
		LIGHT_ENTITY,
		SCRIPTED_CAMERA_ENTITY,
		USE_ID_OFFSET
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



#endif		/*  __CopyEntityDesc_H__  */
