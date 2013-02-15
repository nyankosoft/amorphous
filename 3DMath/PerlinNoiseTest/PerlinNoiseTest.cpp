#include "gds/base.hpp"
#include "gds/3DMath/Perlin.hpp"
#include "gds/Support/BitmapImage.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/2DArray.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Macro.h"
#include <float.h>

using namespace amorphous;


/// NOTE: this function produces the same noise image every time the app is run.
/// Different noise images are produced when the function is called multiple times during an execution of the app.
void RenderPerlinNoise( CBitmapImage& img )
{
	static CTimer s_Timer;
	ONCE( s_Timer.Start() );

	int octaves = 1; float freq = 1; float amp = 1; int seed = (int)(s_Timer.GetTime() * 1000.0);
	LoadParamFromFile( "params.txt", "octaves", octaves );
	LoadParamFromFile( "params.txt", "freq",    freq );
	LoadParamFromFile( "params.txt", "amp",     amp );
	LoadParamFromFile( "params.txt", "seed",    seed );

	Perlin pn( octaves, freq, amp, seed );

	float min_val = FLT_MAX, max_val = -FLT_MAX;
	int w = img.GetWidth();
	int h = img.GetHeight();

	C2DArray<float> buffer;
	buffer.resize( w, h );

	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			float fx = (float)x / (float)w;
			float fy = (float)y / (float)h;
//			float f = pn.Get( fx, fy );
			float f = pn.GetTilable( fx, fy, 1.0f, 1.0f );
			min_val = take_min( min_val, f );
			max_val = take_max( max_val, f );

			buffer(x,y) = f;
		}
	}

	float val_range = max_val - min_val;

	printf( "(min,max) = (%f,%f)\n", min_val, max_val );

	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			float normalized_val = ( buffer(x,y) - min_val ) / val_range;
			U8 r=0,g=0,b=0;
			r = g = b = get_clamped( (U8)(normalized_val * 255), (U8)0, (U8)255 );
			img.SetPixel( x, y, r, g, b );
		}
	}
}


int main( int argc, char *argv[] )
{
	int img_width = 64;
	LoadParamFromFile( "params.txt", "image_size", img_width );
	CBitmapImage img( img_width, img_width, 24 );
	RenderPerlinNoise( img );
	bool saved = img.SaveToFile( "pn.png" );
	return 0;
}
