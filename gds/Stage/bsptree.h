//=========================================================
//    bsptree.h
//
//=========================================================

#ifndef __BSPTREE_H__
#define __BSPTREE_H__

#include <stdlib.h>

#include "3DMath/Plane.h"
#include "BinaryNode.h"
#include "trace.h"


class CBSPTree
{
	SNode_f* m_paNode;
	SPlane* m_paPlane;

	int m_NumNodes;
	int m_NumPlanes;

public:

	CBSPTree() : m_paNode(NULL), m_paPlane(NULL), m_NumNodes(0), m_NumPlanes(0) {}
	CBSPTree( const SNode_f *pNodes, int num_nodes, const SPlane* pPlanes, int num_planes );
	~CBSPTree();

	void Release();

	void Init( const SNode_f *pNodes, int num_nodes, const SPlane* pPlanes, int num_planes );

	short CheckPosition( STrace& tr, short sStartNodeIndex = 0 );
	short CheckPosition( Vector3& rvPoint, short sStartNodeIndex = 0 );

	short ClipTrace( STrace& tr );

	bool ClipTrace_r( short sNodeIndex, float p1f, float p2f,
						   Vector3& vP1, Vector3& vP2, STrace &rTrace);

	SPlane GetPlane( const SNode_f& rNode ) { return m_paPlane[rNode.sPlaneIndex]; }

	SNode_f& GetNode( int index ) { return m_paNode[index]; }

	bool LoadFromFile( const char *pcFilename );

	bool WriteToFile( const char *pcFilename );

	CBSPTree& operator=( const CBSPTree& src );
	
	void CreateBoxBSPTree( float fWidth_X, float fHeight_Y, float fDepth_Z );

	friend class CBSPTreeForAABB;
};


class CBSPTreeForAABB
{
	static CBSPTree ms_BSPTree;

public:

	//This static function must be called before using 'ClipTraceToAABB()'
	static void Initialize()
	{
		SNode_f aNodes[6];
		SPlane aPlanes[6];

		for(int i=0; i<6; i++)
		{
			aNodes[i].sPlaneIndex = i;
			aNodes[i].sFrontChild = CONTENTS_EMPTY;
			aNodes[i].sBackChild = i+1;	
			aNodes[i].sCellIndex = 0;

		}
		aNodes[5].sBackChild = CONTENTS_SOLID;

		aPlanes[0].normal = Vector3(1, 0, 0); aPlanes[0].type = 0;
		aPlanes[1].normal = Vector3(-1, 0, 0); aPlanes[1].type = 0;

		aPlanes[4].normal = Vector3(0, 0, 1); aPlanes[4].type = 2;
		aPlanes[5].normal = Vector3(0, 0, -1); aPlanes[5].type = 2;

		aPlanes[2].normal = Vector3(0, 1, 0); aPlanes[2].type = 1;
		aPlanes[3].normal = Vector3(0, -1, 0); aPlanes[3].type = 1;

		ms_BSPTree.Init( aNodes, 6, aPlanes, 6 );
	}

	inline void SetAABB( Vector3& vHalfLength )
	{
		ms_BSPTree.m_paPlane[0].dist = vHalfLength.x;
		ms_BSPTree.m_paPlane[1].dist = vHalfLength.x;
		ms_BSPTree.m_paPlane[2].dist = vHalfLength.y;
		ms_BSPTree.m_paPlane[3].dist = vHalfLength.y;
		ms_BSPTree.m_paPlane[4].dist = vHalfLength.z;
		ms_BSPTree.m_paPlane[5].dist = vHalfLength.z;
	}

	// clip trace to the current aabb
	// bounding volume must be dot(BVTYPE_DOT) i.e. line segment
	inline void ClipLineTrace( STrace& rTrace )
	{
		ms_BSPTree.ClipTrace( rTrace );
	}

	inline void ClipTraceToAABB( STrace& rTrace, AABB3& raabb )
	{
		ms_BSPTree.m_paPlane[0].dist =  raabb.vMax.x;
		ms_BSPTree.m_paPlane[1].dist = -raabb.vMin.x;
		ms_BSPTree.m_paPlane[4].dist =  raabb.vMax.z;
		ms_BSPTree.m_paPlane[5].dist = -raabb.vMin.z;
		ms_BSPTree.m_paPlane[2].dist =  raabb.vMax.y;
		ms_BSPTree.m_paPlane[3].dist = -raabb.vMin.y;

		char cOriginalBVType = rTrace.bvType;
		if(cOriginalBVType == BVTYPE_AABB)
		{
			rTrace.bvType = BVTYPE_DOT;
			ms_BSPTree.m_paPlane[0].dist -= rTrace.aabb.vMin.x;
			ms_BSPTree.m_paPlane[1].dist += rTrace.aabb.vMax.x;
			ms_BSPTree.m_paPlane[4].dist -= rTrace.aabb.vMin.z;
			ms_BSPTree.m_paPlane[5].dist += rTrace.aabb.vMax.z;
			ms_BSPTree.m_paPlane[2].dist -= rTrace.aabb.vMin.y;
			ms_BSPTree.m_paPlane[3].dist += rTrace.aabb.vMax.y;
		}

		ms_BSPTree.ClipTrace( rTrace );
		rTrace.bvType = cOriginalBVType;
	}
};


#endif	/*  __BSPTREE_H__  */
