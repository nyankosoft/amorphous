//-----------------------------------------------------------------------------
// File: StaticGeometryCompiler_Console.cpp
// static geometry compiler (command line interface)
//-----------------------------------------------------------------------------

//#include <vld.h>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "gds/Graphics.hpp"
#include "gds/Graphics/LogOutput_OnScreen.hpp"
#include "gds/Support/FileOpenDialog_Win32.hpp"
#include "gds/Support/MiscAux.hpp"
#include "gds/Stage/StaticGeometry.hpp"
#include "gds/XML/XMLDocumentLoader.hpp"

#include "StaticGeometryCompilerFG.h"
#include "StaticGeometryCompiler.h"
#include "StaticGeometryCompiler_Main.h"

using namespace std;
using namespace amorphous;


string m_OutputFilepath;

//LogOutput_ScrolledTextBuffer *g_pLogOutput = NULL;


int RunApp( const string& cmd_line )
{
	LOG_FUNCTION_SCOPE();

	string initial_working_directory = lfs::get_cwd();

	int ret = RunStaticGeometryCompiler( cmd_line, initial_working_directory );

    return ret;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	string initial_working_directory = lfs::get_cwd();

	string cmd_line;
	if( 2 <= argc )
		cmd_line = argv[1];

	try
	{
		int ret = RunApp( cmd_line );
	}
	catch( exception& e )
	{
		g_Log.Print( WL_ERROR, "exception: %s", e.what() );
	}

//	g_Log.RemoveLogOutput( g_pLogOutput );
//	SafeDelete( g_pLogOutput );

	return 0;
}
