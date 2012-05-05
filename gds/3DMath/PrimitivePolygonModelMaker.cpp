#include <gds/3DMath/PrimitivePolygonModelMaker.hpp>
#include <gds/3DMath/Matrix23.hpp>
#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics/3DGameMath.hpp>
#include <gds/Support/Log/DefaultLog.hpp>
#include <gds/Support/Vec3_StringAux.hpp>

using namespace std;


// Place the created cylinder on the horizontal plane (= plane perpendicular to y-axis)
// Vertices on the side of the cylinder are created before the ones on top & bottom
// - Rationale: top / bottom polygons are optional
void CreateCylinder( float height, const float *pafRadius, int num_segments,
                     PrimitiveModelStyle::Name style,
                     vector<Vector3>& vecDestPos,
                     vector<Vector3>& vecDestNormal,
					 vector< vector<int> >& vecDestPoly )
{
	bool create_top_polygons    = true;
	bool create_bottom_polygons = true;
	Vector3 vUp = Vector3(0,1,0);//vDirFromBottomToTop = Vec3GetNormalized( vCore );
	const float afRadius[2] = { pafRadius[0], pafRadius[1] };
	vector<Vector3> vecNormal;

	// create vertices
	int num_divisions = 1;
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
		if( style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED )
		{
			// Duplicate the points on the rims (top)
			vecDestPos.insert( vecDestPos.end(), vecDestPos.begin(), vecDestPos.begin() + num_segments );
		}
//		else // i.e. style == PrimitiveModelStyle::EDGE_VERTICES_WELDED
			// No need to duplicate points on the top and bottom rims
	}

	if( create_bottom_polygons )
	{
		bottom_center_vertex_index = vecDestPos.size();
		vecDestPos.push_back( bottom_center );
		if( style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED )
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

	vecDestNormal.resize( 0 );
	for( int i=0; i<num_divisions+1; i++ )
		vecDestNormal.insert( vecDestNormal.end(), rim_normals.begin(), rim_normals.end() );

	if( create_top_polygons )
	{
		if( style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED )
			vecDestNormal.insert( vecDestNormal.end(), num_segments+1,  vUp ); // top
		else
			vecDestNormal.push_back(  vUp );
	}

	if( create_bottom_polygons )
	{
		if( style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED )
			vecDestNormal.insert( vecDestNormal.end(), num_segments+1, -vUp ); // bottom
		else
			vecDestNormal.push_back( -vUp );
	}

	// create polygon indices

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
			vecDestPoly.back()[0] = upper_start  +  j;
			vecDestPoly.back()[1] = upper_start  + (j + 1) % num_segments;
			vecDestPoly.back()[2] = lower_start  + (j + 1) % num_segments;
			vecDestPoly.back()[3] = lower_start  +  j;
		}
	}

	// top (triangles)
	if( create_top_polygons )
	{
		int rim_vertex_offset  = (style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED) ? top_center_vertex_index + 1 : 0;
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = top_center_vertex_index;
			vecDestPoly.back()[1] = rim_vertex_offset + (i+1) % num_segments;
			vecDestPoly.back()[2] = rim_vertex_offset + i;
		}
	}

	// bottom (triangles)
	if( create_bottom_polygons )
	{
		int rim_vertex_offset  = (style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED) ? bottom_center_vertex_index + 1 : num_segments * num_divisions;
		for( int i=0; i<num_segments; i++ )
		{
			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().resize( 3 );
			vecDestPoly.back()[0] = bottom_center_vertex_index;
			vecDestPoly.back()[1] = rim_vertex_offset + i;
			vecDestPoly.back()[2] = rim_vertex_offset + (i+1) % num_segments;
		}
	}
}


void CreateCylinder( const Vector3& vPos0_Top, const Vector3& vPos1_Bottom, const float *pafRadius, int num_segments,
                     PrimitiveModelStyle::Name style,
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

	::CreateCylinder( Vec3Length(vCore), pafRadius, num_segments, style,
		vecDestPos, vecDestNormal, vecDestPoly );

	const int num_verts = (int)vecDestPos.size();
	for( int i=0; i<num_verts; i++ )
	{
		vecDestPos[i] = pose[1] * vecDestPos[i];
		vecDestNormal[i] = matOrient * vecDestNormal[i];
	}


//	LOG_PRINT( "vDirFromBottomToTop: " + to_string( vDirFromBottomToTop ) );
}
