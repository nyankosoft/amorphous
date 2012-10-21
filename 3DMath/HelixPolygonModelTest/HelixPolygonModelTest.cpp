#include "gds/3DMath/HelixPolygonModelMaker.hpp"
#include "gds/Support/wavefront_obj_output.hpp"
#include "gds/Support/ParamLoader.hpp"

using namespace std;


class tex2d { public: float u,v; tex2d(float _u,float _v) : u(_u), v(_v) {} };

int CreateHelixes( bool single_mesh_file )
{
	vector<Vector3> positions, normals;
	vector< vector<unsigned int> > polygons;
	vector<tex2d> tex_uvs;

	float coil_length = 1.0f;
	float coil_radii[] = { 0.12f };
	unsigned int num_coils = 10;
	float cord_radius = 0.03f;
	unsigned int num_coil_sides[] = { 8, 16, 32 };
	unsigned int num_cord_sides[] = { 8, 16, 32 };

	for( int i=0; i<numof(coil_radii); i++ )
	{
		for( int j=0; j<numof(num_coil_sides); j++ )
		{
			for( int k=0; k<numof(num_cord_sides); k++ )
			{
				if( !single_mesh_file )
				{
					positions.resize(0);
					normals.resize(0);
					polygons.resize(0);
				}

				CreateUniformCylindricalHelix(
					coil_length,       // coil length
					coil_radii[i],     // coil radius
					num_coils,         // num coils
					0.03f,             // cord radius
					num_coil_sides[j], // num coil sides
					num_cord_sides[k], // num cord sides
					true,
					PolygonModelStyle::EDGE_VERTICES_UNWELDED,
					true,
					PolygonModelStyle::EDGE_VERTICES_UNWELDED,
					positions,
					normals,
					polygons
					);

				if( single_mesh_file )
				{
					for( size_t n=0; n<positions.size(); n++ )
					{
						positions[n].x += coil_radii[i] * 2.0f + 0.2f;
					}

				}
				else
				{
					string pathname = fmt_string( "coil_%.3f_%dx%d.obj", coil_radii[i], num_coil_sides[j], num_cord_sides[k] );
					wavefront_obj::write_to_obj_file( pathname, positions, tex_uvs, normals, polygons );
				}
			}
		}
	}

	if( single_mesh_file )
		wavefront_obj::write_to_obj_file( "helixes.obj", positions, tex_uvs, normals, polygons );

	return 0;
}

int CreateSingleUniformCylindricalHelix()
{
	vector<Vector3> positions, normals;
	vector< vector<unsigned int> > polygons;

	float helix_length = 1.0f;
	float helix_radius = 0.15f;
	unsigned int num_coils = 10;
	float cord_radius = 0.03f;
	unsigned int num_helix_sides = 16;
	unsigned int num_cord_sides = 12;

	LoadParamFromFile( "params.txt", "helix_length",    helix_length );
	LoadParamFromFile( "params.txt", "helix_radius",    helix_radius );
	LoadParamFromFile( "params.txt", "num_coils",       num_coils );
	LoadParamFromFile( "params.txt", "cord_radius",     cord_radius );
	LoadParamFromFile( "params.txt", "num_helix_sides", num_helix_sides );
	LoadParamFromFile( "params.txt", "num_cord_sides",  num_cord_sides );

	CreateUniformCylindricalHelix(
		helix_length,    // coil length
		helix_radius,    // coil radius
		num_coils,       // num coils
		0.03f,           // cord radius
		num_helix_sides, // num coil sides
		num_cord_sides,  // num cord sides
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		true,
		PolygonModelStyle::EDGE_VERTICES_UNWELDED,
		positions,
		normals,
		polygons
		);

	vector<tex2d> tex_uvs;

	wavefront_obj::write_to_obj_file( "helix.obj", positions, tex_uvs, normals, polygons );

	return 0;
}


int main( int argc, char *argv[] )
{
	CreateHelixes( true );

	CreateSingleUniformCylindricalHelix();

	return 0;
}
