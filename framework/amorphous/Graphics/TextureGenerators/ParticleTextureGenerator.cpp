#include "ParticleTextureGenerator.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/GraphicsResources.hpp"
#include "amorphous/3DMath/Gaussian.hpp"


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


} // namespace amorphous
