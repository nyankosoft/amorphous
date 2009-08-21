#ifndef __TRACE_H__
#define __TRACE_H__


#include "3DMath/Vector3.hpp"
#include "3DMath/3DStructs.hpp"

#include <vector>

#define NUM_MAXTOUCHENTITIES	512

class CCopyEntity;

#define TRACETYPE_IGNORE_ALL_ENTITIES		1	///< check trace only against static geometry (map)
#define TRACETYPE_IGNORE_NOCLIP_ENTITIES	2	///< check collisions only with collidable entities
#define TRACETYPE_IGNORE_MAP				4	///< do not check collisions with the map (static geometry)
#define TRACETYPE_GET_MATERIAL_INFO			8	///< get material index of the contacted surface


//=========================================================================================
// STrace
//=========================================================================================
struct STrace
{
	Vector3* pvStart;	///< [in] start of the line segment
	Vector3* pvGoal;	///< [in] end of the line segment

	/// [in] volume of the entity approximated by aabb (local space)
	AABB3 aabb;

	/// [in] volume swept by the movement of this trace (world space)
	AABB3 aabb_swept;

	char bvType;	///< [in] bounding volume type (doesn't mean much, actually)
	float fRadius;	///< [in] bounding sphere radius

	CCopyEntity *pSourceEntity;

	short sTraceType;

	/// [in] entity group of the trace
	short GroupIndex;

	/// the following variables store the result of trace clipping

	/// [out] the furthest position which the line segment can reach
	/// e.g.) if the line segment hit some souface, this will be the contact position
	Vector3 vEnd;

	/// [out] end fraction
	float fFraction;

	/// [out] contacted surface
	SPlane plane;

	/// [out] contacted entity
	CCopyEntity* pTouchedEntity;

	/// [out] how many copy-entities this trace touched with
	/// (not accurate)
	int iNumTouches;

	/// [out] index to the material of the contacted surface
	int iMaterialIndex;

	/// [out] true if the trace is ended up in an invalid position ( inside a wall, floor, etc. )
	bool in_solid;

	/// some properties are automatically set to default values
	inline STrace();

	/// create the axis aligned bounding box that contains the entire trace
	inline void SetAABB();

};


inline STrace::STrace()
:
pvStart(NULL), pvGoal(NULL), fFraction(1.0f), vEnd(Vector3(0,0,0)), bvType(BVTYPE_AABB),
fRadius(0.0f), pSourceEntity(NULL), iNumTouches(0), pTouchedEntity(NULL),
iMaterialIndex(-1), in_solid(false), sTraceType(0)
{
//	memset( this, 0, sizeof(STrace) );
//	pvStart = NULL;
//	pvGoal = NULL;
	aabb.Nullify();
	aabb_swept.Nullify();
//	vEnd = Vector3(0,0,0);
//	fFraction = 1.0f;	//
//	pSourceEntity = NULL;
//	bvType = BVTYPE_AABB;	//
//	fRadius = 0;
	plane.normal = Vector3(0,0,0);
	plane.dist = 0;
//	pTouchedEntity = NULL;
//	iNumTouches = 0;
//	iMaterialIndex = -1;
//	in_solid = false;
//	sTraceType = 0;

	// TODO: use valid enum value for group index
	GroupIndex = 0;
}


inline void STrace::SetAABB()
{
	aabb_swept.vMin.x = pvStart->x < pvGoal->x ? pvStart->x : pvGoal->x;
	aabb_swept.vMin.y = pvStart->y < pvGoal->y ? pvStart->y : pvGoal->y;
	aabb_swept.vMin.z = pvStart->z < pvGoal->z ? pvStart->z : pvGoal->z;

	aabb_swept.vMax.x = pvStart->x < pvGoal->x ? pvGoal->x : pvStart->x;
	aabb_swept.vMax.y = pvStart->y < pvGoal->y ? pvGoal->y : pvStart->y;
	aabb_swept.vMax.z = pvStart->z < pvGoal->z ? pvGoal->z : pvStart->z;

	if(bvType != BVTYPE_DOT)
	{
		aabb_swept.vMin += aabb.vMin;
		aabb_swept.vMax += aabb.vMax;
	}

}



//=========================================================================================
// CTrace
//=========================================================================================
class CTrace
{
	/// holds line segmen (input)
	Vector3 m_vStart;
	Vector3 m_vGoal;

	AABB3 m_AABB;			/// volume of the entity approximated by aabb (local space)
	AABB3 m_SweptAABB;		/// volume swept by the movement of this trace (world space)
	char m_cBVType;
	float m_fRadius;

	/// holds a contact position
	Vector3 m_vEnd;
	float m_fFraction;

	CCopyEntity *m_pSourceEntity;
	bool m_bNoClipAgainstMap;
	short m_sClipType;

/*	int m_iNumTouchEntities;
	int m_iTouchEntityIndex;
	static CCopyEntity *ms_apTouchEntity[NUM_MAXTOUCHENTITIES];
	static int ms_iTouchEntityIndex;
*/

	std::vector<CCopyEntity *> *m_pvecpTouchEntity;

public:

	CTrace()
	: m_vStart(Vector3(0,0,0)), m_vGoal(Vector3(0,0,0)),
	m_fRadius(0), m_cBVType(BVTYPE_DOT), 
	m_pSourceEntity(NULL), m_pvecpTouchEntity(NULL)
	{
//		m_iNumTouchEntities = 0;
		m_sClipType = 0;
	}

	inline char GetBVType() { return m_cBVType; }
	inline Vector3 GetStartPosition() { return m_vStart; }
	inline Vector3 GetGoalPosition()  { return m_vGoal; }
	inline Vector3 GetEndPosition()   { return m_vEnd; }
	inline float GetEndFraction()         { return m_fFraction; }
	inline Vector3 GetSphereCenter()  { return m_vEnd; }
	inline float GetRadius()              { return m_fRadius; }
	inline AABB3& GetSweptAABB()         { return m_SweptAABB; }

	inline void SetEndFraction(float fFraction)  { m_fFraction = fFraction; }
	inline void SetEndPosition(Vector3 vEnd) { m_vEnd = vEnd; }

	inline short GetTraceType() { return m_sClipType; }
	inline void SetTraceType( short sClipType ) { m_sClipType |= sClipType; }

	inline void SetSphere( const Vector3& rvCenter, float fRadius )
	{
		m_AABB.SetMaxAndMin( Vector3( fRadius, fRadius, fRadius),
					         Vector3(-fRadius,-fRadius,-fRadius) );
		m_SweptAABB = m_AABB;
		m_SweptAABB.vMin += rvCenter; m_SweptAABB.vMax += rvCenter;
		m_cBVType = BVTYPE_SPHERE;
		m_vEnd = rvCenter;
		m_fRadius = fRadius;
	}

	inline void SetLineSegment( const Vector3& rvStart, const Vector3& rvGoal )
	{
		m_cBVType = BVTYPE_DOT;
		m_vStart = rvStart;
		m_vGoal  = rvGoal;
		m_fFraction = 1.0f;
		m_SweptAABB.Nullify();
		m_SweptAABB.AddPoint( rvStart );
		m_SweptAABB.AddPoint( rvGoal );
	}

	inline void SetTouchEntityBuffer( std::vector<CCopyEntity *> *pvecpTouchEntity ) { m_pvecpTouchEntity = pvecpTouchEntity; }

	inline size_t GetNumTouchEntities() { return m_pvecpTouchEntity ? m_pvecpTouchEntity->size() : 0; }

	inline CCopyEntity* GetTouchEntity(int i)
	{
		return (*m_pvecpTouchEntity)[i];

/*		if( m_iNumTouchEntities == 0 )
			return NULL;

		int iIndex = m_iTouchEntityIndex + i;
		if( NUM_MAXTOUCHENTITIES <= iIndex )
			iIndex = iIndex - NUM_MAXTOUCHENTITIES;
		return ms_apTouchEntity[iIndex];*/
	}

	inline void AddTouchEntity(CCopyEntity *pCopyEnt)
	{
		m_pvecpTouchEntity->push_back( pCopyEnt );
/*		if( m_iNumTouchEntities == 0 )
			m_iTouchEntityIndex = ms_iTouchEntityIndex;

		m_iNumTouchEntities++;

		ms_apTouchEntity[ms_iTouchEntityIndex++] = pCopyEnt;

		if( NUM_MAXTOUCHENTITIES <= ms_iTouchEntityIndex )
			ms_iTouchEntityIndex = 0;*/
	}

	inline void ClearTouchEntity()
	{
		if( m_pvecpTouchEntity )
			m_pvecpTouchEntity->resize( 0 );
//		m_iTouchEntityIndex = ms_iTouchEntityIndex;
//		m_iNumTouchEntities = 0;
	}
};


struct SContact
{
	CCopyEntity *pBody0;
	CCopyEntity *pBody1;	// NULL for static geometry

	Vector3 vContactPoint;	// collision point, in world space (i.e. sensible place to apply a restoring force)
	Vector3 vNormal;		// normalised direction in world space pointing towards body 0 from the other body/terrain

	// depth of the penetration. Can be -ve, indicating penetration isn't happening
	float fPenetrationDepth;
};



#endif	/*  __TRACE_H__  */