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

#include "../fwd.hpp"
#include "../General3DVertex.hpp"
#include "../IndexedPolygon.hpp"
#include "3DMeshModelArchive.hpp"


namespace amorphous
{


//class C3DModelLoadDesc
class GeometryFilter
{
public:

	class Target
	{
	public:
		std::vector<std::string> Layers;
		std::vector<std::string> Surfaces;
		std::vector<std::string> PolygonGroups;
	};


	/// exclusive filter (strict filter)
	/// - compile only the geometry specified as Include targets
	Target Include;

	/// inclusive filter (loose filter)
	/// - take in any geometry not specified here
	Target Exclude;

	bool IncludeName(
		const std::string& target_name,
		const std::vector<std::string>& to_include,
		const std::vector<std::string>& to_exclude
		) const
	{
		using namespace std;

		if( !to_include.empty()
		 && find( to_include.begin(), to_include.end(), target_name ) == to_include.end() )
		{
			// include filter is present and the target name was not found in the list
			return false;
		}

		if( !to_exclude.empty()
		 && find( to_exclude.begin(), to_exclude.end(), target_name ) != to_exclude.end() )
		{
			// exclude filter is present and the target name was found in the list
			return false;
		}

		return true;
	}

	bool IncludeLayer( const std::string& layer_name )                 const { return IncludeName( layer_name,         Include.Layers,        Exclude.Layers ); }
	bool IncludeSurface( const std::string& surface_name )             const { return IncludeName( surface_name,       Include.Surfaces,      Exclude.Surfaces ); }
	bool IncludePolygonGroups( const std::string& polygon_group_name ) const { return IncludeName( polygon_group_name, Include.PolygonGroups, Exclude.PolygonGroups ); }
};


/**
Represents a mesh that comprises vertices and indexed polygons.
- Polygons may be non-triangulated.
- Designed for low-level, primitive mesh operations.
- Not designed for rendering meshes at runtime.
  To render a mesh in realtime application, convert the mesh
  to C3DMeshModelArchive with C3DMeshModelBuilder

*/
class General3DMesh
{
	unsigned int m_MeshFlag;

	unsigned int m_VertexFormatFlag;

	/// shared pointer of the vertex buffer. The pointer is shared by all the polygons in m_vecPolygon
	boost::shared_ptr< std::vector<General3DVertex> > m_pVertexBuffer;

	/// Polygons that hold indices to vertices
	/// - Can be retrieved later
	/// - The polygons can include non-triangle polygons (with more than 4 vertices)
	/// - They are triangulated and sotred in mesh model archive by C3DMeshModelBuilder
	std::vector<IndexedPolygon> m_vecPolygon;

	/// stores surface property
	std::vector<CMMA_Material> m_vecMaterial;

	/// skeleton hierarchy
	CMMA_Bone m_SkeletonRootBone;

public:

	inline General3DMesh();

	/// Converts the general 3d mesh into the triangulated mesh archive
	/// \param dest [out] mesh archive
	void Create3DMeshModelArchive( C3DMeshModelArchive& dest );

	boost::shared_ptr< std::vector<General3DVertex> > GetVertexBuffer() { return m_pVertexBuffer; }

	const boost::shared_ptr< std::vector<General3DVertex> > GetVertexBuffer() const { return m_pVertexBuffer; }


	std::vector<IndexedPolygon>& GetPolygonBuffer() { return m_vecPolygon; }

	const std::vector<IndexedPolygon>& GetPolygonBuffer() const { return m_vecPolygon; }

	void ClearPolygonBuffer() { m_vecPolygon.clear(); }


	std::vector<CMMA_Material>& GetMaterialBuffer() { return m_vecMaterial; }

	int GetNumMaterials() const { return (int)m_vecMaterial.size(); }

	void ClearMaterials() { m_vecMaterial.clear(); }

	inline int GetMaterialIndexFromName( const std::string& material_name );


	CMMA_Bone& GetSkeletonRootBoneBuffer() { return m_SkeletonRootBone; }

	unsigned int GetVertexFormatFlags() const { return m_VertexFormatFlag; }

	void SetVertexFormatFlags( unsigned int flags ) { m_VertexFormatFlag = flags; }

	inline void UpdatePolygonBuffer();

	inline void GetIndexedTriangles( std::vector<Vector3>& vecVertex,
		                             std::vector<int>& vecIndex,
		                             std::vector<int>& vecMaterialIndex );

	inline void FlipPolygons();

	inline void CalculateVertexNormalsFromPolygonPlanes();

//	void Append( General3DMesh& mesh );

	/// Sets positoins, normals, and texture coordinates.
	/// NOTE: the vertex format flags are not updated. Client code is responsible
	///       for updating the the vertex format flags.
	inline void SetVertices( const std::vector<Vector3>& positions, const std::vector<Vector3>& normals, const std::vector<TEXCOORD2>& tex_uvs );
	
	template<typename T>
	inline void SetPolygons( const std::vector< std::vector<T> >& polygons );

	friend class C3DModelLoader;
};


// ============================ inline implementations ============================

inline General3DMesh::General3DMesh()
:
m_MeshFlag(0),
m_VertexFormatFlag(0)
{
	m_pVertexBuffer
		= boost::shared_ptr< std::vector<General3DVertex> >( new std::vector<General3DVertex>() );
}


inline int General3DMesh::GetMaterialIndexFromName( const std::string& material_name )
{
	for( size_t i=0; i<m_vecMaterial.size(); i++ )
	{
		if( material_name == m_vecMaterial[i].Name )
			return (int)i;
	}

	return -1;
}


inline void General3DMesh::UpdatePolygonBuffer()
{
	size_t i, num_pols = m_vecPolygon.size();
	for( i=0; i<num_pols; i++ )
	{
		m_vecPolygon[i].UpdateAABB();
		m_vecPolygon[i].UpdatePlane();
	}
}


inline void General3DMesh::GetIndexedTriangles( std::vector<Vector3>& vecVertex,
		                                         std::vector<int>& vecIndex,
		                                         std::vector<int>& vecMaterialIndex )
{
	std::vector<IndexedPolygon> triangulated_polygon_buffer;
	Triangulate( triangulated_polygon_buffer, m_vecPolygon );

	// copy vertices
	const size_t num_vertices = m_pVertexBuffer->size();
	vecVertex.resize( num_vertices );
	for( size_t i=0; i<num_vertices; i++ )
	{
		vecVertex[i] = (*m_pVertexBuffer)[i].m_vPosition;
	}

	// copy triangle and material indices
	const size_t num_triangles = triangulated_polygon_buffer.size();
	vecIndex.resize( num_triangles * 3 );
	vecMaterialIndex.resize( num_triangles );
	for( size_t i=0; i<num_triangles; i++ )
	{
		IndexedPolygon& triangle = triangulated_polygon_buffer[i];
		vecIndex[i*3  ] = triangle.m_index[0];
		vecIndex[i*3+1] = triangle.m_index[1];
		vecIndex[i*3+2] = triangle.m_index[2];

		vecMaterialIndex[i] = triangle.m_MaterialIndex;
	}
}


inline void General3DMesh::FlipPolygons()
{
	const size_t num_polygons = m_vecPolygon.size();
	for( size_t i=0; i<num_polygons; i++ )
	{
		m_vecPolygon[i].Flip();
	}
}


inline void General3DMesh::CalculateVertexNormalsFromPolygonPlanes()
{
	using std::vector;

	boost::shared_ptr< std::vector<General3DVertex> > pVB = GetVertexBuffer();
	if( !pVB )
		return;

	std::vector<General3DVertex>& vertices = *pVB;

	if( vertices.empty() )
		return;

	// indices of polygons that share the i-th vertex
	vector< vector<int> > polygon_indices;
	polygon_indices.resize( vertices.size() );

	for( size_t i=0; i<m_vecPolygon.size(); i++ )
	{
		for( size_t j=0; j<m_vecPolygon[i].m_index.size(); j++ )
		{
			polygon_indices[ m_vecPolygon[i].m_index[j] ].push_back( (int)i );
		}
	}

	for( size_t i=0; i<vertices.size(); i++ )
	{
		Vector3 sum_normal = Vector3(0,0,0);
		for( size_t j=0; j<polygon_indices[i].size(); j++ )
		{
			sum_normal += m_vecPolygon[ polygon_indices[i][j] ].GetPlane().normal;
		}

		sum_normal.Normalize();

		vertices[i].m_vNormal = sum_normal;
	}
}


inline void General3DMesh::SetVertices(
	const std::vector<Vector3>& positions,
	const std::vector<Vector3>& normals,
	const std::vector<TEXCOORD2>& tex_uvs )
{
	if( positions.size() != normals.size()
	 || normals.size()   != tex_uvs.size() )
	{
		return;
	}

	const size_t num_vertices = positions.size();

	boost::shared_ptr< std::vector<General3DVertex> > pVB = GetVertexBuffer();
	if( !pVB )
		return;

	std::vector<General3DVertex>& vertices = *pVB;

	vertices.resize( num_vertices );
	for( size_t i=0; i<num_vertices; i++ )
	{
		vertices[i].m_vPosition = positions[i];
		vertices[i].m_vNormal   = normals[i];
		vertices[i].m_TextureCoord.resize( 1 );
		vertices[i].m_TextureCoord[0] = tex_uvs[i];
	}
}


/// T must be a singed or an unsigned integer type
template<typename T>
inline void General3DMesh::SetPolygons( const std::vector< std::vector<T> >& polygons )
{
	std::vector<IndexedPolygon>& polygon_buffer = GetPolygonBuffer();
	polygon_buffer.resize( 0 );

	size_t num_polygons = polygons.size();
	polygon_buffer.resize( num_polygons );
	for( size_t i=0; i<num_polygons; i++ )
	{
		const size_t num_points = polygons[i].size();
		polygon_buffer[i].m_index.resize( num_points );
		for( size_t j=0; j<num_points; j++ )
			polygon_buffer[i].m_index[j] = (int)polygons[i][j];

		polygon_buffer[i].SetVertexBuffer( GetVertexBuffer() );
	}
}


inline Result::Name CalculateAABB( const General3DMesh& src_mesh, AABB3& dest_aabb )
{
	dest_aabb.Nullify();

	if( !src_mesh.GetVertexBuffer() )
		return Result::INVALID_ARGS;

	const std::vector<General3DVertex>& vert_buffer = *(src_mesh.GetVertexBuffer().get());
	const size_t num_verts = vert_buffer.size();
	for( size_t i=0; i<num_verts; i++ )
	{
		dest_aabb.AddPoint( vert_buffer[i].m_vPosition );
	}

	return Result::SUCCESS;
}



inline boost::shared_ptr<General3DMesh> CreateGeneral3DMesh()
{
	boost::shared_ptr<General3DMesh> pMesh
		= boost::shared_ptr<General3DMesh>( new General3DMesh() );

	return pMesh;
}
} // namespace amorphous



#endif /* __General3DMesh_H__ */
