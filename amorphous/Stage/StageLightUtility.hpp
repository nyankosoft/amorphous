#ifndef __StageLightUtility_HPP__
#define __StageLightUtility_HPP__


#include "StageUtility.hpp"
#include "LightEntity.hpp"


namespace amorphous
{



class StageLightUtility : public StageUtility
{
//...

public:

	/// default ctor. Added to compile this code with boost::python.
	StageLightUtility() {}

	StageLightUtility( boost::shared_ptr<CStage> pStage )
		:
	StageUtility(pStage)
	{}

	LightEntityHandle CreateHSPointLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 );

	LightEntityHandle CreateHSDirectionalLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir );

	LightEntityHandle CreateHSSpotlightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& pos, const Vector3& dir,
		float attenu0, float attenu1, float attenu2,
		float inner_cone_angle = (float)PI * 0.25f, float outer_cone_angle = -1.0f );
};


} // namespace amorphous


#endif /* __StageLightUtility_HPP__ */
