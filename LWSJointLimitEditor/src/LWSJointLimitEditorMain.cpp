#include <string>
#include "amorphous/Support/FileOpenDialog_Win32.hpp"
#include "LWSJointLimitEditor.hpp"

using namespace std;


int main( int argc, char *argv[] )
{
	// debug - print the command line arguments
	for( int i=0; i<argc; i++ )
	{
		printf( "arg[%d]: %s\n", i, argv[i] );
	}

//	int c = getchar();
//	return 0;

	string lws_filepath;
	string joint_limit_list_filepath;

	for( int i=1; i<argc; i++ )
	{
		const string arg( argv[i] );

		// minimum valid filename length
		// = 3(ext) + 1(dot) + 1(filename) = 5

		if( arg.length() < 5 )
			continue;

		if( arg.rfind( ".lws" ) == arg.length() - 4 )
			lws_filepath = arg;
		else if( arg.rfind( ".csv" ) == arg.length() - 4 )
			joint_limit_list_filepath = arg;
	}

	if( argc == 1 )
	{
//		GetFilename( lws_filepath );
	}

	lws_filepath              = "../../../../CharacterAnimation/female99-age17-muscle73-weight66-height1.52/scenes/walk.lws";
	joint_limit_list_filepath = "../../../../CharacterAnimation/female99-age17-muscle73-weight66-height1.52/scenes/limits.csv";

//	if( joint_limit_list_filepath.length() == 0 )
//		joint_limit_list_filepath = "../data/no_limits.csv";

	if( joint_limit_list_filepath.length() == 0 )
	{
		printf( "Need a joint limit list file (.csv)\n" );
		return 0;
	}

	if( joint_limit_list_filepath.length() == 0 )
	{
		printf( "Need a LightWave scene file (.lws)\n" );
		return 0;
	}

	LWSJointLimitEditor limit_editor;
	limit_editor.LoadSceneFile( lws_filepath );
	limit_editor.LoadCSVJointLimitTable( joint_limit_list_filepath );

	limit_editor.Write();

	return 0;
}
