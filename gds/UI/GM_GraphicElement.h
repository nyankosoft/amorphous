
#ifndef  __GM_GRAPHICELEMENT_H__
#define  __GM_GRAPHICELEMENT_H__


#include "GM_Control.h"

#include "Graphics/FloatRGBAColor.h"


class CGM_Dialog;


//========================================================================================
// CGM_BlendColor
//========================================================================================

class CGM_BlendColor
{
public:
//	void Init( D3DCOLOR defaultColor, D3DCOLOR disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), D3DCOLOR hiddenColor = 0 );
	void Blend( int iState, float fElapsedTime, float fRate = 0.7f );
    
	/// holds colors for all possible control states and used to modulate them
	SFloatRGBAColor m_aColor[ CGM_Control::NUM_CONTROL_STATES ];

    SFloatRGBAColor m_CurrentColor;
};



//========================================================================================
// CGM_FontElement
//========================================================================================

class CGM_FontElement
{
public:

    /// index to the cached font in dialog manager
	int m_FontIndex;

	CGM_BlendColor m_Color;

public:

	CGM_FontElement() : m_FontIndex(0) {}

};


class CGM_TextureRectElement
{
public:

	SRect m_Rect;

	/// index to the cached texture in dialog manager
	int m_TextureIndex;

	CGM_BlendColor m_Color;

public:

	CGM_TextureRectElement() : m_TextureIndex(-1) {}
};



/*
class CGM_GraphicElement
{
public:

	CGM_FontElement Font
	CGM_TextureRectElement TexRect

public:
	CGM_GraphicElement();
	~CGM_GraphicElement() {}
};
*/


#endif		/*  __GM_GRAPHICELEMENT_H__  */


