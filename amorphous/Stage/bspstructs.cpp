#include "bspstructs.hpp"


namespace amorphous
{

float g_fEpsilon = 0.001f;
Vector3 g_vEpsilon(0.001f, 0.001f, 0.001f);


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

// OPC_DONTCARE_NORMAL_DIRECTION
//	the face is regarded as on-plane if it simply overlaps on the plane 
// OPC_IF_NORMAL_SAME_DIRECTION (default)
//	the face is regarded as on-plane if its normal is in the same direction as the plane
int	ClassifyFace( const SPlane& plane, CFace& face, int iOnPlaneCondition )
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
		case PNT_ONPLANE:	// Points on the plane do not affect the classification
		default:;
		}
	}

	if( front == 0 && back == 0 )
	{
		// All the points of the face are on the plane.
		const SPlane &plane2 = face.GetPlane();

		if( iOnPlaneCondition == OPC_DONTCARE_NORMAL_DIRECTION )
			return FCE_ONPLANE;		//the face is on-plane regardless of its normal direction

		else if( fabs( plane.normal.x - plane2.normal.x ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.y - plane2.normal.y ) < NORMAL_EPSILON 
		&&  fabs( plane.normal.z - plane2.normal.z ) < NORMAL_EPSILON )
		{
			// the normals of the face and the plane face the same direction
			return FCE_ONPLANE;
		}
		else
		{	// Consider this face is behind the plane although all the points are on the plane.
			// rationale: the normals of the face and the plane face the opposite directions
			return FCE_BACK;
		}
	}
	if( front != 0 && back != 0 )
		return FCE_SPLIT;
	if( front > 0 )
		return FCE_FRONT;
	return FCE_BACK;
}


} // namespace amorphous
