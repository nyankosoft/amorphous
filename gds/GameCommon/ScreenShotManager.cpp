//=================================================================================
//  ScreenShotManager.cpp
//
//  take an array of DWORD and output it as a bmp image file
//
//=================================================================================

#include "ScreenShotManager.h"
#include <stdio.h>
#include <wingdi.h>
#include <direct.h>

CScreenShotManager::CScreenShotManager()
{
}


CScreenShotManager::~CScreenShotManager()
{
}


int CScreenShotManager::GetScreenshotFileIndex()
{
	int current_index, next_index;

	FILE *fp = fopen( "screenshots\\ssr.dat", "rb" );

	if(!fp)
	{
		// the file which records screenshot count is missing
		// either the application is running for the first time or the record file has been deleted
		fp = fopen( "screenshots\\ssr.dat", "wb" );
		if( !fp )
		{
			// probably the directory 'screenshots' does not exist
			mkdir( "screenshots" );
			fp = fopen( "screenshots\\ssr.dat", "wb" );
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

	fp = fopen( "screenshots\\ssr.dat", "wb" );
	next_index = current_index + 1;
	fwrite( &next_index, sizeof(int), 1, fp );
	fclose(fp);

	if( 9999 < current_index )
		current_index %= 10000;

	return current_index;
}


void CScreenShotManager::OutputImageDataToFile( U32 *pdwImageData, int iImageWidth, int iImageHeight )
{

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
	sprintf( acFilename, "screenshots\\image%04d.bmp", iFileIndex );
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

}



/*	BITMAPV5HEADER bmv5h;
	memset( &bmv5h, 0, sizeof(BITMAPV5HEADER) );
	bmv5h.bV5Size        = sizeof(BITMAPV5HEADER);
	bmv5h.bV5Width       =   iImageWidth;
	bmv5h.bV5Height      = - iImageHeight;	// specify negative value to make bitmap top-left DIB
	bmv5h.bV5Planes      = 1; 
    bmv5h.bV5BitCount    = 24; 
    bmv5h.bV5Compression = BI_RGB;	// uncompressed format
//    bmv5h.bV5SizeImage   = 0; 
//    bmv5h.bV5XPelsPerMeter; 
//    bmv5h.bV5YPelsPerMeter; 
//    bmv5h.bV5ClrUsed; 
//    bmv5h.bV5ClrImportant; 
//    bmv5h.bV5RedMask; 
//    bmv5h.bV5GreenMask; 
//    bmv5h.bV5BlueMask; 
//    bmv5h.bV5AlphaMask; 
//    bmv5h.bV5CSType; 
//    bmv5h.bV5Endpoints; 
//    bmv5h.bV5GammaRed; 
//    bmv5h.bV5GammaGreen; 
//    bmv5h.bV5GammaBlue; 
//    bmv5h.bV5Intent; 
//    bmv5h.bV5ProfileData; 
//    bmv5h.bV5ProfileSize; 
//    bmv5h.bV5Reserved;*/



/*	int i,j,x,y;
	FILE* fp;
	char acFilename[32];
	for(i=0; i<this->m_apLightmapTexture.size(); i++)
	{
		sprintf(acFilename, "%s_%03d.bmp", pcBodyFileName, i);
		fp = fopen(acFilename, "w");
		//Write the header part of the BMP file
		fwrite( &bmfh, sizeof(BITMAPFILEHEADER), 1, fp );
		fwrite( &bmih, sizeof(BITMAPINFOHEADER), 1, fp );
		for(j=0;j<256;j++)
		{
			RGBQUAD rgb;	rgb.rgbBlue = rgb.rgbGreen = rgb.rgbRed = (BYTE)j;
			rgb.rgbReserved = (BYTE)0;
			fwrite( &rgb, sizeof(RGBQUAD), 1, fp );
		}
		
		BYTE col; float f;
		//Write the texture data
		for(y=0; y<m_iTextureWidth; y++)
		{
			for(x=0; x<m_iTextureWidth; x++)
			{
				SFloatRGBColor color = (m_apLightmapTexture.at(i))[ TexelAt(x,y) ];
				f = (color.fRed > color.fGreen) ? color.fRed : color.fGreen;
				f = (f > color.fBlue) ? f : color.fBlue;
				col = (BYTE)(f * 255);
				fwrite( &col, sizeof(BYTE), 1, fp );
			}
		}

		fclose(fp);
	}*/
