#include "NoiseTextureGenerators.hpp"


void CUniformSingleColorNoiseTextureGenerator::FillTexture( CLockedTexture& texture )
{
	float density = m_fDensity;
	float min_val = m_fMin;
	float max_val = m_fMax;
	float range = m_fMax - m_fMin;
	float r = m_Color.red;
	float g = m_Color.green;
	float b = m_Color.blue;

	const int w = texture.GetWidth();//dest_bitmap_buffer.size_x();
	const int h = w;// Assumes that the textute is square. dest_bitmap_buffer.size_y();
	for( int y=0; y<h; y++ )
	{
		for( int x=0; x<w; x++ )
		{
			if( density < RangedRand( 0.0f, 0.1f ) )
				continue;
			float val = RangedRand( min_val, max_val );
			clamp( val, 0.0f, 1.0f );
//			texture.SetPixel( x, y, SFloatRGBAColor( val, val, val, 1.0f ) );
			texture.SetPixelARGB32( x, y, SFloatRGBAColor( r, g, b, val ).GetARGB32() );
		}
	}
}


void CStripeTextureGenerator::FillTexture( CLockedTexture& texture )
{
	const int w = texture.GetWidth();//dest_bitmap_buffer.size_x();
	const int h = w;// Assumes that the textute is square. dest_bitmap_buffer.size_y();
	int stripe_width = m_StripeWidth;
	SFloatRGBAColor colors[] = { m_Color0, m_Color1 };
	int sy = 0;
	while(sy < h)
	{
		for( int i=0; i<2; i++ )
		{
			int y=0;
			for( ; y<stripe_width && sy+y < h; y++ )
			{
				SFloatRGBAColor color( colors[i] );
				for( int x=0; x<w; x++ )
				{
//					texture.SetPixel( x, y, SFloatRGBAColor( val, val, val, 1.0f ) );
					texture.SetPixelARGB32( x, sy + y, color.GetARGB32() );
				}
			}
			sy += y;
		}
	}
}
