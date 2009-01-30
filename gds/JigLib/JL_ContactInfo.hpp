#ifndef __JL_CONTACTINFO_H__
#define __JL_CONTACTINFO_H__


#include "3DMath/Vector3.hpp"

#include "Support/FixedVector.hpp"

class CJL_ShapeBase;



#define USE_COMBINED_COLLISION_POINTS_INFO


//====================================================================================
// CJL_CollPointInfo
//====================================================================================

class CJL_CollPointInfo
{
public:
	CJL_CollPointInfo() {}

	CJL_CollPointInfo( const Vector3& _vR0, const Vector3& _vR1, Scalar initial_penetration )
		: InitialPenetration(initial_penetration),
		  vR0(_vR0), vR1(_vR1),
		  fMinSeparationVel(0.0f), fDenominator(0.0f) {}

	/// estimated penetrations before the objects collide (can be -ve)?
	Scalar InitialPenetration;

	/// contact positions relative to actor0 (in world space)
	Vector3 vR0;

	/// contact positions relative to actor1 (in world space)
	Vector3 vR1;

	Scalar fMinSeparationVel;

	Scalar fDenominator;

	Vector3 vContactPosition;
};



//====================================================================================
// CJL_CollisionInfo
//====================================================================================

class CJL_CollisionInfo
{
public:

	enum {MAX_COLLISION_POINTS = 10};

	/// gets set to true after we've been processed, and to false when the body
	/// we are associated with has been affected by another constraint/collision
	bool m_Satisfied;

	CJL_ShapeBase *pBody0;
	CJL_ShapeBase *pBody1;
	
	/// normalized direction in world space pointing towards shape0 from the other body
	Vector3 m_vDirToBody0;

	Scalar fElasticity;
	Scalar fStaticFriction;
	Scalar fDynamicFriction;

	/// used when the body1 is static geometry and pBody1 is set to NULL
	int Body1MaterialIndex;

	TCFixedVector< CJL_CollPointInfo, MAX_COLLISION_POINTS > m_vecPointInfo;

public:

	inline CJL_CollisionInfo() {}

	inline CJL_CollisionInfo( CJL_ShapeBase *pShape0,
		                      CJL_ShapeBase *pShape1,
					          const Vector3& vDirToShape0,
					          const CJL_CollPointInfo *pCollPointInfo,
							  unsigned num_point_infos )
							  :
	pBody0(pShape0), pBody1(pShape1), m_vDirToBody0(vDirToShape0)
	{
		m_vecPointInfo.resize( 0 );
		if( MAX_COLLISION_POINTS < num_point_infos )
			num_point_infos = MAX_COLLISION_POINTS;
		for( unsigned i=0; i<num_point_infos; i++ )
		{
			m_vecPointInfo.push_back( pCollPointInfo[i] );
		}

		// TODO: per-triangle material support for static mesh
		Body1MaterialIndex = 0;
	}

	inline Scalar CalcAndSetElasticity( Scalar e0, Scalar e1 )
	{
		return (fElasticity = e0 * e1);
	}

	inline Scalar CalcAndSetStaticFriction( Scalar f0, Scalar f1 )
	{
		return (fStaticFriction = f0 * f1);
	}

	inline Scalar CalcAndSetDynamicFriction( Scalar f0, Scalar f1 )
	{
		return (fDynamicFriction = f0 * f1);
	}

	friend class CJL_CollisionFunctor; 
};


class CJL_ContactInfo
{
public:
	CJL_ShapeBase *pBody0;
	CJL_ShapeBase *pBody1;	// NULL for static geometry

	Vector3 vContactPoint;	// collision point, in world space (i.e. sensible place to apply a restoring force)
	Vector3 vNormal;		// normalised direction in world space pointing towards body 0 from the other body/terrain

	//==============================================================
	// The following properties may get modified when this collision
	// info is updated.
	//==============================================================

	/// depth of the penetration. Can be -ve, indicating penetration isn't happening
	Scalar fPenetrationDepth;

	/// estimated penetrations before the objects collide (can be -ve)?
	Scalar InitialPenetration;

	/// Set/Used by Physics in process/preprocess_collision
	Vector3 vR0; // position relative to body 0 (in world space)
	Vector3 vR1; // position relative to body 1 (if there is a body1)

	Scalar vr_extra; // extra speed (in dir_to_0) for restoring deviation

	Scalar fElasticity;

	Scalar fStaticFriction;

	Scalar fDynamicFriction;

	/// used by the physics to cache desired minimum separation velocity
	/// in the normal direction
	Scalar fMinSeparationVel;

	Scalar fDenominator;

	bool m_bSatisfied;

	short sStaticGeometryMaterialIndex;	// material index of 'pBody1' when it is static geometry

	inline CJL_ContactInfo()
		:
	sStaticGeometryMaterialIndex(0)
	{}

};


#endif  /*  __JL_CONTACTINFO_H__  */
