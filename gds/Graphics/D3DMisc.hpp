#ifndef  __D3DMISC_H__
#define  __D3DMISC_H__


#include "Direct3D/Direct3D9.hpp"


inline void GetViewportSize( int& width, int& height )
{
	D3DVIEWPORT9 viewport;
	DIRECT3D9.GetDevice()->GetViewport( &viewport );
	width  = (int)viewport.Width;
	height = (int)viewport.Height;
}


inline void GetViewportSize( float& width, float& height )
{
	D3DVIEWPORT9 viewport;
	DIRECT3D9.GetDevice()->GetViewport( &viewport );
	width  = (float)viewport.Width;
	height = (float)viewport.Height;
}


#endif  /*  __D3DMISC_H__  */
