#ifndef __General3DMesh_H__
#define __General3DMesh_H__

/*
valid combinations of geometry filters
- layer filter (Include) + surface filter (exclude)
- (add other valid combinations)
*/


#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

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

	boost::shared_ptr< std::vector<CGeneral3DVertex> > m_pVertexBuffer;

//	std::vector<CGeneral3DVertex> m_vecVertexBuffer;

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

	boost::shared_ptr< std::vector<CGeneral3DVertex> > GetVertexBuffer() { return m_pVertexBuffer; }

	const boost::shared_ptr< std::vector<CGeneral3DVertex> > GetVertexBuffer() const { return m_pVertexBuffer; }


	std::vector<CIndexedPolygon>& GetPolygonBuffer() { return m_vecPolygon; }

	const std::vector<CIndexedPolygon>& GetPolygonBuffer() const { return m_vecPolygon; }

	void ClearPolygonBuffer() { m_vecPolygon.clear(); }


	std::vector<CMMA_Material>& GetMaterialBuffer() { return m_vecMaterial; }

	int GetNumMaterials() const { return (int)m_vecMaterial.size(); }

	void ClearMaterials() { m_vecMaterial.clear(); }

	inline int GetMaterialIndexFromName( const std::string& material_name );


	CMMA_Bone& GetSkeletonRootBoneBuffer() { return m_SkeletonRootBone; }

	unsigned int GetVertexFormatFlags() const { return m_VertexFormatFlag; }

	void SetVertexFormatFlags( unsigned int flags ) { m_VertexFormatFlag = flags; }

	inline void UpdatePolygonBuffer()
	{
		size_t i, num_pols = m_vecPolygon.size();
		for( i=0; i<num_pols; i++ )
		{
			m_vecPolygon[i].UpdateAABB();
			m_vecPolygon[i].UpdatePlane();
		}
	}

	void Append( CGeneral3DMesh& mesh );

	friend class C3DModelLoader;
};


// ============================ inline implementations ============================

inline CGeneral3DMesh::CGeneral3DMesh()
:
m_MeshFlag(0),
m_VertexFormatFlag(0)
{
	m_pVertexBuffer
		= boost::shared_ptr<std::vector<CGeneral3DVertex>>( new std::vector<CGeneral3DVertex>() );
}


inline int CGeneral3DMesh::GetMaterialIndexFromName( const std::string& material_name )
{
	for( size_t i=0; i<m_vecMaterial.size(); i++ )
	{
		if( material_name == m_vecMaterial[i].Name )
			return (int)i;
	}

	return -1;
}


inline boost::shared_ptr<CGeneral3DMesh> CreateGeneral3DMesh()
{
	boost::shared_ptr<CGeneral3DMesh> pMesh
		= boost::shared_ptr<CGeneral3DMesh>( new CGeneral3DMesh() );

	return pMesh;
}


#endif /* __General3DMesh_H__ */
