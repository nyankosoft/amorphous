//=================================================================================
//  ScreenShotManager.cpp
//
//  take an array of DWORD and output it as a bmp image file
//
//=================================================================================

#include "ScreenShotManager.hpp"
#include <boost/filesystem.hpp>
//#include <wingdi.h>
#include "Support/BitmapImage.hpp"
#include "Support/StringAux.hpp"


namespace amorphous
{

using namespace std;
using namespace boost;


CScreenShotManager::CScreenShotManager()
:
m_ImageFileExtension("bmp"),
m_ImageOutputDirectoryPath("./Screenshots/")
{
}


CScreenShotManager::~CScreenShotManager()
{
}


int CScreenShotManager::GetScreenshotFileIndex()
{
	int current_index, next_index;

	string dir_path = m_ImageOutputDirectoryPath;

	// This is "./screenshots/ssr.dat" by default
	string screenshot_record_file = m_ImageOutputDirectoryPath + "ssr.dat";

	FILE *fp = fopen( screenshot_record_file.c_str(), "rb" );

	if(!fp)
	{
		// the file which records screenshot count is missing
		// either the application is running for the first time or the record file has been deleted
		fp = fopen( screenshot_record_file.c_str(), "wb" );
		if( !fp )
		{
			// probably the directory 'screenshots' does not exist
			filesystem::create_directory( dir_path );
			fp = fopen( screenshot_record_file.c_str(), "wb" );
			if(!fp)
				return -1;
		}
		next_index = 1;
		fwrite( &next_index, sizeof(int), 1, fp );
		fclose(fp);
		return 0;
	}

	fread( &current_index, sizeof(int), 1, fp );
	fclose(fp);

	fp = fopen( screenshot_record_file.c_str(), "wb" );
	next_index = current_index + 1;
	fwrite( &next_index, sizeof(int), 1, fp );
	fclose(fp);

	const int num_max_screenshot_image_files = 10000;
	if( num_max_screenshot_image_files <= current_index )
		current_index %= num_max_screenshot_image_files;

	return current_index;
}


void CScreenShotManager::OutputImageDataToFile( U32 *pdwImageData, int iImageWidth, int iImageHeight )
{
	string img_ext = m_ImageFileExtension;

//	FILE *fp = fopen( "dbg_img_content.txt", "w" );

	const int depth = 24;
	BitmapImage img( iImageWidth, iImageHeight, depth );
	int x,y;
	for( y=0; y<iImageHeight; y++ )
	{
		for( x=0; x<iImageWidth; x++ )
		{
			SFloatRGBColor pixel;
			pixel.red   = (float)( 0x000000FF & (pdwImageData[y * iImageWidth + x] >> 16) ) / 256.0f;
			pixel.green = (float)( 0x000000FF & (pdwImageData[y * iImageWidth + x] >>  8) ) / 256.0f;
			pixel.blue  = (float)( 0x000000FF &  pdwImageData[y * iImageWidth + x] )        / 256.0f;

			img.SetPixel( x, y, pixel );

//			if( fp )
//				fprintf( fp, "%.3f, %.3f, %.3f\n", pixel.red, pixel.green, pixel.blue );
		}
	}

//	if( fp )
//		fclose(fp);

	img.SaveToFile( m_ImageOutputDirectoryPath + fmt_string( "image%04d.", GetScreenshotFileIndex() ) + img_ext );
	
/*
	// create bmp image from scratch
	// - Maybe slightly faster than the above
	// - Could be of some use if one has to take many screenshots in a rapid succession

	// init bitmap info header
	BITMAPINFOHEADER bmih;
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = iImageWidth;
	bmih.biHeight = iImageHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;


	// init bitmap file header
	BITMAPFILEHEADER bmfh;
	memset(&bmfh,0,sizeof(BITMAPFILEHEADER));
	memcpy(&bmfh.bfType,"BM",2);
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize; // bmv5h.biSize;	// + sizeof(RGBQUAD) * 256;

	// compute the size of the entire file
	bmfh.bfSize    = sizeof(BITMAPFILEHEADER) + bmih.biSize + sizeof(BYTE) * 3 * iImageWidth * iImageHeight;

	int iFileIndex = GetScreenshotFileIndex();

	char acFilename[512];
	sprintf( acFilename, "./screenshots/image%04d.bmp", iFileIndex );
	FILE *fp = fopen( acFilename, "wb" );

	fwrite( &bmfh, sizeof(BITMAPFILEHEADER), 1, fp );
	fwrite( &bmih, sizeof(BITMAPINFOHEADER), 1, fp );

	int x,y;
	BYTE red, green, blue;
	for( y=iImageHeight-1; 0<=y; y-- )
	{
		for( x=0; x<iImageWidth; x++ )
		{
			red   = (BYTE)( 0x000000FF & (pdwImageData[y * iImageWidth + x] >> 16) );
			green = (BYTE)( 0x000000FF & (pdwImageData[y * iImageWidth + x] >>  8) );
			blue  = (BYTE)( 0x000000FF &  pdwImageData[y * iImageWidth + x] );

			fwrite( &blue,  sizeof(BYTE), 1, fp );
			fwrite( &green, sizeof(BYTE), 1, fp );
			fwrite( &red,   sizeof(BYTE), 1, fp );
		}
	}

	fclose(fp);
*/
}


} // namespace amorphous
