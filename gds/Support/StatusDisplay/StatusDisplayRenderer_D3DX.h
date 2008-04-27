
#ifndef  __STATUSDISPLAYRENDERER_D3DX_H__
#define  __STATUSDISPLAYRENDERER_D3DX_H__

#include "StatusDisplayRenderer.h"


class CFont;


class CStatusDisplayRenderer_D3DX : public CStatusDisplayRenderer
{

	CFont *m_pFont;

public:

	CStatusDisplayRenderer_D3DX();
	~CStatusDisplayRenderer_D3DX();

	void InitFont( const char *pcFontName, int width, int height );
	void Render( CStatusDisplay* pStatusDisplay );

};



#endif		/*  __STATUSDISPLAYRENDERER_D3DX_H__  */