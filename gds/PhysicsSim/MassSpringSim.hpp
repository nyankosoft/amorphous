#ifndef  __MS_MASSSPRINGSIM_H__
#define  __MS_MASSSPRINGSIM_H__


#include <vector>
using namespace std;

#include "3DMath/Vector3.hpp"
#include "3DMath/AABTree.hpp"

#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;


#include <d3dx9.h>


class CMS_Sphere : public IArchiveObjectBase
{
public:

	Vector3 vLocalPosition;
	Vector3 vWorldPosition;

	float fRadius;
	float fRadiusSq;

	int iFixedPointGroup;

	AABB3 aabb;

public:

	const AABB3& GetAABB() const { return aabb; }

	void Serialize( IArchive& ar, const unsigned int version );

};


class CMS_Plane
{
	SPlane plane;

	int iFixedPointGroup;
};


/*
class CMS_Capsule
{};
*/


class CMS_PointProperty : public IArchiveObjectBase
{
public:

	enum eProperty
	{
		NUM_MAX_SPRINGS_PER_VERTEX	= 12
	};

	Vector3 vBasePosition;

	int iNumSprings;

	int aiPointIndex[NUM_MAX_SPRINGS_PER_VERTEX];

	float afSpringLength[NUM_MAX_SPRINGS_PER_VERTEX];

	float afSpring[NUM_MAX_SPRINGS_PER_VERTEX];

	int aiNormalPointIndex[2];

//	bool bIsFixed;

	/// indicates whether the point is pinned to a certain position of the mesh
	/// and should be excluded from the physics simulation.
	/// iFixedPointGroup >= 0 : the point is pinned
	/// iFixedPointGroup < 0 : the point is not pinned
	int iFixedPointGroup;

	float fMass;

public:

	CMS_PointProperty();

	void Serialize( IArchive& ar, const unsigned int version );
};


class CMS_PointState
{
public:
	Vector3 vPosition;
	Vector3 vForce;
	Vector3 vVelocity;

	Vector3 vNormal;

	inline void ClearForce() { vForce = Vector3(0,0,0); }
	inline void ResetVelocity() { vVelocity = Vector3(0,0,0); }
};


class CMS_MassSpringArchive : public IArchiveObjectBase
{
public:

	float m_fSpringConst;

	vector<CMS_PointProperty> m_vecControlPoint;

	/// indices to pinned points
	vector<int> m_veciFixedPointIndex;

	/// collision objects
	vector<CMS_Sphere> m_vecSphere;
//	vector<CMS_Plane> m_vecPlane;

public:

	void Serialize( IArchive& ar, const unsigned int version );

};



class CMS_MassSpringSim
{
	float m_fSpringConst;

	int m_iNumControlPoints;

	CMS_PointProperty *m_paPointProperty;

	CMS_PointState *m_paPoint;

	CMS_PointState *m_paTempPoint[2];

	int m_iNumFixedPoints;
	int *m_paFixedPointIndex;


	float m_fSimTimeStep;
	float m_fOverlapTime;

	Vector3 m_vGravityAccel;
	
	Vector3 m_vWindForce;

	/// collision objects
//	vector<CMS_Sphere> m_vecSphere; // maintained by aabtree
	vector<CMS_Plane> m_vecPlane;

	CNonLeafyAABTree<CMS_Sphere> m_AABTree;

/*
	void Integrate();

	void SatisfyConstraints();
*/

private:

	void FBEulerMethod( float dt );

	void UpdateForce( CMS_PointState *paPointState );

public:

	CMS_MassSpringSim();

	~CMS_MassSpringSim();

	/// prepares for rendering
	void ComputeNormals();

	bool LoadFromFile( const char *pcFilename );

	bool LoadFromArchive( CMS_MassSpringArchive& rMassSpringModel );

	void SaveToFile( const char *pcFilename );

	void Release();

	void Update( float fFrameTime );

	void SetBasePositions();

	void SetGravity( const Vector3& vGravityAccel ) { m_vGravityAccel = vGravityAccel;}

	void SetWindForce( const Vector3& vWindForce ) { m_vWindForce = vWindForce;}

	void SetSimulationTimeStep( float fTimeStep ) { m_fSimTimeStep = fTimeStep; }

	inline int GetNumControlPoints() { return m_iNumControlPoints; }

	inline CMS_PointState *GetControlPoints() { return m_paPoint; }

	void UpdateWorldProperties( D3DXMATRIX *paWorldTransform );

	void UpdateWorldProperties( Matrix34 *paWorldTransform );

	/// update the positions of control points that are pinned to the mesh
	void UpdateFixedMSpringPointPositions( Matrix34 *paMatrix );

	vector<CMS_Sphere>& GetCollisionSphere() { return m_AABTree.GetGeometryBuffer(); }

};


#endif		/*  __MS_MASSSPRINGSIM_H__  */
