#ifndef __INDEXEDTRIANGLE_H__
#define __INDEXEDTRIANGLE_H__

#include "AABB3.hpp"

#include "Support/Serialization/SerializationEx.hpp"
using namespace GameLib1::Serialization;


class CTriangleMesh;
class CBSPTreeForTriangleMesh;


class CIndexedTriangle : public IArchiveObjectBase
{
	friend class CTriangleMesh;
	friend class CBSPTreeForTriangleMesh;

	int m_aiVertexIndex[3];
	int m_iMaterialID;
	Vector3 m_vNormal;
	AABB3 m_aabb;
	bool m_bEdgeCollision[3];	///< indicates whether the i-th edge should be checked for collision
	bool m_bVertexCollision[3];	///< indicates whether the i-th vertex should be checked for collision

	int m_iTestCounter;

public:

	inline CIndexedTriangle();
	~CIndexedTriangle() {}

	inline int GetIndex(int i) { return m_aiVertexIndex[i]; }
	const AABB3& GetAABB() const { return m_aabb; }
	inline Vector3& GetNormal() { return m_vNormal; }

	// returns true if the i-th edge should be checked for collision
	inline bool EdgeCollision( int index ) { return m_bEdgeCollision[index]; }

	// returns true if the i-th vertex should be checked for collision (currently not used)
	inline bool VertexCollision( int index ) { return m_bVertexCollision[index]; }

	int GetTestCounter() const { return m_iTestCounter; }

	void SetTestCounter( int counter ) { m_iTestCounter = counter; }

	inline virtual void Serialize( IArchive& ar, const unsigned int version );

};


//=================================================================================================================
// CIndexedTriangle
//=================================================================================================================

inline CIndexedTriangle::CIndexedTriangle()
{
	m_iTestCounter = -1;

	int i;
	for( i=0; i<3; i++ )
	{
		m_bEdgeCollision[i] = false;
		m_bVertexCollision[i] = false;
	}
}


inline void CIndexedTriangle::Serialize( IArchive& ar, const unsigned int version )
{
	ar & m_iMaterialID;
	ar & m_iTestCounter;
	ar & m_vNormal;
	ar & m_aabb;

	for( int i=0; i<3; i++ )
	{
		ar & m_aiVertexIndex[i];
		ar & m_bEdgeCollision[i];
		ar & m_bVertexCollision[i];
	}
}


#endif /* __INDEXEDTRIANGLE_H__ */
