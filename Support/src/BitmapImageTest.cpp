#include "amorphous/Support/Log/LogOutput.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/MiscAux.hpp"
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

	BitmapImage img( img_archive );

	bool saved = img.SaveToFile( "./images/dest.jpg" );
	if( saved )
		return 0;
	else
		return -1;
}


void RunImageSaveTest()
{
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

	const int bpps[] = { 24, 32 };
	for( int i=0; i<numof(bpps); i++ )
	{
		for( int j=0; j<numof(colors); j++ )
		{
			BitmapImage img( 64, 64, bpps[i] );
			img.FillColor( colors[j] );

			img.SaveToFile( string(basenames[j]) + "_bpp" + to_string(bpps[i]) + string(".png") );
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

	string log_filepath = "BitmapImageTest-" + string(GetBuildInfo()) + ".html";
	boost::shared_ptr<LogOutput_HTML> pLog( new LogOutput_HTML(log_filepath) );
	GlobalLog().AddLogOutput( pLog.get() );

	return RunTests();
}
