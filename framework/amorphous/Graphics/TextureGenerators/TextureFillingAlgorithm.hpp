#ifndef __TextureFillingAlgorithm_HPP__
#define __TextureFillingAlgorithm_HPP__


#include <vector>
#include "../fwd.hpp"
#include "../FloatRGBAColor.hpp"
#include "TextureFilter.hpp"
#include "../../Support/Serialization/Serialization.hpp"
#include "../../Support/Serialization/Serialization_Color.hpp"


namespace amorphous
{
using namespace serialization;


class LockedTexture
{
public:

	virtual ~LockedTexture() {}

	virtual int GetWidth() = 0;

	virtual int GetHeight() = 0;

	virtual bool IsValid() const  = 0;

	virtual void GetPixel( int x, int y, SFloatRGBAColor& dest ) = 0;

	virtual void SetPixelARGB32( int x, int y, U32 argb_color ) = 0;

	/// \param alpha [0,255]
	virtual void SetAlpha( int x, int y, U8 alpha ) = 0;

	virtual void Clear( const SFloatRGBAColor& color ) = 0;

	virtual void Clear( U32 argb_color )
	{
		SFloatRGBAColor color;
		color.SetARGB32( argb_color );
		Clear( color );
	}
	
	SFloatRGBAColor GetPixel( int x, int y )
	{
		SFloatRGBAColor dest;
		GetPixel(x,y,dest);
		return dest;
	}
};


/// Used to fill the texture content when
/// - A texture resource is created.
/// - A texture resource is released and recreated after the graphics device is lost
class TextureFillingAlgorithm : public IArchiveObjectBase
{
public:

	// types of texture generator - used for serialization
	enum Type
	{
		TG_SINGLE_COLOR_TEXTURE_GENERATOR,
		TG_GRID_TEXTURE_GENERATOR,
		TG_HORIZONTAL_GRADATION_TEXTURE_GENERATOR,
		TG_VERTICAL_GRADATION_TEXTURE_GENERATOR,
		TG_PERLIN_NOISE_TEXTURE_GENERATOR,
		TG_UNIFORM_SINGLE_COLOR_NOISE_TEXTURE_GENERATOR,
		TG_STRIPE_TEXTURE_GENERATOR,
		TG_BUILTIN_IMAGE_TEXTURE_LOADER,
		NUM_TEXTURE_GENERATOR_TYPES
	};

	std::vector< boost::shared_ptr<TextureImageFilter> > m_pFilters;

public:

	virtual ~TextureFillingAlgorithm() {}

	/// called by the system after the texture resource is created
	virtual void FillTexture( LockedTexture& texture ) = 0;

	void AddFilter( boost::shared_ptr<TextureImageFilter> pFilter ) { m_pFilters.push_back( pFilter ); }

//	virtual unsigned int GetArchiveObjectID() const { return TG_XXX; }

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
//		ar.Polymorphic( m_pFilters );
	}
};


} // amorphous



#endif /* __TextureFillingAlgorithm_HPP__ */
