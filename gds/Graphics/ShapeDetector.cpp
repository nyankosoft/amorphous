#include "ShapeDetector.hpp"
#include "../3DMath.hpp"
#include "../Support/Log/DefaultLog.hpp"

using namespace std;
using namespace boost;


/// Used for matching 2 normals, not for ordering planes.
class plane_normal_comparision
{
public:
	bool operator()(const Plane& left, const Plane& right) const
	{
		const  float normal_test_tolerance_sq = 0.000001f;
		return normal_test_tolerance_sq < Vec3LengthSq(left.normal - right.normal);
	}
};

static inline have_shared_point( const CIndexedPolygon& polygon0, const CIndexedPolygon& polygon1 )
{
	const uint num_verts0 = (uint)polygon0.m_index.size();
	const uint num_verts1 = (uint)polygon1.m_index.size();
	for( uint i=0; i<num_elements; i++ )
	{
		for( uint j=0; j<num_elements; j++ )
		{
			if( polygon0.m_index[i] == polygon1.m_index[j] )
				return true;
		}
	}

	return false;
}


bool CShapeDetector::IsAABox( const CGeneral3DMesh& src_mesh, AABB3& aabb )
{
	LOG_PRINT_ERROR( " Not implemented." );

	return false;
}


bool CShapeDetector::IsBox( const CGeneral3DMesh& connected_mesh, CBoxDesc& desc, Matrix34& pose )
{
	// collect normals
	// If the mesh
	// 1. has 6 different polygon normals,
	// 2. can be sorted to 3 pairs, and each pair has normals face in the opposite directions,
	// 3. 3 pairs have perpendicular normals,
	// detect it as a box

//	std::set<Vector3> polygon_normals;
	std::set<Plane,plane_normal_comparision> polygon_planes;
	const std::vector<CIndexedPolygon>& polygons = connected_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	for( int i=0; i<num_polygons; i++ )
	{
//		polygon_normals.insert( polygons[i].GetPlane().normal );
		polygon_planes.insert( polygons[i].GetPlane() );
	}

//		if( polygon_normals.size() != 6 )
	if( polygon_planes.size() != 6 )
		return false;

	int perpendicular_pairs_found[6];
	for( int i=0; i<6; i++ )
		perpendicular_pairs_found[i] = 0;

	const float perpendicular_test_tolerance_sq = 0.000001f;
//	vector<Vector3> six_normals;
//	six_normals.assign( polygon_normals.begin(), polygon_normals.end() );
	vector<Plane> six_planes;
	six_planes.assign( polygon_planes.begin(), polygon_planes.end() );
	int num_perpendicular_pairs = 0;
	vector< pair<int,int> > perpendicular_pairs;
	for( int i=0; i<6; i++ )
	{
		if( perpendicular_pairs_found[i] )
			continue;

		for( int j=i+1; j<6; j++ )
		{
			if( perpendicular_pairs_found[j] )
				continue;

//			if( Vec3LengthSq( six_normals[i] + six_normals[j] ) < perpendicular_test_error_sq )
			if( Vec3LengthSq( six_planes[i].normal + six_planes[j].normal ) < perpendicular_test_tolerance_sq )
			{
				perpendicular_pairs_found[i] = 1;
				perpendicular_pairs_found[j] = 1;
//				num_perpendicular_pairs++;
				perpendicular_pairs.push_back( pair<int,int>(i,j) );
				break;
			}
		}
	}

	if( perpendicular_pairs.size() != 3 )
		return false; // normals are not facing in the opposite directions

	// Consider this a box.

	// Compute the pose and size of the box

	float dist_sums[3] = {0,0,0};
	for( int i=0; i<3; i++ )
		dist_sums[i] = six_planes[perpendicular_pairs[i].first].dist - six_planes[perpendicular_pairs[i].second].dist;

	// Take the absolutes just in case, although sum of dists should always be positive.
	desc.vLengths.x = fabsf( dist_sums[0] );
	desc.vLengths.y = fabsf( dist_sums[1] );
	desc.vLengths.z = fabsf( dist_sums[2] );

	for( int i=0; i<3; i++ )
		pose.matOrient.SetColumn( i, six_planes[perpendicular_pairs[i].first].normal );

	pose.vPosition = GetAABB( polygons ).GetCenterPosition();

	return true;
}


bool CShapeDetector::IsConvex( const CGeneral3DMesh& connected_mesh )
{
	is_not_sphere = false;

	const shared_ptr< vector<CGeneral3DVertex> >& pVertBuffer = connected_mesh.GetVertexBuffer();
	if( !pVertBuffer )
		return false;

	vector<CGeneral3DVertex>& vert_buffer = *pVertBuffer;

	// max_angle_between_normals_of_connected_polygons
	float max_angle = -FLT_MAX;
	const std::vector<CIndexedPolygon>& polygons = connected_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	for( int i=0; i<num_polygons; i++ )
	{
		// angles between normals
		if( have_shared_point( polygons[i], polygons[j] ) )
		{
			float angle = Vec3GetAngleBetween( polygons[i].GetPlane().normal, polygons[j].GetPlane().normal );
			if( 60.0f < rad_to_deg(angle) )
				is_not_sphere = true;
//			if( max_angle < angle )
//				max_angle = angle;
		}

		float convex_normal_tolerance = 0.000001f;
		const Plane& plane = polygons[i].GetPlane();
		for( int j=i+1; j<num_polygons; j++ )
		{
			const int num_verts = (int)polygons[j].m_index.size();
			for( int k=0; k<num_verts; k++ )
			{
				const Vector3& pos = vert_buffer[ polygons[j].m_index[k] ].m_vPosition;
				if( convex_normal_tolerance < Vec3Dot( plane.normal, pos ) - plane.dist )
					return false; // Found concave polygons.
			}
		}
	}

	return true;
}


bool CShapeDetector::IsSphere( const CGeneral3DMesh& src_mesh, Sphere& sphere )
{
	vector<CGeneral3DVertex>& vertex_buffer = src_mesh.GetVertexBuffer();
	const int num_vertices = (int)vertex_buffer.size();

	if( num_vertices < 5 )
		return false;

	AABB3 aabb;
	aabb.Nullify();
	for( int i=0; i<num_vertices; i++ )
		aabb.AddPoint( vertex_buffer[i].m_vPosition );

	const Vector3 vCenter = aabb.GetCenterPosition();

	const float ref_radius_sq = Vec3LengthSq( vertex_buffer[0].m_vPosition - vCenter );

	const float radius_error = 0.005f;
	const float radius_error_sq = radius_error * radius_error;
	for( int i=1; i<num_vertices; i++ )
	{
		const float radius_sq = Vec3LengthSq( vertex_buffer[i].m_vPosition - vCenter );
		if( radius_error_sq < fabs( ref_radius_sq - radius_sq ) )
			return false;
	}

	sphere.vCenter = vCenter
	sphere.radius = sqrtf( ref_radius_sq );
	return true;
}


/**
- Meshes that are not detected as capsules
  - capsules that are unevenly scaled along 1 or 2 axis.
  - capsules that have redudant vertices on sides, or the cylinder part of the capsule.
*/
bool CShapeDetector::IsCapsule( const CGeneral3DMesh& src_mesh, Capsule& capsule )
{
	if( !src_mesh.GetVertexBuffer() )
		return false;

	vector<CGeneral3DVertex>& vertex_buffer = src_mesh.GetVertexBuffer();

	const int num_polygons = 0;
	const int num_vertices = (int)vertex_buffer.size();

	if( num_polygons <= 6 )
		return false;

	// find 2 points with the maximum distance
	Vector3 vEndPos[2] = { Vector3(0,0,0), Vector3(0,0,0) };
	int axis_point_indices[2] = { -1, -1 };
	float max_dist_sq = 0;
	float dist_sq = 0;
	for( int i=0; i<num_vertices; i++ )
	{
		Vector3 vPos0 = vertex_buffer[i].m_vPosition;
		for( int j=i+1; j<num_vertices; j++ )
		{
			Vector3 vPos1 = vertex_buffer[j].m_vPosition;

			dist_sq = Vec3LengthSq( vPos1 - vPos0 );
			if( max_dist_sq < dist_sq )
			{
				max_dist_sq = dist_sq;
				axis_point_indices[0] = i;
				axis_point_indices[1] = j;
			}
		}
	}

	vEndPos[0] = vertex_buffer[axis_point_indices[0]].m_vPosition;
	vEndPos[1] = vertex_buffer[axis_point_indices[1]].m_vPosition;

	// calc axis
	Vector3 vAxis = Vec3GetNormalized( vEndPos[1] - vEndPos[0] );

	// calc max distance from the axis, save it as radius
	max_dist_sq = 0;
	for( int i=0; i<num_vertices; i++ )
	{
		const Vector3 vFromEndPos0 = vPos[i] - vEndPos[0];
		const float proj = Vec3Dot( vFromEndPos0, vAxis );
		Vector3 vAxisToPos = - vAxis * proj + vFromEndPos0;
		dist_sq = Vec3LengthSq(vAxisToPos);
		if( max_dist_sq < dist_sq )
		{
			max_dist_sq = dist_sq;
		}
	}

	const float r = sqrtf(max_dist_sq);

	// see if all the points are on either of the hemispheres
	Vector3 vCenters[2] = { vEndPos[0] + vAxis * r, vEndPos[1] - vAxis * r };
	const float radius_error = 0.005f;
	const float radius_error_sq = radius_error * radius_error;
	for( int i=0; i<num_vertices; i++ )
	{
		const Vector3 vPos = vertex_buffer[i].m_vPosition;

		if( i == axis_point_indices[0]
		 || i == axis_point_indices[1] )
		{
			continue;
		}

		if( radius_error_sq < Vec3LengthSq( vPos - vCenters[0] )
		 && radius_error_sq < Vec3LengthSq( vPos - vCenters[1] ) )
		{
			// Does not belong to either of the two hemispheres
			return false;
		}
	}

	capsule.pose.qRotation.FromMatrix33( CreateOrientationFromFwdDirection( Vec3GetNormalized(vEndPos[1] - vEndPos[0]) ) );
	capsule.pose.vTranslation = (vEndPos[1] + vEndPos[0]) * 0.5f;
	capsule.radius = r;
	capsule.length = Vec3Length( vEndPos[1] - vEndPos[0] );

	return true;
}

/*
bool CShapeDetector::IsCylinder( const CGeneral3DMesh& connected_mesh, CCylinderDesc& cylinder );
{
	return false;
}
*/
