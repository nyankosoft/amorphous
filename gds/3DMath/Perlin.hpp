#ifndef __3DMATH_PERLIN_HPP__
#define __3DMATH_PERLIN_HPP__


#include <stdlib.h>
#include <math.h>


#define SAMPLE_SIZE 1024

//template<int SAMPLE_SIZE>
class Perlin
{
public:

	Perlin( int octaves,float freq,float amp,int seed )
		:
	m_Octaves( octaves ),
	m_Frequency( freq ),
	m_Amplitude( amp ),
	m_Seed( seed ),
	m_Start( true )
	{}


	float Get(float x,float y)
	{
		float vec[2];
		vec[0] = x;
		vec[1] = y;
		return perlin_noise_2D(vec);
	}

	float GetTilable(float x,float y,float w,float h)
	{
		// Not implemented yet.
//		return 0;

		x = fmod(x,w);
		y = fmod(y,h);

		float f =
		Get(x, y) * (w - x) * (h - y) +
		Get(x - w, y) * (x) * (h - y) +
		Get(x - w, y - h) * (x) * (y) +
		Get(x, y - h) * (w - x) * (y);

		return f / (w*h);
	}

private:

	void init_perlin(int n,float p);
	float perlin_noise_2D(float vec[2]);

	float noise1(float arg);
	float noise2(float vec[2]);
	float noise3(float vec[3]);
	inline void normalize2(float v[2]) const;
	inline void normalize3(float v[3]) const;
	void init(void);

	int   m_Octaves;
	float m_Frequency;
	float m_Amplitude;
	int   m_Seed;

	int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];

	bool  m_Start;
};



#endif /* __3DMATH_PERLIN_HPP__ */
