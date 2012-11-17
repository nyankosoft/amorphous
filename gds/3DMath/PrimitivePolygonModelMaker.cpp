#include <gds/3DMath/PrimitivePolygonModelMaker.hpp>
#include <gds/3DMath/Matrix23.hpp>
#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics/3DGameMath.hpp>
//#include <gds/Support/Log/DefaultLog.hpp>
#include <gds/Support/Vec3_StringAux.hpp>

using namespace std;


// Place the created cylinder on the horizontal plane (= plane perpendicular to y-axis)
// Vertices on the side of the cylinder are created before the ones on top & bottom.
// - Rationale: top / bottom polygons are optional
// Vertices on the side of the cylinder are created from top to bottom.
void CreateCylinder( float height, const float *pafRadius,
                     int num_segments, int num_divisions,
                     bool create_top_polygons,
                     PolygonModelStyle::Name top_style,
                     bool create_bottom_polygons,
                     PolygonModelStyle::Name bottom_style,
                     vector<Vector3>& vecDestPos,
                     vector<Vector3>& vecDestNormal,
					 vector< vector<int> >& vecDestPoly )
{
	Vector3 vUp = Vector3(0,1,0);//vDirFromBottomToTop = Vec3GetNormalized( vCore );
	const float afRadius[2] = { pafRadius[0], pafRadius[1] };
	vector<Vector3> vecNormal;

	if( vecDestPos.size() != vecDestNormal.size() )
		return;

	if( num_segments < 3 || num_divisions < 1 )
		return;

	int vertex_index_offset = (int)vecDestPos.size();

	// create vertices
	for( int i=0; i<num_divisions+1; i++ )
	{
		const Matrix34 pose = Matrix34( -vUp * height * ( (float)i / (float)num_divisions - 0.5f ), Matrix33Identity() );

		for( int j=0; j<num_segments; j++ )
		{
			const float angle = (float)j / (float)num_segments * 2.0f * (float)PI;
			Vector2 vLocalPos = Matrix22Rotation( angle ) * Vector2(1,0);//vHorizAxis2D;
			vLocalPos.x *= afRadius[0];
			vLocalPos.y *= afRadius[1];
//			Vector3 vPos = pose * Vector3( vLocalPos.x, vLocalPos.y, 0 );
			Vector3 vPos = pose * Vector3( vLocalPos.x, 0, vLocalPos.y );

//			LOG_PRINT( "cylinder vertex: " + to_string( vPos ) );

			vecDestPos.push_back( vPos );

			vecNormal.push_back( Vec3GetNormalized( vPos - pose.vPosition ) );
		}
	}

	int top_center_vertex_index    = 0;
	int bottom_center_vertex_index = 0;
	Vector3 top_center    =  vUp * height * 0.5f;
	Vector3 bottom_center = -vUp * height * 0.5f;

	if( create_top_polygons )
	{
		top_center_vertex_index    = vecDestPos.size();
		vecDestPos.push_back( top_center );
		if( top_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
		{
			// Duplicate the points on the rims (top)
			vecDestPos.insert( vecDestPos.end(), vecDestPos.begin(), vecDestPos.begin() + num_segments );
		}
//		else // i.e. style == PolygonModelStyle::EDGE_VERTICES_WELDED
			// No need to duplicate points on the top and bottom rims
	}

	if( create_bottom_polygons )
	{
		bottom_center_vertex_index = vecDestPos.size();
		vecDestPos.push_back( bottom_center );
		if( bottom_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
		{
			// Duplicate the points on the rims (bottom)
			vector<Vector3>::iterator start = vecDestPos.begin() + num_segments * num_divisions;
			vecDestPos.insert( vecDestPos.end(), start, start + num_segments );
		}
	}

	// Add normals

	// side vertices
	vector<Vector3> rim_normals;
	rim_normals.resize( num_segments );
	for( int i=0; i<num_segments; i++ )
	{
		Vector3 pos = vecDestPos[i];
		rim_normals[i] = Vec3GetNormalized( Vector3( pos.x, 0, pos.z ) );
	}

	for( int i=0; i<num_divisions+1; i++ )
		vecDestNormal.insert( vecDestNormal.end(), rim_normals.begin(), rim_normals.end() );

	if( create_top_polygons )
	{
		if( top_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
			vecDestNormal.insert( vecDestNormal.end(), num_segments+1,  vUp ); // top
		else
			vecDestNormal.push_back(  vUp );
	}

	if( create_bottom_polygons )
	{
		if( bottom_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
			vecDestNormal.insert( vecDestNormal.end(), num_segments+1, -vUp ); // bottom
		else
			vecDestNormal.push_back( -vUp );
	}

	// create polygon indices

	const int num_polygons
		= num_divisions * num_segments
		+ (create_top_polygons    ? num_segments : 0)
		+ (create_bottom_polygons ? num_segments : 0);

	vecDestPoly.reserve( vecDestPoly.size() + num_polygons );

	// side (quads)
	for( int i=0; i<num_divisions; i++ )
	{
		// polygons on the enclosing side of the cylinder
		int upper_start = num_segments * i;
		int lower_start = num_segments * (i+1);
		for( int j=0; j<num_segments; j++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 4 );
			vecDestPoly.back()[0] = vertex_index_offset + upper_start  +  j;
			vecDestPoly.back()[1] = vertex_index_offset + upper_start  + (j + 1) % num_segments;
			vecDestPoly.back()[2] = vertex_index_offset + lower_start  + (j + 1) % num_segments;
			vecDestPoly.back()[3] = vertex_index_offset + lower_start  +  j;
		}
	}

	// top (triangles)
	if( create_top_polygons )
	{
		int rim_vertex_offset  = (top_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED) ? top_center_vertex_index + 1 : 0;
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = vertex_index_offset + top_center_vertex_index;
			vecDestPoly.back()[1] = vertex_index_offset + rim_vertex_offset + (i+1) % num_segments;
			vecDestPoly.back()[2] = vertex_index_offset + rim_vertex_offset + i;
		}
	}

	// bottom (triangles)
	if( create_bottom_polygons )
	{
		int rim_vertex_offset  = (bottom_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED) ? bottom_center_vertex_index + 1 : num_segments * num_divisions;
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = vertex_index_offset + bottom_center_vertex_index;
			vecDestPoly.back()[1] = vertex_index_offset + rim_vertex_offset + i;
			vecDestPoly.back()[2] = vertex_index_offset + rim_vertex_offset + (i+1) % num_segments;
		}
	}
}


void CreateCylinder( const Vector3& vPos0_Top, const Vector3& vPos1_Bottom, const float *pafRadius,
                     int num_segments, int num_divisions,
                     bool create_top_polygons,
                     PolygonModelStyle::Name top_style,
                     bool create_bottom_polygons,
                     PolygonModelStyle::Name bottom_style,
                     vector<Vector3>& vecDestPos,
                     vector<Vector3>& vecDestNormal,
					 vector< vector<int> >& vecDestPoly )
{
	Vector3 vCore = vPos0_Top - vPos1_Bottom;
	Vector3 vDirFromBottomToTop = Vec3GetNormalized( vCore );
//	const Matrix33 matOrient = CreateOrientFromFwdDir( vDirFromBottomToTop );
	const Matrix33 matOrient = CreateOrientFromNormal( vDirFromBottomToTop );
	const Matrix34 pose[2] = { Matrix34( vPos0_Top, matOrient ), Matrix34( vPos1_Bottom, matOrient ) };
//	const float afRadius[2] = { pafRadius[0], pafRadius[1] };
	vector<Vector3> vecNormal;

	::CreateCylinder( Vec3Length(vCore), pafRadius, num_segments, num_divisions,
		create_top_polygons, top_style, create_bottom_polygons, bottom_style,
		vecDestPos, vecDestNormal, vecDestPoly );

	const int num_verts = (int)vecDestPos.size();
	for( int i=0; i<num_verts; i++ )
	{
		vecDestPos[i] = pose[1] * vecDestPos[i];
		vecDestNormal[i] = matOrient * vecDestNormal[i];
	}


//	LOG_PRINT( "vDirFromBottomToTop: " + to_string( vDirFromBottomToTop ) );
}


void CreatePipe( float length, const float outer_radius, const float inner_radius,
                 int num_segments, int num_divisions,
                 bool create_top_polygons,
                 PolygonModelStyle::Name top_style,
                 bool create_bottom_polygons,
                 PolygonModelStyle::Name bottom_style,
                 vector<Vector3>& vecDestPos,
                 vector<Vector3>& vecDestNormal,
                 vector< vector<int> >& vecDestPoly )
{
	if( outer_radius <= inner_radius )
		return;

	if( num_segments < 3 || num_divisions < 1 )
		return;

	const int num_polygons_to_create
		= num_segments * num_divisions * 2
		+ create_top_polygons ? num_segments : 0
		+ create_bottom_polygons ? num_segments : 0;

	vecDestPoly.reserve( vecDestPoly.size() + num_polygons_to_create );

	const int num_vertices_to_create
		= num_segments * (num_divisions+1) * 2
		+ (top_style    == PolygonModelStyle::EDGE_VERTICES_UNWELDED) ? num_segments * 2 : 0
		+ (bottom_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED) ? num_segments * 2 : 0;

	vecDestPos.reserve(    vecDestPos.size()    + num_vertices_to_create );
	vecDestNormal.reserve( vecDestNormal.size() + num_vertices_to_create );

	int vertex_index_offset = (int)vecDestPos.size();

	const float height = length;

	const float outer_radii[2] = { outer_radius, outer_radius };

	CreateCylinder( height, outer_radii,
                     num_segments, num_divisions,
                     false, // create_top_polygons
					 PolygonModelStyle::EDGE_VERTICES_UNWELDED,
                     false, // create_bottom_polygons,
                     PolygonModelStyle::EDGE_VERTICES_UNWELDED,
                     vecDestPos,
                     vecDestNormal,
					 vecDestPoly );

	int inner_vert_index_offset = (int)vecDestPos.size();
	int inner_polygon_index_offset = (int)vecDestPoly.size();

	const float inner_radii[2] = { inner_radius, inner_radius };

	CreateCylinder( height, inner_radii,
                     num_segments, num_divisions,
                     false, // create_top_polygons
					 PolygonModelStyle::EDGE_VERTICES_UNWELDED,
                     false, // create_bottom_polygons,
                     PolygonModelStyle::EDGE_VERTICES_UNWELDED,
                     vecDestPos,
                     vecDestNormal,
					 vecDestPoly );

	int outer_top_vert_index = 0;
	int outer_bottom_vert_index = 0;
	int inner_top_vert_index = 0;
	int inner_bottom_vert_index = 0;

	if( create_top_polygons )
	{
		if( top_style == PolygonModelStyle::EDGE_VERTICES_WELDED )
		{
			outer_top_vert_index = vertex_index_offset;
			inner_top_vert_index = vertex_index_offset + num_segments * num_divisions;
		}
		else if( top_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
		{
			// duplicate top & bottom vertices

			// outer top positions
			outer_top_vert_index = (int)vecDestPos.size();
			vector<Vector3> position_buffer;
			position_buffer.insert( position_buffer.begin(), vecDestPos.begin(), vecDestPos.begin() + num_segments );
			vecDestPos.insert( vecDestPos.end(), position_buffer.begin(), position_buffer.end() );

			// inner top positions
			inner_top_vert_index = (int)vecDestPos.size();
			position_buffer.resize( 0 );
			position_buffer.insert(
				position_buffer.begin(),
				vecDestPos.begin() + inner_vert_index_offset,
				vecDestPos.begin() + inner_vert_index_offset + num_segments );
			vecDestPos.insert( vecDestPos.end(), position_buffer.begin(), position_buffer.end() );

			// normals
			vecDestNormal.insert( vecDestNormal.end(), num_segments * 2, Vector3(0,1,0) ); // up
		}
	}

	// Flip the inner side normals
	for( int i=inner_vert_index_offset; i<inner_vert_index_offset + num_segments * (num_divisions+1); i++ )
	{
		vecDestNormal[i] *= -1.0f;
	}

	// Flip the inner side polygons
	int i0=0, i1=0;
	for( int i=inner_polygon_index_offset; i<(int)vecDestPoly.size(); i++ )
	{
		vector<int>& polygon = vecDestPoly[i];
		i0 = polygon[0];
		i1 = polygon[1];
		polygon[0] = polygon[3];
		polygon[1] = polygon[2];
		polygon[3] = i0;
		polygon[2] = i1;
	}

	if( create_bottom_polygons )
	{
		if( top_style == PolygonModelStyle::EDGE_VERTICES_WELDED )
		{
			outer_bottom_vert_index = inner_vert_index_offset;
			inner_bottom_vert_index = inner_vert_index_offset + num_segments * num_divisions;
		}
		else if( top_style == PolygonModelStyle::EDGE_VERTICES_UNWELDED )
		{
			// duplicate bottom vertices

			// outer bottom positions
			outer_bottom_vert_index = (int)vecDestPos.size();
			vector<Vector3> position_buffer;
			position_buffer.insert(
				position_buffer.begin(),
				vecDestPos.begin() + num_segments * num_divisions,
				vecDestPos.begin() + num_segments * (num_divisions+1) );
			vecDestPos.insert( vecDestPos.end(), position_buffer.begin(), position_buffer.end() );

			// positions
			inner_bottom_vert_index = (int)vecDestPos.size();
			position_buffer.resize( 0 );
			position_buffer.insert( position_buffer.begin(),
				vecDestPos.begin() + inner_vert_index_offset + num_segments * num_divisions,
				vecDestPos.begin() + inner_vert_index_offset + num_segments * (num_divisions+1) );
			vecDestPos.insert( vecDestPos.end(), position_buffer.begin(), position_buffer.end() );

			// normals
			vecDestNormal.insert( vecDestNormal.end(), num_segments * 2, Vector3(0,-1,0) ); // down
		}
	}

	if( create_top_polygons )
	{
		// Create polygons at the top
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 4 );
			vecDestPoly.back()[0] = outer_top_vert_index + (i + 1) % num_segments;
			vecDestPoly.back()[1] = outer_top_vert_index + i;
			vecDestPoly.back()[2] = inner_top_vert_index + i;
			vecDestPoly.back()[3] = inner_top_vert_index + (i + 1) % num_segments;
		}
	}

	if( create_bottom_polygons )
	{
		// Create polygons at the bottom
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 4 );
			vecDestPoly.back()[0] = inner_bottom_vert_index + (i + 1) % num_segments;
			vecDestPoly.back()[1] = inner_bottom_vert_index + i;
			vecDestPoly.back()[2] = outer_bottom_vert_index + i;
			vecDestPoly.back()[3] = outer_bottom_vert_index + (i + 1) % num_segments;
		}
	}
}
