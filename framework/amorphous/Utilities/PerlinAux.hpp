#ifndef __PERLIN_AUX_HPP__
#define __PERLIN_AUX_HPP__


#include <float.h>
#include "../base.hpp"
#include "../3DMath/Perlin.hpp"
#include "../Support/array2d.hpp"
#include "../Support/Serialization/Serialization.hpp"


namespace amorphous
{

using namespace serialization;


class PerlinNoiseParams : public IArchiveObjectBase
{
public:
	int octaves;
	float freq;
	float amp;
	float min_value;
	float max_value;
	int seed;
	bool tilable;

	PerlinNoiseParams()
		:
	octaves(10),
	freq(10),
	amp(10),
	min_value(0),
	max_value(1),
	seed(0),
	tilable(false)
	{}

	PerlinNoiseParams(
		int octaves_,
		float freq_ = 10,
		float amp_ = 10,
		float min_value_ = 0,
		float max_value_ = 1,
		int seed_ = 0,
		bool tilable_ = false
		)
		:
	octaves(octaves_),
	freq(freq_),
	amp(amp_),
	min_value(min_value_),
	max_value(max_value_),
	seed(seed_),
	tilable(tilable_)
	{}

	void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & octaves;
		ar & freq;
		ar & amp;
		ar & min_value;
		ar & max_value;
		ar & seed;
		ar & tilable;
	}
};


inline float StdPerlin(     Perlin& obj, float x, float y, float w, float h ) { return obj.Get(x,y); }
inline float TilablePerlin( Perlin& obj, float x, float y, float w, float h ) { return obj.GetTilable(x,y,w,h); }


inline void GetPerlinTexture( const PerlinNoiseParams& params, array2d<float>& dest )
{
	Perlin pn( params.octaves, params.freq, params.amp, params.seed );

	float (*pPerlinFunc) (Perlin&,float,float,float,float);
	pPerlinFunc = params.tilable ? TilablePerlin : StdPerlin;

	const int w = dest.size_x();
	const int h = dest.size_y();
	float min_val = FLT_MAX, max_val = -FLT_MAX;
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			float fx = (float)x / (float)w;
			float fy = (float)y / (float)h;
//			float f = pn.Get( fx, fy );
			float f = pPerlinFunc( pn, fx, fy, 1.0f, 1.0f );
			min_val = take_min( min_val, f );
			max_val = take_max( max_val, f );

			dest(x,y) = f;
		}
	}

	float val_range = max_val - min_val;

//	printf( "(min,max) = (%f,%f)\n", min_val, max_val );

	float dest_min   = params.min_value;
	float dest_range = params.max_value - params.min_value;
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			float normalized_val = ( dest(x,y) - min_val ) / val_range; // normalized_val is [0,1]
			dest(x,y) = dest_min + normalized_val * dest_range;
		}
	}
}


} // amorphous



#endif /* __PERLIN_AUX_HPP__ */
