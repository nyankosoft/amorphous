#ifndef __GridPolygonModelMaker_HPP__
#define __GridPolygonModelMaker_HPP__


#include "Matrix34.hpp"


namespace amorphous
{


inline Matrix34 CalcPoseForFlatMesh( unsigned int axis, int sign )
{
	Matrix34 pose = Matrix34Identity();

	const float pi      = (float)PI;
	const float half_pi = (float)PI * 0.5f;
	float angle = 0;
	switch( axis )
	{
	case 0: pose.matOrient = Matrix33RotationY(-half_pi * (float)sign) * Matrix33RotationX(-half_pi);
		break;

	case 1:
		if( sign == -1 )
			pose.matOrient = Matrix33RotationZ(pi);
		break;

	case 2:
		pose.matOrient = Matrix33RotationX(-half_pi);
		if( sign == 1 )
			pose.matOrient = Matrix33RotationX(pi) * pose.matOrient;
		break;

	default:
		break;
	}

	return pose;
}


/// \param[in] axis [0,2]
/// \param[in] sign 1 or -1
inline void CreateGridPoints(
		float width,
		float height,
		uint num_divisions_x,
		uint num_divisions_y,
		unsigned int axis,// = 1,
		int sign,// = 1
		std::vector<Vector3>& points
	)
{
	Matrix34 pose = CalcPoseForFlatMesh( axis, sign );

	const unsigned int num_vertices = (num_divisions_x+1) * (num_divisions_y+1);
	points.insert( points.end(), num_vertices, Vector3(0,0,0) );

	const float div_x = (float)num_divisions_x;
	const float div_y = (float)num_divisions_y;

	// Create grid points
	// Notice the loop end conditions use <= instead of <.
	for( uint i=0; i<=num_divisions_y; i++ )
	{
		for( uint j=0; j<=num_divisions_x; j++ )
		{
			Vector3 p;
			p.x = width  * (float)j / div_x - width  * 0.5f;
			p.y = 0;
			p.z = height * (float)i / div_y - height * 0.5f;
			points[i * (num_divisions_x+1) + j] = pose.matOrient * p;
		}
	}
}


} // namespace amorphous


#endif /* __GridPolygonModelMaker_HPP__ */
