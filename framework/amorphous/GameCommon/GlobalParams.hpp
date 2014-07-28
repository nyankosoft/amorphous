#ifndef __amorphous_GlobalParams_HPP__
#define __amorphous_GlobalParams_HPP__


#include "Support/ParamLoader.hpp"
#include "Support/Log/LogOutputBase.hpp"


namespace amorphous
{


//>>>========================== default config file ==========================
/*
graphics_library  OpenGL
screen_resolution 1280	720
fullscreen	no
window_pos  50 50
screenshot_format  png
screenshot_resolution  1280 720
audio_library  OpenAL
*/
//<<<========================== default config file ==========================


/**
 - Global settings for a game application
   - Always loaded from a text file.
   - The application can let the user modify the content of the file
     to configure the settings.

*/
class GlobalParams
{
public:

	bool FullScreen;
	int ScreenWidth;
	int ScreenHeight;

	/// top-left corner of the window
	/// - valid only in windowed mode
	/// - set to -1 if not specified,
	/// and the window will be placed in the center of the desktop
	int WindowLeftPos;
	int WindowTopPos;

	std::string ScreenshotImageFormat;
	int ScreenshotResolutionWidth;
	int ScreenshotResolutionHeight;
//	std::string ScreenshotOutputDirectory;

	int LogVerbosity;

	std::string AudioLibraryName;

	std::string GraphicsLibraryName;

private:

	void LoadLogVerbosity( ParamLoader& loader );

public:

	GlobalParams();

	bool LoadFromFile( const std::string& filename );

	bool CreateDefaultConfigFile();
};


extern GlobalParams g_GlobalParams;

} // namespace amorphous



#endif  /*  __amorphous_GlobalParams_HPP__  */
