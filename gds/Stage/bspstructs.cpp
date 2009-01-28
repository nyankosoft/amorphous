#include "bspstructs.h"
#include "../Graphics/FloatRGBColor.h"

float g_fEpsilon = 0.001f;
D3DXVECTOR3 g_vEpsilon(0.001f, 0.001f, 0.001f);


//=====================================================================================
// CPortalFace::Methods()                           - class CPortalFace : public CFace
//=====================================================================================

CPortalFace::CPortalFace(const CPortalFace& face) : CFace(face)
{
	this->m_asCell[0] = face.m_asCell[0];
	this->m_asCell[1] = face.m_asCell[1];
	this->m_asNode[0] = face.m_asNode[0];
	this->m_asNode[1] = face.m_asNode[1];
}

CPortalFace CPortalFace::operator=(CPortalFace face)
{
	this->CFace::operator=(face);

	this->m_asCell[0] = face.m_asCell[0];
	this->m_asCell[1] = face.m_asCell[1];
	this->m_asNode[0] = face.m_asNode[0];
	this->m_asNode[1] = face.m_asNode[1];

	return *this;
}



//-------------------------------------------------------------------------------------/
// Global Functions                                                                    /
//-------------------------------------------------------------------------------------/

int ClassifyPoint(SPlane& plane, D3DXVECTOR3& point)
{
	float pdist = D3DXVec3Dot( &point, &plane.normal ) - plane.dist;
	if( pdist > DIST_EPSILON )   //if point is in front of the plane
		return PNT_FRONT;
	if( pdist < -DIST_EPSILON )  //if point is behind the plane
		return PNT_BACK;
	return PNT_ONPLANE;          //point is on the plane
}


bool AlmostSamePlanes(SPlane& plane1, SPlane& plane2)
{
	if(	fabs(plane1.dist - plane2.dist) < DIST_EPSILON
		&& fabs( plane1.normal.x - plane2.normal.x ) < NORMAL_EPSILON 
		&& fabs( plane1.normal.y - plane2.normal.y ) < NORMAL_EPSILON 
		&& fabs( plane1.normal.z - plane2.normal.z ) < NORMAL_EPSILON )
		return true;
	else
		return false;
}

// OPC_DONTCARE_NORMAL_DIRECTION
//	the face is regarded as on-plane if it simply overlaps on the plane 
// OPC_IF_NORMAL_SAME_DIRECTION (default)
//	the face is regarded as on-plane if its normal is in the same direction as the plane
int	ClassifyFace(SPlane& plane, CFace& face, int iOnPlaneCondition)
{
	int front = 0;
	int back = 0;
	int iNumPnts = face.GetNumVertices();

	for(int i=0; i<iNumPnts ;i++)
	{
		switch( ClassifyPoint( plane, face.GetVertex(i) ) )
		{
		case PNT_FRONT:		front++;	break;
		case PNT_BACK:		back++;		break;
		case PNT_ONPLANE:	//平面上に乗っている頂点は分類に関与しないこととする。
		default:;
		}
	}

	if( front == 0 && back == 0 )
	{
		SPlane &plane2 = face.GetPlane();

		if( iOnPlaneCondition == OPC_DONTCARE_NORMAL_DIRECTION )
			return FCE_ONPLANE;		//the face is on-plane regardless of its normal direction

		else if( fabs( plane.normal.x - plane2.normal.x ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.y - plane2.normal.y ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.z - plane2.normal.z ) < NORMAL_EPSILON )
		{
			return FCE_ONPLANE;	//the 2 normals face the same direction
		}
		else
		{	//確かに平面上だが、法線が逆向きの面は裏に行く
			return FCE_BACK;	//the 2 normals face the opposite directions
		}
	}
	if( front != 0 && back != 0 )
		return FCE_SPLIT;
	if( front > 0 )
		return FCE_FRONT;
	return FCE_BACK;
}
