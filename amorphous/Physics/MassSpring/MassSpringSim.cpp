#include "MassSpringSim.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/FixedVector.hpp"

using namespace std;


namespace amorphous
{


int g_Counter = 0;


//=================================================================================
// CMS_Sphere
//=================================================================================

void CMS_Sphere::Serialize( IArchive& ar, const unsigned int version )
{
	ar & vLocalPosition & vWorldPosition;
	ar & fRadius & fRadiusSq;
	ar & iFixedPointGroup;
}


//=================================================================================
// CMS_PointProperty
//=================================================================================

CMS_PointProperty::CMS_PointProperty()
{
	iNumSprings = 0;

//	bIsFixed = false;
	iFixedPointGroup = -1;

	fMass = 0.1f;
}


void CMS_PointProperty::Serialize( IArchive& ar, const unsigned int version )
{
	ar & vBasePosition;

	ar & iNumSprings;

	int i;
	for( i=0; i<NUM_MAX_SPRINGS_PER_VERTEX; i++ )
        ar & aiPointIndex[i];

	for( i=0; i<NUM_MAX_SPRINGS_PER_VERTEX; i++ )
        ar & afSpringLength[i];

	for( i=0; i<NUM_MAX_SPRINGS_PER_VERTEX; i++ )
		ar & afSpring[i];

	ar & aiNormalPointIndex[0] & aiNormalPointIndex[1];

//	ar & bIsFixed;
	ar & iFixedPointGroup;

	ar & fMass;
}



//=================================================================================
// CMS_MassSpringArchive
//=================================================================================

void CMS_MassSpringArchive::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_fSpringConst;

	ar & m_vecControlPoint;

	ar & m_veciFixedPointIndex;

	ar & m_vecSphere;
}



//=================================================================================
// CMS_MassSpringSim
//=================================================================================

CMS_MassSpringSim::CMS_MassSpringSim()
{
	m_iNumControlPoints = 0;

	m_paPointProperty = NULL;

	m_paPoint = NULL;

	m_paTempPoint[0] = NULL;
	m_paTempPoint[1] = NULL;

	m_iNumFixedPoints = 0;
	m_paFixedPointIndex = NULL;

	m_fSimTimeStep = 0.001f;
	m_fOverlapTime = 0;

	m_vGravityAccel = Vector3(0, -9.8f, 0);
	m_vWindForce = Vector3(0,0,0);
}


CMS_MassSpringSim::~CMS_MassSpringSim()
{
	Release();
}


void CMS_MassSpringSim::Release()
{
	SafeDeleteArray( m_paPointProperty );
	SafeDeleteArray( m_paPoint );
	SafeDeleteArray( m_paTempPoint[0] );
	SafeDeleteArray( m_paTempPoint[1] );
	m_iNumControlPoints = 0;

	SafeDeleteArray( m_paFixedPointIndex );
	m_iNumFixedPoints = 0;
}


bool CMS_MassSpringSim::LoadFromFile( const char *pcFilename )
{
	// load archive
	BinaryArchive_Input archive( pcFilename );

	CMS_MassSpringArchive mass_spring_model;

	if( !(archive >> mass_spring_model) )
		return false;

	return LoadFromArchive( mass_spring_model );
}


bool CMS_MassSpringSim::LoadFromArchive( CMS_MassSpringArchive& rMassSpringModel )
{
	Release();

	m_iNumControlPoints = (int)rMassSpringModel.m_vecControlPoint.size();

	// allocate memory for control points
	m_paPointProperty = new CMS_PointProperty [m_iNumControlPoints];
	m_paPoint         = new CMS_PointState [m_iNumControlPoints];
	m_paTempPoint[0]  = new CMS_PointState [m_iNumControlPoints];
	m_paTempPoint[1]  = new CMS_PointState [m_iNumControlPoints];

	int i, iNumPoints = m_iNumControlPoints;
	for( i=0; i<iNumPoints; i++ )
	{
		m_paPointProperty[i] = rMassSpringModel.m_vecControlPoint[i];
	}


	// load indices to fixed points
	m_iNumFixedPoints = (int)rMassSpringModel.m_veciFixedPointIndex.size();
	m_paFixedPointIndex = new int [m_iNumFixedPoints];
	for( i=0; i<m_iNumFixedPoints; i++ )
		m_paFixedPointIndex[i] = rMassSpringModel.m_veciFixedPointIndex[i];

	SetBasePositions();

	m_fSpringConst = rMassSpringModel.m_fSpringConst;

	// set collision spheres
	vector<CMS_Sphere>& rvecDestSphereBuffer = m_AABTree.GetGeometryBuffer();
	rvecDestSphereBuffer = rMassSpringModel.m_vecSphere;

	// set default positions
	SetBasePositions();

	// create space partitioning tree
	AABB3 aabb;
	aabb.vMin = Vector3( -0.5f, -1.0f, -0.4f );
	aabb.vMax = Vector3(  0.5f,  1.0f,  0.4f );
	m_AABTree.Build( aabb, 4 );

	return true;
}


void CMS_MassSpringSim::SetBasePositions()
{
	int j;
	const int iNumPoints = m_iNumControlPoints;
	for( int i=0; i<iNumPoints; i++ )
	{
		Vector3 vPos = m_paPointProperty[i].vBasePosition;
		m_paPoint[i].vPosition = vPos;

		m_paPoint[i].ClearForce();
		m_paPoint[i].ResetVelocity();

		for( j=0; j<2; j++ )
		{
			m_paTempPoint[j][i].vPosition = vPos;

			m_paTempPoint[j][i].ClearForce();
			m_paTempPoint[j][i].ResetVelocity();
		}
	}

	vector<CMS_Sphere>& rvecSphere = m_AABTree.GetGeometryBuffer();

	const size_t iNumSpheres = rvecSphere.size();
	for( size_t i=0; i<iNumSpheres; i++ )
	{
		rvecSphere[i].vWorldPosition = rvecSphere[i].vLocalPosition;
	}


	m_AABTree.SetWorldPosition( Vector3(0,0,0) );

}



//#define WINDFORCE 5.0f
#define AIRDAMPING 2.0f

#define LEGFORCE 10000.0f
#define LEGDAMPING 20.0f


void CMS_MassSpringSim::UpdateForce( CMS_PointState *paPoint )
{
	Vector3 vDelta, vForce;

	Vector3 vGravityAccel = m_vGravityAccel;
	Vector3 vWindForce = m_vWindForce;

	CMS_PointProperty *paProperty = m_paPointProperty;

	float fSpringConst = m_fSpringConst;
	float d;


	const int iNumPoints = m_iNumControlPoints;
	int iNumAdjacentPoints;

	// clear forces
	for( int i=0; i<iNumPoints; i++ )
	{
		paPoint[i].vForce = Vector3(0,0,0);
	}


	vector<CMS_Sphere>& rvecSphere = m_AABTree.GetGeometryBuffer();
	size_t iNumSpheres = rvecSphere.size();

	// calc forces at each control point
	for( int i=0; i<iNumPoints; i++ )
	{
		CMS_PointState& rPoint = paPoint[i];
		CMS_PointProperty& rProperty = paProperty[i];

		vForce = Vector3(0,0,0);
	
		if( 0 <= rProperty.iFixedPointGroup )
			continue;

		// Simulated wind
/*		float r = g_Counter / 70.0f; // Not very accurate
		float r2 = g_Counter / 13.0f; // Not very accurate

		rPoint.vForce.x += cosf( r ) * sinf( r2 ) * WINDFORCE;
		rPoint.vForce.z += sinf( r ) * sinf( r2 ) * WINDFORCE;
*/
		rPoint.vForce += vWindForce;

		rPoint.vForce += Vector3( rand() * 0.01f / (float)RAND_MAX, 0, rand() * 0.01f / (float)RAND_MAX );

		// Drag (air resistance)
		rPoint.vForce -= rPoint.vVelocity * AIRDAMPING;


		// gravity
		rPoint.vForce += rProperty.fMass * vGravityAccel;

		// spring constraints
		iNumAdjacentPoints = rProperty.iNumSprings;
		for( int j=0; j<iNumAdjacentPoints; j++ )
		{
			CMS_PointState& rPoint2 = paPoint[rProperty.aiPointIndex[j]];

			vDelta = rPoint.vPosition - rPoint2.vPosition;

			d = Vec3Length( vDelta );

/*			if( d < 0.0001f )
				int iTooClose = 1;

			if( 0.5f < d )
				int iTornApart = 1;
*/
//			vForce = vDelta / d * ( rProperty.afSpringLength[j] - d ) * fSpringConst;
			vForce = vDelta / d * ( rProperty.afSpringLength[j] - d ) * rProperty.afSpring[j];

			float spr = rProperty.afSpring[j];
			if( spr < 0 || 800.0f < spr )
				assert(0);

			rPoint.vForce += vForce;
			rPoint2.vForce -= vForce;
		}


		static vector<int> vecIndex;
		vecIndex.resize(0);
//		vecIndex.reserve(16);

		m_AABTree.GetPossiblyIntersectingAABBs( paPoint[i].vPosition, vecIndex );
		iNumSpheres = vecIndex.size();

		vector<CMS_Sphere>& rvecSphere = m_AABTree.GetGeometryBuffer();

		Vector3 vDelta;
//		float d;
		for( size_t j=0; j<iNumSpheres; j++ )
		{
//			CMS_Sphere& rSphere = m_vecSphere[j];
			CMS_Sphere& rSphere = rvecSphere[ vecIndex[j] ];

			vDelta = paPoint[i].vPosition - rSphere.vWorldPosition;
			d = Vec3LengthSq( vDelta );
			if( d < rSphere.fRadiusSq )
			{
				// found collision
				d = sqrtf( d );
				vDelta = vDelta * ( rSphere.fRadius - d ) / d;
//				paPoint[i].vPosition += vDelta * ( m_vecSphere[j].fRadius - d );
				paPoint[i].vForce += vDelta * LEGFORCE;
				paPoint[i].vForce -= paPoint[i].vVelocity * LEGDAMPING;
			}
		}
	}
}


void CMS_MassSpringSim::FBEulerMethod( float dt )
{
	CMS_PointState *paPoint = m_paPoint;
	CMS_PointProperty *paProperty = m_paPointProperty;

	UpdateForce( paPoint );

	int i, iNumControlPoints = m_iNumControlPoints;
	for( i=0; i < iNumControlPoints; ++i )
	{
		if( 0 <= paProperty[i].iFixedPointGroup )
			continue;

		paPoint[i].vVelocity += paPoint[i].vForce / paProperty[i].fMass * dt;
		paPoint[i].vPosition += paPoint[i].vVelocity * dt;
//		Grids[j][i].Vel += ( Grids[j][i].Frc / Grids[j][i].Mass ) * DELTATIME;
//		Grids[j][i].Pos += Grids[j][i].Vel * DELTATIME;
	}
}


void CMS_MassSpringSim::ComputeNormals()
{
	CMS_PointState *paPoint = m_paPoint;
	CMS_PointProperty *paProperty = m_paPointProperty;

	Vector3 vNormal;

	int i, iNumControlPoints = m_iNumControlPoints;
	for( i=0; i < iNumControlPoints; ++i )
	{
		Vector3& v0 = paPoint[paProperty[i].aiNormalPointIndex[0]].vPosition - paPoint[i].vPosition;
		Vector3& v1 = paPoint[paProperty[i].aiNormalPointIndex[1]].vPosition - paPoint[i].vPosition;
		vNormal = Vec3Cross(v0,v1);

		Vec3Normalize( paPoint[i].vNormal, vNormal );
	}
}


void CMS_MassSpringSim::UpdateFixedMSpringPointPositions( Matrix34 *paMatrix )
{
	CMS_PointState *paPoint = m_paPoint;
	CMS_PointProperty *paProperty = m_paPointProperty;
	int *paFixedPointIndex = m_paFixedPointIndex;

	int i, iNumFixedPoints = m_iNumFixedPoints;
	int pnt_index, grp_index;
	for( i=0; i<iNumFixedPoints; i++ )
	{
		pnt_index = paFixedPointIndex[i];
		grp_index = paProperty[pnt_index].iFixedPointGroup;

		paMatrix[grp_index].Transform( paPoint[pnt_index].vPosition, paProperty[pnt_index].vBasePosition );
	}
/*
	int num_spheres = m_vecSphere.size();
	for( i=0; i<num_spheres; i++ )
	{
		CMS_Sphere& rSphere = m_vecSphere[i];

		grp_index = rSphere.iFixedPointGroup;

		if( grp_index < 0 )
			continue;

		paMatrix[grp_index].Transform( rSphere.vWorldPosition, rSphere.vLocalPosition );
	}*/
}


void CMS_MassSpringSim::Update( float fFrameTime )
{
	float dt;
	int num_loops;

	// update collision objects world pose  in the tree

	// update center AABBs of the sphere
	float r;
	Vector3 vPos;
	vector<CMS_Sphere>& rvecSphere = m_AABTree.GetGeometryBuffer();
	const size_t num_spheres = rvecSphere.size();

	for( size_t i=0; i<num_spheres; i++ )
	{
		r = rvecSphere[i].fRadius;
		vPos = rvecSphere[i].vWorldPosition;

		rvecSphere[i].aabb.vMin = vPos + Vector3( -r, -r, -r );
		rvecSphere[i].aabb.vMax = vPos + Vector3(  r,  r,  r );
	}

	// update 
	m_AABTree.UpdateRegisteredGeometries();


	if( false /*SIM_DONT_DIVIDE_TIMESTEP*/ )
	{
		dt = fFrameTime;
		num_loops = 1;

		FBEulerMethod( dt );
	}
	else
	{
		// split the timestep into fixed size chunks

		dt = m_fSimTimeStep;	// time step

		float total_time = fFrameTime + m_fOverlapTime;

		num_loops = (int) (total_time / dt);

		m_fOverlapTime = total_time - num_loops * dt;

        for ( int i = 0 ; i < num_loops ; ++i )
			FBEulerMethod( dt );
	}

	g_Counter++;
}


//void CMS_MassSpringSim::UpdateWorldProperties( D3DXMATRIX *paWorldTransform )
void CMS_MassSpringSim::UpdateWorldProperties( Matrix34 *paWorldTransform )
{
	CMS_PointState *paPoint = m_paPoint;
	CMS_PointProperty *paProperty = m_paPointProperty;
	int *paFixedPointIndex = m_paFixedPointIndex;

	const int num_fixed_points = m_iNumFixedPoints;
	int pnt_index, grp_index;
	for( int i=0; i<num_fixed_points; i++ )
	{
		pnt_index = paFixedPointIndex[i];
		grp_index = paProperty[pnt_index].iFixedPointGroup;

//		D3DXVec3TransformCoord( &(paPoint[pnt_index].vPosition),
//			                    &(paProperty[pnt_index].vBasePosition),
//								&paWorldTransform[grp_index] );

		paWorldTransform[grp_index].Transform( paPoint[pnt_index].vPosition, paProperty[pnt_index].vBasePosition );
	}

	vector<CMS_Sphere>& rvecSphere = m_AABTree.GetGeometryBuffer();
	const size_t num_spheres = rvecSphere.size();
	for( size_t i=0; i<num_spheres; i++ )
	{
		CMS_Sphere& rSphere = rvecSphere[i];
		grp_index = rSphere.iFixedPointGroup;

		if( grp_index < 0 )
			continue;

//		D3DXVec3TransformCoord( &rSphere.vWorldPosition,
//			                    &rSphere.vLocalPosition,
//								&paWorldTransform[grp_index] );

		paWorldTransform[grp_index].Transform( rSphere.vWorldPosition, rSphere.vLocalPosition );
	}
}


} // amorphous
