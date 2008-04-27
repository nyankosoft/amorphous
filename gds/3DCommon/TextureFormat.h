#ifndef  __TextureFormat_H__
#define  __TextureFormat_H__


#include "Support/Log/DefaultLog.h"


class TextureFormat
{
public:
	enum Format
	{
		R16F,
		A16R16G16B16F,
		A8R8G8B8,
		X8R8G8B8,
		R5G6B5,
		NUM_FORMATS
	};
};


inline D3DFORMAT ConvertTextureFormatToD3DFORMAT( TextureFormat::Format tex_format )
{
	D3DFORMAT d3d_fmt;

	switch(tex_format)
	{
	case TextureFormat::R16F:          d3d_fmt = D3DFMT_R16F;
	case TextureFormat::A16R16G16B16F: d3d_fmt = D3DFMT_A16B16G16R16F;
	case TextureFormat::A8R8G8B8:      d3d_fmt = D3DFMT_A8R8G8B8;
	case TextureFormat::X8R8G8B8:      d3d_fmt = D3DFMT_X8R8G8B8;
	case TextureFormat::R5G6B5:        d3d_fmt = D3DFMT_R5G6B5;
	default:
		LOG_PRINT_ERROR( "invalid texture format" );
		return D3DFMT_A8R8G8B8;
	}
	
	return d3d_fmt;
}


#endif		/*  __TextureFormat_H__  */
