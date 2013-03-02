#include "BMPImageExporter.hpp"

#include <stdio.h>
#include <wingdi.h>


/*
	// Save a Direct3D texture to an image file.

	CBMPImageExporter bmp_exporter;
	DWORD *pdwTexelData;

	LPDIRECT3DTEXTURE9 pTex = ...;

	D3DLOCKED_RECT locked_rect;
	pTex->LockRect( 0, &locked_rect, NULL, 0 );
	pdwTexelData = (DWORD *)locked_rect.pBits;

	bmp_exporter.OutputImage_24Bit( filename, m_TextureDesc.Width, m_TextureDesc.Height, pdwTexelData );

	pTex->UnlockRect( 0 );
*/


namespace amorphous
{

CBMPImageExporter::CBMPImageExporter(){}

CBMPImageExporter::~CBMPImageExporter(){}


bool CBMPImageExporter::OutputImage_24Bit( const char *pcFilename,
										   int iImageWidth,
										   int iImageHeight,
										   DWORD *padwImageData )
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

	FILE *fp = fopen( pcFilename, "wb" );

	if( !fp )
		return false;

	fwrite( &bmfh, sizeof(BITMAPFILEHEADER), 1, fp );
	fwrite( &bmih, sizeof(BITMAPINFOHEADER), 1, fp );

	int x,y;
	BYTE red, green, blue;
	for( y=iImageHeight-1; 0<=y; y-- )
	{
		for( x=0; x<iImageWidth; x++ )
		{
			red   = (BYTE)( 0x000000FF & (padwImageData[y * iImageWidth + x] >> 16) );
			green = (BYTE)( 0x000000FF & (padwImageData[y * iImageWidth + x] >>  8) );
			blue  = (BYTE)( 0x000000FF &  padwImageData[y * iImageWidth + x] );

			fwrite( &blue,  sizeof(BYTE), 1, fp );
			fwrite( &green, sizeof(BYTE), 1, fp );
			fwrite( &red,   sizeof(BYTE), 1, fp );
		}
	}

	fclose(fp);

	return true;
}

} // namespace amorphous
