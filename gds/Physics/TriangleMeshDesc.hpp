#ifndef  __PhysTriangleMeshDesc_H__
#define  __PhysTriangleMeshDesc_H__


#include <vector>
#include "3DMath/Vector3.hpp"

#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/Serialization_3DMath.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
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
