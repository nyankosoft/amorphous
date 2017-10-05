#include "ParticleTextureGenerator.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/GraphicsResources.hpp"
#include "amorphous/3DMath/Vector2.hpp"
#include "amorphous/3DMath/Gaussian.hpp"
#include "amorphous/3DMath/Perlin.hpp"
#include "amorphous/Support/MTRand.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include <ctime>
#include <random>


namespace amorphous
{


void ParticleTextureGenerator::FillTexture( LockedTexture& texture )
{
	int w = texture.GetWidth();
	int h = texture.GetHeight();

	//		array2d<float> buffer;
	//		buffer.resize( w, h, 0.0f );

	float threshold = 0.001f;
	float rho = m_fStandardDeviation;
	float r = GetGaussianDistributionEffectiveRadius(m_fStandardDeviation,threshold);

	SFloatRGBAColor base_color = m_Color;
	SFloatRGBAColor c;
	float base_alpha = m_Color.alpha;
	for( int y=0; y<h; y++ )
	{
		float fy = (float)y / (float)h - 0.5f; // [-0.5,0.5)
		fy *= 2.0f * r;

		for( int x=0; x<w; x++ )
		{
			float fx = (float)x / (float)w - 0.5f; // [-0.5,0.5)
			fx *= 2.0f * r;

			float g = GaussianDistribution( fx, fy, rho );
			c.red   = base_color.red   * g;
			c.green = base_color.green * g;
			c.blue  = base_color.blue  * g;
			c.alpha = 0;
			texture.SetPixel( x, y, c );
		}
	}
}


void SmokeParticleTextureGenerator::CreateTile( LockedTexture& texture, int sx, int sy, int tile_width, int tile_height, int seed )
{
//	int w = texture.GetWidth();
//	int h = texture.GetHeight();

	Perlin perlin( 10, 7, 1, seed );

	// The results of Perlin::Get(x,y) falls within a particular range depending on parameter values?
	// octaves, frequency, amp, seed -> range -> (return_value+offset) * scaling
	// 10, 9, 1, 0 -> roughly (-0.8, 0.8) -> (return_value+0.8) * 0.6 -> roughly (0.0,0.9)
	float offset  = 0.90f;
	float scaling = 0.55f;

	float min_value = FLT_MAX, max_value = -FLT_MAX;

	Vector2 off_center_shifts[] = {
		Vector2( RangedRand(-2.0f,2.0f), RangedRand(-2.0f,2.0f) ),
		Vector2( RangedRand(-2.0f,2.0f), RangedRand(-2.0f,2.0f) ),
		Vector2( RangedRand(-2.0f,2.0f), RangedRand(-2.0f,2.0f) ),
		Vector2( RangedRand(-2.0f,2.0f), RangedRand(-2.0f,2.0f) ),
		Vector2( RangedRand(-2.0f,2.0f), RangedRand(-2.0f,2.0f) )
	};

	float r = 1.5f;
	float rho = 0.5f;
	SFloatRGBAColor c;

	for( int y=0; y<tile_height; y++ )
	{
		float py = (float)y / (float)tile_height;
		float fy = py - 0.5f; // fy: [-0.5,0.5)
		fy *= 2.0f * r; // fy: [-r,r)

		for( int x=0; x<tile_width; x++ )
		{
			float px = (float)x / (float)tile_width;
			float fx = px - 0.5f; // fx: [-0.5,0.5)
			fx *= 2.0f * r; // fx: [-r,r)

			float noise = ( perlin.Get(px,py) + offset ) * scaling;

			// For debugging
			min_value = take_min( noise, min_value );
			max_value = take_max( noise, max_value );

			float g = GaussianDistribution( fx, fy, rho );
			c.red   = get_clamped( g * noise, 0.0f, 1.0f );//0.0f;//base_color.red   * g;
			c.green = get_clamped( g * noise, 0.0f, 1.0f );//0.0f;//base_color.green * g;
			c.blue  = get_clamped( g * noise, 0.0f, 1.0f );//0.0f;//base_color.blue  * g;
			c.alpha = 1.0f;//g;
			texture.SetPixel( sx + x, sy + y, c );
		}
	}

	LOG_PRINTF(("min_value: %f, max_value: %f", min_value, max_value));
}


void SmokeParticleTextureGenerator::FillTexture( LockedTexture& texture )
{
	// Get a random integer to use as a seed.
	std::time_t now = std::time(0);
	std::mt19937 gen{static_cast<std::uint32_t>(now)};

	int num_horizontal_tiles = 4;
	int num_vertical_tiles   = 4;

	int tile_width  = texture.GetWidth()  / num_horizontal_tiles;
	int tile_height = texture.GetHeight() / num_vertical_tiles;

	for( int i=0; i<num_vertical_tiles; i++ )
	{
		int sy = i * tile_height;

		for( int j=0; j<num_horizontal_tiles; j++ )
		{
			int sx = j * tile_width;

			int seed = gen();
			CreateTile( texture, sx, sy, tile_width, tile_height, seed );
		}
	}

}


} // namespace amorphous
