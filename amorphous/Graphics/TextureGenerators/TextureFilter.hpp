#ifndef __TextureImageFilter_HPP__
#define __TextureImageFilter_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../GraphicsResourceDescs.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/Serialization/Serialization.hpp"
#include "../../Support/Serialization/Serialization_BoostSmartPtr.hpp"


namespace amorphous
{
using namespace serialization;


class TextureImageFilter : public IArchiveObjectBase
{
public:

	TextureImageFilter(){}

	virtual ~TextureImageFilter(){}

	virtual void ApplyFilter( LockedTexture& texture ) = 0;
};


//class ContrastFilter : public TextureImageFilter
//{
//public:
//
//	ContrastFilter( ??? )
//
//	void ApplyFilter( LockedTexture& texture )
//	{
//	}
//};


class ContrastBrightnessFilter : public TextureImageFilter
{
	float m_ContrastFactor;
	float m_BrightnessShift;

public:

	ContrastBrightnessFilter()
		:
	m_ContrastFactor(0),
	m_BrightnessShift(0)
	{}

	ContrastBrightnessFilter( float contrast_factor, float brightness_shift )
		:
	m_ContrastFactor(contrast_factor),
	m_BrightnessShift(brightness_shift)
	{}

	void ApplyFilter( LockedTexture& texture );

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_ContrastFactor;
		ar & m_BrightnessShift;
	}
};


} // amorphous



#endif /* __TextureImageFilter_HPP__ */
