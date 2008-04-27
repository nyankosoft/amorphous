
#include "BSPTreeForBox.h"


CBSPTreeForBox::CBSPTreeForBox()
{
}

CBSPTreeForBox::~CBSPTreeForBox()
{
}


#define MAX_DIVIDE_NUM	6


static const CBSPTreeForBox::SBinaryNode g_aNode[6] = { -2,1, -2,2, -2,3, -2,4, -2,5, -2,-1 };


bool CBSPTreeForBox::ClipTrace( Vector3& vEndPos,
							    float& fEndFrac,
								const Vector3& vStart,
								const Vector3& vGoal,
								const float fEpsilon )
{
	short nodenum, prev_nodenum;
//	SNode_f* paNode = m_paNode;
//	SPlane* paPlane = m_paPlane;

	Vector3 vP1, vP2;
	float fd1, fd2;
	float f, fStart, fGoal;

	static int nodenums[MAX_DIVIDE_NUM];
	static float fGoals[MAX_DIVIDE_NUM];
	static Vector3 vGoals[MAX_DIVIDE_NUM];
	static char rearside[MAX_DIVIDE_NUM];
	int num_backtracks = 0;

	int axis = 0;
	int side = 1;

	nodenum = 0; prev_nodenum = 0;

	const Vector3 &vP0 = vStart;

	fStart = 0;
	vP1 = vStart;
	fGoal = 1;
/*	if(tr.fFraction == 1)	//hasn't hit anything yet
		vP2 = *tr.pvGoal;
	else	//already hit something
		vP2 = tr.vEnd;
*/
	vP2 = vGoal;

	while(1)
	{
		while(0 <= nodenum)
		{
			axis = nodenum % 3;
			side = ( nodenum / 3 == 0 ) ? 1 : -1;

			fd1 = vP1[axis] * side - m_aDist[axis];
			fd2 = vP2[axis] * side - m_aDist[axis];

			if(0 <= fd1 && 0 <= fd2)
			{
				// no intersection between the box and the line segment
				prev_nodenum = nodenum;
				nodenum = SBinaryNode::EMPTY;
//				nodenum = g_aNode[nodenum].child[SBinaryNode::CHILD_FRONT];
				continue;
			}
			if(fd1 < 0 && fd2 < 0)
			{
				prev_nodenum = nodenum;
				nodenum = g_aNode[nodenum].child[SBinaryNode::CHILD_BACK];
				continue;
			}

			// need to split the line segment.

			if(fd1 < 0)
				f = (fd1 + fEpsilon) / (fd1 - fd2);
			else
				f = (fd1 - fEpsilon) / (fd1 - fd2);

			if(f < 0)
				f = 0;	//This occurs when fd1 < EPSILON
			else if(1 < f)
				f = 1;

			// store the rear-line data in stack
			fGoals[num_backtracks] = fGoal;
			vGoals[num_backtracks] = vP2;
			nodenums[num_backtracks] = nodenum;

			// prepare for the next fore-line check
			fGoal = fStart + f * (fGoal - fStart);
			vP2 = vP1 + f * (vP2 - vP1);

			prev_nodenum = nodenum;

			if( 0 <= fd1 )
			{
				nodenum = g_aNode[nodenum].child[SBinaryNode::CHILD_FRONT];
				rearside[num_backtracks] = 1;	// the rear part of the line segment is behind the plane
			}
			else
			{
				nodenum = g_aNode[nodenum].child[SBinaryNode::CHILD_BACK];
				rearside[num_backtracks] = 0;	// the rear part of the line segment is in front of the plane
			}

			num_backtracks++;

		}

		if( nodenum == SBinaryNode::EMPTY )
		{
			if(0 < num_backtracks) //if a subsequent line exists,
			{
				// prepare for checking the next rear line segment.
				num_backtracks--;
				fStart = fGoal;
				fGoal  = fGoals[num_backtracks];
				vP1 = vP2;
				vP2 = vGoals[num_backtracks];

				prev_nodenum = nodenums[num_backtracks];
				if(rearside[num_backtracks] == 1)
					nodenum	= g_aNode[ prev_nodenum ].child[SBinaryNode::CHILD_BACK];
				else
					nodenum = g_aNode[ prev_nodenum ].child[SBinaryNode::CHILD_FRONT];

///				pTouchPlane = paPlane + paNode[prev_nodenum].sPlaneIndex;

				continue;
			}
			else
			{
				vEndPos = vP2;
//				tr.in_solid = false;
				return false;		// no intersection detected
			}
		}

		if( nodenum == SBinaryNode::SOLID )
		{
			vEndPos = vP1;
			fEndFrac = fStart;
/*			while( CheckPosition( tr ) == CONTENTS_SOLID )
			{
				fStart -= 0.1f;
				if( fStart < 0 )
				{	// returns the original position (this part should be improved)
//					if( fStart != fStart )
//						int error = 1;
					// MessageBox(NULL, "Cannot get out of the solid area.","Error", MB_OK|MB_ICONWARNING);
					tr.fFraction = 0.0f;
///					tr.vEnd = vP1;
					tr.vEnd = *tr.pvStart;
					tr.in_solid = true;
					return CONTENTS_SOLID;
				}
				tr.vEnd = vP0 + fStart * (tr.vEnd - vP0);
			}*/
//			tr.in_solid = false;

			// record the plane 'tr' hit
//			tr.iNumTouches++;
//			tr.plane = *pTouchPlane;

			return true;
		}
	}

	return false;
}
