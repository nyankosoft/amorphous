#include "amorphous/3DMath/PrimitivePolygonModelMaker.hpp"
#include "amorphous/Support/wavefront_obj_output.hpp"
#include "amorphous/Support/ParamLoader.hpp"

using namespace std;


class tex2d { public: float u,v; tex2d(float _u,float _v) : u(_u), v(_v) {} };


int create_cylinder_test( int num_sides, int num_divisions )
{
	vector<Vector3> positions, normals;
	vector< vector<int> > polygons;

	float radii[2] = { 0.2f, 0.2f };

	CreateCylinder(
		1.0f, radii, num_sides, num_divisions,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		positions,
		normals,
		polygons
		);

	string pathname = fmt_string( "output/cylinder-s%03d-d%03d.obj", num_sides, num_divisions );

	wavefront_obj::write_to_obj_file( pathname, positions, vector<tex2d>(), normals, polygons );

	return 0;
}


int create_pipe_test( int num_sides, int num_divisions )
{
	vector<Vector3> positions, normals;
	vector< vector<int> > polygons;

	CreatePipe(
		1.0f, 0.25f, 0.20f, num_sides, num_divisions,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		positions,
		normals,
		polygons
		);

	string pathname = fmt_string( "output/pipe-s%03d-d%03d.obj", num_sides, num_divisions );

	wavefront_obj::write_to_obj_file( pathname, positions, vector<tex2d>(), normals, polygons );

	return 0;
}


int run()
{
	int num_sides[] = { 2, 4, 8, 16, 32, 64, 128, 19, 23, 47, 73 };
	int num_divisions[] = { 1, 2, 3, 4, 16 };

	for( int i=0; i<numof(num_sides); i++ )
	{
		for( int j=0; j<numof(num_divisions); j++ )
		{
			create_cylinder_test( num_sides[i], num_divisions[j] );
			create_pipe_test( num_sides[i], num_divisions[j] );
		}
	}

	return 0;
}


int main( int argc, char *argv[] )
{
	run();

	return 0;
}
