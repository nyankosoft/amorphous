#ifndef  __D3DSurfaceFormat_HPP__
#define  __D3DSurfaceFormat_HPP__


#include "../SurfaceFormat.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <d3dx9.h>


namespace amorphous
{


inline TextureFormat::Format FromD3DSurfaceFormat( D3DFORMAT d3d_fmt )
{
	switch(d3d_fmt)
	{
	case D3DFMT_R16F:          return TextureFormat::R16F;
	case D3DFMT_R32F:          return TextureFormat::R32F;
	case D3DFMT_A16B16G16R16F: return TextureFormat::A16R16G16B16F;
	case D3DFMT_A8R8G8B8:      return TextureFormat::A8R8G8B8;
	case D3DFMT_X8R8G8B8:      return TextureFormat::X8R8G8B8;
	case D3DFMT_R5G6B5:        return TextureFormat::R5G6B5;
	case D3DFMT_A1R5G5B5:      return TextureFormat::A1R5G5B5;
	case D3DFMT_X1R5G5B5:      return TextureFormat::X1R5G5B5;
	default:
		LOG_PRINT_ERROR( "An invalid D3DFORMAT" );
		return TextureFormat::Invalid;
	}
}


inline const char *GetD3DSurfaceFormatName( D3DFORMAT d3d_fmt )
{
	switch(d3d_fmt)
	{
	case D3DFMT_R16F:          return "D3DFMT_R16F";
	case D3DFMT_R32F:          return "D3DFMT_R32F";
	case D3DFMT_A16B16G16R16F: return "D3DFMT_A16B16G16R16F";
	case D3DFMT_A8R8G8B8:      return "D3DFMT_A8R8G8B8";
	case D3DFMT_X8R8G8B8:      return "D3DFMT_X8R8G8B8";
	case D3DFMT_R5G6B5:        return "D3DFMT_R5G6B5";
	case D3DFMT_A1R5G5B5:      return "D3DFMT_A1R5G5B5";
	case D3DFMT_X1R5G5B5:      return "D3DFMT_X1R5G5B5";
	default:
		return "An unknown D3DFORMAT";
	}
}


inline D3DFORMAT ConvertTextureFormatToD3DFORMAT( TextureFormat::Format tex_format )
{
	switch(tex_format)
	{
	case TextureFormat::R16F:          return D3DFMT_R16F;
	case TextureFormat::R32F:          return D3DFMT_R32F;
	case TextureFormat::A16R16G16B16F: return D3DFMT_A16B16G16R16F;
	case TextureFormat::A8R8G8B8:      return D3DFMT_A8R8G8B8;
	case TextureFormat::X8R8G8B8:      return D3DFMT_X8R8G8B8;
	case TextureFormat::R5G6B5:        return D3DFMT_R5G6B5;
	case TextureFormat::A1R5G5B5:      return D3DFMT_A1R5G5B5;
	case TextureFormat::G16R16F:       return D3DFMT_G16R16F;
	case TextureFormat::G32R32F:       return D3DFMT_G32R32F;
	default:
		LOG_PRINT_ERROR( "An invalid texture format" );
		return D3DFMT_A8R8G8B8;
	}
}


inline D3DXIMAGE_FILEFORMAT GetD3DXImageFormatFromFileExt( const std::string& file_ext )
{
	if( file_ext == "bmp" ) return D3DXIFF_BMP;
	else if( file_ext == "jpg" ) return D3DXIFF_JPG;
	else if( file_ext == "tga" ) return D3DXIFF_TGA;
	else if( file_ext == "png" ) return D3DXIFF_PNG;
	else if( file_ext == "dds" ) return D3DXIFF_DDS;
	else if( file_ext == "ppm" ) return D3DXIFF_PPM;
	else if( file_ext == "dib" ) return D3DXIFF_DIB;
	else if( file_ext == "hdr" ) return D3DXIFF_HDR;
	else if( file_ext == "pfm" ) return D3DXIFF_PFM;
	else
	{
		LOG_PRINT_ERROR( " An unsupported image file format" );
		return D3DXIFF_BMP; // save as bmp image by default
	}
}


inline D3DXIMAGE_FILEFORMAT GetD3DXImageFormatFromFilepath( const std::string& pathname )
{
	if( pathname.length() <= 4 )
	{
		LOG_PRINT_ERROR( " An invalid pathname for an image file." );
		return D3DXIFF_BMP;
	}

	const std::string ext = pathname.substr( pathname.length() - 3 );
	return GetD3DXImageFormatFromFileExt( ext );
}

} // namespace amorphous



#endif		/*  __SurfaceFormat_HPP__  */
