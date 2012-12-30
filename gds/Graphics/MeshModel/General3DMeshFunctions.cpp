#include "General3DMeshFunctions.hpp"
#include "General3DMesh.hpp"


namespace amorphous
{

using namespace std;


static void GetConnectedSets_r( const std::vector<CIndexedPolygon>& polygons, //const CGeneral3DMesh& src_mesh,// std::vector< boost::shared_ptr<CGeneral3DMesh> >& pDestMeshes,
							   std::vector<int>& group,
							   std::vector<int>& already_selected )
{
//	const std::vector<CIndexedPolygon>& polygons = src_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();

	const CIndexedPolygon& polygon0 = polygons[group.back()];
	const int num_verts0 = polygon0.GetNumVertices();

	for( int i=0; i<num_polygons; i++ )
	{
		if( already_selected[i] )
			continue;

		const CIndexedPolygon& polygon1 = polygons[i];
		const int num_verts1 = polygon1.GetNumVertices();
		for( int j=0; j<num_verts0; j++ )
		{
			for( int k=0; k<num_verts1; k++ )
			{
				if( polygon0.m_index[j] == polygon1.m_index[k] )
				{
					already_selected[i] = 1;
					group.push_back(i);
					GetConnectedSets_r( polygons, group, already_selected );
					break;
				}
			}

			if( already_selected[i] )
				break;
		}
	}
}


void GetConnectedSets( const CGeneral3DMesh& src_mesh, std::vector< std::vector<int> >& connected_polygon_sets )
{
	using namespace std;

	const std::vector<CIndexedPolygon>& polygons = src_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();

	vector<int> already_selected;
	already_selected.resize( num_polygons, 0 );

	vector< vector<int> >& groups = connected_polygon_sets;
	groups.resize( 0 );
	groups.reserve( 16 );

//	GetConnectedSets_r( src_mesh.GetPolygonBuffer(), groups, already_selected );

	for( int i=0; i<num_polygons; i++ )
	{
		if( already_selected[i] )
			continue;

		already_selected[i] = 1;

		// collect connected polygons
		groups.push_back( vector<int>() );

		groups.back().push_back( i );

		GetConnectedSets_r( polygons, groups.back(), already_selected );
	}
}


void GetConnectedSets( const CGeneral3DMesh& src_mesh, std::vector< boost::shared_ptr<CGeneral3DMesh> >& pDestMeshes )
{
	std::vector< std::vector<int> > connected_polygon_sets;

	GetConnectedSets( src_mesh, connected_polygon_sets );

	const int num_connected_sets = (int)connected_polygon_sets.size();

	for( int i=0; i<num_connected_sets; i++ )
	{
		pDestMeshes.push_back( boost::shared_ptr<CGeneral3DMesh>() );
		pDestMeshes.back().reset( new CGeneral3DMesh );

		CGeneral3DMesh& connected = *(pDestMeshes.back());

		const vector<int>& connected_polygons = connected_polygon_sets[i];
		connected.GetPolygonBuffer().resize( connected_polygons.size() );
		for( int j=0; j<(int)connected_polygons.size(); j++ )
		{
			connected.GetPolygonBuffer()[j] = src_mesh.GetPolygonBuffer()[ connected_polygons[j] ];
		}

		*(connected.GetVertexBuffer()) = *(src_mesh.GetVertexBuffer());
	}
}



} // namespace amorphous
