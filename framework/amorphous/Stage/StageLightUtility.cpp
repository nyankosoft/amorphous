#include "StageLightUtility.hpp"
#include "amorphous/Stage/LightEntity.hpp"
#include "amorphous/3DMath/3DGameMath.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using std::map;
using namespace boost;


LightEntityHandle ReturnLightEntityHandle( CCopyEntity *pEntity )
{
	if( pEntity )
	{
		shared_ptr<LightEntity> pLightEntity
			= dynamic_pointer_cast<LightEntity,CCopyEntity>( pEntity->Self().lock() );

		return LightEntityHandle( pLightEntity );
	}
	else
		return LightEntityHandle();
}


LightEntityHandle StageLightUtility::CreateHSPointLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, Vector3& pos, float attenu0, float attenu1, float attenu2 )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return LightEntityHandle();

	LightEntityDesc desc( Light::HEMISPHERIC_POINT );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.aColor[0]  = upper_color;
	desc.aColor[1]  = lower_color;
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	desc.afAttenuation[0] = attenu0;
	desc.afAttenuation[1] = attenu1;
	desc.afAttenuation[2] = attenu2;

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericPointLight__" );
	desc.pBaseEntityHandle = &basehandle;

	desc.WorldPose.vPosition = pos;
	desc.WorldPose.matOrient = Matrix33Identity();

//	int shadow_for_light = 1; // true(1) by default
//	Vector3 dir = Vector3(0,-1,0); // default direction = vertically down

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}


LightEntityHandle StageLightUtility::CreateHSDirectionalLightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& dir )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return LightEntityHandle();

	LightEntityDesc desc( Light::HEMISPHERIC_DIRECTIONAL );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.SetHSUpperColor( upper_color );
	desc.SetHSLowerColor( lower_color );
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericDirectionalLight__" );
	desc.pBaseEntityHandle = &basehandle;

	Vector3 checked_dir;
	Vec3Normalize( checked_dir, dir );
	if( Vec3Length( checked_dir ) < 0.001f )
		checked_dir = Vector3(0,-1,0);

	desc.WorldPose.matOrient = CreateOrientFromFwdDir( checked_dir );
	desc.WorldPose.vPosition = Vector3(0,0,0);

//	int shadow_for_light = 1; // true(1) by default

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}


LightEntityHandle StageLightUtility::CreateHSSpotlightEntity( const std::string& name,
		const SFloatRGBAColor& upper_color, const SFloatRGBAColor& lower_color,
		float intensity, const Vector3& pos, const Vector3& dir,
		float attenu0, float attenu1, float attenu2,
		float inner_cone_angle, float outer_cone_angle )
{
	shared_ptr<CStage> pStage = m_pStage.lock();
	if( !pStage )
		return LightEntityHandle();

	LightEntityDesc desc( Light::HEMISPHERIC_SPOTLIGHT );

	int group = 0;

	bool shadow_for_light = true;

	// alias
	desc.strName    = name;
	desc.SetHSUpperColor( upper_color );
	desc.SetHSLowerColor( lower_color );
	desc.fIntensity = intensity;
	desc.LightGroup = group;

	desc.afAttenuation[0] = attenu0;
	desc.afAttenuation[1] = attenu1;
	desc.afAttenuation[2] = attenu2;

	BaseEntityHandle basehandle;
	basehandle.SetBaseEntityName( "__HemisphericSpotlight__" );
	desc.pBaseEntityHandle = &basehandle;

	Vector3 checked_dir;
	Vec3Normalize( checked_dir, dir );
	if( Vec3Length( checked_dir ) < 0.001f )
		checked_dir = Vector3(0,-1,0);

	desc.WorldPose.matOrient = CreateOrientFromFwdDir( checked_dir );
	desc.WorldPose.vPosition = pos;

	desc.fInnerConeAngle = inner_cone_angle;
	desc.fOuterConeAngle = (0.0f < outer_cone_angle) ? outer_cone_angle : inner_cone_angle;

//	int shadow_for_light = 1; // true(1) by default

//	CreateEntityFromDesc( desc );
	CCopyEntity *pEntity = pStage->CreateEntity( desc );

	return ReturnLightEntityHandle( pEntity );
}


} // namespace amorphous
