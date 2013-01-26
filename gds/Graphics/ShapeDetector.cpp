#include "ShapeDetector.hpp"
#include "MeshModel/General3DMesh.hpp"
#include "MeshModel/PrimitiveShapeMeshes.hpp"
#include "../3DMath.hpp"
#include "../Support/Log/DefaultLog.hpp"
#include <set>


namespace amorphous
{

using namespace std;
using namespace boost;



/*
Should concave polygons be supported by physics engine?
- rationale: make client code of the physics engine simpler,

  Steps in PhysX engine
  1. Dividing a polygon mesh into a set of convex meshes
  2. Create convex mesh shapes
  3. Create an actor
- Should define a mesh type?
  - ConvexSets, in addition to the convex and concave meshes
  - Mesh suited for dividing into convex meshes
*/


/// Used for matching 2 normals, not for ordering planes.
class plane_normal_comparision
{
public:
	bool operator()(const Plane& left, const Plane& right) const
	{
		const  float normal_test_tolerance_sq = 0.000001f;
		return Vec3LengthSq(left.normal - right.normal) < normal_test_tolerance_sq;
	}
};

static inline bool have_shared_point( const IndexedPolygon& polygon0, const IndexedPolygon& polygon1 )
{
	const uint num_verts0 = (uint)polygon0.m_index.size();
	const uint num_verts1 = (uint)polygon1.m_index.size();
	for( uint i=0; i<num_verts0; i++ )
	{
		for( uint j=0; j<num_verts1; j++ )
		{
			if( polygon0.m_index[i] == polygon1.m_index[j] )
				return true;
		}
	}

	return false;
}


bool CShapeDetector::IsAABox( const General3DMesh& src_mesh, AABB3& aabb )
{
	float aa_plane_dists[6] = {0,0,0,0,0,0};
	int aa_plane_found[6];
	memset( aa_plane_found, 0, sizeof(aa_plane_found) );

	const Vector3 aa_normals[] =
	{
		Vector3(1,0,0),
		Vector3(0,1,0),
		Vector3(0,0,1),
		Vector3(-1,0,0),
		Vector3(0,-1,0),
		Vector3(0,0,-1),
	};

	const std::vector<IndexedPolygon>& polygons = src_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	for( int i=0; i<num_polygons; i++ )
	{
		const Plane& plane = polygons[i].GetPlane();
		bool is_plane_aa = false;
		for( int j=0; j<6; j++ )
		{
			if( 0.001f < Vec3LengthSq( plane.normal - aa_normals[j] ) )
				continue;

			// aligned along the j-th axis

			if( aa_plane_found[j] )
			{
				// The plane must be the same with those of the other polygons aligned along this axis
				if( 0.001f < fabsf( plane.dist - aa_plane_dists[j] ) )
					return false; // has the same normal but the planes are different.
			}
			else
			{
				aa_plane_dists[j] = plane.dist;
				is_plane_aa = true;
				break;
			}
		}

		if( !is_plane_aa )
			return false;
	}

	aabb.vMax.x =  aa_plane_dists[0];
	aabb.vMax.y =  aa_plane_dists[1];
	aabb.vMax.z =  aa_plane_dists[2];
	aabb.vMin.x = -aa_plane_dists[3];
	aabb.vMin.y = -aa_plane_dists[4];
	aabb.vMin.z = -aa_plane_dists[5];

	return true;
}


bool CShapeDetector::IsBox( const General3DMesh& src_mesh, CBoxDesc& desc, Matrix34& pose )
{
	// collect normals
	// If the mesh
	// 1. has 6 different polygon normals,
	// 2. can be sorted to 3 pairs, and each pair has normals face in the opposite directions,
	// 3. 3 pairs have perpendicular normals,
	// detect it as a box

	std::vector<Plane> polygon_planes;
	const std::vector<IndexedPolygon>& polygons = src_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	for( int i=0; i<num_polygons; i++ )
	{
		bool is_on_new_plane = false;
		for( int j=0; j<(int)polygon_planes.size(); j++ )
		{
			if( AlmostSamePlanes( polygons[i].GetPlane(), polygon_planes[j] ) )
				continue;

			is_on_new_plane = true;
			break;
		}
		
		polygon_planes.push_back( polygons[i].GetPlane() );

		if( 6 < polygon_planes.size() )
			break;
	}

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
		dist_sums[i] = six_planes[perpendicular_pairs[i].first].dist + six_planes[perpendicular_pairs[i].second].dist;

	// Take the absolutes just in case, although sum of dists should always be positive.
	desc.vLengths.x = fabsf( dist_sums[0] );
	desc.vLengths.y = fabsf( dist_sums[1] );
	desc.vLengths.z = fabsf( dist_sums[2] );

	for( int i=0; i<3; i++ )
		pose.matOrient.SetColumn( i, six_planes[perpendicular_pairs[i].first].normal );

	pose.vPosition = GetAABB( polygons ).GetCenterPosition();

	return true;
}


bool CShapeDetector::IsConvex( const General3DMesh& src_mesh )
{
	bool is_not_sphere = false;

	const boost::shared_ptr< vector<General3DVertex> >& pVertBuffer = src_mesh.GetVertexBuffer();
	if( !pVertBuffer )
		return false;

	vector<General3DVertex>& vert_buffer = *pVertBuffer;

	// max_angle_between_normals_of_connected_polygons
	float max_angle = -FLT_MAX;
	const std::vector<IndexedPolygon>& polygons = src_mesh.GetPolygonBuffer();
	const int num_polygons = (int)polygons.size();
	for( int i=0; i<num_polygons; i++ )
	{
		float convex_normal_tolerance = 0.000001f;
		const Plane& plane = polygons[i].GetPlane();
		for( int j=i+1; j<num_polygons; j++ )
		{
			// angles between normals
			if( have_shared_point( polygons[i], polygons[j] ) )
			{
				float angle = Vec3GetAngleBetween( polygons[i].GetPlane().normal, polygons[j].GetPlane().normal );
				if( 60.0f < rad_to_deg(angle) )
					is_not_sphere = true;
//				if( max_angle < angle )
//					max_angle = angle;
			}

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


bool CShapeDetector::IsSphere( const General3DMesh& src_mesh, Sphere& sphere )
{
	if( !src_mesh.GetVertexBuffer() )
		return false;

	const vector<General3DVertex>& vertex_buffer = *(src_mesh.GetVertexBuffer());
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

	// Save the detected sphere
	sphere.center = vCenter;
	sphere.radius = sqrtf( ref_radius_sq );

	return true;
}


/**
- Meshes that are not detected as capsules
  - capsules that are unevenly scaled along 1 or 2 axis.
  - capsules that have redudant vertices on sides, or the cylinder part of the capsule.
*/
bool CShapeDetector::IsCapsule( const General3DMesh& src_mesh, Capsule& capsule )
{
	if( !src_mesh.GetVertexBuffer() )
		return false;

	const vector<General3DVertex>& vertex_buffer = *(src_mesh.GetVertexBuffer());

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
		const Vector3 vPos = vertex_buffer[i].m_vPosition;
		const Vector3 vFromEndPos0 = vPos - vEndPos[0];
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

	Transform pose;
	pose.qRotation.FromRotationMatrix( CreateOrientFromFwdDir( Vec3GetNormalized(vEndPos[1] - vEndPos[0]) ) );
	pose.vTranslation = (vEndPos[1] + vEndPos[0]) * 0.5f;
	capsule.radius = r;
//	capsule.length = Vec3Length( vEndPos[1] - vEndPos[0] );
	capsule.p0 = vEndPos[0];
	capsule.p1 = vEndPos[1];

	return true;
}

/*
bool CShapeDetector::IsCylinder( const General3DMesh& src_mesh, CCylinderDesc& cylinder );
{
	return false;
}
*/

bool CShapeDetector::DetectShape( const General3DMesh& src_mesh, CShapeDetectionResults& results )
{
	AABB3 aabb;
	CBoxDesc box_desc;
	Matrix34 pose( Matrix34Identity() );
	Sphere sphere;
	Capsule cap;

	if( !src_mesh.GetVertexBuffer()
	 || src_mesh.GetVertexBuffer()->empty()
	 || src_mesh.GetPolygonBuffer().empty() )
	{
		return false;
	}

	if( IsAABox( src_mesh, aabb ) )
	{
		LOG_PRINT( " Detected an AABB: " + to_string(aabb) );
		results.shape = MeshShape::AXIS_ALIGNED_BOX;
		results.aabb = aabb;
		return true;
	}
	else if( IsBox( src_mesh, box_desc, pose ) )
	{
		LOG_PRINT( " Detected an OBB: " + to_string(OBB3(pose,box_desc.vLengths*0.5f)) );
		results.shape = MeshShape::ORIENTED_BOX;
		results.pose = pose;
		return true;
	}
	else if( IsSphere( src_mesh, sphere ) )
	{
		LOG_PRINT( " Detected a sphere." );
		results.shape = MeshShape::SPHERE;
		return true;
	}
	else if( IsCapsule( src_mesh, cap ) )
	{
		LOG_PRINT( " Detected a capsule." );
		results.shape = MeshShape::CAPSULE;
		results.capsule = cap;
		return true;
	}
	else if( IsConvex( src_mesh ) )
	{
		results.shape = MeshShape::CONVEX;
		return true;
	}

	return false;
}


} // namespace amorphous
