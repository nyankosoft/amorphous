// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2005.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement
//     http://www.geometrictools.com/License/WildMagic3License.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
 
Quaternion::Quaternion ( Scalar fX, Scalar fY, Scalar fZ, Scalar fW )
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}

//----------------------------------------------------------------------------
Quaternion::Quaternion (const Quaternion& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;

//	memcpy(this, &q, sizeof(Quaternion));
}

/*
//----------------------------------------------------------------------------
Quaternion::Quaternion (const Matrix33& rkRot)
{
	FromRotationMatrix(rkRot);
}
//----------------------------------------------------------------------------
 
Quaternion::Quaternion (const Vector3& rvAxis, Scalar fAngle)
{
    FromAxisAngle(rvAxis,fAngle);
}
//----------------------------------------------------------------------------
 
Quaternion::Quaternion (const Vector3 akRotColumn[3])
{
    FromRotationMatrix(akRotColumn);
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::operator= (const Quaternion& q)
{
    memcpy(m_afTuple,q.m_afTuple,4*sizeof(Scalar));
    return *this;
}
//----------------------------------------------------------------------------
 
int Quaternion::CompareArrays (const Quaternion& q) const
{
    return memcmp(m_afTuple,q.m_afTuple,4*sizeof(Scalar));
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator== (const Quaternion& q) const
{
    return CompareArrays(q) == 0;
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator!= (const Quaternion& q) const
{
    return CompareArrays(q) != 0;
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator< (const Quaternion& q) const
{
    return CompareArrays(q) < 0;
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator<= (const Quaternion& q) const
{
    return CompareArrays(q) <= 0;
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator> (const Quaternion& q) const
{
    return CompareArrays(q) > 0;
}
//----------------------------------------------------------------------------
 
bool Quaternion::operator>= (const Quaternion& q) const
{
    return CompareArrays(q) >= 0;
}
//----------------------------------------------------------------------------
*/


inline Quaternion Quaternion::operator+ (const Quaternion& q) const
{
	Quaternion qDest;
	qDest.x = x + q.x;
	qDest.y = y + q.y;
	qDest.z = z + q.z;
	qDest.w = w + q.w;
	return qDest;
/*
	Quaternion kSum;
    for (int i = 0; i < 4; i++)
        kSum.m_afTuple[i] = m_afTuple[i] + q.m_afTuple[i];
    return kSum;*/
}
//----------------------------------------------------------------------------
 
inline Quaternion Quaternion::operator- (const Quaternion& q) const
{
	Quaternion qDest;
	qDest.x = x - q.x;
	qDest.y = y - q.y;
	qDest.z = z - q.z;
	qDest.w = w - q.w;
	return qDest;
/*
	Quaternion kDiff;
    for (int i = 0; i < 4; i++)
        kDiff.m_afTuple[i] = m_afTuple[i] - q.m_afTuple[i];
    return kDiff;*/
}
//----------------------------------------------------------------------------


inline Quaternion Quaternion::operator* (const Quaternion& q) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

    Quaternion kProd;

    kProd.w =
        w*q.w -
        x*q.x -
        y*q.y -
        z*q.z;

    kProd.x =
        w*q.x +
        x*q.w +
        y*q.z -
        z*q.y;

    kProd.y =
        w*q.y +
        y*q.w +
        z*q.x -
        x*q.z;

    kProd.z =
        w*q.z +
        z*q.w +
        x*q.y -
        y*q.x;

    return kProd;
}



//----------------------------------------------------------------------------

inline Quaternion Quaternion::operator* (const Scalar fScalar) const
{
    Quaternion kProd;
	kProd.x = fScalar * x;
	kProd.y = fScalar * y;
	kProd.z = fScalar * z;
	kProd.w = fScalar * w;

//    for (int i = 0; i < 4; i++)
//        kProd.m_afTuple[i] = fScalar*m_afTuple[i];

    return kProd;
}

 
inline Quaternion Quaternion::operator/ (const Scalar fScalar) const
{
	return operator *( 1.0f / fScalar );
/*
    Quaternion kProd;
	kProd.x =  x / fScalar;
	kProd.y =  y / fScalar;
	kProd.z =  z / fScalar;
	kProd.w =  w / fScalar;

    return kProd;*/
}


/* 
//----------------------------------------------------------------------------

inline Quaternion Quaternion::operator/ (Scalar fScalar) const
{
    Quaternion kQuot;
    int i;

    if ( fScalar != (Scalar)0.0 )
    {
        Scalar fInvScalar = ((Scalar)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            kQuot.m_afTuple[i] = fInvScalar*m_afTuple[i];
    }
    else
    {
        for (i = 0; i < 4; i++)
            kQuot.m_afTuple[i] = Math<Real>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
 
inline Quaternion Quaternion::operator- () const
{
    Quaternion kNeg;
    for (int i = 0; i < 4; i++)
        kNeg.m_afTuple[i] = -m_afTuple[i];
    return kNeg;
}

//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::operator+= (const Quaternion& q)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] += q.m_afTuple[i];
    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::operator-= (const Quaternion& q)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] -= q.m_afTuple[i];
    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::operator*= (Scalar fScalar)
{
    for (int i = 0; i < 4; i++)
        m_afTuple[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::operator/= (Scalar fScalar)
{
    int i;

    if ( fScalar != (Scalar)0.0 )
    {
        Scalar fInvScalar = ((Scalar)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            m_afTuple[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 4; i++)
            m_afTuple[i] = Math<Real>::MAX_REAL;
    }

    return *this;
}
*/

//----------------------------------------------------------------------------
inline Quaternion& Quaternion::FromRotationMatrix ( const Matrix33& rRot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

    Scalar fTrace = rRot(0,0) + rRot(1,1) + rRot(2,2);
    Scalar fRoot;

    if ( fTrace > (Scalar)0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
//		fRoot = Math<Real>::Sqrt(fTrace + (Scalar)1.0);  // 2w

		fRoot = sqrtf( fTrace + (Scalar)1.0 );  // 2w
		w = ((Scalar)0.5)*fRoot;

        fRoot = ((Scalar)0.5)/fRoot;  // 1/(4w)

        x = (rRot(2,1)-rRot(1,2))*fRoot;
        y = (rRot(0,2)-rRot(2,0))*fRoot;
        z = (rRot(1,0)-rRot(0,1))*fRoot;
    }
    else
    {
        // |w| <= 1/2
        int i = 0;
        if ( rRot(1,1) > rRot(0,0) )
            i = 1;
        if ( rRot(2,2) > rRot(i,i) )
            i = 2;
//		int j = ms_iNext[i];
//		int k = ms_iNext[j];
		int j = (i + 1) % 3;
		int k = (j + 1) % 3;

//		fRoot = Math<Real>::Sqrt(rRot(i,i)-rRot(j,j)-rRot(k,k)+(Scalar)1.0);
		fRoot = sqrtf( rRot(i,i) - rRot(j,j) - rRot(k,k) + (Scalar)1.0 );
        Scalar* apfQuat[3] = { &x, &y, &z };
        *apfQuat[i] = ((Scalar)0.5)*fRoot;
        fRoot = ((Scalar)0.5)/fRoot;
        w = (rRot(k,j)-rRot(j,k))*fRoot;
        *apfQuat[j] = (rRot(j,i)+rRot(i,j))*fRoot;
        *apfQuat[k] = (rRot(k,i)+rRot(i,k))*fRoot;
    }

    return *this;
}

//----------------------------------------------------------------------------
inline void Quaternion::ToRotationMatrix (Matrix33& rRot) const
{
    Scalar fTx  = ((Scalar)2.0)*x;
    Scalar fTy  = ((Scalar)2.0)*y;
    Scalar fTz  = ((Scalar)2.0)*z;
    Scalar fTwx = fTx*w;
    Scalar fTwy = fTy*w;
    Scalar fTwz = fTz*w;
    Scalar fTxx = fTx*x;
    Scalar fTxy = fTy*x;
    Scalar fTxz = fTz*x;
    Scalar fTyy = fTy*y;
    Scalar fTyz = fTz*y;
    Scalar fTzz = fTz*z;

    rRot(0,0) = (Scalar)1.0-(fTyy+fTzz);
    rRot(0,1) = fTxy-fTwz;
    rRot(0,2) = fTxz+fTwy;
    rRot(1,0) = fTxy+fTwz;
    rRot(1,1) = (Scalar)1.0-(fTxx+fTzz);
    rRot(1,2) = fTyz-fTwx;
    rRot(2,0) = fTxz-fTwy;
    rRot(2,1) = fTyz+fTwx;
    rRot(2,2) = (Scalar)1.0-(fTxx+fTyy);
}


inline Matrix33 Quaternion::ToRotationMatrix() const
{
	Matrix33 mat;
	ToRotationMatrix( mat );
	return mat;
}


/*
//----------------------------------------------------------------------------

inline Quaternion& Quaternion::FromRotationMatrix (
    const Vector3 akRotColumn[3])
{
    Matrix3<Real> kRot;
    for (int iCol = 0; iCol < 3; iCol++)
    {
        kRot(0,iCol) = akRotColumn[iCol][0];
        kRot(1,iCol) = akRotColumn[iCol][1];
        kRot(2,iCol) = akRotColumn[iCol][2];
    }
    return FromRotationMatrix(kRot);
}
//----------------------------------------------------------------------------
 
inline void Quaternion::ToRotationMatrix (Vector3 akRotColumn[3]) const
{
    Matrix3<Real> kRot;
    ToRotationMatrix(kRot);
    for (int iCol = 0; iCol < 3; iCol++)
    {
        akRotColumn[iCol][0] = kRot(0,iCol);
        akRotColumn[iCol][1] = kRot(1,iCol);
        akRotColumn[iCol][2] = kRot(2,iCol);
    }
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::FromAxisAngle (
    const Vector3& rvAxis, Scalar fAngle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Scalar fHalfAngle = ((Scalar)0.5)*fAngle;
    Scalar fSin = Math<Real>::Sin(fHalfAngle);
    w = Math<Real>::Cos(fHalfAngle);
    x = fSin*rvAxis[0];
    y = fSin*rvAxis[1];
    z = fSin*rvAxis[2];

    return *this;
}
//----------------------------------------------------------------------------
*/
inline void Quaternion::ToAxisAngle (Vector3& rvAxis, Scalar& rfAngle)
    const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    Scalar fSqrLength = x*x + y*y + z*z;

    if ( fSqrLength > 0.00000001f /*Math<Real>::ZERO_TOLERANCE*/ )
    {
//        rfAngle = ((Scalar)2.0)*Math<Real>::ACos(w);
//        Scalar fInvLength = Math<Real>::InvSqrt(fSqrLength);
		
		rfAngle = 2.0f * (Scalar)acos(w);
        Scalar fInvLength = 1.0f / sqrtf(fSqrLength);

        rvAxis.x = x * fInvLength;
        rvAxis.y = y * fInvLength;
        rvAxis.z = z * fInvLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        rfAngle = (Scalar)0.0;
        rvAxis[0] = (Scalar)1.0;
        rvAxis[1] = (Scalar)0.0;
        rvAxis[2] = (Scalar)0.0;
    }
}


//----------------------------------------------------------------------------
 
inline Scalar Quaternion::GetLength() const
{
    return sqrtf( w*w + x*x + y*y + z*z);
}


//----------------------------------------------------------------------------
 
inline Scalar Quaternion::GetLengthSq() const
{
	return  w*w + x*x + y*y + z*z;
}

//----------------------------------------------------------------------------
inline Scalar Quaternion::Dot(const Quaternion& q) const
{
    Scalar fDot = (Scalar)0.0;
	fDot = x * q.x + y * q.y + z * q.z + w * q.w;
    return fDot;
}

//----------------------------------------------------------------------------

inline Scalar Quaternion::Normalize()
{
    Scalar fLength = GetLength();

    if ( fLength > 0.000001f /*Math<Real>::ZERO_TOLERANCE*/ )
    {
        Scalar fInvLength = ((Scalar)1.0f) / fLength;
        w *= fInvLength;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    }
    else
    {
        fLength = (Scalar)0.0f;
        w = (Scalar)0.0f;
        x = (Scalar)0.0f;
        y = (Scalar)0.0f;
        z = (Scalar)0.0f;
    }

    return fLength;
}

//----------------------------------------------------------------------------

inline Quaternion Quaternion::GetInverse() const
{
	 Quaternion qInverse;
	 GetInverse( qInverse );
	 return qInverse;
}


inline void Quaternion::GetInverse( Quaternion &qInverse ) const
{
    Scalar fNorm = (Scalar)0.0;
/*
    int i;
    for (i = 0; i < 4; i++)
        fNorm += m_afTuple[i]*m_afTuple[i];*/

	fNorm = GetLengthSq();

    if ( fNorm > (Scalar)0.0 )
    {
        Scalar fInvNorm = ((Scalar)1.0)/fNorm;
        qInverse.x = -x*fInvNorm;
        qInverse.y = -y*fInvNorm;
        qInverse.z = -z*fInvNorm;
        qInverse.w = w*fInvNorm;
    }
    else
    {
        // return an invalid result to flag the error
		qInverse.x = (Scalar)0.0;
		qInverse.y = (Scalar)0.0;
		qInverse.z = (Scalar)0.0;
		qInverse.w = (Scalar)0.0;
    }

}

/*
//----------------------------------------------------------------------------
 
inline Quaternion Quaternion::Conjugate () const
{
    return Quaternion(w,-x,-y,
        -z);
}
//----------------------------------------------------------------------------
 
inline Quaternion Quaternion::Exp () const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    Quaternion kResult;

    Scalar fAngle = Math<Real>::Sqrt(x*x +
        y*y + z*z);

    Scalar fSin = Math<Real>::Sin(fAngle);
    kResult.w = Math<Real>::Cos(fAngle);

    int i;

    if ( Math<Real>::FAbs(fSin) >= Math<Real>::ZERO_TOLERANCE )
    {
        Scalar fCoeff = fSin/fAngle;
        for (i = 1; i <= 3; i++)
            kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
    }
    else
    {
        for (i = 1; i <= 3; i++)
            kResult.m_afTuple[i] = m_afTuple[i];
    }

    return kResult;
}

//----------------------------------------------------------------------------
*/
 

/*
inline Quaternion Quaternion::Log () const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion kResult;
    kResult.w = (Scalar)0.0;

    int i;

    if ( Math<Real>::FAbs(w) < (Scalar)1.0 )
    {
        Scalar fAngle = Math<Real>::ACos(w);
        Scalar fSin = Math<Real>::Sin(fAngle);
        if ( Math<Real>::FAbs(fSin) >= Math<Real>::ZERO_TOLERANCE )
        {
            Scalar fCoeff = fAngle/fSin;
            for (i = 1; i <= 3; i++)
                kResult.m_afTuple[i] = fCoeff*m_afTuple[i];
            return kResult;
        }
    }

    for (i = 1; i <= 3; i++)
        kResult.m_afTuple[i] = m_afTuple[i];
    return kResult;
}
//----------------------------------------------------------------------------
 
inline Vector3 Quaternion::Rotate (const Vector3& rkVector)
    const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3<Real> kRot;
    ToRotationMatrix(kRot);
    return kRot*rkVector;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::Slerp (Scalar fT, const Quaternion& rkP,
    const Quaternion& q)
{
    Scalar fCos = rkP.Dot(q);
    Scalar fAngle = Math<Real>::ACos(fCos);

    if ( Math<Real>::FAbs(fAngle) >= Math<Real>::ZERO_TOLERANCE )
    {
        Scalar fSin = Math<Real>::Sin(fAngle);
        Scalar fInvSin = ((Scalar)1.0)/fSin;
        Scalar fCoeff0 = Math<Real>::Sin(((Scalar)1.0-fT)*fAngle)*fInvSin;
        Scalar fCoeff1 = Math<Real>::Sin(fT*fAngle)*fInvSin;
        *this = fCoeff0*rkP + fCoeff1*q;
    }
    else
    {
        *this = rkP;
    }

    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::SlerpExtraSpins (Scalar fT,
    const Quaternion& rkP, const Quaternion& q, int iExtraSpins)
{
    Scalar fCos = rkP.Dot(q);
    Scalar fAngle = Math<Real>::ACos(fCos);

    if ( Math<Real>::FAbs(fAngle) >= Math<Real>::ZERO_TOLERANCE )
    {
        Scalar fSin = Math<Real>::Sin(fAngle);
        Scalar fPhase = Math<Real>::PI*iExtraSpins*fT;
        Scalar fInvSin = ((Scalar)1.0)/fSin;
        Scalar fCoeff0 = Math<Real>::Sin(((Scalar)1.0-fT)*fAngle-fPhase)*fInvSin;
        Scalar fCoeff1 = Math<Real>::Sin(fT*fAngle + fPhase)*fInvSin;
        *this = fCoeff0*rkP + fCoeff1*q;
    }
    else
    {
        *this = rkP;
    }

    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::Intermediate (const Quaternion& q0,
    const Quaternion& q1, const Quaternion& q2)
{
    // assert:  Q0, Q1, Q2 all unit-length
    Quaternion kQ1Inv = q1.Conjugate();
    Quaternion kP0 = kQ1Inv*q0;
    Quaternion kP2 = kQ1Inv*q2;
    Quaternion kArg = -((Scalar)0.25)*(kP0.Log()+kP2.Log());
    Quaternion kA = q1*kArg.Exp();
    *this = kA;

    return *this;
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::Squad (Scalar fT, const Quaternion& q0,
    const Quaternion& rkA0, const Quaternion& rkA1, const Quaternion& q1)
{
    Scalar fSlerpT = ((Scalar)2.0)*fT*((Scalar)1.0-fT);
    Quaternion kSlerpP = Slerp(fT,q0,q1);
    Quaternion kSlerpQ = Slerp(fT,rkA0,rkA1);
    return Slerp(fSlerpT,kSlerpP,kSlerpQ);
}
//----------------------------------------------------------------------------
 
inline Quaternion& Quaternion::Align (const Vector3& rkV1,
    const Vector3& rkV2)
{
    // If V1 and V2 are not parallel, the axis of rotation is the unit-length
    // vector U = Cross(V1,V2)/Length(Cross(V1,V2)).  The angle of rotation,
    // A, is the angle between V1 and V2.  The quaternion for the rotation is
    // q = cos(A/2) + sin(A/2)*(ux*i+uy*j+uz*k) where U = (ux,uy,uz).
    //
    // (1) Rather than extract A = acos(Dot(V1,V2)), multiply by 1/2, then
    //     compute sin(A/2) and cos(A/2), we reduce the computational costs by
    //     computing the bisector B = (V1+V2)/Length(V1+V2), so cos(A/2) =
    //     Dot(V1,B).
    //
    // (2) The rotation axis is U = Cross(V1,B)/Length(Cross(V1,B)), but
    //     Length(Cross(V1,B)) = Length(V1)*Length(B)*sin(A/2) = sin(A/2), in
    //     which case sin(A/2)*(ux*i+uy*j+uz*k) = (cx*i+cy*j+cz*k) where
    //     C = Cross(V1,B).
    //
    // If V1 = V2, then B = V1, cos(A/2) = 1, and U = (0,0,0).  If V1 = -V2,
    // then B = 0.  This can happen even if V1 is approximately -V2 using
    // floating point arithmetic, since Vector3::Normalize checks for
    // closeness to zero and returns the zero vector accordingly.  The test
    // for exactly zero is usually not recommend for floating point
    // arithmetic, but the implementation of Vector3::Normalize guarantees
    // the comparison is robust.  In this case, the A = pi and any axis
    // perpendicular to V1 may be used as the rotation axis.

    Vector3 kBisector = rkV1 + rkV2;
    kBisector.Normalize();

    Scalar fCosHalfAngle = rkV1.Dot(kBisector);
    Vector3 kCross;

    w = fCosHalfAngle;

    if ( fCosHalfAngle != (Scalar)0.0 )
    {
        kCross = rkV1.Cross(kBisector);
        x = kCross.X();
        y = kCross.Y();
        z = kCross.Z();
    }
    else
    {
        Scalar fInvLength;
        if ( Math<Real>::FAbs(rkV1[0]) >= Math<Real>::FAbs(rkV1[1]) )
        {
            // V1.x or V1.z is the largest magnitude component
            fInvLength = Math<Real>::InvSqrt(rkV1[0]*rkV1[0] +
                rkV1[2]*rkV1[2]);
            x = -rkV1[2]*fInvLength;
            y = (Scalar)0.0;
            z = +rkV1[0]*fInvLength;
        }
        else
        {
            // V1.y or V1.z is the largest magnitude component
            fInvLength = Math<Real>::InvSqrt(rkV1[1]*rkV1[1] +
                rkV1[2]*rkV1[2]);
            x = (Scalar)0.0;
            y = +rkV1[2]*fInvLength;
            z = -rkV1[1]*fInvLength;
        }
    }

    return *this;
}


//----------------------------------------------------------------------------
 inline void Quaternion::DecomposeTwistTimesSwing(
    const Vector3& rkV1, Quaternion& rkTwist, Quaternion& rkSwing)
{
    Vector3 kV2 = Rotate(rkV1);
    rkSwing = Align(rkV1,kV2);
    rkTwist = (*this)*rkSwing.Conjugate();
}
//----------------------------------------------------------------------------
inline void Quaternion::DecomposeSwingTimesTwist (
    const Vector3& rkV1, Quaternion& rkSwing, Quaternion& rkTwist)
{
    Vector3 kV2 = Rotate(rkV1);
    rkSwing = Align(rkV1,kV2);
    rkTwist = rkSwing.Conjugate()*(*this);
}
//----------------------------------------------------------------------------
*/


//----------------------------------------------------------------------------
 
/*
inline Quaternion operator* ( Scalar fScalar, const Quaternion& q )
{
	Quaternion kProd;
	for (int i = 0; i < 4; i++)
		kProd[i] = fScalar*q[i];
	return kProd;
}*/
