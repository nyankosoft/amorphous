#ifndef  __BMPIMAGEEXPORTER_H__
#define  __BMPIMAGEEXPORTER_H__

#include <windows.h>

class CBMPImageExporter
{

public:
	CBMPImageExporter();
	~CBMPImageExporter();

	bool OutputImage_24Bit( const char *pcFilename,
		                    int iImageWidth,
							int iImageHeight,
							DWORD *padwImageData );

};



#endif		/*  __BMPIMAGEEXPORTER_H__  */