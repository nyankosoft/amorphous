#include "MeshOperations.hpp"
#include "Graphics/MeshModel/General3DMesh.hpp"
#include "3DMath/Sphere.hpp"
#include <set>

using namespace std;
using namespace boost;


static void GetConnected( int pnt_id,
						  std::set<int>& setCheckedPoints,
						  const std::vector<CIndexedPolygon>& polygons,
						  CConnectedSet& connected,
						  std::set<int>& setConnectedPoly
						  )
{
	if( setCheckedPoints.find( pnt_id ) == setCheckedPoints.end() )
	{
		// not processed yet
		setCheckedPoints.insert( pnt_id );
		connected.vecPoint.push_back( pnt_id );
	}
	else
		return; // already processed

//	vector<int> vecPntID;
	for( size_t i=0; i<polygons.size(); i++ )
	{
//		vecPntID.resize( 0 );
//		polygons[i].GetPointIDs( vecPntID );
		const vector<int>& vecPntID = polygons[i].m_index;
//		bool poly_has_point = false;
		for( size_t j=0; j<vecPntID.size(); j++ ) // (A)
		{
			if( vecPntID[j] != pnt_id )
				continue;

			setConnectedPoly.insert( (int)i );

			// the polygon has the currently focused point
			// Check all the other points
			for( size_t k=0; k<vecPntID.size(); k++ ) // (B)
			{
				if( vecPntID[k] == pnt_id )
					continue;

				GetConnected(
						vecPntID[k],
						setCheckedPoints,
						polygons,
						connected,
						setConnectedPoly
						);

			}

			// All the points on the polygon are added in the loop (B).
			// All the points on the connected polygons have been added in recursive calls of GetConnected().
			break;
		}
	}
}


void GetConnectedSets( const std::vector<int>& vecPoint,
					   const std::vector<CIndexedPolygon>& polygons,
					   std::vector<CConnectedSet>& vecConnected )
{
	set<int> setCheckedPoints;
	vecConnected.reserve( 16 );

	// temporarily holds connected polygons
	// the content is later moved to CConnectedSet::vecPoly
	set<int> setConnectedPoly;

	const size_t num_points = vecPoint.size();
	for( size_t i=0; i<num_points; i++ )
	{
		if( setCheckedPoints.find( vecPoint[i] ) != setCheckedPoints.end() )
			continue; // already processed

		vecConnected.push_back( CConnectedSet() );

		setConnectedPoly.clear();

		GetConnected(
			vecPoint[i],
			setCheckedPoints,
			polygons,
			vecConnected.back(),
			setConnectedPoly
			);

		vecConnected.back().vecPoly.assign( setConnectedPoly.begin(), setConnectedPoly.end() );
	}
}


void GetConnectedSets( CGeneral3DMesh& mesh,
					   std::vector<CConnectedSet>& vecConnected )
{
//	vector<LWPntID> vecPoint;
//	vector<CLWPolygon> vecPoly;

//	mesh_info.GetPoints( vecPoint );
//	mesh_info.GetPolygons( vecPoly );

	vector<int> points;
	const vector<CIndexedPolygon>& polygons = mesh.GetPolygonBuffer();

	GetConnectedSets( points, polygons, vecConnected );
}


/// Returns true if a given set of vertices of a mesh is on a sphere
bool AreVerticesOnSphere( const CGeneral3DMesh& mesh, // [in]
						  const std::vector<int>& vertex_indices, // [in]
						  Sphere& sphere, // [out]
						  float radius_error_tolerance ) // [in]
{
	const shared_ptr< vector<CGeneral3DVertex> >& pVertices = mesh.GetVertexBuffer();
	if( !pVertices )
		return false;

	const vector<CGeneral3DVertex>& vertices = *pVertices;

	if( vertices.size() <= 1 )
		return false;

	AABB3 aabb;
	aabb.Nullify();
	const int num_target_vertices = (int)vertex_indices.size();
	for( int i=0; i<num_target_vertices; i++ )
	{
		aabb.AddPoint( vertices[vertex_indices[i]].m_vPosition );
	}

	const Vector3 vCenter = aabb.GetCenterPosition();

	const float r = Vec3Length( vCenter - vertices.front().m_vPosition );
	const float r_squared = r * r;
	const float tolerance_for_diff_of_squared_radius
		= 2.0f * r * radius_error_tolerance
		+ radius_error_tolerance * radius_error_tolerance;

	for( int i=1; i<num_target_vertices; i++ )
	{
		float diff_of_squared_radius = fabs( r_squared - Vec3LengthSq( vCenter - vertices[i].m_vPosition ) );
		if( tolerance_for_diff_of_squared_radius < diff_of_squared_radius )
			return false;
	}

	return true;
}
