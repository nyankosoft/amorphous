#ifndef __General3DMesh_H__
#define __General3DMesh_H__

/*
valid combinations of geometry filters
- layer filter (Include) + surface filter (exclude)
- (add other valid combinations)
*/


#include <vector>
#include <list>
#include "../fwd.h"
#include "../General3DVertex.h"
#include "../IndexedPolygon.h"
#include "3DMeshModelArchive.h"
using namespace MeshModel;


//class C3DModelLoadDesc
class CGeometryFilter
{
public:

	class CTarget
	{
	public:
		std::vector<std::string> Layers;
		std::vector<std::string> Surfaces;
		std::vector<std::string> PolygonGroups;
	};


	/// exclusive filter (strict filter)
	/// - compile only the geometry specified as Include targets
	CTarget Include;

	/// inclusive filter (loose filter)
	/// - take in any geometry not specified here
	CTarget Exclude;
};


class CGeneral3DMesh
{
	unsigned int m_MeshFlag;

	unsigned int m_VertexFormatFlag;

	std::vector<CGeneral3DVertex> m_vecVertexBuffer;

	/// Polygons that hold indices to vertices
	/// - Can be retrieved later
	/// - The polygons can include non-triangle polygons (with more than 4 vertices)
	/// - They are triangulated and sotred in mesh model archive by C3DMeshModelBuilder
	std::vector<CIndexedPolygon> m_vecPolygon;

	/// stores surface property
	std::vector<CMMA_Material> m_vecMaterial;

	/// skeleton hierarchy
	CMMA_Bone m_SkeletonRootBone;

public:

	inline CGeneral3DMesh();

	/// Converts the general 3d mesh into the triangulated mesh archive
	/// \param dest [out] mesh archive
	void Create3DMeshModelArchive( C3DMeshModelArchive& dest );

	std::vector<CGeneral3DVertex>& GetVertexBuffer() { return m_vecVertexBuffer; }

	std::vector<CIndexedPolygon>& GetPolygonBuffer() { return m_vecPolygon; }

	std::vector<CMMA_Material>& GetMaterialBuffer() { return m_vecMaterial; }

	int GetNumMaterials() const { return (int)m_vecMaterial.size(); }

	CMMA_Bone& GetSkeletonRootBoneBuffer() { return m_SkeletonRootBone; }

	unsigned int GetVertexFormatFlags() const { return m_VertexFormatFlag; }

	void SetVertexFormatFlags( unsigned int flags ) { m_VertexFormatFlag = flags; }

	void UpdatePolygonAABBs() { UpdateAABBs( m_vecPolygon ); }

	void Append( CGeneral3DMesh& mesh );

	friend class C3DModelLoader;
};


// ============================ inline implementations ============================

inline CGeneral3DMesh::CGeneral3DMesh()
:
m_MeshFlag(0),
m_VertexFormatFlag(0)
{
}


#endif /* __General3DMesh_H__ */
