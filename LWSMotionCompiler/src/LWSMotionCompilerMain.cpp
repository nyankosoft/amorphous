#include "LWSMotionCompiler.hpp"
#include "gds/base.hpp"
#include "gds/Support/lfs.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"


using namespace std;
using namespace boost;






/*
bool ends_with( const std::string& target, const std::string& end )
{
	if( target.substr( target.length() - end.length(), end.length() );
}
*/

int main( int argc, char *argv[] )
{
	using namespace filesystem;

	path init_wd = lfs::get_cwd();

	if( init_wd.leaf() != "app" )
		lfs::set_wd( "../app" );

	string filepath;
	if( 2 <= argc )
		filepath  = argv[1];
	else
	{
		GetFilename( filepath );
	}

	if( filepath.length() == 0 )
		return 0;

	CLWSMotionCompiler test;
	test.BuildFromDescFile( filepath );

	return 0;
}
