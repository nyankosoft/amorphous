#ifndef __TextureStage_HPP__
#define __TextureStage_HPP__


#include "../base.hpp"


namespace amorphous
{


class TexStageArg
{
public:
	enum Name
	{
		PREV,
		DIFFUSE,
		TEXTURE,
		NUM_ARGS
	};
};


class TexStageOp
{
public:
	enum Name
	{
		SELECT_ARG0,
		SELECT_ARG1,
		MODULATE,
		DISABLE,
		NUM_OPERATIONS
	};
};


class CTextureStage
{
public:

	CTextureStage()
		:
	ColorOp(TexStageOp::SELECT_ARG0),
	ColorArg0(TexStageArg::TEXTURE), ColorArg1(TexStageArg::TEXTURE),
	AlphaOp(TexStageOp::SELECT_ARG0),
	AlphaArg0(TexStageArg::TEXTURE), AlphaArg1(TexStageArg::TEXTURE)
	{}

	~CTextureStage() {}

	TexStageOp::Name ColorOp;
	TexStageArg::Name ColorArg0, ColorArg1;

	TexStageOp::Name AlphaOp;
	TexStageArg::Name AlphaArg0, AlphaArg1;
};


class CTextureTransformParams
{
public:
	CTextureTransformParams() : NumElements(0) {}
	~CTextureTransformParams(){}

	uint NumElements;
};



} // amorphous



#endif /* __TextureStage_HPP__ */
