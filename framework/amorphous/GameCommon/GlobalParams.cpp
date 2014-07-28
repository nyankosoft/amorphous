#include "GlobalParams.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <string>


namespace amorphous
{

using namespace std;


// instance of the global params
GlobalParams g_GlobalParams;


GlobalParams::GlobalParams()
:
FullScreen(false),
ScreenWidth(1280),
ScreenHeight(720),
WindowLeftPos(-1),
WindowTopPos(-1),
ScreenshotImageFormat( "png" ),
ScreenshotResolutionWidth( -1 ),
ScreenshotResolutionHeight( -1 ),
LogVerbosity(WL_WARNING),
AudioLibraryName("OpenAL"),
GraphicsLibraryName("OpenGL")
{}


void GlobalParams::LoadLogVerbosity( ParamLoader& loader )
{
	string verbosity_title;
	loader.LoadParam( "log_verbosity", verbosity_title );

	int verbosity = WL_WARNING;
	if( verbosity_title == "verbose" )      verbosity = WL_VERBOSE;
	else if( verbosity_title == "caution" ) verbosity = WL_CAUTION;
	else if( verbosity_title == "warning" ) verbosity = WL_WARNING;
	else if( verbosity_title == "error" )   verbosity = WL_ERROR;

	GlobalLog().SetRefVal( verbosity );
}


bool GlobalParams::LoadFromFile( const std::string& filename )
{
	ParamLoader loader( filename );

	if( !loader.IsReady() )
		return false;

	loader.LoadParam( "graphics_library",         GraphicsLibraryName );
	loader.LoadBoolParam( "fullscreen", "yes/no", FullScreen );
	loader.LoadParam( "screen_resolution",        ScreenWidth, ScreenHeight );
	loader.LoadParam( "window_pos",               WindowLeftPos, WindowTopPos );
	loader.LoadParam( "screenshot_format",        ScreenshotImageFormat );
	loader.LoadParam( "screenshot_resolution",    ScreenshotResolutionWidth, ScreenshotResolutionHeight );
	loader.LoadParam( "audio_library",            AudioLibraryName );

	LoadLogVerbosity( loader );

	return true;
}


bool GlobalParams::CreateDefaultConfigFile()
{
	const char *text =
		"graphics_library  OpenGL\n"\
		"screen_resolution  1280 720\n"\
		"fullscreen  no\n"\
		"window_pos  50 50\n"\
		"screenshot_format  png\n"\
		"screenshot_resolution  1280 720\n"\
		"audio_library  OpenAL\n";

	FILE *fp = fopen("config","w");

	if( fp )
	{
		fprintf( fp, text );

		int ret = fclose(fp);
		if( ret != 0 )
		{
			LOG_PRINT_ERROR( " Failed to close the file.");
			return false;
		}
	}
	else
	{
		LOG_PRINT_ERROR( " Failed to open the file.");
		return false;
	}

	return true;
}


} // namespace amorphous
