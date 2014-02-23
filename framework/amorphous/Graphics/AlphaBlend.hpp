#ifndef __ALPHABLEND_H__
#define __ALPHABLEND_H__


namespace amorphous
{

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

} // namespace amorphous


#endif  /* __ALPHABLEND_H__ */
