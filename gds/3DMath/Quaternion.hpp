// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef __3DMATH_QUATERNION_H__
#define __3DMATH_QUATERNION_H__

#include "Matrix33.hpp"


namespace amorphous
{


class Quaternion
{
public:

	inline Quaternion () {}  // uninitialized for performance in array construction

	inline Quaternion ( Scalar fX, Scalar fY, Scalar fZ, Scalar fW );
	inline Quaternion ( const Quaternion& q );

    /// Creates a quaternion from a rotation matrix
	inline Quaternion ( const Matrix33& rRot );

    // quaternion for the rotation of the axis-angle pair
//	inline Quaternion (const Vector3& rvAxis, Scalar fAngle);

    // quaternion for the rotation matrix with specified columns
//	inline Quaternion (const Vector3 akRotColumn[3]);


    // assignment
//	inline Quaternion& operator= (const Quaternion& q);

    // comparison
	inline bool operator== (const Quaternion& q) const;
//	inline bool operator!= (const Quaternion& q) const;
//	inline bool operator<  (const Quaternion& q) const;
//	inline bool operator<= (const Quaternion& q) const;
//	inline bool operator>  (const Quaternion& q) const;
//	inline bool operator>= (const Quaternion& q) const;

    // arithmetic operations
	inline Quaternion operator+ (const Quaternion& q) const;
	inline Quaternion operator- (const Quaternion& q) const;
	inline Quaternion operator* (const Quaternion& q) const;
	inline Quaternion operator* (const Scalar fScalar) const;
	inline Quaternion operator/ (const Scalar fScalar) const;
//  inline Quaternion operator- () const;

    // arithmetic updates
//	inline Quaternion& operator+= (const Quaternion& q);
//	inline Quaternion& operator-= (const Quaternion& q);
//	inline Quaternion& operator*= (Scalar fScalar);
//	inline Quaternion& operator/= (Scalar fScalar);

    // conversion between quaternions, matrices, and axis-angle
	inline Quaternion& FromRotationMatrix (const Matrix33& rRot);
	inline void ToRotationMatrix (Matrix33& rRot) const;
	inline Matrix33 ToRotationMatrix() const;
//	inline Quaternion& FromRotationMatrix (const Vector3 akRotColumn[3]);
//	inline void ToRotationMatrix (Vector3 akRotColumn[3]) const;
//	inline Quaternion& FromAxisAngle (const Vector3& rvAxis, Scalar fAngle);	*/

	inline void ToAxisAngle (Vector3& rvAxis, Scalar& rfAngle) const;


    /// functions of a quaternion

	/// length of 4-tuple
	inline Scalar GetLength() const;

	/// squared length of 4-tuple
	inline Scalar GetLengthSq() const;

	inline Scalar Dot(const Quaternion& q) const;  // dot product of 4-tuples

	inline Scalar Normalize();  // make the 4-tuple unit length

	/// apply to non-zero quaternion
	inline Quaternion GetInverse() const;

	inline void GetInverse( Quaternion &qInverse ) const;

//	inline Quaternion Conjugate () const;
//	inline Quaternion Exp () const;  // apply to quaternion with w = 0
//	inline Quaternion Log () const;  // apply to unit-length quaternion


    // rotation of a vector by a quaternion
    inline Vector3 Rotate ( const Vector3& src ) const;

    // spherical linear interpolation
    inline Quaternion& Slerp( Scalar f, const Quaternion& q0, const Quaternion& q1 );
/*
    inline Quaternion& SlerpExtraSpins (Scalar fT, const Quaternion& rkP,
        const Quaternion& q, int iExtraSpins);

    // intermediate terms for spherical quadratic interpolation
    inline Quaternion& Intermediate (const Quaternion& q0,
        const Quaternion& q1, const Quaternion& q2);

    // spherical quadratic interpolation
    inline Quaternion& Squad (Scalar fT, const Quaternion& q0,
        const Quaternion& rkA0, const Quaternion& rkA1,
        const Quaternion& q1);

    // Compute a quaternion that rotates unit-length vector V1 to unit-length
    // vector V2.  The rotation is about the axis perpendicular to both V1 and
    // V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
    // any axis of rotation will do, such as the permutation (z2,x2,y2), where
    // V2 = (x2,y2,z2).
    inline Quaternion& Align (const Vector3& rkV1, const Vector3& rkV2);

    // Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    inline void DecomposeTwistTimesSwing (const Vector3& rkV1,
        Quaternion& rkTwist, Quaternion& rkSwing);

    // Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
    // quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
    // q, q_swing represents the rotation about the axis perpendicular to
    // V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1.
    inline void DecomposeSwingTimesTwist (const Vector3& rkV1,
        Quaternion& rkSwing, Quaternion& rkTwist);

    // special values
    static const Quaternion IDENTITY;  // the identity rotation
    static const Quaternion ZERO;
*/
public:

    float x, y, z, w;
};


inline Quaternion operator* ( Scalar fScalar, const Quaternion& q ) { return Quaternion( q ) * fScalar; }


} // namespace amorphous


#include "Quaternion.inl"

#endif  /*  __3DMATH_QUATERNION_H__  */



