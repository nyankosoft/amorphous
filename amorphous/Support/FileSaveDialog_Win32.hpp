//====================================================================
//  FileOpenDialog_Win32.hpp
//  opens a dialog box for specifying a file to save
//====================================================================

#ifndef __FILE_SAVE_DIALOG_WIN32_H__
#define __FILE_SAVE_DIALOG_WIN32_H__


#include <stdio.h>
#include <windows.h>
#include <commdlg.h>
#include <string>


namespace amorphous
{

#define FILE_SAVE_DIALOG_MAX_FILENAMES_LENGTH 1024


/// open a dialog to get a filename
/// filename is stored in 'pcDestFilename'
/// reutrns false is no filename is selected
inline bool GetSaveFileName( char* pcDestFilename, char* pcFilterString = NULL )
{
	// ofn.lpstrFile should be NULL if no initialization for File Name edit control
	pcDestFilename[0] = '\0';

	OPENFILENAME ofn;

	// fill out 'ofn'
	memset(&ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrFilter = pcFilterString;
	ofn.lpstrFile = pcDestFilename;
	ofn.nMaxFile = FILE_SAVE_DIALOG_MAX_FILENAMES_LENGTH;
	strcpy(pcDestFilename, "");

	if( !(GetSaveFileName(&ofn)) )
	{
		// no filename was selected
		return false;
	}

	return true;
}


inline bool GetSaveFileName( std::string strDestFilename, char* pcFilterString = NULL )
{
	char acTempFilename[FILE_SAVE_DIALOG_MAX_FILENAMES_LENGTH];

	acTempFilename[0] = '\0';

	bool selected = GetSaveFileName( acTempFilename, pcFilterString );

	if( selected )
	{
		strDestFilename = acTempFilename;
	}

	return selected;
}

} // amorphous



#endif  /*  __FILE_SAVE_DIALOG_WIN32_H__  */
