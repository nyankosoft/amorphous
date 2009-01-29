#ifndef  __WINDOWMISC_H__
#define  __WINDOWMISC_H__


#include <windows.h>


inline void GetNonClientAreaSize( HWND hWnd, long& frame_width, long& frame_height )
{
	RECT rect;
	int ww, wh, cw, ch;

	// calc boundary width outside the client rect
	GetClientRect(hWnd, &rect);		// クライアント部分のサイズの取得
	cw = rect.right - rect.left;	// クライアント領域外の横幅を計算
	ch = rect.bottom - rect.top;	// クライアント領域外の縦幅を計算

	// calc the entire window size
	GetWindowRect(hWnd, &rect);		// ウインドウ全体のサイズ取得
	ww = rect.right - rect.left;	// ウインドウ全体の幅の横幅を計算
	wh = rect.bottom - rect.top;	// ウインドウ全体の幅の縦幅を計算
	frame_width  = ww - cw;			// クライアント領域以外に必要な幅
	frame_height = wh - ch;			// クライアント領域以外に必要な高さ
}


inline long GetNonClientAreaWidth( HWND hWnd )
{
	RECT rect, client_rect;
	GetClientRect(hWnd, &client_rect);
	GetWindowRect(hWnd, &rect);

	return (rect.right - rect.left) - (client_rect.left - client_rect.right);
}


inline long GetNonClientAreaHeight( HWND hWnd )
{
	RECT rect, client_rect;
	GetClientRect(hWnd, &client_rect);
	GetWindowRect(hWnd, &rect);

	return (rect.bottom - rect.top) - (client_rect.bottom - client_rect.top);
}


/**
 * change the size of the client area.
 * call this function after CreateWindow() to adjust the resolution of the client area
 * in windowed mode.
 */
inline void ChangeClientAreaSize( HWND hWnd, int new_width, int new_height )
{
	long frame_width = 0, frame_height = 0;

	GetNonClientAreaSize( hWnd, frame_width, frame_height );

	// change window size
	BOOL b = SetWindowPos( hWnd, HWND_TOP, 0, 0, new_width + frame_width, new_height + frame_height, SWP_NOMOVE );
}


#endif  /*  __WINDOWMISC_H__  */
