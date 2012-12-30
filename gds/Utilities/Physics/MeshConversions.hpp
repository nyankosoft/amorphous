#ifndef __MeshConversions_HPP__
#define __MeshConversions_HPP__


#include "../../Physics/TriangleMeshDesc.hpp"
#include "../../Graphics/MeshModel/General3DMesh.hpp"


namespace amorphous
{


inline void General3DMeshToTriangleMeshDesc( const CGeneral3DMesh& src, physics::CTriangleMeshDesc& dest )
{
	using std::vector;

	vector<CIndexedPolygon> triangulated_polygons;
	Triangulate( triangulated_polygons, src.GetPolygonBuffer() );

	const boost::shared_ptr< vector<CGeneral3DVertex> > pVertices = src.GetVertexBuffer();

	if( !pVertices )
		return;

	const vector<CGeneral3DVertex>& vertices = *pVertices;
	const int num_verts = (int)vertices.size();
	dest.m_vecVertex.resize( num_verts );
	for( int i=0; i<num_verts; i++ )
	{
		dest.m_vecVertex[i] = vertices[i].m_vPosition;
	}

	const int num_triangles = (int)triangulated_polygons.size();
	dest.m_vecIndex.resize( num_triangles * 3 );
	for( int i=0; i<num_triangles; i++ )
	{
		if( triangulated_polygons[i].m_index.size() != 3 )
			continue;

		dest.m_vecIndex[i*3]   = triangulated_polygons[i].m_index[0];
		dest.m_vecIndex[i*3+1] = triangulated_polygons[i].m_index[1];
		dest.m_vecIndex[i*3+2] = triangulated_polygons[i].m_index[2];
	}
}


} // amorphous



#endif /* __MeshConversions_HPP__ */
