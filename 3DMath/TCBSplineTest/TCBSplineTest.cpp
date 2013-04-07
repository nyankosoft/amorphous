#include "amorphous/Support/BitmapImage.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/3DMath/TCBSpline.hpp"
#include "amorphous/3DMath/Vector2.hpp"

using std::vector;
using namespace amorphous;


void draw_3x3_dot( CBitmapImage& img, int x, int y, const SFloatRGBAColor& color )
{
	for( int i=-1; i<=1; i++ )
	{
		for( int j=-1; j<=1; j++ )
		{
			img.SetPixel( x+j, y+i, color );
		}
	}
}

int main( int argc, char *argv[] )
{
	CBitmapImage img( 1024, 1024, 24 );

	img.FillColor( SFloatRGBAColor::White() );

	vector<Vector2> points( 16, Vector2(0,0) );
	points[ 0] = Vector2( 100, 200 );
	points[ 1] = Vector2( 100, 100 );
	points[ 2] = Vector2( 350, 100 );
	points[ 3] = Vector2( 350, 200 );
	points[ 4] = Vector2( 600, 200 );
	points[ 5] = Vector2( 600, 100 );
	points[ 6] = Vector2( 720, 100 );
	points[ 7] = Vector2( 720, 500 );
	points[ 8] = Vector2( 500, 500 );
	points[ 9] = Vector2( 500, 520 );
	points[10] = Vector2( 480, 520 );
	points[11] = Vector2( 350, 600 );
	points[12] = Vector2( 350, 750 );
	points[13] = Vector2( 450, 750 );
	points[14] = Vector2( 600, 750 );
	points[15] = Vector2( 750, 750 );

	float tension = 0, continuity = 0, bias = 0;

	LoadParamFromFile( "params.txt", "tension",    tension );
	LoadParamFromFile( "params.txt", "continuity", continuity );
	LoadParamFromFile( "params.txt", "bias",       bias );

	for( int i=1; i<(int)points.size()-2; i++ )
	{
		const int num_subdivisions = 64;
		for( int j=0; j<num_subdivisions; j++ )
		{
			float fraction = (float)j / (float)num_subdivisions;
			Vector2 pos = InterpolateWithTCBSpline(
				fraction,
				points[i-1],
				points[i],
				points[i+1],
				points[i+2],
				tension,
				continuity,
				bias
			);

			draw_3x3_dot( img, (int)pos.x, (int)pos.y, SFloatRGBAColor::Blue() );
		}
	}

	for( int i=0; i<(int)points.size(); i++ )
		draw_3x3_dot( img, (int)points[i].x, (int)points[i].y, SFloatRGBAColor::Red() );

//	char image_pathname[1024];
//	memset( image_pathname, 0, sizeof(image_pathname) );
//	sprintf( image_pathname, "out_t=%0.3f,c=%0.3f,b=%0.3f.png", tension, continuity, bias );
	img.SaveToFile( "./out.png" );

	return 0;
}
