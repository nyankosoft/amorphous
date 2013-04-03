#ifndef __TextureFilter_HPP__
#define __TextureFilter_HPP__


#include "TextureFillingAlgorithm.hpp"
#include "../GraphicsResourceDescs.hpp"
#include "../../Support/Log/DefaultLog.hpp"
#include "../../Support/Serialization/Serialization.hpp"
#include "../../Support/Serialization/Serialization_BoostSmartPtr.hpp"


namespace amorphous
{
using namespace serialization;


class TextureFilter : public IArchiveObjectBase
{
public:

	TextureFilter(){}

	virtual ~TextureFilter(){}

	virtual void ApplyFilter( LockedTexture& texture ) = 0;
};


//class ContrastFilter : public TextureFilter
//{
//public:
//
//	ContrastFilter( ??? )
//
//	void ApplyFilter( LockedTexture& texture )
//	{
//	}
//};


class ContrastBrightnessFilter : public TextureFilter
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



#endif /* __TextureFilter_HPP__ */
