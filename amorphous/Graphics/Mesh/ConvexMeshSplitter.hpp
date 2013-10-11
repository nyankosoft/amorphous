#ifndef __ConvexMeshSplitter_HPP__
#define __ConvexMeshSplitter_HPP__


#include "../../base.hpp"
#include "../../3DMath/Plane.hpp"
#include "../../3DMath/Matrix34.hpp"
#include "../fwd.hpp"
#include <vector>
#include <map>


namespace amorphous
{


Result::Name SplitMeshByPlane( const CustomMesh& src, const Plane& split_plane, CustomMesh& dest_front, CustomMesh& dest_back );


class MeshSplitResults
{
public:

	boost::shared_ptr<CustomMesh> m_pFrontMesh;

	boost::shared_ptr<CustomMesh> m_pBackMesh;
};


class EdgeSplitInfo
{
public:
	Vector3 pos;
	U16 front_vertex_index;
	U16 back_vertex_index;

	EdgeSplitInfo()
		:
	pos(Vector3(0,0,0)),
	front_vertex_index(0),
	back_vertex_index(0)
	{}
};


class ConvexMeshSplitter
{
	std::map< std::pair<U16,U16>, EdgeSplitInfo > m_EdgeToEdgeSplitInfo;

	std::vector<Vector3> m_SplitSurfacePoints;

	MeshSplitResults m_MeshSplitResults;

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

	ConvexMeshSplitter(){}

	~ConvexMeshSplitter(){}

//	Result::Name SplitMesh( const CustomMesh& src, const Plane& split_plane, CustomMesh& dest_front, CustomMesh& dest_back );

	Result::Name SplitMesh( const CustomMesh& src, const Matrix34& src_mesh_pose, const Plane& split_plane );

	const MeshSplitResults& GetSplitResults() const { return m_MeshSplitResults; }

	void GetSplitResults( MeshSplitResults& dest ) const { dest = m_MeshSplitResults; }
};


} // namespace amorphous



#endif /* __ConvexMeshSplitter_HPP__ */
