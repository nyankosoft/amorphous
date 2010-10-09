#ifndef __MATRIX34_H__
#define __MATRIX34_H__


#include "Vector3.hpp"
#include "Matrix33.hpp"


template<typename T>
class tMatrix34
{
public:

	/// rotation
	tMatrix33<T> matOrient;

	/// translation
	tVector3<T> vPosition;

public:

	inline tMatrix34() {}

	inline tMatrix34( const tVector3<T>& pos, const tMatrix33<T>& orientation );

	inline void Identity();

	inline void Default() { Identity(); }

	/// apply the affine transformation to a three element vector
	inline void Transform( tVector3<T>& rvDest, const tVector3<T>& rvSrc ) const;

	/// apply the inverse transformation to a three element vector
	inline void InvTransform( tVector3<T>& rvDest, const tVector3<T>& rvSrc ) const;

	/// get the inverse -- assumes that the orientation matrix is orthogonal
	inline void GetInverseROT( tMatrix34<T>& dest ) const;

	/// Non-peformance friendly version of GetInverseROT()
	inline tMatrix34<T> GetInverseROT() const;

	/// copy orientation & translation from 4x4 row major matrix
	/// translation needs to be stored in the fourth row of
	/// the original row major matrix
	inline void CopyFromRowMajorMatrix44( T *pSrcData );

	/// get data in the form of 4x4 row major matrix
	inline void GetRowMajorMatrix44( T *pDest ) const;

	inline bool operator==( const tMatrix34<T>& rhs ) const;

	bool operator!=( const tMatrix34<T>& rhs ) const { return !(*this == rhs); }

	inline tVector3<T> operator*( const tVector3<T> &rhs ) const;
};



//===================================================================================
// inline implementations
//===================================================================================

template<typename T>
inline tMatrix34<T>::tMatrix34( const tVector3<T>& pos, const tMatrix33<T>& orientation ) : vPosition(pos), matOrient(orientation) {}


template<typename T>
inline void tMatrix34<T>::Identity()
{
	vPosition = tVector3<T>(0,0,0);
	matOrient = tMatrix33Identity<T>();
}


template<typename T>
inline void tMatrix34<T>::Transform( tVector3<T>& rvDest, const tVector3<T>& rvSrc ) const
{
	rvDest = matOrient * rvSrc;
	rvDest += vPosition;
}


template<typename T>
inline void tMatrix34<T>::InvTransform( tVector3<T>& rvDest, const tVector3<T>& rvSrc ) const
{
	matOrient.TransformByTranspose( rvDest, ( rvSrc - vPosition ) );
}


template<typename T>
inline void tMatrix34<T>::GetInverseROT( tMatrix34<T>& dest ) const
{
	dest.matOrient = Matrix33Transpose( matOrient );
	dest.vPosition = dest.matOrient * ( -1.0f * vPosition );
}


template<typename T>
inline tMatrix34<T> tMatrix34<T>::GetInverseROT() const
{
	tMatrix34<T> dest;
	GetInverseROT( dest );
	return dest;
}


// copy orientation & translation from 4x4 row major matrix
template<typename T>
inline void tMatrix34<T>::CopyFromRowMajorMatrix44( T *pSrcData )
{
	matOrient.CopyRowMajorMatrix4( pSrcData );
	vPosition.x = *(pSrcData + 4 * 3 + 0);
	vPosition.y = *(pSrcData + 4 * 3 + 1);
	vPosition.z = *(pSrcData + 4 * 3 + 2);
}


template<typename T>
inline void tMatrix34<T>::GetRowMajorMatrix44( T *pDest ) const
{
	matOrient.GetRowMajorMatrix44( pDest );
	*(pDest + 4 * 3 + 0) = vPosition.x;
	*(pDest + 4 * 3 + 1) = vPosition.y;
	*(pDest + 4 * 3 + 2) = vPosition.z;
}


template<typename T>
inline bool tMatrix34<T>::operator==( const tMatrix34<T>& rhs ) const
{
	return ( (vPosition == rhs.vPosition) && (matOrient == rhs.matOrient) );
}


template<typename T>
inline tVector3<T> tMatrix34<T>::operator*( const tVector3<T> &rhs ) const
{
	return matOrient * rhs + vPosition;
}



//===================================================================================
// global operators
//===================================================================================

// used to calc the combination of two poses
// usually, 
//   lhs == world pose
//   rhs == local pose
template<typename T>
inline tMatrix34<T> operator*(const tMatrix34<T> & lhs, const tMatrix34<T> & rhs)
{
	tMatrix34<T> out;

	out.vPosition = lhs.matOrient * rhs.vPosition + lhs.vPosition;
	out.matOrient = lhs.matOrient * rhs.matOrient;
	return out;
}



//===================================================================================
// global functions
//===================================================================================

template<typename T>
inline tMatrix34<T> tMatrix34Identity()
{
	return tMatrix34<T>( tVector3<T>(0,0,0), tMatrix33Identity<T>() );

}


//===================================================================================
// typedefs
//===================================================================================

typedef tMatrix34<float> Matrix34;
typedef tMatrix34<double> dMatrix34;

#define Matrix34Identity tMatrix34Identity<float>
#define dMatrix3Identity tMatrix34Identity<double>



#endif  /*  __MATRIX34_H__  */
