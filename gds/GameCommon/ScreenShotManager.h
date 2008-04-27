
#ifndef  __SCREENSHOTMANAGER_H__
#define  __SCREENSHOTMANAGER_H__

#include "../base.h"


class CScreenShotManager
{

	int GetScreenshotFileIndex();


public:
	CScreenShotManager();
	~CScreenShotManager();

	void OutputImageDataToFile( U32 *pdwImageData, int iImageWidth, int iImageHeight );

};



#endif		/*  __SCREENSHOTMANAGER_H__  */
