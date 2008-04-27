//====================================================================
//  FileOpenDialog_Win32.h
//  standard file selection dialog by windows API
//====================================================================

#ifndef __FILE_OPEN_DIALOG_WIN32_VC8_H__
#define __FILE_OPEN_DIALOG_WIN32_VC8_H__


#include <stdio.h>
#include <windows.h>
#include <commdlg.h>

#include <string>

std::wstring to_wstr( char* p )
{
	std::wstring dest;
	size_t i, len = strlen(p);
	for( i=0; i<len; i++ )
		dest.push_back( p[i] );

	return dest;
}

std::string to_string( wchar_t* p )
{
	std::string dest;
	size_t i, len = wcslen(p);
	for( i=0; i<len; i++ )
		dest.push_back( p[i] );

	return dest;
}


#define FILE_OPEN_DIALOG_MAX_FILENAMES_LENGTH 1024


/// open a dialog to get a filename
/// filename is stored in 'pcDestFilename'
/// reutrns false is no filename is selected
/// this could change working directory - check using getcwd() before & after the call
/// to see the effect
inline bool GetFilename( LPSTR pcDestFilename, LPSTR pcFilterString = NULL )
{
	WCHAR filter[64];
	WCHAR acTempFilename[FILE_OPEN_DIALOG_MAX_FILENAMES_LENGTH];
	memset( acTempFilename, 0, sizeof(acTempFilename) );

	if( pcFilterString )
		wcscpy( filter, to_wstr(pcFilterString).c_str() );
	else
		wcscpy( filter, L"" );

	OPENFILENAME ofn;

	// fill out 'ofn'
	memset(&ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = acTempFilename;
	ofn.nMaxFile = FILE_OPEN_DIALOG_MAX_FILENAMES_LENGTH;
//	wcscpy(pcDestFilename, L"");

	if( !(GetOpenFileName(&ofn)) )
	{
		// no filename was selected
		return false;
	}

	strcpy( pcDestFilename, to_string(acTempFilename).c_str() );

	return true;
}



#include <string>
#include <vector>


inline bool GetFilename( std::string& strDestFilename, LPSTR pcFilterString = NULL )
{
	char acTempFilename[FILE_OPEN_DIALOG_MAX_FILENAMES_LENGTH];

	bool selected = GetFilename( acTempFilename, pcFilterString );

	if( selected )
	{
		strDestFilename = acTempFilename;
	}

	return selected;
}


// returns selected filenames
// - when a single file is selected
//   - the first string (vecstrDestFilename[0]) contains path & filename
// - when multiple files are selected
//   - the first string (vecstrDestFilename[0]) holds the path
//   - the subsequent strings (vecstrDestFilename[1] ... [n]) hold filenames
/*
inline bool GetFilenames( std::vector<std::string>& vecstrDestFilename, LPSTR pcFilterString = NULL )
{
	LPWSTR pcFileNameBuffer = new wchar_t [4096];
	pcFileNameBuffer[0] = '\0';

	OPENFILENAME ofn;

	// fill out 'ofn'
	memset(&ofn, 0, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.lpstrFilter = pcFilterString;
	ofn.lpstrFile = pcFileNameBuffer;
	ofn.nMaxFile = 4095;
//	strcpy(pcDestFilename, "");

	if( !(GetOpenFileName(&ofn)) )
	{
		// no filename was selected
		delete [] pcFileNameBuffer;
		return false;
	}

	// get path
	vecstrDestFilename.push_back( std::string() );
	vecstrDestFilename.back() = pcFileNameBuffer;

	size_t pos = wcslen(pcFileNameBuffer);

	// 'pos' is set to the NULL character before the first filename

	while(1)
	{
		if( pcFileNameBuffer[pos] == '\0' )
		{
			if( pcFileNameBuffer[pos+1] == '\0' )
				break;	// found two consecutive NULL characters - reached the end of the buffer
			else
			{
				vecstrDestFilename.push_back( std::string() );
				vecstrDestFilename.back() = &pcFileNameBuffer[pos+1];
			}
		}
		pos++;
	}

	delete [] pcFileNameBuffer;
	return true;
}
*/

#endif  /*  __FILE_OPEN_DIALOG_WIN32_VC8_H__  */
