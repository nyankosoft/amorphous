#ifndef  __SurfaceFormat_HPP__
#define  __SurfaceFormat_HPP__


class TextureFormat
{
public:
	enum Format
	{
		Invalid = -1,
		R16F = 0,
		R32F,
		A16R16G16B16F,
		A8R8G8B8,
		X8R8G8B8,
		A1R5G5B5,
		X1R5G5B5,
		R5G6B5,
		G16R16F, ///< For variance shadow map with Direct3D
		G32R32F, ///< For variance shadow map with Direct3D
		NumFormats
	};
};


inline const char *GetSurfaceFormatName( TextureFormat::Format format )
{
	switch( format )
	{
	case TextureFormat::R16F:          return "R16F";
	case TextureFormat::R32F:          return "R32F";
	case TextureFormat::A16R16G16B16F: return "A16R16G16B16F";
	case TextureFormat::A8R8G8B8:      return "A8R8G8B8";
	case TextureFormat::X8R8G8B8:      return "X8R8G8B8";
	case TextureFormat::R5G6B5:        return "R5G6B5";
	case TextureFormat::A1R5G5B5:      return "A1R5G5B5";
	case TextureFormat::G16R16F:       return "G16R16F";
	case TextureFormat::G32R32F:       return "G32R32F";
	default:
		return "(an unknown surface format)";
	}

}


#endif		/*  __SurfaceFormat_HPP__  */
