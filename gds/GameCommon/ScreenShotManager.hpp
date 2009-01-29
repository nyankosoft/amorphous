#ifndef  __SCREENSHOTMANAGER_H__
#define  __SCREENSHOTMANAGER_H__

#include "../base.h"
#include <string>


class CScreenShotManager
{
	std::string m_ImageFileExtension;

	std::string m_ImageOutputDirectoryPath;

	int GetScreenshotFileIndex();

public:

	CScreenShotManager();
	~CScreenShotManager();

	void OutputImageDataToFile( U32 *pdwImageData, int iImageWidth, int iImageHeight );

	void SetImageFileExtension( const std::string& ext ) { m_ImageFileExtension = ext; }

	void SetImageOutputDirectoryPath( const std::string& dir_path ) { m_ImageOutputDirectoryPath = dir_path; }
};


#endif		/*  __SCREENSHOTMANAGER_H__  */
