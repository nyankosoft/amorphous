#ifndef __PerlinNoiseTextureGenerator_HPP__
#define __PerlinNoiseTextureGenerator_HPP__


#include "../Utilities/PerlinAux.hpp"


class CPerlinNoiseTextureGenerator : public CTextureFillingAlgorithm
{
public:

	CPerlinNoiseParams m_Params;

public:

	inline CPerlinNoiseTextureGenerator( const CPerlinNoiseParams& pn_params )
		:
	m_Params(pn_params)
	{}

	virtual ~CPerlinNoiseTextureGenerator() {}

	virtual void FillTexture( CLockedTexture& texture )
	{
		int w = texture.GetWidth();
		int h = texture.GetHeight();

		C2DArray<float> buffer;
		buffer.resize( w, h, 0.0f );
		GetPerlinTexture( m_Params, buffer );

		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				U8 a=255,r=0,g=0,b=0;
				r = g = b = get_clamped( (U8)(buffer(x,y) * 255), (U8)0, (U8)255 );
				U32 argb32 = a << 24 | r << 16 | g << 8 | b;
				texture.SetPixelARGB32( x, y, argb32 );
			}
		}
	}
};


class CPerlinNoiseNormalMapGenerator : public CPerlinNoiseTextureGenerator
{
public:

	inline CPerlinNoiseNormalMapGenerator( const CPerlinNoiseParams& pn_params )
		:
	CPerlinNoiseTextureGenerator(pn_params)
	{}

	virtual ~CPerlinNoiseNormalMapGenerator() {}

	void FillTexture( CLockedTexture& texture )
	{
		int w = texture.GetWidth();
		int h = texture.GetHeight();

		C2DArray<float> buffer;
		buffer.resize( w, h, 0.0f );
		GetPerlinTexture( m_Params, buffer );

		for( int y=0; y<h; y++ )
		{
			for( int x=0; x<w; x++ )
			{
				Vector3 center( Vector3( 0, 0, buffer(x,y) ) );
				Vector3 sum_normal( Vector3(0,0,0) );
				// indices (clockwise order from the top)
				Vector3 tips[4] =
				{
					Vector3( 0,-1, buffer( x,                      get_clamped(y-1,0,h-1) ) ),
					Vector3( 1, 0, buffer( get_clamped(x+1,0,w-1), y ) ),
					Vector3( 0, 1, buffer( x,                      get_clamped(y+1,0,w-1) ) ),
					Vector3(-1, 0, buffer( get_clamped(x-1,0,w-1), y ) )
				};

//				int ix[4] = { x,                      get_clamped(x+1,0,w-1), x,                      get_clamped(x-1,0,w-1) };
//				int iy[4] = { get_clamped(y-1,0,h-1), y,                      get_clamped(y+1,0,w-1), y };
				for( int i=0; i<4; i++ )
				{
					Vector3 normal = Vec3Cross( tips[i] - center, tips[(i+1)%4] - center );
					sum_normal += Vec3GetNormalized( normal );
				}

				Vector3 ave_normal = Vec3GetNormalized( sum_normal );
				Vector3 offset_normal = ( ave_normal + Vector3(1,1,1) ) * 0.5f;

				U8 a = 255;
				U8 r = get_clamped( (U8)(offset_normal.x * 255), (U8)0, (U8)255 );
				U8 g = get_clamped( (U8)(offset_normal.y * 255), (U8)0, (U8)255 );
				U8 b = get_clamped( (U8)(offset_normal.z * 255), (U8)0, (U8)255 );
				U32 argb32 = a << 24 | r << 16 | g << 8 | b;
				texture.SetPixelARGB32( x, y, argb32 );
			}
		}
	}
};



#endif /* __PerlinNoiseTextureGenerator_HPP__ */
