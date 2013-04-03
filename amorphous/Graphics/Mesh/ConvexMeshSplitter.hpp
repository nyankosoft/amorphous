#ifndef __ConvexMeshSplitter_HPP__
#define __ConvexMeshSplitter_HPP__


#include "../../base.hpp"
#include "../../3DMath/Plane.hpp"
#include "../fwd.hpp"
#include <vector>
#include <map>


namespace amorphous
{


Result::Name SplitMeshByPlane( const CustomMesh& src, const Plane& split_plane, CustomMesh& dest_front, CustomMesh& dest_back );


class CMeshSplitResults
{
public:

	boost::shared_ptr<CustomMesh> m_pFrontMesh;

	boost::shared_ptr<CustomMesh> m_pBackMesh;
};


class CEdgeSplitInfo
{
public:
	Vector3 pos;
	U16 front_vertex_index;
	U16 back_vertex_index;

	CEdgeSplitInfo()
		:
	pos(Vector3(0,0,0)),
	front_vertex_index(0),
	back_vertex_index(0)
	{}
};


class CConvexMeshSplitter
{
	std::map< std::pair<U16,U16>, CEdgeSplitInfo > m_EdgeToEdgeSplitInfo;

	std::vector<Vector3> m_SplitSurfacePoints;

	CMeshSplitResults m_MeshSplitResults;

	Result::Name SplitMeshByPlane( const CustomMesh& src, const Plane& split_plane, CustomMesh& dest_front, CustomMesh& dest_back );

	void SplitTriangle(
		const CustomMesh& src,
		const std::vector<char>& vertex_sides,
		const std::vector< std::pair<uint,uint> >& new_vertex_indices,
		int triangle_index,
		const Plane& split_plane,
		CustomMesh& dest_front,
		CustomMesh& dest_back,
		std::vector<U16>& front_polygon_indices,
		std::vector<U16>& back_polygon_indices,
		std::vector<U16>& split_surface_vertex_indices_front,
		std::vector<U16>& split_surface_vertex_indices_back
		);

	Result::Name CreateTrianglesOnSplitSurface(
		CustomMesh& dest_front,
		CustomMesh& dest_back,
		std::vector<U16>& front_mesh_indices,
		std::vector<U16>& back_mesh_indices
		);

public:

	CConvexMeshSplitter(){}

	~CConvexMeshSplitter(){}

//	Result::Name SplitMesh( const CustomMesh& src, const Plane& split_plane, CustomMesh& dest_front, CustomMesh& dest_back );

	Result::Name SplitMesh( const CustomMesh& src, const Plane& split_plane );

	const CMeshSplitResults& GetSplitResults() const { return m_MeshSplitResults; }

	void GetSplitResults( CMeshSplitResults& dest ) const { dest = m_MeshSplitResults; }
};


} // namespace amorphous



#endif /* __ConvexMeshSplitter_HPP__ */
