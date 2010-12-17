#ifndef __BASEFACE_H__
#define __BASEFACE_H__


#include <vector>
#include "fwd.hpp"
#include "3DMath/Plane.hpp"
#include "3DMath/aabb3.hpp"
#include "Graphics/Direct3D/FVF_MapVertex.h"
#include "Graphics/Direct3D/Conversions.hpp"


extern float g_fEpsilon;
extern Vector3 g_vEpsilon;


#define NORMAL_EPSILON 0.001
#define DIST_EPSILON 0.001


enum { FCE_ONPLANE, FCE_FRONT, FCE_BACK, FCE_SPLIT };
enum { PRT_FRONT, PRT_BACK };
enum { PVS_NOTVIS, PVS_FRONT, PVS_BACK, PVS_BOTH };


#define NUM_VERTICES_PER_FACE 4
enum { CLIP_NORMAL, CLIP_INVERT };

struct SNode;
class CLineSegment;


//==========================================================================================
// CPlaneBuffer
//==========================================================================================

class CPlaneBuffer : public std::vector<SPlane>
{

public:
	CPlaneBuffer() {}
	short AddPlane(SPlane &rPlane);
	bool AddPlaneFromFace(CFace& rFace);

};


//==========================================================================================
// CFace
//==========================================================================================

class CFace
{
protected:

	std::vector<MAPVERTEX> m_pVertices;

	CPlaneBuffer* m_pPlaneBuffer;
	short m_sPlaneIndex;

	AABB3 m_AABB;

public:

	CFace();
	CFace(const CFace& face);
	~CFace(){}

	// vertex access
	inline Vector3 GetVertex( int i ) const { return ToVector3( m_pVertices[i].vPosition ); }
	inline MAPVERTEX GetMAPVERTEX( int i) const { return m_pVertices[i]; }
	inline void SetMAPVERTEX( int i, MAPVERTEX& v );

	inline void AddVertex(const Vector3& v);
	inline void AddMAPVERTEX(MAPVERTEX& v);

	// the number of vertices of this face
	inline int GetNumVertices() const { return (int)m_pVertices.size(); }

	inline void ClearVertices() { m_pVertices.clear(); m_AABB.Nullify(); }

	void SetLightmapUV( short i, float u, float v );

	const AABB3& GetAABB() const { return m_AABB; }

	// check collision with a ray
	// reeturn true if the ray is intersecting with the face
	bool ClipTrace( CLineSegment& line_segment );

	//operation on plane
	SPlane& GetPlane() { return m_pPlaneBuffer->at( m_sPlaneIndex ); }

	// copy plane data to a node
	void SetPlaneTo(SNode& rNode);

	// copy plane data from a node to this face
	void SetPlaneFrom(SNode& rNode);

	bool MakePlane(SPlane& rPlane);
	bool HasSamePlaneWith(CFace& rFace);
	friend bool CPlaneBuffer::AddPlaneFromFace(CFace& rFace);

	//operation on face
	virtual void Split(CFace& front, CFace& back, SPlane& cutplane);
	void ClipFaceWithPlane(SPlane& rPlane);
	bool ClipVisibility(CFace& rSrcFace, CFace& rDestFace, int iClipStyle);
	void MakeLargeFaceFromPlane();
	void AddToAABB(AABB3& aabb);

	bool SharingPointWith(CFace& rFace);
	float CalculateArea();	//Get the area of this face
	bool IsOnTriangle( int iTriangleIndex, Vector3& rvPosition );

	//operators
	bool operator<(CFace& face) {return ( m_pVertices.size() < face.m_pVertices.size() );}
	bool operator==(CFace& face) {return (memcmp(this, &face, sizeof(CFace)) == 0);}
	CFace operator=(CFace face); //copy the vertices and the plane

};


//============================ inline implementations ============================


inline void CFace::AddVertex(const Vector3 &v)
{
	MAPVERTEX vMap;
	vMap.vPosition = ToD3DXVECTOR3( v );
	AddMAPVERTEX( vMap );

	m_AABB.AddPoint( v );
}


inline void CFace::AddMAPVERTEX(MAPVERTEX& v)
{
	m_pVertices.push_back(v);

	m_AABB.AddPoint( ToVector3( v.vPosition ) );
}


inline void CFace::SetMAPVERTEX( int i, MAPVERTEX& v )
{
	m_pVertices[i] = v;

	m_AABB.AddPoint( ToVector3( v.vPosition ) );
}


#endif  /*  __BASEFACE_H__  */
