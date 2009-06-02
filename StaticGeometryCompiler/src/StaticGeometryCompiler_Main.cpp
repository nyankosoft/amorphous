//-----------------------------------------------------------------------------
// File: StaticGeometryCompiler_Main.cpp
// for console application
//-----------------------------------------------------------------------------

//#include <vld.h>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "Graphics/all.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Support.hpp"
#include "Support/FileOpenDialog_Win32.hpp"
#include "Support/MiscAux.hpp"
#include "Stage/StaticGeometry.hpp"
#include "XML/XMLDocumentLoader.hpp"

#include "StaticGeometryCompilerFG.h"
#include "StaticGeometryCompiler.h"
#include "StaticGeometryCompiler_Main.h"


std::string m_OutputFilepath;

CLogOutput_ScrolledTextBuffer *g_pLogOutput = NULL;


int RunApp( const std::string& cmd_line )
{
	string initial_working_directory = fnop::get_cwd();

	int ret = RunStaticGeometryCompiler( cmd_line, initial_working_directory );

	LOG_PRINT( " Entering the main message loop..." );

	LOG_PRINT( " Cleaning up..." );

    return ret;
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
	string initial_working_directory = fnop::get_cwd();

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

	g_Log.RemoveLogOutput( g_pLogOutput );
	SafeDelete( g_pLogOutput );

	return 0;
}
