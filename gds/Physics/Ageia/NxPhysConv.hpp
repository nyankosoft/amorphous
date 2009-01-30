#ifndef __NxPhysConv_H__
#define __NxPhysConv_H__


#include "3DMath/Matrix34.hpp"
#include "../Actor.hpp"
#include "../Ray.hpp"
#include "../Enums.hpp"
#include "../RaycastHit.hpp"

#include "NxPhysics.h"


namespace physics
{

//
// physics to Nx
//

inline NxForceMode ToNxForceMode( ForceMode::Mode mode )
{
	static const NxForceMode s_PhysToNxForceMode[] = {
		NX_FORCE,
		NX_IMPULSE,
		NX_VELOCITY_CHANGE,
		NX_SMOOTH_IMPULSE,
		NX_SMOOTH_VELOCITY_CHANGE,
		NX_ACCELERATION
	};

	return s_PhysToNxForceMode[ mode ];
}


inline NxRay ToNxRay( const CRay& ray )
{
	return NxRay( ToNxVec3(ray.Origin), ToNxVec3(ray.Direction) );
}


inline NxTimeStepMethod ToNxTimestepMethod( Timestep::Method timestep_method )
{
	static const NxTimeStepMethod s_PhysToNxTimestepMethod[] = {
		NX_TIMESTEP_FIXED,
		NX_TIMESTEP_VARIABLE,
		NX_TIMESTEP_INHERIT
	};

	return s_PhysToNxTimestepMethod[ timestep_method ];
}

inline NxRaycastHit ToNxRaycastHit( const CRaycastHit& hit )
{
	NxRaycastHit dest;
	dest.shape          = NULL;//hit.;
	dest.worldImpact    = ToNxVec3( hit.WorldImpactPos );
	dest.worldNormal    = ToNxVec3( hit.WorldNormal );
	dest.faceID         = hit.FaceID;
//	dest.internalFaceID = hit.;
	dest.distance       = hit.fDistance;
//	dest.u              = hit.;
//	dest.v              = hit.;
	dest.materialIndex  = hit.MaterialID;
	dest.flags          = hit.Flags;

	return dest;
}


inline CRaycastHit FromNxRaycastHit( const NxRaycastHit& src )
{
	CRaycastHit dest;

	if( src.shape )
		dest.pShape     = (CShape *)src.shape->userData;

	dest.WorldImpactPos = ToVector3( src.worldImpact );
	dest.WorldNormal    = ToVector3( src.worldNormal );
	dest.FaceID		    = src.faceID;
//	dest.			    = src.internalFaceID;
	dest.fDistance	    = src.distance;
//	dest.			    = src.u;
//	dest.			    = src.v;
	dest.MaterialID	    = src.materialIndex;
	dest.Flags		    = src.flags;

	return dest;
}


//
// Nx to physics
//

inline Timestep::Method ToTimestepMethod( NxTimeStepMethod timestep_method )
{
	switch(timestep_method)
	{
	case NX_TIMESTEP_FIXED:    return Timestep::Fixed;
	case NX_TIMESTEP_VARIABLE: return Timestep::Variable;
	case NX_TIMESTEP_INHERIT:  return Timestep::Inherit;
	default: return Timestep::Fixed;
	}

	return Timestep::Fixed;
}


} // namespace physics




#endif /* __NxPhysConv_H__ */
