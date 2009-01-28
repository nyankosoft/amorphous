
#ifndef  __GM_GRAPHICELEMENT_H__
#define  __GM_GRAPHICELEMENT_H__


#include "GM_Control.h"

#include "Graphics/FloatRGBAColor.h"
#include "Graphics/2DRect.h"


class CGM_Dialog;


enum eGM_ControlState
{
	GM_STATE_NORMAL = 0,
    GM_STATE_DISABLED,
    GM_STATE_HIDDEN,
    GM_STATE_FOCUS,
    GM_STATE_MOUSEOVER,
    GM_STATE_PRESSED
};


#define NUM_MAX_CONTROL_STATES	16



//========================================================================================
// CGM_BlendColor
//========================================================================================

class CGM_BlendColor
{
public:
//	void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
	void Blend( int iState, float fElapsedTime, float fRate = 0.7f );
    
    SFloatRGBAColor m_aColor[ NUM_MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
    SFloatRGBAColor m_CurrentColor;
};



//========================================================================================
// CGM_FontElement
//========================================================================================

class CGM_FontElement
{
public:
	CGM_FontElement() { m_iFontIndex = 0; }

	int m_iFontIndex;		// index to the cached font in dialog manager
	CGM_BlendColor m_FontColor;

};


class CGM_RectRenderRoutine;

class CGM_TextureRectElement
{
	C2DRect m_2DRect;

	SRect m_Rect;

	void UpdateRect();

public:

	/// index to the cached texture in dialog manager
	int m_iTextureIndex;

	CGM_BlendColor m_RectColor;

	CGM_RectRenderRoutine *m_pRenderRoutine;

public:

	CGM_TextureRectElement() { m_iTextureIndex = -1; m_pRenderRoutine = NULL; }

	void Draw( CGM_Dialog *pDialog );

	SRect& GetRect() { return m_Rect; }

	void SetPosition( float fPosX, float fPosY, float fWidth, float fHeight );

	void SetRenderRoutine( CGM_RectRenderRoutine *pRenderRoutine ) { m_pRenderRoutine = pRenderRoutine; }
};



#endif		/*  __GM_GRAPHICELEMENT_H__  */






//#include <d3d9types.h>



/*
class CGM_GraphicElement
{
public:
	int m_iFont;
	int m_iTexture;

	CGM_BlendColor m_FontColor;
	CGM_BlendColor m_TextureColor;

public:
	CGM_GraphicElement();
	~CGM_GraphicElement() {}

	void SetFont( int iFont, D3DCOLOR defaultFontColor, DWORD dwTextFormat );
//	void SetTexture( UINT iTexture, RECT* prcTexture, D3DCOLOR defaultTextureColor );

};
*/

/*
class CGM_GraphicElementHolder
{
public:
	int control_type;
	int iElement;
	CGM_GraphicElement element;
};
*/