#ifndef __ALPHABLEND_H__
#define __ALPHABLEND_H__


class AlphaBlend
{
public:
	enum Mode
	{
		Zero,
		One,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSaturate,
		NumModes
	};
};

class TextureAddress
{
public:
	enum Mode
	{
		Wrap,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,
		NumModes
	};
};



#endif  /* __ALPHABLEND_H__ */
