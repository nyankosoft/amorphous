#ifndef __convert_python_to_x_HPP__
#define __convert_python_to_x_HPP__


#include <string>
#include "Python.h"
#include "../Support/Log/DefaultLog.hpp"
#include "../3DMath/aabb2.hpp"
#include "../3DMath/aabb3.hpp"


inline void convert_python_to_cpp_Vector2( PyObject* src, void *address )
{
	Vector2 dest(0,0);
	PyObject* attr_x = PyObject_GetAttrString( src, "x" );
	PyObject* attr_y = PyObject_GetAttrString( src, "y" );

	double x=0,y=0;
	PyArg_Parse( attr_x, "d", &x );
	PyArg_Parse( attr_y, "d", &y );
	dest.x = (float)x;
	dest.y = (float)y;

	memcpy( address, &dest, sizeof(dest) );
}


inline void convert_python_to_cpp_Vector3( PyObject* src, void *address )
{
	Vector3 dest(0,0,0);
	PyObject* attr_x = PyObject_GetAttrString( src, "x" );
	PyObject* attr_y = PyObject_GetAttrString( src, "y" );
	PyObject* attr_z = PyObject_GetAttrString( src, "z" );

	double x=0,y=0,z=0;
	PyArg_Parse( attr_x, "d", &x );
	PyArg_Parse( attr_y, "d", &y );
	PyArg_Parse( attr_z, "d", &z );
	dest.x = (float)x;
	dest.y = (float)y;
	dest.y = (float)z;

	memcpy( address, &dest, sizeof(dest) );
}


inline void convert_python_to_cpp_Plane( PyObject* src, void *address )
{
	Plane dest;
	PyObject* attr_normal = PyObject_GetAttrString( src, "normal" );
	PyObject* attr_dist   = PyObject_GetAttrString( src, "dist" );

	convert_python_to_cpp_Vector3( attr_normal, &dest.normal );

	double dist=0;
	PyArg_Parse( attr_dist, "d", &dest.dist );
	dest.dist = (float)dist;

	memcpy( address, &dest, sizeof(dest) );
}


inline void convert_python_to_cpp_AABB3( PyObject* src, void *address )
{
	AABB3 dest;
	PyObject* attr_min = PyObject_GetAttrString( src, "min" );
	PyObject* attr_max = PyObject_GetAttrString( src, "max" );

	convert_python_to_cpp_Vector3( attr_min, &dest.vMin );
	convert_python_to_cpp_Vector3( attr_max, &dest.vMax );

	memcpy( address, &dest, sizeof(dest) );
}

inline void convert_python_to_cpp_FloatRGBA( PyObject* src, void *address )
{
	SFloatRGBAColor dest( SFloatRGBAColor::White() );
//	PyArg_ParseTuple( src, "ff", &pos.x, &pos.y );
//	PyArg_Parse( src, "ff", &pos.x, &pos.y );
	PyObject* attr_r = PyObject_GetAttrString( src, "red" );
	PyObject* attr_g = PyObject_GetAttrString( src, "green" );
	PyObject* attr_b = PyObject_GetAttrString( src, "blue" );
	PyObject* attr_a = PyObject_GetAttrString( src, "alpha" );

//	PyObject* float_x = PyNumber_Float( attr_x );

	double r=0,g=0,b=0,a=0;
	PyArg_Parse( attr_r, "d", &r );
	PyArg_Parse( attr_g, "d", &g );
	PyArg_Parse( attr_b, "d", &b );
	PyArg_Parse( attr_a, "d", &a );
	dest.fRed   = (float)r;
	dest.fGreen = (float)g;
	dest.fBlue  = (float)b;
	dest.fAlpha = (float)a;

	memcpy( address, &dest, sizeof(dest) );
}


#endif /* __convert_python_to_x_HPP__ */
