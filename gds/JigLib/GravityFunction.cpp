
#include "GravityFunction.hpp"
#include "3DMath/D3DX_Adapter.hpp"


//float g_fGFreeMaxRadius = 8.0f;
//float g_fOmegaSq = 0.16333333333f;

float CGravityFunction::ms_fGFreeMaxRadius = 8.0f;
float CGravityFunction::ms_fOmegaSq = 0.16333333333f;


// cylinder is placed along z-axis
D3DXVECTOR3 GravityAccel_CylindricalColony( D3DXVECTOR3& rvPosition )
{
	D3DXVECTOR3 vPosOnZPlane = D3DXVECTOR3( rvPosition.x, rvPosition.y, 0 );
	float fRadiusSq = Vec3LengthSq( &vPosOnZPlane );

///	if( fRadiusSq < g_fGFreeMaxRadius * g_fGFreeMaxRadius )
	if( fRadiusSq < CGravityFunction::ms_fGFreeMaxRadius * CGravityFunction::ms_fGFreeMaxRadius )
		return D3DXVECTOR3(0,0,0);	// no gravity
	else
	{
//		float fRadius = sqrtf( fRadiusSq );
///		return vPosOnZPlane * g_fOmegaSq;
		return vPosOnZPlane * CGravityFunction::ms_fOmegaSq;
	}
}