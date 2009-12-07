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


#endif		/*  __SurfaceFormat_HPP__  */
