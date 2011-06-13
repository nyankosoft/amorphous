#ifndef  __PhysTriangleMeshDesc_H__
#define  __PhysTriangleMeshDesc_H__


#include <vector>
#include "../3DMath/Vector3.hpp"
#include "../Support/Serialization/Serialization.hpp"
#include "../Support/Serialization/Serialization_3DMath.hpp"
using namespace GameLib1::Serialization;

#include "fwd.hpp"


namespace physics
{


class CTriangleMeshDesc : public IArchiveObjectBase
{
public:

	/// vertices for triangles
	std::vector<Vector3> m_vecVertex;

	/// index for vertices
	/// every 3 indices represent vertices that makes a triangle
	std::vector<int> m_vecIndex;

	/// index to material for each triangle
	std::vector<int> m_vecMaterialIndex;

public:

	CTriangleMeshDesc()	{}

	virtual ~CTriangleMeshDesc() {}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_vecVertex & m_vecIndex & m_vecMaterialIndex;
	}

	bool IsValid() const
	{
		if( 3 <= m_vecVertex.size()
		 && 2 <= m_vecIndex.size() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

//	unsigned int GetArchiveObjectID() const { return ; }

/*	void Scale( float factor )
	{
		size_t i, num_vertices = vecVertex.size();
		for( i=0; i<num_vertices; i++ )
		{
			vecVertex[i] * factor;
		}
	}*/
};


} // namespace physics


#endif  /*  __PhysTriangleMeshDesc_H__  */
