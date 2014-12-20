#include "ScreenshotRenderer.hpp"
#include "amorphous/Graphics/TextureRenderTarget.hpp"
#include "amorphous/App/GameWindowManager.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{


static std::string sg_ImageFormat = "png";


/*
 * Returns -1 on error
 *
 */
unsigned int GetNextImageFileNumber( const std::string& directory_path, const std::string& basename )
{
	using namespace boost::filesystem;

	if( !exists(directory_path) )
		return -1;
	
	unsigned int count = 0;

	boost::system::error_code ec;
	directory_iterator itr( directory_path, ec );

	if( ec != boost::system::errc::success )
		return 0;

	directory_iterator end_itr; // default construction yields past-the-end
	for( ; itr != end_itr; ++itr )
	{
		const path& pathname = *itr;

		if( pathname.leaf().string().find( basename ) == 0 )
			count += 1;
	}

	return count;
}


/// \param image_format Starts with '.', e.g. ".png"
void _TakeScreenshot(
	ScreenshotRenderer& renderer,
	const std::string& directory_path,
	const std::string& basename,
	const char *image_format
	)
{
	using namespace boost::filesystem;

	if( !exists(directory_path) )
	{
		boost::system::error_code ec;
		create_directories( directory_path, ec );
		if( ec.value() != 0 )
		{
			LOG_PRINT_ERROR( "create_directories() failed (error: " + ec.message() + ")." );
			return;
		}
	}

	// Decide on the image format

	const char *default_image_format = "png";

	if( !image_format )
		return;

	std::string image_format_to_use;
	if( 0 < strlen(image_format) )
	{
		LOG_PRINT( "Set the image format (argument): " + image_format_to_use );
		image_format_to_use = image_format;
	}
	else
	{
		// This obviously does not work unless the working directory stays the same.
		// If there is a way to find the path of the directory where the executable is located,
		// it may be preferable to load the value from the config file every time like this
		// because the user can change the image format without restarting the application
//		LoadParamFromFile( "config", "screenshot_image_format", image_format_to_use );

		image_format_to_use = sg_ImageFormat;

		LOG_PRINT( "Set the image format: " + image_format_to_use );

		if( image_format_to_use.length() == 0 )
			image_format_to_use = default_image_format;
	}

	unsigned int count = GetNextImageFileNumber( directory_path, basename );

	if( count == -1 )
		return;

	boost::shared_ptr<TextureRenderTarget> pTextureRenderTarget = TextureRenderTarget::Create();
	const int screen_width  = GetGameWindowManager().GetScreenWidth();
	const int screen_height = GetGameWindowManager().GetScreenHeight();
	pTextureRenderTarget->Init( screen_width, screen_height );
	pTextureRenderTarget->SetRenderTarget();

	renderer.Render();

	std::string image_filename = fmt_string( "%s%04d.", basename.c_str(), count ) + image_format_to_use;

	LOG_PRINT( "image_filename: " + image_filename );

	pTextureRenderTarget->ResetRenderTarget();
	path directory_path_and_basename = path(directory_path) / path(basename);
//	std::string img_file_path = fmt_string( "screenshots/image%04d", count ) + "." + img_ext;
	std::string img_file_pathname
		= fmt_string( "%s%04d.", directory_path_and_basename.string().c_str(), count ) + image_format_to_use;

	pTextureRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( img_file_pathname );
}


void SetScreenshotImageFormat( const std::string& image_format )
{
	sg_ImageFormat = image_format;
}


} // namespace amorphous
