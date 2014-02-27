#include "../../../Project1/amorphous/Support/MTRand.hpp"
#include "../../../Project1/amorphous/base.hpp"
#include "gettimeofday_win32.hpp"

using namespace std;
using namespace amorphous;


void test_gaussian_rand()
{
	float x = 0, y = 0;
	float min_x =  FLT_MAX, min_y =  FLT_MAX;
	float max_x = -FLT_MAX, max_y = -FLT_MAX;

	for( int i=0; i<1000000; i++ )
	{
		GaussianRand( x, y );
		min_x = take_min( x, min_x );
		min_y = take_min( y, min_y );
		max_x = take_max( x, max_x );
		max_y = take_max( y, max_y );
	}

	printf( "min(x,y) = (%f,%f), max(x,y) = (%f,%f)\n",
		min_x, min_y, max_x, max_y );
}


int main( int argc, char *argv[] )
{
	timeval tv;
	int ret = gettimeofday(&tv,NULL);
	InitRand( (unsigned long)tv.tv_usec );

	while(true)
		test_gaussian_rand();

	return 0;
}
