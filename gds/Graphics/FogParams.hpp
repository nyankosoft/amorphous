#ifndef  __FogParams_HPP__
#define  __FogParams_HPP__


#include "FloatRGBAColor.hpp"


namespace amorphous
{

/*
Usage

CFogParams fog_params;
GraphicsDevice().SetFogParams( fog_params )
GraphicsDevice().Enable( RenderStateType::FOG )
*/

class FogMode
{
public:
	enum Name
	{
		LINEAR,
		EXP,
		EXP2,
		NUM_FOG_MODES
	};
};


class CFogParams
{
public:

	FogMode::Name Mode;
	SFloatRGBAColor Color;
	float Start;
	float End;
	float Density;

//	int Hint; don't care / nicest / fastest

public:

	CFogParams()
		:
	Mode( FogMode::LINEAR ),
	Color( SFloatRGBAColor::White() ),
	Start(1.0f),
	End(100.0f),
	Density(1.0f)
	{}

	virtual ~CFogParams() {}

};


} // amorphous



#endif /*  __FogParams_HPP__  */
