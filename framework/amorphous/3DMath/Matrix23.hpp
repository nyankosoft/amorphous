#ifndef __Matrix23_H__
#define __Matrix23_H__


#include "Vector2.hpp"
#include "Matrix22.hpp"


namespace amorphous
{


template<typename T>
class tMatrix23
{
public:

	/// rotation
	tMatrix22<T> matOrient;

	/// translation
	tVector2<T> vPosition;

public:

	inline tMatrix23<T>() {}

	inline tMatrix23( const tVector2<T>& pos, const  tMatrix22<T>& orientation );

	inline void Identity();

	inline void Default() { Identity(); }

	/// apply the affine transformation to a three element vector
	inline void Transform( tVector2<T>& rvDest, const tVector2<T>& rvSrc ) const;

	/// apply the inverse transformation to a three element vector
	inline void InvTransform( tVector2<T>& rvDest, const tVector2<T>& rvSrc ) const;

	/// get the inverse -- assumes that the orientation matrix is orthogonal
	inline void GetInverseROT( tMatrix23<T>& dest ) const;

	/// Non-peformance friendly version of GetInverseROT()
	inline tMatrix23<T> GetInverseROT() const;

	/// copy orientation & translation from 4x4 row major matrix
	/// translation needs to be stored in the fourth row of
	/// the original row major matrix
//	inline void CopyFromRowMajorMatrix44( Scalar *pSrcData );

	/// get data in the form of 4x4 row major matrix
//	inline void GetRowMajorMatrix44( Scalar *pDest ) const;

	//friend tMatrix23<T> operator*(const tMatrix23<T> & lhs, const tMatrix23<T> & rhs);

};



//===================================================================================
// inline implementations
//===================================================================================

template<typename T>
inline tMatrix23<T>::tMatrix23( const tVector2<T>& pos, const  tMatrix22<T>& orientation )
:
vPosition(pos),
matOrient(orientation) {}


template<typename T>
inline void tMatrix23<T>::Identity()
{
	vPosition = tVector2<T>(0,0);
	matOrient =  tMatrix22Identity<T>();
}


template<typename T>
inline void tMatrix23<T>::Transform( tVector2<T>& rvDest, const tVector2<T>& rvSrc ) const
{
	rvDest = matOrient * rvSrc;
	rvDest += vPosition;
}


template<typename T>
inline void tMatrix23<T>::InvTransform( tVector2<T>& rvDest, const tVector2<T>& rvSrc ) const
{
	matOrient.TransformByTranspose( rvDest, ( rvSrc - vPosition ) );
}


template<typename T>
inline void tMatrix23<T>::GetInverseROT( tMatrix23<T>& dest ) const
{
	dest.matOrient =  Matrix22Transpose( matOrient );
	dest.vPosition = dest.matOrient * ( -1.0f * vPosition );
}


template<typename T>
inline tMatrix23<T> tMatrix23<T>::GetInverseROT() const
{
	tMatrix23<T> dest;
	GetInverseROT( dest );
	return dest;
}


// used to calc the combination of two poses
// usually,
//   lhs == world pose
//   rhs == local pose
template<typename T>
inline tMatrix23<T> operator*(const tMatrix23<T> & lhs, const tMatrix23<T> & rhs)
{
	tMatrix23<T> out;

	out.vPosition = lhs.matOrient * rhs.vPosition + lhs.vPosition;
	out.matOrient = lhs.matOrient * rhs.matOrient;
	return out;
}


template<typename T>
inline tVector2<T> operator*(const tMatrix23<T> & pose, const tVector2<T> & pos)
{
	return pose.matOrient * pos + pose.vPosition;
}


// global functions

template<typename T>
inline tMatrix23<T> tMatrix23Identity()
{
	return tMatrix23<T>( tVector2<T>(0,0),  tMatrix22Identity<T>() );

}


//===================================================================================
// typedefs
//===================================================================================

typedef tMatrix23<float> Matrix23;
typedef tMatrix23<double> dMatrix23;

#define Matrix23Identity tMatrix23Identity<float>
#define dMatrix23Identity tMatrix23Identity<double>


} // namespace amorphous



#endif  /*  __Matrix23_H__  */
