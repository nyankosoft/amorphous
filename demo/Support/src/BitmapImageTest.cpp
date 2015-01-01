#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/ImageArchiveAux.hpp"
#include "amorphous/base.hpp"

using namespace std;
using namespace amorphous;


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
				img64.FillColor( colors[k] );

				image_file_pathname = string("results/bitmap_images/") + string(basenames[k]) + "_064x064_bpp" + to_string(bpps[j]) + "." + string(extensions[i]);
				img64.SaveToFile( image_file_pathname );

				BitmapImage img256( 256, 256, bpps[j] );
				img256.FillColor( colors[k] );

				image_file_pathname = string("results/bitmap_images/") + string(basenames[k]) + "_256x256_bpp" + to_string(bpps[j]) + "." + string(extensions[i]);
				img256.SaveToFile( image_file_pathname );
			}
		}
	}
}


int RunTests()
{
	RunImageSaveTest();

//	TestImageArchive();

//	TestPerformanceToOpenImageFiles();

	return 0;
}


int test_BitmapImage( int argc, char *argv[] )
{
	InitFreeImage();

	return RunTests();
}
