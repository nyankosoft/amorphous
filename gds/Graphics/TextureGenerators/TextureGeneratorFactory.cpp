#include "TextureGeneratorFactory.hpp"
#include "SingleColorTextureGenerator.hpp"
#include "GridTextureGenerator.hpp"
#include "GradationTextureGenerators.hpp"
#include "PerlinNoiseTextureGenerator.hpp"
#include "NoiseTextureGenerators.hpp"
#include "BuiltinImageTextureLoader.hpp"


namespace amorphous
{


IArchiveObjectBase *TextureGeneratorFactory::CreateObject(const unsigned int id)
{
	switch( id )
	{
	case TextureFillingAlgorithm::TG_SINGLE_COLOR_TEXTURE_GENERATOR:               return new SingleColorTextureGenerator;
	case TextureFillingAlgorithm::TG_GRID_TEXTURE_GENERATOR:                       return new GridTextureGenerator;
	case TextureFillingAlgorithm::TG_HORIZONTAL_GRADATION_TEXTURE_GENERATOR:       return new HorizontalGradationTextureGenerator;
	case TextureFillingAlgorithm::TG_VERTICAL_GRADATION_TEXTURE_GENERATOR:         return new VerticalGradationTextureGenerator;
	case TextureFillingAlgorithm::TG_PERLIN_NOISE_TEXTURE_GENERATOR:               return new PerlinNoiseTextureGenerator;
	case TextureFillingAlgorithm::TG_UNIFORM_SINGLE_COLOR_NOISE_TEXTURE_GENERATOR: return new UniformSingleColorNoiseTextureGenerator;
	case TextureFillingAlgorithm::TG_STRIPE_TEXTURE_GENERATOR:                     return new StripeTextureGenerator;
//	case TextureFillingAlgorithm::TG_BUILTIN_IMAGE_TEXTURE_LOADER:                 return new BuiltinImageTextureLoader;
	default:
		return NULL;
	}
}


} // amorphous
