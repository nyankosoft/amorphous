#include "MeshArchiveToGeneral3DMeshConverer.hpp"
#include "3DMeshModelArchive.hpp"
#include "General3DMesh.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


Result::Name CMeshArchiveToGeneral3DMeshConverer::Convert( const C3DMeshModelArchive& src, General3DMesh& dest )
{
	LOG_PRINT_ERROR( " Not implemented yet." );
	return Result::UNKNOWN_ERROR;

	// copy vertices

	std::shared_ptr< vector<General3DVertex> >& pVerts = dest.GetVertexBuffer();
	if( !pVerts )
		return Result::INVALID_ARGS;

	vector<General3DVertex>& dest_verts = *pVerts;

	const CMMA_VertexSet& src_verts = src.GetVertexSet();
	const uint num_vertices = (uint)src_verts.GetNumVertices();
	for( uint i=0; i<num_vertices; i++ )
	{
		dest_verts[i].m_vPosition = src_verts.vecPosition[i];
	}

	// copy triangle polygons

	if( src.GetVertexIndex().size() % 3 != 0 )
		return Result::INVALID_ARGS;

	std::vector<IndexedPolygon>& dest_polygons = dest.GetPolygonBuffer();

	const uint num_triangles = (uint)src.GetVertexIndex().size() / 3;
	dest_polygons.resize( num_triangles );
	for( uint i=0; i<num_triangles; i++ )
	{
		IndexedPolygon& dest_polygon = dest_polygons[i];
		dest_polygon.SetVertexBuffer( pVerts );
		dest_polygon.m_index.resize( 3 );
		dest_polygon.m_index[i*3]   = src.GetVertexIndex()[i*3];
		dest_polygon.m_index[i*3+1] = src.GetVertexIndex()[i*3+1];
		dest_polygon.m_index[i*3+2] = src.GetVertexIndex()[i*3+2];
	}

	dest.UpdatePolygonBuffer();

	return Result::SUCCESS;
}


} // namespace amorphous
