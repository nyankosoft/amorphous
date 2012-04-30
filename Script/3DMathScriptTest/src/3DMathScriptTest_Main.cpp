// #define fopen_s fopen
// The above does not work - results in error in stdio.h

#include "gds/3DMath/Matrix34.hpp"
#include "gds/Script/PyModule_3DMath.hpp"
#include "gds/Support/stream_buffer.hpp"
#include "gds/Support/lfs.hpp"
#include <boost/python.hpp>

using namespace std;
using namespace boost;


void ExecutePythonScript( const std::string& script_filepath )
{
	// Retrieve the main module
	python::object main = python::import("__main__");

	// Retrieve the main module's namespace
	python::object global(main.attr("__dict__"));

	stream_buffer script_buffer;
	bool loaded = script_buffer.LoadTextFile( script_filepath );

	if( !loaded )
		return;

	// run the Python script.
	python::object result = python::exec( &(script_buffer.get_buffer()[0]), global, global );
}


void ExecuteTest()
{
	ExecutePythonScript( "./scripts/math3d_test.py" );
}


int main( int argc, char *argv[] )
{
	string init_directory_path = lfs::get_cwd();

	if( init_directory_path.rfind( "app" ) == string::npos )
	{
		lfs::set_wd( "../app" );
	}

	Py_Initialize();

	RegisterPythonModule_math3d();

	ExecuteTest();

	Py_Finalize();

	return 0;
}