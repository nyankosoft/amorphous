
#ifndef __TFIXEDVECTOR_H__
#define __TFIXEDVECTOR_H__

#include <assert.h>

template <class CArrayType, int iArraySize>
class TCFixedVector
{
	CArrayType m_Array[iArraySize];
	int m_iIndex;

public:

	TCFixedVector() { m_iIndex = 0; }

	inline int size() const { return m_iIndex; }

	inline void clear() { m_iIndex = 0; }

	inline void push_back( const CArrayType& rElement );

//	inline void push_back( CArrayType element );

	inline CArrayType& back() { return m_Array[m_iIndex-1]; }

	/// does not actually delete the object at the end
	/// only reduce the size of the vector
	inline void pop_back() { assert( 0 < m_iIndex ); m_iIndex -= 1; }

	inline CArrayType& operator[] ( const int iIndex );

	inline const CArrayType& operator[] ( const int iIndex ) const;

	inline void resize( int size, const CArrayType& val = CArrayType() );

	/// insert an element at pos.
	/// incompatible with insert() in stl vector. but added for convenience
	inline void insert_at( int pos, const CArrayType& val = CArrayType() );

	inline CArrayType& at( int i ) { return m_Array[i]; }

};


template <class CPointerType, int iArraySize>
class TCFixedPointerVector
{
	CPointerType *m_Array[iArraySize];
	int m_iIndex;

public:

	inline TCFixedPointerVector() : m_iIndex(0) {}

	inline int size() { return m_iIndex;}
	inline void clear() { m_iIndex = 0; }
	inline void push_back( CPointerType *pElement );
	inline CPointerType *back() { return m_Array[m_iIndex-1]; }
	inline CPointerType *operator[] (int iIndex);

};


//======================================================================================
// inline implementations
//======================================================================================


template <class CArrayType, int iArraySize>
inline void TCFixedVector<CArrayType, iArraySize>::push_back( const CArrayType& rElement )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = rElement;
}

template <class CArrayType, int iArraySize>
inline void TCFixedVector<CArrayType, iArraySize>::resize( int size, const CArrayType& val )
{
	int i;
	for( i=0; i<size; i++ )
		m_Array[i] = val;

	m_iIndex = size;
}


template <class CArrayType, int iArraySize>
inline void insert_at( int pos, const CArrayType& val = CArrayType() )
{
	int i, size = size();

	push_back( CArrayType() );

	for( i=size; i<=pos+1; i-- )
	{
		m_Array[i] = m_Array[i-1];
	}

	m_Array[pos] = val;
}

/*
template <class CArrayType, int iArraySize>
inline void TCFixedVector<CArrayType, iArraySize>::push_back( CArrayType element )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = element;
}*/


template <class CArrayType, int iArraySize>
inline CArrayType& TCFixedVector<CArrayType, iArraySize>::operator[] ( const int iIndex )
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}


template <class CArrayType, int iArraySize>
inline const CArrayType& TCFixedVector<CArrayType, iArraySize>::operator[] ( const int iIndex ) const
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}





template <class CPointerType, int iArraySize>
inline void TCFixedPointerVector<CPointerType, iArraySize>::push_back( CPointerType *pElement )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = pElement;
}


template <class CPointerType, int iArraySize>
inline CPointerType *TCFixedPointerVector<CPointerType, iArraySize>::operator[] (int iIndex)
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}


#endif  /*  __TFIXEDVECTOR_H__  */