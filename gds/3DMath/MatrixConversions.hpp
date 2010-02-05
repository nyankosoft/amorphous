#ifndef __3DMath_MatrixConversions_HPP__
#define __3DMath_MatrixConversions_HPP__


#include "Matrix34.hpp"
#include "Matrix44.hpp"


// global functions

inline void ToMatrix44( const Matrix34& src, Matrix44& dest );
inline void Matrix44CameraMatrixFromCameraPose( const Matrix34& src_cam_pose, Matrix44& dest_cam_matrix );
inline Matrix44 Matrix44CameraMatrixFromCameraPose( const Matrix34& src_cam_pose );


inline void ToMatrix44( const Matrix34& src, Matrix44& dest )
{
	const Vector3& t = src.vPosition;
	const Vector3& Rx = src.matOrient.GetColumn(0);
	const Vector3& Ry = src.matOrient.GetColumn(1);
	const Vector3& Rz = src.matOrient.GetColumn(2);
	dest(0,0) = Rx.x; dest(0,1) = Ry.x; dest(0,2) = Rz.x; dest(0,3) = t.x;
	dest(1,0) = Rx.y; dest(1,1) = Ry.y; dest(1,2) = Rz.y; dest(1,3) = t.y;
	dest(2,0) = Rx.z; dest(2,1) = Ry.z; dest(2,2) = Rz.z; dest(2,3) = t.z;
	dest(3,0) = 0;    dest(3,1) = 0;    dest(3,2) = 0;    dest(3,3) = 1;
}


inline void Matrix44CameraMatrixFromCameraPose( const Matrix34& src_cam_pose, Matrix44& dest_cam_matrix )
{
	Matrix44& dest = dest_cam_matrix;

	const Vector3& w = src_cam_pose.matOrient.GetColumn(2);
	const Vector3& u = src_cam_pose.matOrient.GetColumn(0);
	const Vector3& v = src_cam_pose.matOrient.GetColumn(1);
	const Vector3& p = src_cam_pose.vPosition;

	dest(0,0) = u.x; dest(0,1) = u.y; dest(0,2) = u.z; dest(0,3) = -Vec3Dot( u, p );
	dest(1,0) = v.x; dest(1,1) = v.y; dest(1,2) = v.z; dest(1,3) = -Vec3Dot( v, p );
	dest(2,0) = w.x; dest(2,1) = w.y; dest(2,2) = w.z; dest(2,3) = -Vec3Dot( w, p );
	dest(3,0) = 0;   dest(3,1) = 0;   dest(3,2) = 0;   dest(3,3) = 1;
}


inline Matrix44 Matrix44CameraMatrixFromCameraPose( const Matrix34& src_cam_pose )
{
	Matrix44 dest;
	Matrix44CameraMatrixFromCameraPose( src_cam_pose, dest );
	return dest;
}



#endif /* __3DMath_MatrixConversions_HPP__ */
