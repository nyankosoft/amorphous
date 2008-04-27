
#include "JL_Shape_Box.h"

#include "JL_ShapeDesc_Box.h"
#include "TriangleMesh.h"
#include "JL_PhysicsActor.h"

#include "GameCommon/BasicGameMath.h"


const SEdgeIndex CJL_Shape_Box::ms_aEdgeIndex[12] =
{
	{ 0, 1 },	// top
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },

	{ 4, 5 },	// surround
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },

	{ 0, 4 },	// bottom
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};


CJL_Shape_Box::CJL_Shape_Box()
{
	m_vSideLength = Vector3(0,0,0);
}


CJL_Shape_Box::~CJL_Shape_Box()
{
}


void CJL_Shape_Box::InitSpecific( CJL_ShapeDesc& rShapeDesc )
{
	m_vSideLength = ((CJL_ShapeDesc_Box *)&rShapeDesc)->vSideLength;

	Scalar x = m_vSideLength.x / 2.0f;
	Scalar y = m_vSideLength.y / 2.0f;
	Scalar z = m_vSideLength.z / 2.0f;

	// set edge points for box
	m_avEdgePoint[0] = Vector3(-x, y, z);
	m_avEdgePoint[1] = Vector3( x, y, z);
	m_avEdgePoint[2] = Vector3( x, y,-z);
	m_avEdgePoint[3] = Vector3(-x, y,-z);
	m_avEdgePoint[4] = Vector3(-x,-y, z);
	m_avEdgePoint[5] = Vector3( x,-y, z);
	m_avEdgePoint[6] = Vector3( x,-y,-z);
	m_avEdgePoint[7] = Vector3(-x,-y,-z);
}


void CJL_Shape_Box::SetSize( Scalar fWidth_X, Scalar fHeight_Y, Scalar fDepth_Z )
{
	// create bsp-tree for box volume
//	m_BSPTree.CreateBoxBSPTree( fWidth_X, fHeight_Y, fDepth_Z );
}


void CJL_Shape_Box::UpdateWorldProperties()
{
	// update world pose
	m_WorldPose.vPosition = m_pPhysicsActor->GetOrientation() * m_LocalPose.vPosition + m_pPhysicsActor->GetPosition();
	m_WorldPose.matOrient = m_pPhysicsActor->GetOrientation() * m_LocalPose.matOrient;

	m_WorldAABB.Nullify();

	int i;
	Vector3 v;
	for( i=0; i<8; i++ )
	{
		// transform edge points from box space to world space
		m_WorldPose.Transform( m_avWorldEdgePoint[i], m_avEdgePoint[i] );

		// update world aabb
		m_WorldAABB.AddPoint( m_avWorldEdgePoint[i] );
	}
}


// returns true if the box and the triangle are overlapping
// also returns penetration depth along the specified axis
bool CJL_Shape_Box::SepAxisTest( Scalar& depth, Vector3& rvAxis, CTriangle& rTriangle, float fCollTolerance )
{
//	Scalar fColTolerance = 0.005f;
//	Scalar fColTolerance = 0.000f;
	Scalar min0, min1, max0, max1;
	GetBoxSpan( min0, max0, rvAxis );
	rTriangle.GetSpan( min1, max1, rvAxis );

//	if( max0 < min1 + fCollTolerance ||
//		max1 < min0 + fCollTolerance )
	if( max0 + fCollTolerance < min1 ||
		max1 + fCollTolerance < min0 )
	{
		return false;	// box and triangle are disjoint
	}

	if( min0 < min1 && max1 < max0 )
	{	// edge in the box
		depth = fmin( max1 - min0, max0 - min1 );
	}
	else if( min1 < min0 && max0 < max1 )
	{	// box in the edge
		depth = fmin( max1 - min0, max0 - min1 );
	}
	else
	{
		depth  = ( max0 < max1 ) ? max0 : max1;
		depth -= ( min0 < min1 ) ? min1 : min0;
	}

	return true;
}


// returns true if intersecting on the given axis
bool CJL_Shape_Box::SepAxisTest( Scalar& depth, Vector3& rvAxis, CJL_Shape_Box& rBox, float fCollTolerance )
{
//	Scalar fColTolerance = 0.005f;
	Scalar min0, min1, max0, max1;
	GetBoxSpan( min0, max0, rvAxis );
	rBox.GetBoxSpan( min1, max1, rvAxis );

//	if( max0 < min1 + fColTolerance ||
//		max1 < min0 + fColTolerance )
	if( max0 + fCollTolerance < min1 ||
		max1 + fCollTolerance < min0 )
	{
		return false;	// box and triangle are disjoint
	}

	if( min0 < min1 && max1 < max0 )
	{	// edge in the box
		depth = max0 - min1;
	}
	else if( min1 < min0 && max0 < max1 )
	{	// box in the edge
		depth = max1 - min0;
	}
	else
	{
		depth  = ( max0 < max1 ) ? max0 : max1;
		depth -= ( min0 < min1 ) ? min1 : min0;
	}

	return true;
}