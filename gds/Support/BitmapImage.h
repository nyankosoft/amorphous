#ifndef __BitmapImage_H__
#define __BitmapImage_H__


#include <string>
#include <boost/shared_ptr.hpp>
#include "FreeImage.h"

#include "3DCommon/FloatRGBColor.h"
#include "Support/Log/DefaultLog.h"

#pragma comment( lib, "FreeImage.lib" )


class CBitmapImage
{
	FIBITMAP* m_pFreeImageBitMap;

public:

	CBitmapImage() {}

	inline CBitmapImage( int width, int height, int bpp );

//	inline CBitmapImage( const C2DArray<SFloatRGBColor>& texel_buffer, int bpp );

	~CBitmapImage() { FreeImage_Unload( m_pFreeImageBitMap ); }

	inline bool LoadFromFile( const std::string& pathname, int flag = 0 );

	inline bool SaveToFile( const std::string& pathname, int flag = 0 );

	inline void SetPixel( int x, int y, const SFloatRGBColor& color );

	FIBITMAP *GetFBITMAP() { return m_pFreeImageBitMap; }

	inline int GetWidth() const;

	inline int GetHeight() const;
};

/*
inline CBitmapImage::CBitmapImage( const C2DArray<SFloatRGBColor>& texel_buffer, int bpp )
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}
*/


inline CBitmapImage::CBitmapImage( int width, int height, int bpp )
{
	m_pFreeImageBitMap = FreeImage_Allocate( width, height, bpp );
}


inline void CBitmapImage::SetPixel( int x, int y, const SFloatRGBColor& color )
{
	RGBQUAD quad;
	quad.rgbRed   = color.GetRedByte();
	quad.rgbGreen = color.GetGreenByte();
	quad.rgbBlue  = color.GetBlueByte();

	FreeImage_SetPixelColor( m_pFreeImageBitMap, x, y, &quad );
}


inline int CBitmapImage::GetWidth() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetWidth(m_pFreeImageBitMap);
	else
		return 0;
}


inline int CBitmapImage::GetHeight() const
{
	if( m_pFreeImageBitMap )
		return (int)FreeImage_GetHeight(m_pFreeImageBitMap);
	else
		return 0;
}


//	FreeImage_SetOutputMessage(GDS_FreeImageErrorHandler);


// ----------------------------------------------------------

/** Generic image loader
	@param pathname Pointer to the full file name
	@param flag Optional load flag constant
	@return Returns the loaded dib if successful, returns NULL otherwise
*/
//FIBITMAP* GenericLoader( const std::string& pathname, int flag )
inline bool CBitmapImage::LoadFromFile( const std::string& pathname, int flag )
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType( pathname.c_str(), 0 );
	if(fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
	}

	// check that the plugin has reading capabilities ...
	if( (fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) )
	{
		// ok, let's load the file
		m_pFreeImageBitMap = FreeImage_Load(fif, pathname.c_str(), flag);

		// unless a bad file format, we are done !
		if( m_pFreeImageBitMap )
			return true;
		else
			return false;
	}

	return false;
}

/** Generic image writer
	@param dib Pointer to the dib to be saved
	@param pathname Pointer to the full file name
	@param flag Optional save flag constant
	@return Returns true if successful, returns false otherwise
*/
inline bool CBitmapImage::SaveToFile( const std::string& pathname, int flag )
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BOOL bSuccess = FALSE;

	if(m_pFreeImageBitMap)
	{
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename( pathname.c_str() );
		if(fif != FIF_UNKNOWN )
		{
			// check that the plugin has sufficient writing and export capabilities ...
			WORD bpp = FreeImage_GetBPP(m_pFreeImageBitMap);
			if(FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp))
			{
				// ok, we can save the file
				bSuccess = FreeImage_Save(fif, m_pFreeImageBitMap, pathname.c_str(), flag);
				LOG_PRINT( "Saved an image file to disk: " + pathname );

				// unless an abnormal bug, we are done !
			}
		}
		else
			LOG_PRINT_ERROR( "Failed to save an image file to disk: " + pathname );
	}

	return (bSuccess == TRUE) ? true : false;
}


//
// Global Functions
//

inline boost::shared_ptr<CBitmapImage> CreateBitmapImage( const std::string& pathname, int flag = 0 )
{
	boost::shared_ptr<CBitmapImage> pImage
		= boost::shared_ptr<CBitmapImage>( new CBitmapImage() );

	bool bSuccess = pImage->LoadFromFile( pathname, flag );

	return pImage;
}


inline void GDS_FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char *message )
{
	if( fif != FIF_UNKNOWN )
	{
		g_Log.Print( "Free Image: %s Format", FreeImage_GetFormatFromFIF(fif) );
	}

	g_Log.Print( "Free Image: %s", message );
}


#endif /* __BitmapImage_H__ */
