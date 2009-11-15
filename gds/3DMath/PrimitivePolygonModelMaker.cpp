#include <gds/3DMath/PrimitivePolygonModelMaker.hpp>
#include <gds/3DMath/Matrix23.hpp>
#include <gds/3DMath/Matrix34.hpp>
#include <gds/Graphics/3DGameMath.hpp>
#include <gds/Support/Log/DefaultLog.hpp>
#include <gds/Support/Vec3_StringAux.hpp>

using namespace std;


// Place the created cylinder on the horizontal plane
void CreateCylinder( float height, const float *pafRadius, int num_segments,
                     PrimitiveModelStyle::Name style,
                     vector<Vector3>& vecDestPos,
                     vector<Vector3>& vecDestNormal,
					 vector< vector<int> >& vecDestPoly )
{

//	Vector3 vCore = vPos0_Top - vPos1_Bottom;
	Vector3 vUp = Vector3(0,1,0);//vDirFromBottomToTop = Vec3GetNormalized( vCore );
//	const Matrix33 matOrient = CreateOrientFromFwdDir( vDirFromBottomToTop );
//	const Matrix34 pose[2] = { Matrix34( vPos0_Top, matOrient ), Matrix34( vPos1_Bottom, matOrient ) };
	const Matrix34 pose[2] = { Matrix34( vUp * height, Matrix33Identity() ), Matrix34Identity() };
	const float afRadius[2] = { pafRadius[0], pafRadius[1] };
	vector<Vector3> vecNormal;

//	LOG_PRINT( "vDirFromBottomToTop: " + to_string( vDirFromBottomToTop ) );

//	const Vector3 vHorizAxis = matOrient.GetColumn(0);
//	const Vector3 vVertAxis  = matOrient.GetColumn(1);
//	const Vector2 vHorizAxis2D = Vector2(vHorizAxis.x,vHorizAxis.y);

	// create vertices
	for( int i=0; i<2; i++ )
	{
		vecDestPos.push_back( pose[i].vPosition );

		for( int j=0; j<num_segments; j++ )
		{
			const float angle = (float)j / (float)num_segments * 2.0f * (float)PI;
			Vector2 vLocalPos = Matrix22Rotation( angle ) * Vector2(1,0);//vHorizAxis2D;
			vLocalPos.x *= afRadius[0];
			vLocalPos.y *= afRadius[1];
//			Vector3 vPos = pose[i] * Vector3( vLocalPos.x, vLocalPos.y, 0 );
			Vector3 vPos = pose[i] * Vector3( vLocalPos.x, 0, vLocalPos.y );

//			LOG_PRINT( "cylinder vertex: " + to_string( vPos ) );

			vecDestPos.push_back( vPos );

			vecNormal.push_back( Vec3GetNormalized( vPos - pose[i].vPosition ) );
		}
	}

	if( style == PrimitiveModelStyle::EDGE_VERTICES_WELDED )
	{
		// No need to duplicate points on the top and bottom rims
	}
	else if( style == PrimitiveModelStyle::EDGE_VERTICES_UNWELDED )
	{
		// Duplicate the points on the rims (top & bottom)
		for( int i=0; i<num_segments; i++ )
			vecDestPos.push_back( vecDestPos[ 1 + i ] );
		
		for( int i=0; i<num_segments; i++ )
			vecDestPos.push_back( vecDestPos[ num_segments + 2 + i ] );
	}

	// Add normals
	vecDestNormal.resize( 0 );
	if( style == PrimitiveModelStyle::EDGE_VERTICES_WELDED )
	{
	}
	else
	{
//		vecDestNormal.insert( vecDestNormal.begin(), num_segments+1,  vDirFromBottomToTop ); // top
//		vecDestNormal.insert( vecDestNormal.end(),   num_segments+1, -vDirFromBottomToTop ); // bottom
		vecDestNormal.insert( vecDestNormal.begin(), num_segments+1,  vUp ); // top
		vecDestNormal.insert( vecDestNormal.end(),   num_segments+1, -vUp ); // bottom
		vector<Vector3> vecRimNormal;

		vecRimNormal.resize( num_segments );
		for( int i=0; i<num_segments; i++ )
			vecRimNormal[i] = Vec3GetNormalized( vecDestPos[i+1] - pose[0].vPosition );

		// normals for side vertices
		for( int i=0; i<2; i++ )
		{
			for( int j=0; j<num_segments; j++ )
				vecDestNormal.push_back( vecRimNormal[j] );
		}
	}

	// create polygon indices
	const int center[2] = { 0, num_segments + 1 };
	for( int i=0; i<2; i++ )
	{
		// invert the indexing order of vertices for triangles on top(i==0) of the cylinder.
		const int idx0 = (i==0) ? 2 : 1;
		const int idx1 = (i==0) ? 1 : 2;

		// triangles on top & bottom of the cylinder
		for( int j=0; j<num_segments; j++ )
		{
			const int triangle_indices[3] =
			{
				 center[i],
				 center[i] + 1 + j,
				 center[i] + 1 + ((j+1) % num_segments)
			};

			vecDestPoly.push_back( vector<int>() );
			vecDestPoly.back().push_back( triangle_indices[0] );
			vecDestPoly.back().push_back( triangle_indices[idx0] );
			vecDestPoly.back().push_back( triangle_indices[idx1] );
		}
	}

	// polygons on the enclosing side of the cylinder
	int top_start    = (style == PrimitiveModelStyle::EDGE_VERTICES_WELDED) ? 1 : (num_segments+1) * 2;
	int bottom_start = (style == PrimitiveModelStyle::EDGE_VERTICES_WELDED) ? num_segments+1 : (num_segments+1) * 2 + num_segments;
	for( int i=0; i<num_segments; i++ )
	{
		vecDestPoly.push_back( vector<int>() );
		vecDestPoly.back().push_back( top_start    +  i );
		vecDestPoly.back().push_back( top_start    + (i + 1) % num_segments );
		vecDestPoly.back().push_back( bottom_start + (i + 1) % num_segments );
		vecDestPoly.back().push_back( bottom_start +  i );
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
