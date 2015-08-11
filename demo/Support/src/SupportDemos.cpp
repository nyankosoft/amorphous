#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Support/MiscAux.hpp"

using namespace std;
using namespace amorphous;


int test_fixed_string( int argc, char *argv[] );;
int test_TextFileScanner( int argc, char *argv[] );
int test_clipboard_utils( int argc, char *argv[] );
int test_BitmapImage( int argc, char *argv[] );
int test_ImageSplitter( int argc, char *argv[] );
int test_FreeType( int argc, char *argv[] );
int demo_AES( int argc, char *argv[] );


int main( int argc, char *argv[] )
{
	const std::string iwd = amorphous::lfs::get_cwd(); // initial working directory
	if( iwd.substr( iwd.length() - 4 ) != "/app"
	 && iwd.substr( iwd.length() - 4 ) != "\\app" )
	{
		// working directory is not set to the directory which contains the application binary
		chdir( "../app" );
	}

	std::string log_filepath = "BitmapImageTest-" + std::string(GetBuildInfo()) + ".html";
	InitHTMLLog( log_filepath );

//	test_fixed_string( argc, argv );

//	test_TextFileScanner( argc, argv );

//	test_clipboard_utils( argc, argv );

//	test_BitmapImage( argc, argv );

//	test_ImageSplitter( argc, argv );

//	test_FreeType( argc, argv );

	demo_AES( argc, argv );

	return 0;
}
