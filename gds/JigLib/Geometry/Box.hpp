
#ifndef __GEOMETRY_BOX_H__
#define __GEOMETRY_BOX_H__

#include "3DMath/Vector3.hpp"
#include "3DMath/Matrix33.hpp"




class tBox
{
	Vector3 m_vCentrePos;
	Vector3 m_vSideLengths;
	Matrix33 m_matOrient;

public:
	inline tBox( Vector3 vCentre, Vector3 vSideLengths, Matrix33 matOrient );
    const inline Vector3 GetHalfSideLengths() const { return 0.5f * m_vSideLengths; }
    const inline Vector3 GetCentre() const { return m_vCentrePos; }
	const inline Matrix33 GetOrient() const { return m_matOrient; }

};


inline tBox::tBox( Vector3 vCentre, Vector3 vSideLengths, Matrix33 matOrient )
:
m_vCentrePos(vCentre),
m_vSideLengths(vSideLengths),
m_matOrient(matOrient)
{}





#endif