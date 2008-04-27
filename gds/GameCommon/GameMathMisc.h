#ifndef __GAMEMATH_H__
#define __GAMEMATH_H__

#include <stdlib.h>

/**
 * friction calc function from QUAKE code
 * \param dt [in] delta-t
 * \param fFriction [in] friction
 * \param rfSpeed [in,out] speed. must be positive
 * \param fThreshold [in]
 * if you use this function together with the following classic physics equation
 *  speed = accel * dt;
 *  ApplyQFriction( dt, friction, speed )
 * then the speed will increase up to (accel / friction)
 * the smaller dt will bring speed closer to the (accel / friction) value
 */
inline void ApplyQFriction( float& rfSpeed, float dt, float fFriction, float fThreshold = 0.25f )
{
	float fDrop, fControl, fNewSpeed;
	if(rfSpeed < fThreshold)
		fControl = fThreshold;
	else
		fControl = rfSpeed;
	
	fDrop = fControl * fFriction * dt;

	// scale the Velocity
	// The length of Velocity is changed from fSpeed to fNewSpeed.
	fNewSpeed = rfSpeed - fDrop;
	if(fNewSpeed <= 0)
	{
		rfSpeed = 0.0f;
		return;
	}

	rfSpeed *= fNewSpeed / rfSpeed;
	return;
}


// friction calc function from QUAKE code modified for signed values
inline void ApplySQFriction( float& rfSpeed, float dt, float fFriction, float fThreshold = 0.25f )
{
	float unsigned_val = fabsf(rfSpeed);
	float sign = rfSpeed >= 0 ? 1.0f : -1.0f;

	ApplyQFriction( unsigned_val, dt, fFriction, fThreshold );

	rfSpeed = unsigned_val * sign;
}


#endif /*  __GAMEMATH_H__  */
