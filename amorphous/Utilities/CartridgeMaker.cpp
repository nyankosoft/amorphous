#include "CartridgeMaker.hpp"
#include "../3DMath/PrimitivePolygonModelMaker.hpp"
#include "../3DMath/TCBSpline.hpp"
#include "../Graphics/MeshModel/General3DMesh.hpp"
#include "../Graphics/TextureCoord.hpp"
#include "../Support/Vec3_StringAux.hpp"


namespace amorphous
{

using namespace std;


namespace firearm
{


static Vector3 CalculateNormal( const vector< pair<float,float> >& diameter_and_height, int i )
{
	const float radius = diameter_and_height[i].first * 0.5f;
	const float height = diameter_and_height[i].second;

	const Vector3 z_dir( Vector3(0,0,1) );

	if( diameter_and_height.size() == 1 )
		return Vector3(0,0,0); // No way to calculate a normal out of a single slice

	if( i==0 )
	{
		float next_radius = diameter_and_height[1].first * 0.5f;
		float next_height = diameter_and_height[1].second;
		Vector3 extrude_dir = Vector3( next_radius - radius, next_height-height, 0 );
		return Vec3GetNormalized( Vec3Cross( extrude_dir, z_dir ) );
	}
	else if( i==diameter_and_height.size()-1 )
	{
		float prev_radius = diameter_and_height[i-1].first * 0.5f;
		float prev_height = diameter_and_height[i-1].second;
		Vector3 extrude_dir = Vector3( radius - prev_radius, height-prev_height, 0 );
		return Vec3GetNormalized( Vec3Cross( extrude_dir, z_dir ) );
	}
	else
	{
		// Has 3 or more segments
		float prev_radius = diameter_and_height[i-1].first * 0.5f;
		float prev_height = diameter_and_height[i-1].second;
		float next_radius = diameter_and_height[i+1].first * 0.5f;
		float next_height = diameter_and_height[i+1].second;
		Vector3 prev_extrude_dir = Vector3( radius - prev_radius, height-prev_height, 0 );
		Vector3 next_extrude_dir = Vector3( next_radius - radius, next_height-height, 0 );
		Vector3 prev_normal = Vec3GetNormalized( Vec3Cross( prev_extrude_dir, z_dir ) );
		Vector3 next_normal = Vec3GetNormalized( Vec3Cross( next_extrude_dir, z_dir ) );
		return Vec3GetNormalized( prev_normal + next_normal );
	}

	return Vector3(0,0,0);
}


static float CalculateNormalAngle( const vector< pair<float,float> >& diameter_and_height, int i )
{
	if( diameter_and_height.size() == 2
		&& fabs(diameter_and_height[0].first - diameter_and_height[1].first) < 0.000001 )
	{
		// a vertical wall
		return 0;
	}

	const float radius = diameter_and_height[i].first * 0.5f;
	const float height = diameter_and_height[i].second;

	if( i==0 )
	{
		float next_radius = diameter_and_height[1].first * 0.5f;
		float next_height = diameter_and_height[1].second;
		return -atan( (next_radius - radius) / (next_height-height) );
	}
	else if( i==diameter_and_height.size()-1 )
	{
		float prev_radius = diameter_and_height[i-1].first * 0.5f;
		float prev_height = diameter_and_height[i-1].second;
		return -atan( (radius - prev_radius) /  (height-prev_height) );
	}
	else
	{
		// Has 3 or more segments
		float prev_radius = diameter_and_height[i-1].first * 0.5f;
		float prev_height = diameter_and_height[i-1].second;
		float next_radius = diameter_and_height[i+1].first * 0.5f;
		float next_height = diameter_and_height[i+1].second;
//		float lower_normal_angle = -atan( (radius - prev_radius) / (height-prev_height) );
//		float upper_normal_angle = -atan( (next_radius - radius) / (next_height-height) );
		float lower_normal_angle = (0.000001f<abs(height-prev_height)) ? -atan( (radius - prev_radius) / (height-prev_height) ) : (float)PI * 0.5f;
		float upper_normal_angle = (0.000001f<abs(next_height-height)) ? -atan( (next_radius - radius) / (next_height-height) ) : (float)PI * 0.5f;
		float normal_angle = (lower_normal_angle + upper_normal_angle) * 0.5f;
		return normal_angle;
	}
}


/**
  \param[out] points_at_curved_slice has (num_curve_segments+1) elements upon success
*/
static void CalculatePointsAtCurvedSlice(
	const Vector2& current_point,
	const Vector2& prev_point,
	const Vector2& next_point,
	float curvature_radius,
	unsigned int num_curve_segments,
	vector<Vector2>& points_at_curved_slice
	)
{
	if( num_curve_segments < 2 )
		return;

	// Clear the destination buffer
	points_at_curved_slice.resize( 0 );

	const Vector2 to_prev = prev_point - current_point;
	const Vector2 to_next = next_point - current_point;
	//LOG_PRINT( "to_prev: " + to_string(to_prev) );
	//LOG_PRINT( "to_next: " + to_string(to_next) );

	const Vector2 dir_to_prev = Vec2GetNormalized(to_prev);
	const Vector2 dir_to_next = Vec2GetNormalized(to_next);
	const Vector2 dir_to_fit_circle_center = Vec2GetNormalized(dir_to_prev + dir_to_next);
	//LOG_PRINT( "dir_to_fit_circle_center: " + to_string(dir_to_fit_circle_center) );

	const float a = Vec2GetAngleBetween(dir_to_prev,dir_to_next) * 0.5f;
	//LOG_PRINT( "angle a: " + to_string(rad_to_deg(a)) );
	const float b = (float)PI * 0.5f - a;
	const float dist_from_current_point_to_fit_circle_center = curvature_radius / cos(b);
	//LOG_PRINT( "dist_from_current_point_to_fit_circle_center: " + to_string(dist_from_current_point_to_fit_circle_center,6) );

	const Vector2 fit_circle_center = current_point + dir_to_fit_circle_center * dist_from_current_point_to_fit_circle_center; 
	//LOG_PRINT( "fit_circle_center: " + to_string(fit_circle_center) );
	float d = Vec2Dot(to_prev,to_next);

	const float angle_sign = (dir_to_fit_circle_center.x < 0) ? 1 : -1;

	const float angles_to_cover = b * 2.0f * angle_sign;
	Vector2 lower_tangent_point = current_point + dir_to_prev * dist_from_current_point_to_fit_circle_center * sin(b);
	Vector2 upper_tangent_point = current_point + dir_to_next * dist_from_current_point_to_fit_circle_center * sin(b);
	Vector2 fit_circle_center_to_lower_tangent = lower_tangent_point - fit_circle_center;

	points_at_curved_slice.reserve( num_curve_segments + 1 );

	points_at_curved_slice.push_back( lower_tangent_point );

	for( unsigned int i=1; i<num_curve_segments; i++ )
	{
		float current_angle = angles_to_cover * (float)i / (float)num_curve_segments;
		if( next_point.y < prev_point.y )
			current_angle *= -1.0f;
		Vector2 point_on_curve = fit_circle_center + Matrix22Rotation(current_angle) * fit_circle_center_to_lower_tangent;
		points_at_curved_slice.push_back( point_on_curve );
	}

	points_at_curved_slice.push_back( upper_tangent_point );

	//LOG_PRINT( "points_at_curved_slice: " + to_string(points_at_curved_slice) );
}


static void CalculateDiametersAndHeightsAtCurvedSlice(
	const Vector2& current_point,
	const Vector2& prev_point,
	const Vector2& next_point,
	float curvature_radius,
	unsigned int num_curve_segments,
	vector< pair<float,float> >& diameter_and_height_pairs
	)
{
	vector<Vector2> dest_points;

	CalculatePointsAtCurvedSlice( current_point, prev_point, next_point, curvature_radius, num_curve_segments, dest_points );

	for( size_t i=0; i<dest_points.size(); i++ )
	{
		Vector2 point = dest_points[i];
		diameter_and_height_pairs.push_back( pair<float,float>(point.x*2.0f,point.y) );
	}
}

static void CorrectNormals(
	const vector<Vector3>& points,
	vector<Vector3>& normals,
	const vector< vector<int> >& polygons )
{
	for( size_t i=0; i<normals.size(); i++ )
	{
		bool processed = false;

		for( size_t j=0; j<polygons.size(); j++ )
		{
			const vector<int>& polygon = polygons[j];
			size_t num_points = polygon.size();
			for( size_t k=0; k<num_points; k++ )
			{
				if( polygon[k] == i )
				{
					Vector3 to_next = points[ polygon[(k+1)%num_points] ]            - points[ polygon[k] ];
					Vector3 to_prev = points[ polygon[(k+num_points-1)%num_points] ] - points[ polygon[k] ];
					Vector3 normal_from_polygon = Vec3Cross( to_next, to_prev );
					if( Vec3Dot( normals[i], normal_from_polygon ) < 0 )
						normals[i] *= -1.0f;
					processed = true;
					break;
				}

				if( processed )
					break;
			}
		}
	}
}


// The code create a cylinderical segments are somewhat similar to the one in CreateCylinder()
// The rationale behind writing a sepaate code is that
// - The cartridge maker needs multiple segments with different radii.
//   - Note that when radii are scaled later, the normals need to be recalculated as well.
// - In order to create UV texture coords, the cartridge maker needs cylinder segments whose vertices are duplicated at one vertex.
//   - CreateCylinder() can support this with some modifications, but the recalculations at duplicated vertices would be tricky.
// - The vertices are addd from bottom to top
void CartridgeMaker::AddSegments(
                     const vector< pair<float,float> >& diameter_and_height,
                     int num_sides,
                     bool create_top_polygons,
					 bool weld_top_rim_vertices,
                     bool create_bottom_polygons,
                     bool weld_bottom_rim_vertices,
                     vector<Vector3>& vecDestPos,
                     vector<Vector3>& vecDestNormal,
                     vector<TEXCOORD2>& vecDestTexUV,
					 vector< vector<int> >& vecDestPoly )
{
	Vector3 vUp = Vector3(0,1,0);

	if( diameter_and_height.size() < 2 )
		return;

	const int num_divisions = diameter_and_height.size() - 1;

	if( vecDestPos.size() != vecDestNormal.size() )
		return;

	const int vertex_index_offset = (int)vecDestPos.size();

	bool is_normal_const = false;
	Vector3 const_normal( Vector3(0,0,0) );
	if( diameter_and_height.size() == 2
	 && fabs(diameter_and_height[1].second - diameter_and_height[0].second) < 0.000001 )
	{
		// single segment && segment is horizontal
		is_normal_const = true;
		float radius_diff = diameter_and_height[1].first - diameter_and_height[0].first;
		if( radius_diff <= 0 )
			const_normal = Vector3(0,1,0);
		else
			const_normal = Vector3(0,-1,0);
	}

	// create vertices
	for( int i=0; i<num_divisions+1; i++ )
	{
		const float radius = diameter_and_height[i].first * 0.5f;
		const float height = diameter_and_height[i].second;
//		const Matrix34 pose = Matrix34( -vUp * height * ( (float)i / (float)num_divisions - 0.5f ), Matrix33Identity() );

		const float normal_angle = is_normal_const ? 0 : CalculateNormalAngle( diameter_and_height, i );
		const Vector3 base_normal = CalculateNormal( diameter_and_height, i );

//		const float circumfirence = 2.0f * (float)PI * radius;

		for( int j=0; j<num_sides+1; j++ )
		{
			const float angle = (float)(j%num_sides) / (float)num_sides * 2.0f * (float)PI;
			const Vector3 vDir = Matrix33RotationY( angle ) * Vector3(1,0,0);
			Vector3 vLocalPos = vDir * radius;
//			Vector3 vPos = pose * Vector3( vLocalPos.x, 0, vLocalPos.y );
			Vector3 vPos = Vector3( vLocalPos.x, height, vLocalPos.z );

//			LOG_PRINT( "cylinder vertex: " + to_string( vPos ) );

			vecDestPos.push_back( vPos );

			Vector3 normal
				= is_normal_const ?
				const_normal // vertical up/down
//				: Matrix33RotationY(angle) * Matrix33RotationZ(normal_angle) * Vector3(1,0,0);
				: Matrix33RotationY(angle) * base_normal;

			vecDestNormal.push_back( normal );

//			float u = circumfirence * ( angle / (2.0f * (float)PI) );
			float u = radius * angle;
			vecDestTexUV.push_back( TEXCOORD2( u, height ) * 10.0f );
		}
	}

	int bottom_center_vertex_index = 0;
	int bottom_rim_vertex_indices_offset = 0;
	int top_center_vertex_index    = 0;
	int top_rim_vertex_indices_offset = 0;
//	Vector3 top_center    = vUp * diameter_and_height.back().second;
	float bottom_y = vecDestPos[vertex_index_offset].y;
	Vector3 bottom_center = Vector3(0,bottom_y,0);//vUp * diameter_and_height.front().second;

	if( create_bottom_polygons )
	{
		/// Bottom polygons are added before all the rest of the wall polygons are created,
		/// so they are inserted to the front.

		bottom_center_vertex_index = (int)vecDestPos.size();

		// center vertex
		vecDestPos.push_back( bottom_center );

		Vector3 bottom_normal( Vector3(0,-1,0) );
		vecDestNormal.push_back( bottom_normal );

		if( weld_bottom_rim_vertices )
		{
			bottom_rim_vertex_indices_offset = vertex_index_offset;
		}
		else
		{
			// Duplicate the points on the rims (bottom)
			vector<Vector3> bottom_vertices;
			bottom_vertices.reserve( num_sides ); // reserve for the rims vertices

			// rim vertices
			bottom_vertices.insert(
				bottom_vertices.end(),
				vecDestPos.begin() + vertex_index_offset,
				vecDestPos.begin() + vertex_index_offset + num_sides
				);

			bottom_rim_vertex_indices_offset = (int)vecDestPos.size();

			// Append the duplicated vertices to the head of the destination vector
			vecDestPos.insert( vecDestPos.end(), bottom_vertices.begin(), bottom_vertices.end() );

			// Copy normals for the duplicated vertices (all the normals points down)
			vecDestNormal.insert( vecDestNormal.end(), num_sides, bottom_normal );

			vector<TEXCOORD2> bottom_uvs;
			TEXCOORD2 bottom_uv_offset = TEXCOORD2(1,1);
			float bottom_uv_scale = 0.125f;
			bottom_uvs.push_back( ( TEXCOORD2(0,0) + bottom_uv_offset ) * bottom_uv_scale );
			for( int i=0; i<num_sides; i++ )
			{
				const float angle = (float)i / (float)num_sides * 2.0f * (float)PI;
				const TEXCOORD2 t = TEXCOORD2( cos(angle), sin(angle) ) + bottom_uv_offset;
				bottom_uvs.push_back( t * bottom_uv_scale );
			}
			vecDestTexUV.insert( vecDestTexUV.begin(), bottom_uvs.begin(), bottom_uvs.end() );
		}
	}

	if( create_top_polygons )
	{
		top_center_vertex_index = (int)vecDestPos.size();
		const Vector3 top_center = Vector3( 0, vecDestPos.back().y, 0 );
		vecDestPos.push_back( top_center ); // center vertex

		if( weld_top_rim_vertices )
		{
			top_rim_vertex_indices_offset = vecDestPos.size() - num_sides - 1;

			// No need to add more vertices
		}
		else
		{
			// Duplicate the points on the rims (top)
			vector<Vector3> top_vertices;
			top_vertices.reserve( num_sides );

			// Copy the rim vertices to top_vertices (-1 to skip the center vertex we just added above).
			top_vertices.insert(
				top_vertices.end(),
				vecDestPos.end() - 1 - num_sides,
				vecDestPos.end() - 1 );

			top_rim_vertex_indices_offset = (int)vecDestPos.size();

			vecDestPos.insert( vecDestPos.end(), top_vertices.begin(), top_vertices.end() ); // append to the destination vector

			// Add normals (all the normals point up).
			Vector3 top_normal( Vector3(0,1,0) );
			vecDestNormal.insert( vecDestNormal.end(), num_sides + 1, top_normal );

			TEXCOORD2 top_uv_offset = TEXCOORD2(1,1);
			float top_uv_scale = 0.125f;
			vecDestTexUV.push_back( ( TEXCOORD2(0,0) + top_uv_offset ) * top_uv_scale );
			for( int i=0; i<num_sides; i++ )
			{
				const float angle = (float)i / (float)num_sides * 2.0f * (float)PI;
				const TEXCOORD2 t = TEXCOORD2( cos(angle), sin(angle) ) + top_uv_offset;
				vecDestTexUV.push_back( t * top_uv_scale );
			}
		}
	}

	// create polygon indices

	const int num_polygons
		= num_divisions * num_sides
		+ (create_top_polygons    ? num_sides : 0)
		+ (create_bottom_polygons ? num_sides : 0);

	vecDestPoly.reserve( vecDestPoly.size() + num_polygons );

	// side (quads)
	int side_vertex_index_offset = vertex_index_offset;

	for( int i=0; i<num_divisions; i++ )
	{
		// polygons on the enclosing side of the cylinder
		int lower_start = (num_sides+1) * i;
		int upper_start = (num_sides+1) * (i+1);
		for( int j=0; j<num_sides; j++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 4 );
			vecDestPoly.back()[0] = side_vertex_index_offset + upper_start  + j + 1;
			vecDestPoly.back()[1] = side_vertex_index_offset + upper_start  + j;
			vecDestPoly.back()[2] = side_vertex_index_offset + lower_start  + j;
			vecDestPoly.back()[3] = side_vertex_index_offset + lower_start  + j + 1;
		}
	}

	// top (triangles)
	if( create_top_polygons )
	{
		for( int i=0; i<num_sides; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = top_center_vertex_index;
			vecDestPoly.back()[1] = top_rim_vertex_indices_offset + i;
			vecDestPoly.back()[2] = top_rim_vertex_indices_offset + (i+1) % num_sides;
		}
	}

	// bottom (triangles)
	if( create_bottom_polygons )
	{
		for( int i=0; i<num_sides; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = bottom_center_vertex_index;
			vecDestPoly.back()[1] = bottom_rim_vertex_indices_offset + (i+1) % num_sides;
			vecDestPoly.back()[2] = bottom_rim_vertex_indices_offset + i;
		}
	}
}


Result::Name CartridgeMaker::MakeBullet(
	const BulletDesc& bullet_desc,
	unsigned int num_sides,
	float case_top_height,
	vector<Vector3>& points,
	vector<Vector3>& normals,
	vector<TEXCOORD2>& tex_uvs,
	vector< vector<int> >& polygons
	)
{
//	if( bullet_desc.length < 0.001f
//	 || bullet_desc.diameter < 0.001f )
//	{
//		return Result::INVALID_ARGS;
//	}

	int num_segments = 0;
	int i =0;
	while( i<BulletDesc::NUM_MAX_BULLET_CURVE_PIONTS )
	{
		if( bullet_desc.bullet_curve_points[i].diameter < 0.001f )
			break;
		else
			i++;
	}

//	float case_top_height = 0.05f;

	vector< pair<float,float> > d_and_h_pairs;

	const int num_control_points = (bullet_desc.num_control_points < BulletDesc::NUM_MAX_BULLET_SLICES) ?
		bullet_desc.num_control_points : BulletDesc::NUM_MAX_BULLET_SLICES;

	const float bullet_length
		= bullet_desc.bullet_slice_control_points[num_control_points].position.y
		- bullet_desc.bullet_slice_control_points[0].position.y;
	float height_offset = case_top_height - (bullet_length - bullet_desc.exposed_length);
	for( int i=0; i<num_control_points-1; i++ )
	{
//		if( bullet_desc.create_model_only_for_exposed_part
//		 &&  )
//		{
//		}

		const BulletSliceControlPoint& prev_cp = (i==0) ? bullet_desc.bullet_slice_control_points[0] : bullet_desc.bullet_slice_control_points[i-1];
		const BulletSliceControlPoint& cp      = bullet_desc.bullet_slice_control_points[i];
		const BulletSliceControlPoint& next_cp = bullet_desc.bullet_slice_control_points[i+1];
		const BulletSliceControlPoint& cp_after_next = (i<num_control_points-2) ? bullet_desc.bullet_slice_control_points[i+2] : bullet_desc.bullet_slice_control_points[num_control_points-1];
//		if( cp.position == Vector2(0,0) )
//			break;
		int num_sub_segs_per_segment = 4;
		for( int j=0; j<num_sub_segs_per_segment; j++ )
		{
			const Vector2& p0 = prev_cp.position;
			const Vector2& p1 = cp.position;
			const Vector2& p2 = next_cp.position;
			const Vector2& p3 = cp_after_next.position;
			float frac = (float)j / (float)num_sub_segs_per_segment;
			Vector2 position = InterpolateWithTCBSpline(
				frac, p0, p1, p2, p3, cp.tension, cp.continuity, cp.bias );

			d_and_h_pairs.push_back( pair<float,float>( position.x * 2.0f, height_offset + position.y ) );
		}
	}

	AddSegments( d_and_h_pairs,
		num_sides,
		true,  // create_top_polygons
		true,  // weld_top_rim_vertices
		false, // create_bottom_polygons
		false, // weld_bottom_rim_vertices
		points,
		normals,
		tex_uvs,
		polygons
		);

	return Result::SUCCESS;
}


pair<float,float> CalculateInetrpolatedDiameterAndHeight( const pair<float,float> *src, int i0, int i1, float dist_from_i1 )
{
	float diameter_diff = (src[i1].first  - src[i0].first);
	float radius_diff = diameter_diff * 0.5f;
	float length = (src[i1].second - src[i0].second);
	float long_edge = sqrt( radius_diff*radius_diff + length*length );

	float f = dist_from_i1 / long_edge;
	float radius = src[i1].first + radius_diff * 0.5f * f;
	float height = src[i1].second - length * f;

	return pair<float,float>( radius * 2.0f, height );
}


Result::Name CartridgeMaker::AddPrimerAndPrimerWell( const CaseDesc& src_desc, unsigned int num_sides, std::vector<Vector3>& points, std::vector<Vector3>& normals, vector<TEXCOORD2>& tex_uvs, vector< vector<int> >& polygons )
{
	vector< pair<float,float> > diameter_and_height_pairs;
	diameter_and_height_pairs.reserve( 6 );

	diameter_and_height_pairs.resize( 0 );

	float bottom_plate_diameter = src_desc.case_slices[0].diameter;
	float primer_depth = 2.0f * 0.001f; // 2mm
	float primer_diameter = 4.5f * 0.001f; // 4.5mm
	vector<Vector2> dest_points;
	// create polygons by descending
	const Vector2 prev_point    = Vector2(primer_diameter       * 0.5f, primer_depth);
	const Vector2 current_point = Vector2(primer_diameter       * 0.5f, 0.0f);
	const Vector2 next_point    = Vector2(bottom_plate_diameter * 0.5f, 0.0f);
	// create polygons by ascending
//	const Vector2 prev_point    = Vector2(bottom_plate_diameter * 0.5f, 0.0f);
//	const Vector2 current_point = Vector2(primer_diameter       * 0.5f, 0.0f);
//	const Vector2 next_point    = Vector2(primer_diameter       * 0.5f, primer_depth);
	uint num_curve_segments = 3;
//	CalculatePointsAtCurvedSlice( current_point, prev_point, next_point, 0.0005f, num_curve_segments, dest_points );

	diameter_and_height_pairs.push_back( pair<float,float>(primer_diameter,primer_depth) ); // descend
//	diameter_and_height_pairs.push_back( pair<float,float>(bottom_plate_diameter,0.0f) ); // ascend

	CalculateDiametersAndHeightsAtCurvedSlice(
		current_point,
		prev_point,
		next_point,
		0.0005f,
		num_curve_segments,
		diameter_and_height_pairs
		);

	diameter_and_height_pairs.push_back( pair<float,float>(bottom_plate_diameter,0.0f) ); // descend
//	diameter_and_height_pairs.push_back( pair<float,float>(primer_diameter,primer_depth) ); // ascend

	AddSegments(
		diameter_and_height_pairs,
		num_sides,
		false, // create_top_polygons,
		false,
		false, // create_bottom_polygons,
		false,
		points,
		normals,
		tex_uvs,
		polygons
		);

	diameter_and_height_pairs.resize( 0 );

	// Primer

	// Go up from the bottom
	dest_points.resize( 0 );
	const Vector2 primer_prev_point    = Vector2(primer_diameter * 0.5f * 0.8f, 0.0f);
	const Vector2 primer_current_point = Vector2(primer_diameter * 0.5f,        0.0f);
	const Vector2 primer_next_point    = Vector2(primer_diameter * 0.5f,        primer_depth);

	diameter_and_height_pairs.push_back( pair<float,float>(primer_diameter * 0.7f, 0.0f) );

	CalculateDiametersAndHeightsAtCurvedSlice(
		primer_current_point,
		primer_prev_point,
		primer_next_point,
		0.0005f,
		num_curve_segments,
		diameter_and_height_pairs
		);

	AddSegments(
		diameter_and_height_pairs,
		num_sides,
		false, // create_top_polygons,
		false, // weld_top_rim_vertices
		true,  // create_bottom_polygons,
		true,  // weld_bottom_rim_vertices
		points,
		normals,
		tex_uvs,
		polygons
		);

	diameter_and_height_pairs.resize( 0 );

	return Result::SUCCESS;
}


Result::Name CartridgeMaker::MakeCaseInternals(
	const CaseDesc& src_desc,
	unsigned int num_sides,
	std::vector<Vector3>& points,
	std::vector<Vector3>& normals,
	vector<TEXCOORD2>& tex_uvs,
	vector< vector<int> >& polygons )
{
	vector< pair<float,float> > diameter_and_height_pairs;

	if( src_desc.drill_style == CaseDesc::DS_SIMPLIFIED )
	{
		if( 0 <= src_desc.top_outer_slice_index )
		{
			diameter_and_height_pairs.resize( 0 );

			float wall_thickness = 0.0004f;
//			float wall_thickness = 0.001;
			const CaseSlice& top_outer_slice = src_desc.case_slices[src_desc.top_outer_slice_index];
			pair<float,float> top_outer( top_outer_slice.diameter, top_outer_slice.height );
			diameter_and_height_pairs.push_back( top_outer );

			pair<float,float> top_inner( top_outer_slice.diameter - wall_thickness * 2.0f, top_outer_slice.height );
			diameter_and_height_pairs.push_back( top_inner );

			AddSegments(
				diameter_and_height_pairs,
				num_sides,
				false, // create_top_polygons,
				false, // weld_top_rim_vertices
				false, // create_bottom_polygons,
				false, // weld_bottom_rim_vertices
				points,
				normals,
				tex_uvs,
				polygons
				);

			diameter_and_height_pairs.resize( 0 );
			diameter_and_height_pairs.push_back( top_inner );
			diameter_and_height_pairs.push_back( pair<float,float>(top_outer_slice.diameter - wall_thickness * 2.0f, 0.006f) );

			AddSegments(
				diameter_and_height_pairs,
				num_sides,
				true,  // create_top_polygons,
				false, // weld_top_rim_vertices
				false, // create_bottom_polygons,
				false, // weld_bottom_rim_vertices
				points,
				normals,
				tex_uvs,
				polygons
				);
		}
	}

	return Result::SUCCESS;
}


Result::Name CartridgeMaker::MakeCase( const CaseDesc& src_desc, unsigned int num_sides, std::vector<Vector3>& points, std::vector<Vector3>& normals, vector<TEXCOORD2>& tex_uvs, vector< vector<int> >& polygons )
{
//	vector<Vector3> points, normals;
//	vector< vector<int> > polygons;

	float radii[2] = { 1.0f, 1.0f };
//	int num_sides = 12;

//	src_desc.case_segments;
	int current_segment_index = 0;
	vector< pair<float,float> > diameter_and_height_pairs;
	diameter_and_height_pairs.reserve( 6 );

	const bool create_polygon_mesh_for_primer
		= (src_desc.primer_model == CaseDesc::PM_POLYGON_MESH) && IsCenterfireCartridge(src_desc.caliber);

	if( create_polygon_mesh_for_primer )
	{
		AddPrimerAndPrimerWell(src_desc,num_sides,points,normals,tex_uvs,polygons);
	}

//	float height = 0;
	while(1)
	{
		// Each iteration of this loop represent one continuous segment
		// Not necessarily a segment of a case.

		int num_segments = 1;
		int start_segment_index = current_segment_index;
		int end_segment_index = current_segment_index;

		if( start_segment_index == CaseDesc::MAX_NUM_CASE_SLICES - 1 )
			break;

		if( fabs(src_desc.case_slices[start_segment_index+1].diameter) < 0.000001
		 && fabs(src_desc.case_slices[start_segment_index+1].height)   < 0.000001 )
		{
			break;
		}

		diameter_and_height_pairs.resize( 0 );

		const CaseSlice& start_slice = src_desc.case_slices[start_segment_index];
		diameter_and_height_pairs.push_back( pair<float,float>( start_slice.diameter,    start_slice.height ) );

		for( current_segment_index = start_segment_index + 1;
			 current_segment_index < CaseDesc::MAX_NUM_CASE_SLICES - 1;
			 current_segment_index++ )
		{
			const CaseSlice& current_slice = src_desc.case_slices[current_segment_index];
			if( current_slice.curvature_radius == 0 )
			{
				num_segments += 1;
				end_segment_index = current_segment_index;
				diameter_and_height_pairs.push_back( pair<float,float>( current_slice.diameter, current_slice.height ) );
				break; // Exit the loop without incrementing current_segment_index 
			}
			else
			{
				// Create rounded as a continuous cylinder

				if( current_segment_index == 0 || current_segment_index == CaseDesc::MAX_NUM_CASE_SLICES - 1 )
					break;

				vector<Vector2> points_at_curved_slice;

				const CaseSlice& prev_slice = src_desc.case_slices[current_segment_index-1];
				const CaseSlice& next_slice = src_desc.case_slices[current_segment_index+1];
				const Vector2 current_point = Vector2(current_slice.diameter*0.5f, current_slice.height);
				const Vector2 prev_point    = Vector2(prev_slice.diameter*0.5f, prev_slice.height);
				const Vector2 next_point    = Vector2(next_slice.diameter*0.5f, next_slice.height);

				unsigned int num_curve_segments = 3;
				CalculatePointsAtCurvedSlice( current_point, prev_point, next_point, current_slice.curvature_radius, num_curve_segments, points_at_curved_slice );

				for( size_t i=0; i<points_at_curved_slice.size(); i++ )
				{
					Vector2 point = points_at_curved_slice[i];
					diameter_and_height_pairs.push_back( pair<float,float>(point.x*2.0f,point.y) );
				}

				// Do not break the loop because the segments are continuous.
			}
		}
		
		int point_offset = (int)points.size();

		// Close the top if
		// - reached the bottom of the inside,
		// - or reached the top and the client code has specified to close the top.
//		bool create_top_polygons
//			= (end_segment_index == src_desc.num_case_slices-1)
//			|| (end_segment_index == src_desc.top_outer_slice_index && src_desc.create_internal_polygons == false);

		bool create_top_polygons
			= (end_segment_index == src_desc.top_outer_slice_index && src_desc.drill_style == CaseDesc::DS_CLOSED);

		bool create_bottom_polygons = (start_segment_index == 0 && !create_polygon_mesh_for_primer) ? true : false;

		AddSegments(
			diameter_and_height_pairs,
			num_sides,
			create_top_polygons,
			false, // weld_top_rim_vertices
			create_bottom_polygons,
			false, // weld_bottom_rim_vertices
			points,
			normals,
			tex_uvs,
			polygons
			);

		start_segment_index = end_segment_index;
	}

	MakeCaseInternals( src_desc, num_sides, points, normals, tex_uvs, polygons );

	CorrectNormals( points, normals, polygons );

	return Result::SUCCESS;
}


Result::Name CartridgeMaker::Make(
		const CartridgeDesc& src_desc,
		unsigned int num_sides,
		std::vector<Vector3>& points,
		std::vector<Vector3>& normals,
		std::vector<TEXCOORD2>& tex_uvs,
		std::vector< std::vector<int> >& polygons
		)
{
	Result::Name case_res   = MakeCase( src_desc.case_desc, num_sides, points, normals, tex_uvs, polygons );

	if( case_res != Result::SUCCESS )
		return case_res;

	Result::Name bullet_res = MakeBullet(
		src_desc.bullet_desc, num_sides, src_desc.case_desc.GetTopHeight(),
		points, normals, tex_uvs, polygons
		);

	return bullet_res;
}


} // firearm


} // amorphous
