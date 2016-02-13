#include "amorphous/Graphics/FloatRGBColor.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/ImageArchiveAux.hpp"
#include "amorphous/base.hpp"

using namespace std;
using namespace amorphous;


// Resolved boost link error by doing this:
// http://stackoverflow.com/questions/33815603/linker-error-boost-chrono-to-boost-timer


void LoadImageForTest( const std::string& image_filepath )
{
	LOG_SCOPE( image_filepath.c_str() );

	BitmapImage img;
	img.LoadFromFile( image_filepath );
}


void TestPerformanceToOpenImageFiles()
{
	string img_files[] =
	{
		"./images/test51_tex00.bmp",
		"./images/test51_tex01.bmp",
		"./images/test51_tex02.bmp",
		"./images/test51_tex03.bmp",
		"./images/test51_tex00.jpg",
		"./images/test51_tex01.jpg",
		"./images/test51_tex02.jpg",
		"./images/test51_tex03.jpg"
	};

	for( int i=0; i<numof(img_files); i++ )
	{
		LoadImageForTest( img_files[i] );
	}
}


int TestImageArchive()
{
	ImageArchive img_archive( "./images/src.bmp" );

	if( !img_archive.IsValid() )
		return -1;

	BitmapImage img;
	LoadBitmapImageFromImageArchive( img_archive, img );

	bool saved = img.SaveToFile( "./images/dest.jpg" );
	if( saved )
		return 0;
	else
		return -1;
}


void RunImageSaveTest()
{

	const char *extensions[] = {
		"png",
		"jpg"
	};

	const SFloatRGBAColor colors[] = {
		SFloatRGBAColor::White(),
		SFloatRGBAColor::Red(),
		SFloatRGBAColor::Green(),
		SFloatRGBAColor::Blue(),
		SFloatRGBAColor::Black()
	};

	const char *basenames[] = {
		"white",
		"red",
		"green",
		"blue",
		"black"
	};

	string image_file_pathname;

	for( int i=0; i<numof(extensions); i++ )
	{
		const int bpps[] = { 24, 32 };
		for( int j=0; j<numof(bpps); j++ )
		{
			for( int k=0; k<numof(colors); k++ )
			{
				BitmapImage img64( 64, 64, bpps[j] );
				img64.FillFRGBAColor( colors[k] );

				image_file_pathname = string("results/bitmap_images/") + string(basenames[k]) + "_064x064_bpp" + std::to_string(bpps[j]) + "." + string(extensions[i]);
				img64.SaveToFile( image_file_pathname );

				BitmapImage img256( 256, 256, bpps[j] );
				img256.FillFRGBAColor( colors[k] );

				image_file_pathname = string("results/bitmap_images/") + string(basenames[k]) + "_256x256_bpp" + std::to_string(bpps[j]) + "." + string(extensions[i]);
				img256.SaveToFile( image_file_pathname );
			}
		}
	}
}


void RunPixelOperationsTest( BitmapImage& img )
{
	LOG_PRINTF(("bpp: %d", img.GetFBITMAP() ? FreeImage_GetBPP(img.GetFBITMAP()) : 0));

	int w = img.GetWidth();
	int h = img.GetHeight();
	U8 r=0,g=0,b=0,a=0;
	img.GetPixel(0,0,r,g,b);
	LOG_PRINTF(("pixel(0,0): RGB = %u, %u, %u",r,g,b));
	r=0,g=0,b=0,a=0;
	img.GetPixel(0,0,r,g,b,a);
	LOG_PRINTF(("pixel(0,0): RGBA = %u, %u, %u, %u",r,g,b,a));

	r=0,g=0,b=0,a=0;
	img.GetPixel(w-1,0,r,g,b);
	LOG_PRINTF(("pixel(w-1,0): RGB = %u, %u, %u",r,g,b));
	r=0,g=0,b=0,a=0;
	img.GetPixel(w-1,0,r,g,b,a);
	LOG_PRINTF(("pixel(w-1,0): RGBA = %u, %u, %u, %u",r,g,b,a));

	r=0,g=0,b=0,a=0;
	img.GetPixel(0,h-1,r,g,b);
	LOG_PRINTF(("pixel(0,h-1): RGB = %u, %u, %u",r,g,b));
	r=0,g=0,b=0,a=0;
	img.GetPixel(0,h-1,r,g,b,a);
	LOG_PRINTF(("pixel(0,h-1): RGBA = %u, %u, %u, %u",r,g,b,a));

	r=0,g=0,b=0,a=0;
	img.GetPixel(w-1,h-1,r,g,b);
	LOG_PRINTF(("pixel(w-1,h-1): RGB = %u, %u, %u",r,g,b));
	r=0,g=0,b=0,a=0;
	img.GetPixel(w-1,h-1,r,g,b,a);
	LOG_PRINTF(("pixel(w-1,h-1): RGBA = %u, %u, %u, %u",r,g,b,a));

	r=255; g=255; b=0; a=255;
	img.SetPixel(0,  0,r,g,b);
	r=0; g=0; b=255; a=255;
	img.SetPixel(w-1,0,r,g,b,a);
	r=0; g=255; b=0; a=255;
	img.SetPixel(0,  h-1,r,g,b);
	r=255; g=0; b=0; a=255;
	img.SetPixel(w-1,h-1,r,g,b,a);
}


void RunPixelOperationsTest()
{
	BitmapImage png;
	bool loaded = png.LoadFromFile("images/get_pixel_test.png");

	LOG_PRINT("PNG:");
	RunPixelOperationsTest( png );

	png.SaveToFile("images/set_pixel_test.png");


	BitmapImage jpg;
	loaded = jpg.LoadFromFile("images/get_pixel_test.jpg");

	LOG_PRINT("JPG:");
	RunPixelOperationsTest( jpg );

	jpg.SaveToFile("images/set_pixel_test.jpg");
}


int RunTests()
{
//	RunImageSaveTest();

	RunPixelOperationsTest();

//	TestImageArchive();

//	TestPerformanceToOpenImageFiles();

	return 0;
}


int test_BitmapImage( int argc, char *argv[] )
{
	InitFreeImage();

	return RunTests();
}
