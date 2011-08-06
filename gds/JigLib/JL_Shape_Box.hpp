#ifndef  __JL_SHAPE_BOX_H__
#define  __JL_SHAPE_BOX_H__

#include "JL_ShapeBase.hpp"


class CTriangle;

// stores a pair of indices to edge points
struct SEdgeIndex
{
	int i[2];
};


class CJL_Shape_Box : public CJL_ShapeBase
{
	/// full length along each axis
	Vector3 m_vSideLength;

	Vector3 m_avEdgePoint[8];

	Vector3 m_avWorldEdgePoint[8];

	static const SEdgeIndex ms_aEdgeIndex[12];

public:
	CJL_Shape_Box();
	~CJL_Shape_Box();

	void InitSpecific( CJL_ShapeDesc& rShapeDesc );

	const Vector3& GetSideLength() const { return m_vSideLength; }

	/// calculate the world coordinats of aabb and edge points
	/// must be called every frame before the collision detection
	void UpdateWorldProperties();

//	inline Vector3& GetWorldPosition() { return m_WorldPose.vPosition; }
//	inline void GetCombinedOrient( Matrix33& rmatOrient ) { rmatOrient = m_WorldPose.matOrient; }
//	inline Matrix33& GetCombinedOrient() { return m_WorldPose.matOrient; }

	void SetSize( Scalar fWidth_X, Scalar fHeight_Y, Scalar fDepth_Z );	// full length along each axis
	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CTriangle& rTriangle, float fCollTolerance );	// returns true if intersecting on the given axis
	bool SepAxisTest( Scalar& depth, Vector3& rvAxis, CJL_Shape_Box& rBox, float fCollTolerance );	// returns true if intersecting on the given axis

	// axis must be in world space
	inline void GetBoxSpan( Scalar& rfMin, Scalar& rfMax, const Vector3& rvAxis )
	{
		Scalar fCenter, fRadius;
		fCenter = Vec3Dot( rvAxis, m_WorldPose.vPosition );
		fRadius = m_vSideLength.x * 0.5f * fabsf( Vec3Dot(rvAxis, m_WorldPose.matOrient.GetColumn(0)) )
			    + m_vSideLength.y * 0.5f * fabsf( Vec3Dot(rvAxis, m_WorldPose.matOrient.GetColumn(1)) )
				+ m_vSideLength.z * 0.5f * fabsf( Vec3Dot(rvAxis, m_WorldPose.matOrient.GetColumn(2)) );

		rfMin = fCenter - fRadius;
		rfMax = fCenter + fRadius;
	}

	inline void GetWorldEdge(int i, Vector3& v0, Vector3& v1 )
	{
		v0 = m_avWorldEdgePoint[ ms_aEdgeIndex[i].i[0] ];
		v1 = m_avWorldEdgePoint[ ms_aEdgeIndex[i].i[1] ];
	}

};



#endif		/*  __JL_SHAPE_BOX_H__  */
