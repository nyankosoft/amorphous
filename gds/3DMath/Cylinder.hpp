#ifndef  __Cylinder_HPP__
#define  __Cylinder_HPP__

#include "Vector3.hpp"


class Cylinder
{
public:

	Vector3 pos[2];
	float radius;

	Vector3 GetDir() const { return Vec3GetNormalized( pos[1] - pos[0] ); }

public:

	Cylinder( const Vector3& pos0 = Vector3(0,0,0), const Vector3& pos1 = Vector3(0,1,0), float r = 1.0f )
		:
	radius(r)
	{
		pos[0] = pos0;
		pos[1] = pos1;
	}

	inline bool IsValid() const
	{
		return 0.000001 * 0.000001 < Vec3LengthSq(pos[1] - pos[0]) && 0.000001 < radius;
	}

	inline Vector3 GetCenterPosition() const
	{
		return ( pos[0] + pos[1] ) * 0.5f;
	}

	inline bool IsPointInside( const Vector3& rvPoint ) const
	{
		if( !IsValid() )
			return false;

		const Vector3 src_dir = GetDir();
		Vector3 dir[2] = { src_dir, -src_dir };
		SPlane inward_planes[2] =
		{
			SPlane(  src_dir,  Vec3Dot( src_dir, pos[0] ) ),
			SPlane( -src_dir, -Vec3Dot( src_dir, pos[1] ) )
		};

		if( inward_planes[0].GetDistanceFromPoint( rvPoint ) < 0.0f
		 || inward_planes[1].GetDistanceFromPoint( rvPoint ) < 0.0f )
		{
			return false;
		}

		// Calculate the distance from rvPoint to the center line of the cylinder
		Vector3 vVertical = ( pos[0] + src_dir * Vec3Dot( rvPoint - pos[0], src_dir ) ) - rvPoint;

		if( Vec3LengthSq(vVertical) < radius * radius )
			return true;
		else
			return false;
	}

	float GetLength() const { return Vec3Length( pos[1] - pos[0] ); }
};


#endif /* __Cylinder_HPP__ */
