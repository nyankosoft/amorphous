#ifndef  __JL_BOX__
#define  __JL_BOX__


#include "3DMath/Vector3.h"
#include "3DMath/Matrix34.h"


class CJL_Box
{

public:

	CJL_Box();

	CJL_Box( const  Matrix34& pose, const Vector3& vSideLength )
		: m_Pose(pose), m_vSideLength(vSideLength)
	{
	}

	enum eBoxPointIndex { BRD, BRU, BLD, BLU, FRD, FRU, FLD, FLU };

	inline void GetCornerPoints( Vector3 pvDestEdgePt[8] ) const;

	struct Edge {eBoxPointIndex index0, index1;};
	// returns const list of 12 edges in the following order
	// BRD - BRU
	// BRD - BLD
	// ...

	inline const Edge *GetAllEdges() const { return m_aEdge; }

private:

	Matrix34 m_Pose;

	/// holds full edge lengths
	Vector3 m_vSideLength;

	static const Edge m_aEdge[12];
};


const CJL_Box::Edge CJL_Box::m_aEdge[12] =
{
	{BRD, BRU},
	{BRD, BLD},
	{BRD, FRD},
	{BLD, BLU},
	{BLD, FLD},
	{FRD, FRU},
	{FRD, FLD},
	{BRU, BLU},
	{BRU, FRU},
	{BLU, FLU},
	{FRU, FLU},
	{FLD, FLU},
};


//-------------------------- inline implementations --------------------------

inline void CJL_Box::GetCornerPoints( Vector3 pvDestEdgePt[8] ) const
{
/*	const Vector3& vPos = m_Pose.vPosition;
	const Vector3& vLook  = m_Pose.matOrient.GetColumn(0);
	const Vector3& vLeft  = m_Pose.matOrient.GetColumn(1);
	const Vector3& vUp    = m_Pose.matOrient.GetColumn(2);
	pvDestEdgePt[BRD] = vPos;
	pvDestEdgePt[FRD] = vPos + m_vSideLength.x * vLook;
	pvDestEdgePt[BLD] = vPos + m_vSideLength.y * vLeft;
	pvDestEdgePt[BRU] = vPos + m_vSideLength.z * vUp;
	pvDestEdgePt[FLD] = pvDestEdgePt[BLD] + m_vSideLength.x * vLook;
	pvDestEdgePt[BLU] = pvDestEdgePt[BRU] + m_vSideLength.y * vLeft;
	pvDestEdgePt[FRU] = pvDestEdgePt[FRD] + m_vSideLength.z * vUp;
	pvDestEdgePt[FLU] = pvDestEdgePt[FLD] + m_vSideLength.z * vUp;
*/	

	const Vector3& vPos = m_Pose.vPosition;
	const Vector3& vRight  = m_Pose.matOrient.GetColumn(0);
	const Vector3& vUp     = m_Pose.matOrient.GetColumn(1);
	const Vector3& vFwd    = m_Pose.matOrient.GetColumn(2);

	const Vector3& side = m_vSideLength * 0.5f;
	pvDestEdgePt[BRD] = vPos + side.x * vRight - side.y * vUp - side.z * vFwd;
	pvDestEdgePt[FRD] = vPos + side.x * vRight - side.y * vUp + side.z * vFwd;
	pvDestEdgePt[BLD] = vPos - side.x * vRight - side.y * vUp - side.z * vFwd;
	pvDestEdgePt[BRU] = vPos + side.x * vRight + side.y * vUp - side.z * vFwd;
	pvDestEdgePt[FLD] = vPos - side.x * vRight - side.y * vUp + side.z * vFwd;
	pvDestEdgePt[BLU] = vPos - side.x * vRight + side.y * vUp - side.z * vFwd;
	pvDestEdgePt[FRU] = vPos + side.x * vRight + side.y * vUp + side.z * vFwd;
	pvDestEdgePt[FLU] = vPos - side.x * vRight + side.y * vUp + side.z * vFwd;

}


#endif  /*  __JL_BOX__  */