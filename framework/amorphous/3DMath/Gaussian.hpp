#ifndef __amorphous_Gaussian_HPP__
#define __amorphous_Gaussian_HPP__


#include "misc.hpp"


namespace amorphous
{


/**
  Helper function for GetSampleOffsets function to compute the
  2 parameter Gaussian distrubution using the given standard deviation rho.
  \param[in] x
  \param[in] y
  \param[in] rho
*/
inline float GaussianDistribution(float x, float y, float rho)
{
	float g = 1.0f / sqrtf(2.0f * (float)PI * rho * rho);
	g *= expf(-(x * x + y * y) / (2 * rho * rho));

	return g;
}


inline float GetGaussianDistributionEffectiveRadius(float rho, float threshold)
{
	return sqrtf( -2 * rho * rho * (float)log(threshold*rho*sqrtf(2.0f*(float)PI)) );
}


} // namespace amorphous



#endif /* __amorphous_Gaussian_HPP__ */

