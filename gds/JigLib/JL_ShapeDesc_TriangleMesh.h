
#ifndef  __JL_SHAPEDESC_TRIANGLEMESH_H__
#define  __JL_SHAPEDESC_TRIANGLEMESH_H__


#include "JL_ShapeDesc.h"

#include <vector>


// triangle mesh actor is created by providing vertices and indices for triangles
// to trimesh desc

// XXX 1. pointer to source CJL_TriangleMesh object. During the creation, memory for object is copied,
// XXX so the user is responsible for releasing the source object

class CTriangleMesh;



#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


class CJL_ShapeDesc_TriangleMesh : public CJL_ShapeDesc
{
public:

	/// vertices for triangles
	std::vector<Vector3> *pvecvVertex;

	/// indices to vertices
	std::vector<int> *pveciIndex;

	/// material indices for each triangle
	std::vector<short> *pvecsMaterialIndex;

	int NumMaxTrianglesPerCell;

	float MinimumCellVolume;

	/// pre-process triangle meshs to speed up collision detection during runtime
	/// see GPG4 NovodeX article for details
	bool OptimizeEdgeCollision;

	string RecursionStopCond;	///< "&&"(and) or "||"(or)

	/// filename of an archive
	/// MUST be an archvie of CBSPTreeForTriangleMesh
	string BSPTreeTriangleMeshArchiveFilename;

	inline CJL_ShapeDesc_TriangleMesh()
	{
		sShape = JL_SHAPE_TRIANGLEMESH;

		pveciIndex = NULL;
		pvecvVertex = NULL;
		pvecsMaterialIndex = NULL;

		NumMaxTrianglesPerCell = 8;
		MinimumCellVolume = 8.0f;

		RecursionStopCond = "&&";

		OptimizeEdgeCollision = true;
	}

	~CJL_ShapeDesc_TriangleMesh() {}

	unsigned int GetArchiveObjectID() const { return JL_SHAPEDESC_TRIANGLEMESH; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		CJL_ShapeDesc::Serialize( ar, version );

		//////////////////// NOT IMPLEMENTED ////////////////////

//		ar & *pvecvVertex;
//		ar & *pveciIndex;
//		ar & *pvecsMaterialIndex;
	}
};


#endif  /*  __JL_SHAPEDESC_TRIANGLEMESH_H__  */