#ifndef __3DtoScreenSpaceConversions_HPP__
#define __3DtoScreenSpaceConversions_HPP__


#include <vector>
#include "gds/3DMath/Vector2.hpp"
#include "Camera.hpp"


namespace amorphous
{


inline Vector2 CalculateScreenCoordsFromWorldPosition( const CCamera& cam, const Vector3& vWorldPos, int screen_width, int screen_height )
{
	Vector3 vProjPos
		= cam.GetProjectionMatrix()
		* cam.GetCameraMatrix()
		* vWorldPos;

	float x = (  vProjPos.x + 1.0f ) * 0.5f * (float)screen_width;
	float y = ( -vProjPos.y + 1.0f ) * 0.5f * (float)screen_height;

	return Vector2(x,y);
}


inline void CalculateScreenCoordsFromWorldPositions( const CCamera& cam,
											   const std::vector<Vector3>& vWorldPos,
											   int screen_width,
											   int screen_height,
											   std::vector<Vector2>& vDest )
{
	vDest.resize( vWorldPos.size() );

	const Matrix44 proj_view = cam.GetProjectionMatrix() * cam.GetCameraMatrix();
	const float reference_screen_width  = (float)screen_width;
	const float reference_screen_height = (float)screen_height;

	for( size_t i=0; i<vWorldPos.size(); i++ )
	{
		Vector3 vProjPos = proj_view.TransformCoord( vWorldPos[i] );
		float x = (  vProjPos.x + 1.0f ) * 0.5f * reference_screen_width;
		float y = ( -vProjPos.y + 1.0f ) * 0.5f * reference_screen_height;
		vDest[i] = Vector2(x,y);
	}
}


inline Vector2 CalculateScreenCoordsFromWorldPosition( const CCamera& cam, const Vector3& vWorldPos )
{
	return CalculateScreenCoordsFromWorldPosition( cam, vWorldPos, CGraphicsComponent::GetReferenceScreenWidth(), CGraphicsComponent::GetReferenceScreenHeight() );
}


inline void CalculateScreenCoordsFromWorldPositions( const CCamera& cam,
											   const std::vector<Vector3>& vWorldPos,
											   std::vector<Vector2>& vDest )
{
	CalculateScreenCoordsFromWorldPositions(
		cam,
		vWorldPos,
		CGraphicsComponent::GetReferenceScreenWidth(),
		CGraphicsComponent::GetReferenceScreenHeight(),
		vDest
		);
}


} // amorphous



#endif /* __3DtoScreenSpaceConversions_HPP__ */
