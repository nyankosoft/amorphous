#ifndef  __GM_CAPTIONPARAMETERS_H__
#define  __GM_CAPTIONPARAMETERS_H__


#include "GM_GraphicElement.hpp"

#include <vector>
using namespace std;


class CGM_CaptionRenderRoutine;


class CGM_CaptionParameters
{
public:

	/// font for the caption
//	CGM_FontElement Font;
	int iFontIndex;
	SFloatRGBAColor FontColor;

	/// background rectangle
	CGM_TextureRectElement Rect;

	/// top-left corner of the caption text
	float fPosX, fPosY;

	/// holds a pointer to the caption render routine implemented by the user.
	/// This is for users who like to render caption texts in a particular way.
	/// set this to NULL to draw captions as simple texts
	CGM_CaptionRenderRoutine *pCaptionRenderRoutine;

public:

	CGM_CaptionParameters() { SetDefault(); }
//	~CGM_CaptionParameters() {}

	inline void SetDefault();

};


// =============================== inline implementations ===============================

inline void CGM_CaptionParameters::SetDefault()
{
	iFontIndex = 0;
	FontColor.SetRGBA( 0.0f, 0.0f, 0.0f, 0.0f );

	fPosX = fPosY = 0;

	pCaptionRenderRoutine = NULL;
}



#endif		/*  __GM_CAPTIONPARAMETERS_H__  */
