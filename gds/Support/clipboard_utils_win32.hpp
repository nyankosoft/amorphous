#ifndef __clipboard_utils_win32_HPP__
#define __clipboard_utils_win32_HPP__


#include <Windows.h>
#include <string>


namespace amorphous
{


inline void get_from_clipboard( std::string& text )
{
	if( !OpenClipboard( NULL ) )
	{
		printf( "Cannot open the Clipboard.\n" );
		return;
	}

	// For the appropriate data formats...
	HANDLE hClipboardData = ::GetClipboardData( CF_TEXT );
	if( !hClipboardData )
		return;

//	LPTSTR lptstr = GlobalLock( hClipboardData );
	const char *lptstr = (const char *)GlobalLock( hClipboardData );
	if( !lptstr )
		return;

	text = lptstr;

	GlobalUnlock(hClipboardData);

	CloseClipboard();
}


inline std::string get_text_from_clipboard()
{
	std::string text;
	get_from_clipboard( text );
	return text;
}


inline void send_to_clipboard( const std::string& text )
{
	if( text.length() == 0 )
		return;

	if( !OpenClipboard( NULL ) )
	{
		printf( "Cannot open the Clipboard.\n" );
		return;
	}

	// Remove the current Clipboard contents
	if( !EmptyClipboard() )
	{
		printf( "Cannot empty the Clipboard.\n" );
		return;
	}

	// Get the currently selected data
	HGLOBAL hGlob = GlobalAlloc( GMEM_FIXED, text.length()+1 );
	strcpy_s( (char*)hGlob, text.length()+1, text.c_str() );

	// For the appropriate data formats...
	if( ::SetClipboardData( CF_TEXT, hGlob ) == NULL )
	{
		printf( " Failed to set Clipboard data, error: %d", GetLastError() );
		CloseClipboard();
		GlobalFree(hGlob);
		return;
	}

	CloseClipboard();
}


} // amorphous



#endif /* __clipboard_utils_win32_HPP__ */
