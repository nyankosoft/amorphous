//========================================================================
//    bsptree2.cpp
//		- a collection of collision detection functions using BSP-Tree 
//========================================================================

#include "bsptree.h"
#include "Support/memory_helpers.h"

#include <assert.h>


CBSPTree::CBSPTree( const SNode_f *pNodes, int num_nodes, const SPlane* pPlanes, int num_planes )
{
	m_paNode = NULL; m_NumNodes = 0;
	m_paPlane = NULL; m_NumPlanes = 0;

	Init( pNodes, num_nodes, pPlanes, num_planes );
}


CBSPTree::~CBSPTree()
{
	Release();
}


void CBSPTree::Release()
{
	SafeDeleteArray( m_paNode );
	SafeDeleteArray( m_paPlane );
	m_NumNodes = 0;
	m_NumPlanes = 0;
}


void CBSPTree::Init( const SNode_f *pNodes, int num_nodes, const SPlane* pPlanes, int num_planes )
{
	Release();

	m_paNode = new SNode_f [num_nodes];
	memcpy( m_paNode, pNodes, sizeof(SNode_f) * num_nodes );
	m_NumNodes = num_nodes;

	m_paPlane = new SPlane [num_planes];
	memcpy( m_paPlane, pPlanes, sizeof(SPlane) * num_planes );
	m_NumPlanes = num_planes;
}



#define EPSILON 0.0001f //Before introducing 'grid system' EPSILON 0.001


//Check if the 'tr.vEnd' is in a valid position
//Returns the index of the cell where the 'tr.vEnd' is located
short CBSPTree::CheckPosition( STrace& tr, short sStartNodeIndex )
{
	short prev_nodeindex = 0;
	short nodeindex = sStartNodeIndex;
	float dist;

	SNode_f* paNode = m_paNode + sStartNodeIndex;
	SPlane* paPlane = m_paPlane;

	Vector3& rPoint = tr.vEnd;
	Vector3 vExtents = tr.aabb.GetExtents();

	while(0 <= nodeindex)
	{
		SPlane& rPlane = paPlane[ paNode[ nodeindex ].sPlaneIndex ];

		dist = rPlane.GetDistanceFromPoint( rPoint );

		if( tr.bvType != BVTYPE_DOT )
			dist -= tr.aabb.GetRadiusForPlane( rPlane );

		prev_nodeindex = nodeindex;
		if(0 <= dist)
			nodeindex = paNode[nodeindex].sFrontChild;
		else
			nodeindex = paNode[nodeindex].sBackChild;
	}
	if(nodeindex == CONTENTS_EMPTY)
		return paNode[prev_nodeindex].sCellIndex;
	else //(nodeindex == CONTENTS_SOLID)
	{
		tr.in_solid = true;
		return CONTENTS_SOLID;
	}
}

//only for point
short CBSPTree::CheckPosition( Vector3& rvPoint, short sStartNodeIndex )
{
	short prev_nodeindex = 0;
	short nodeindex = sStartNodeIndex;
	float dist;

	SNode_f* paNode = m_paNode;
	SPlane* paPlane = m_paPlane;

	while(0 <= nodeindex)
	{
		SPlane& rPlane = paPlane[ paNode[ nodeindex ].sPlaneIndex ];

		dist = rPlane.GetDistanceFromPoint( rvPoint );

		prev_nodeindex = nodeindex;
		if(0 <= dist)
			nodeindex = paNode[nodeindex].sFrontChild;
		else
			nodeindex = paNode[nodeindex].sBackChild;
	}
	if(nodeindex == CONTENTS_EMPTY)
		return paNode[prev_nodeindex].sCellIndex;
	else //(nodeindex == CONTENTS_SOLID)
	{
		return CONTENTS_SOLID;
	}
}

#define MAX_DIVIDE_NUM 64

// Clip the line-segment according to the BSP-Tree traversal
// Doesn't change the source data 'tr.pvStart' & 'tr.pvGoal'
// clipped position is output to the 'tr.vEnd'
//This function dynamically shifts the plane of each node, acording to the volume
//of the AABB of the trace

short CBSPTree::ClipTrace( STrace& tr )
{
	if( Vec3LengthSq( *tr.pvGoal - *tr.pvStart ) < 0.000000001f )
		return CONTENTS_EMPTY;

	short nodenum, prev_nodenum;
	SNode_f* paNode = m_paNode;
	SPlane* paPlane = m_paPlane;

	Vector3 vP1, vP2;
	float fd1, fd2;
	float f, fStart, fGoal;

	char cNumBackTrack;
	static short nodenums[MAX_DIVIDE_NUM];
	static float fGoals[MAX_DIVIDE_NUM];
	static Vector3 vGoals[MAX_DIVIDE_NUM];
	static char rearside[MAX_DIVIDE_NUM];
	cNumBackTrack = 0;

	nodenum = 0; prev_nodenum = 0;

	Vector3 &vP0 = *tr.pvStart;

	fStart = 0;
	vP1 = *tr.pvStart;
	fGoal = 1;
	if(tr.fFraction == 1)	//hasn't hit anything yet
		vP2 = *tr.pvGoal;
	else	//already hit something
		vP2 = tr.vEnd;

	SPlane* pTouchPlane = paPlane;
	SPlane* pPlane = NULL;
	
	short sEndCellIndex = 32767;

	while(1)
	{
		while(0 <= nodenum)
		{
			pPlane = paPlane + paNode[nodenum].sPlaneIndex;

			fd1 = pPlane->GetDistanceFromPoint( vP1 );
			fd2 = pPlane->GetDistanceFromPoint( vP2 );

			if( tr.bvType != BVTYPE_DOT )
			{
				fd1 -= tr.aabb.GetRadiusForPlane( *pPlane );
				fd2 -= tr.aabb.GetRadiusForPlane( *pPlane );
			}

			if(0 <= fd1 && 0 <= fd2)
			{
				prev_nodenum = nodenum;
				nodenum = paNode[nodenum].sFrontChild;
				continue;
			}
			if(fd1 < 0 && fd2 < 0)
			{
				prev_nodenum = nodenum;
				nodenum = paNode[nodenum].sBackChild;
				continue;
			}

			//Need to split the line.

			if(fd1 < 0)
				f = (fd1 + EPSILON) / (fd1 - fd2);
			else
				f = (fd1 - EPSILON) / (fd1 - fd2);

			if(f < 0)
				f = 0;	//This occurs when fd1 < EPSILON
			else if(1 < f)
				f = 1;

			//Store the rear-line data in stack
			fGoals[cNumBackTrack] = fGoal;
			vGoals[cNumBackTrack] = vP2;
			nodenums[cNumBackTrack] = nodenum;

			//Prepare for the next fore-line check
			fGoal = fStart + f * (fGoal - fStart);
			vP2 = vP1 + f * (vP2 - vP1);

			prev_nodenum = nodenum;

			if( 0 <= fd1 )
			{
				nodenum = paNode[nodenum].sFrontChild;
				rearside[cNumBackTrack] = 1;	//the rear-line is behind the plane
			}
			else
			{
				nodenum = paNode[nodenum].sBackChild;
				rearside[cNumBackTrack] = 0;	//the rear-line is in front of the plane
			}

			cNumBackTrack++;

			if( MAX_DIVIDE_NUM <= cNumBackTrack )
				assert( !"stack overflow - bsptree traversal in CBSPTree::ClipTrace()" );

		}

		if(nodenum == CONTENTS_EMPTY)
		{
			sEndCellIndex = paNode[prev_nodenum].sCellIndex;

			if(0 < cNumBackTrack) //if a subsequent line exists,
			{//Prepare for checking the next rear-line.
				cNumBackTrack--;
				fStart = fGoal;
				fGoal  = fGoals[cNumBackTrack];
				vP1 = vP2;
				vP2 = vGoals[cNumBackTrack];

				prev_nodenum = nodenums[cNumBackTrack];
				if(rearside[cNumBackTrack] == 1)
					nodenum	= paNode[ prev_nodenum ].sBackChild;
				else
					nodenum = paNode[ prev_nodenum ].sFrontChild;

				pTouchPlane = paPlane + paNode[prev_nodenum].sPlaneIndex;

				continue;
			}
			else
			{
				tr.vEnd = vP2;
				tr.in_solid = false;
				return sEndCellIndex;		// returns the cell where the trace has ended up
			}
		}

		if(nodenum == CONTENTS_SOLID)
		{
			tr.vEnd = vP1;
			while( CheckPosition( tr ) == CONTENTS_SOLID )
			{
				fStart -= 0.1f;
				if( fStart < 0 /*|| fStart != fStart*/ )
				{	// returns the original position (this part should be improved)
//					if( fStart != fStart )
//						int error = 1;
					//MessageBox(NULL, "Cannot get out of the solid area.","Error", MB_OK|MB_ICONWARNING);
					tr.fFraction = 0.0f;
///					tr.vEnd = vP1;
					tr.vEnd = *tr.pvStart;
					tr.in_solid = true;
					return CONTENTS_SOLID;
				}
				tr.vEnd = vP0 + fStart * (tr.vEnd - vP0);
			}
//			tr.fFraction = fStart;
			tr.fFraction = fStart * tr.fFraction;
			tr.in_solid = false;

			//Record the plane 'tr' hit
			tr.iNumTouches++;
			tr.plane = *pTouchPlane;

			return sEndCellIndex;
		}
	}

	return 0;
}

/*
short CBSPTree::ClipTrace( STrace& tr )
{
	ClipTrace_r( 0, 0, 1, *tr.pvStart, *tr.pvGoal, tr);
	return 0;
}
*/
bool CBSPTree::ClipTrace_r( short sNodeIndex, float p1f, float p2f,
						   Vector3& vP1, Vector3& vP2, STrace &rTrace)
{
	float		t1, t2;
	float		frac;
	Vector3 vMid;
	int			side;
	float		midf;

// check for empty
	if (sNodeIndex < 0)
	{
		if (sNodeIndex != CONTENTS_SOLID)
		{
			rTrace.in_solid = false;
		}
		else
			rTrace.in_solid = true;	// original: trace->startsolid = true;
		return true;		// empty
	}

	SNode_f& rNode = m_paNode[sNodeIndex];
	SPlane& rPlane = m_paPlane[rNode.sPlaneIndex];

//
// find the point distances
//

	t1 = rPlane.GetDistanceFromPoint( vP1 );
	t2 = rPlane.GetDistanceFromPoint( vP2 );

	if( rTrace.bvType != BVTYPE_DOT )
	{
		t1 -= rTrace.aabb.GetRadiusForPlane( rPlane );
		t2 -= rTrace.aabb.GetRadiusForPlane( rPlane );
	}

	
#if 1
	if (t1 >= 0 && t2 >= 0)
		return ClipTrace_r(rNode.sFrontChild, p1f, p2f, vP1, vP2, rTrace);
	if (t1 < 0 && t2 < 0)
		return ClipTrace_r(rNode.sBackChild, p1f, p2f, vP1, vP2, rTrace);
#else
	if ( (t1 >= EPSILON && t2 >= EPSILON) || (t2 > t1 && t1 >= 0) )
		return ClipTrace_r(rNode.sFrontChild, p1f, p2f, vP1, vP2, rTrace);
	if ( (t1 <= -EPSILON && t2 <= -EPSILON) || (t2 < t1 && t1 <= 0) )
		return ClipTrace_r(rNode.sBackChild, p1f, p2f, vP1, vP2, rTrace);
#endif

// put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < 0)
		frac = (t1 + EPSILON)/(t1-t2);
	else
		frac = (t1 - EPSILON)/(t1-t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;
		
	midf = p1f + (p2f - p1f)*frac;
	vMid = vP1 + frac*(vP2 - vP1);

	side = (t1 < 0);
	short sNextIndex = (side == 0) ? rNode.sFrontChild : rNode.sBackChild;
	short sInvIndex =  (side == 0) ? rNode.sBackChild : rNode.sFrontChild;

// move up to the node
	if( !ClipTrace_r(sNextIndex, p1f, midf, vP1, vMid, rTrace) )
		return false;

#ifdef PARANOID
	if ( CheckPosition(vMid, sNextIndex) == CONTENTS_SOLID )
	{
		//Con_Printf ("mid PointInHullSolid\n");
		return false;
	}
#endif
	
	if ( CheckPosition(vMid, sInvIndex) != CONTENTS_SOLID )
// go past the node
		return ClipTrace_r(sInvIndex, midf, p2f, vMid, vP2, rTrace);
	
	if ( rTrace.in_solid )	/*  original: trace->allsolid  */
		return false;		// never got out of the solid area
		
//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		rTrace.plane = rPlane;
	}
	else
	{
		rTrace.plane = rPlane;
		rTrace.plane.Flip();
	}

	while ( CheckPosition(vMid, 0) == CONTENTS_SOLID )
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1f;
		if (frac < 0)
		{
			rTrace.fFraction = midf;
			rTrace.vEnd = vMid;
			return false;
		}
		midf = p1f + (p2f - p1f) * frac;
		vMid = vP1 + frac*(vP2 - vP1);
	}

	rTrace.fFraction = midf;
	rTrace.vEnd = vMid;

	return false;
}


// create bsp-tree that represents box volume
void CBSPTree::CreateBoxBSPTree( float fWidth_X, float fHeight_Y, float fDepth_Z )
{
	Release();
	m_paNode  = new SNode_f [6];
	m_paPlane = new SPlane [6];

	m_paPlane[0].normal = Vector3( 1, 0, 0); m_paPlane[0].type = 0;
	m_paPlane[1].normal = Vector3(-1, 0, 0); m_paPlane[1].type = 0;
	m_paPlane[0].dist = m_paPlane[1].dist = fWidth_X / 2.0f;

	m_paPlane[4].normal = Vector3( 0, 0, 1); m_paPlane[4].type = 2;
	m_paPlane[5].normal = Vector3( 0, 0,-1); m_paPlane[5].type = 2;
	m_paPlane[4].dist = m_paPlane[5].dist = fDepth_Z / 2.0f;

	m_paPlane[2].normal = Vector3( 0, 1, 0); m_paPlane[2].type = 1;
	m_paPlane[3].normal = Vector3( 0,-1, 0); m_paPlane[3].type = 1;
	m_paPlane[2].dist = m_paPlane[3].dist = fHeight_Y / 2.0f;

	int i;
	for(i=0; i<6; i++)
	{
		m_paNode[i].sPlaneIndex = i;
		m_paNode[i].sFrontChild = CONTENTS_EMPTY;
		m_paNode[i].sBackChild = i+1;	
		m_paNode[i].sCellIndex = 0;
	}
	m_paNode[5].sBackChild = CONTENTS_SOLID;
}

/*
void CBSPTree::Transform( D3DXMATRIX& rmatWorld, CBSPTree& rLocalBSPTree )
{
	Vector3 vWorldPos;
	vWorldPos.x = rmatWorld._41;
	vWorldPos.x = rmatWorld._42;
	vWorldPos.x = rmatWorld._43;
	rmatWorld._41 = 0.0f;
	rmatWorld._42 = 0.0f;
	rmatWorld._43 = 0.0f;
	D3DXMATRIX& rmatWorldRotation = rmatWorld;

	int i;
	for( i=0; i<6; i++ )
	{
		D3DXVec3TransformCoord( &m_paPlane[i].normal &rLocalBSPTree.m_paPlane[i].normal, &rmatWorldRotation );
		m_paPlane[i].dist = D3DXVec3Dot(&m_paPlane[i].normal,&vWorldPos) + rLocalBSPTree.m_paPlane[i].dist;
	}
}*/


bool CBSPTree::LoadFromFile( const char *pcFilename )
{
	FILE *fp = fopen( pcFilename, "rb" );

	if(!fp)
		return false;

	Release();

	// load bsp-tree nodes
	fread( &m_NumNodes, sizeof(int), 1, fp );
	m_paNode = new SNode_f [m_NumNodes];
	fread( m_paNode, sizeof(SNode_f), m_NumNodes, fp );

	// load planes
	fread( &m_NumPlanes, sizeof(int), 1, fp );
	m_paPlane = new SPlane [m_NumPlanes];
	fread( m_paPlane, sizeof(SPlane), m_NumPlanes, fp );

	fclose(fp);
	return true;
}


bool CBSPTree::WriteToFile( const char *pcFilename )
{
	FILE *fp = fopen( pcFilename, "w" );

	if(!fp)
		return false;

	// load bsp-tree nodes
	fprintf( fp, "%d nodes\n", m_NumNodes );
	int i;
	for(i=0; i<m_NumNodes; i++)
	{
		fprintf( fp, "[%03d] - p%03d  c%03d  F[%03d]  B[%03d]\n",
			                                        i,
			                                        m_paNode[i].sPlaneIndex,
			                                        m_paNode[i].sCellIndex,
			                                        m_paNode[i].sFrontChild,
			                                        m_paNode[i].sBackChild );
	}

	fprintf( fp, "\n%d planes\n", m_NumPlanes );
	for(i=0; i<m_NumPlanes; i++)
	{
		fprintf( fp, "[%03d] - N(%03.2f %03.2f %03.2f)  D%03.2f\n",
			                                        i,
													m_paPlane[i].normal.x,
													m_paPlane[i].normal.y,
													m_paPlane[i].normal.z,
													m_paPlane[i].dist );
	}

	fclose(fp);
	return true;
}


// static member variable definition for 'CBSPTreeForAABB'

CBSPTree CBSPTreeForAABB::ms_BSPTree;
