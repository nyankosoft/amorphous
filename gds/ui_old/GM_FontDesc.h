#ifndef  __GM_FONTDESC_H__
#define  __GM_FONTDESC_H__

#include <string>

class CGM_FontDesc
{
public:

	/// stores the name of the font
	/// used to store a texture filename for a texture font
	std::string strFontName;

	/// character width & height
	int width;
	int height;

	unsigned int type;

	enum eFontType
	{
		FONT_NORMAL,
		FONT_TEXTURE,
	};


	inline CGM_FontDesc();
	inline ~CGM_FontDesc();

};


inline CGM_FontDesc::CGM_FontDesc()
{
	width = height = 0;

	type = FONT_NORMAL;
}


inline CGM_FontDesc::~CGM_FontDesc()
{
}


#endif		/*  __GM_FONTDESC_H__  */