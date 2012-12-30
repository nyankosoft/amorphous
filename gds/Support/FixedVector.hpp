#ifndef __fixed_vector_HPP__
#define __fixed_vector_HPP__

#include <assert.h>


namespace amorphous
{

template <class CArrayType, int iArraySize>
class fixed_vector
{
	CArrayType m_Array[iArraySize];
	int m_iIndex;

public:

	fixed_vector() { m_iIndex = 0; }

	inline int size() const { return m_iIndex; }

	inline void clear() { m_iIndex = 0; }

	inline bool empty() const { return (m_iIndex == 0); }

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

	inline void erase_at( int pos );

	inline CArrayType& at( int i ) { return m_Array[i]; }

};

/// deprecated
#define TCFixedVector fixed_vector 


template <class CPointerType, int iArraySize>
class fixed_pointer_vector
{
	CPointerType *m_Array[iArraySize];
	int m_iIndex;

public:

	inline fixed_pointer_vector() : m_iIndex(0) {}

	inline int size() { return m_iIndex;}
	inline void clear() { m_iIndex = 0; }
	inline void push_back( CPointerType *pElement );
	inline CPointerType *back() { return m_Array[m_iIndex-1]; }
	inline CPointerType *operator[] (int iIndex);

};

/// deprecated
#define TCFixedPointerVector fixed_pointer_vector


//======================================================================================
// inline implementations
//======================================================================================


template <class CArrayType, int iArraySize>
inline void fixed_vector<CArrayType, iArraySize>::push_back( const CArrayType& rElement )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = rElement;
}

template <class CArrayType, int iArraySize>
inline void fixed_vector<CArrayType, iArraySize>::resize( int size, const CArrayType& val )
{
	int i;
	for( i=0; i<size; i++ )
		m_Array[i] = val;

	m_iIndex = size;
}


template <class CArrayType, int iArraySize>
inline void fixed_vector<CArrayType, iArraySize>::insert_at( int pos, const CArrayType& val )
{
	int i, size = this->size();

	push_back( CArrayType() ); // m_iIndex is incremented in this call

	for( i=size; i<=pos+1; i-- )
	{
		m_Array[i] = m_Array[i-1];
	}

	m_Array[pos] = val;
}


template <class CArrayType, int iArraySize>
inline void fixed_vector<CArrayType, iArraySize>::erase_at( int pos )
{
	int i, size = this->size();

	if( size <= pos )
		return;

	for( i=pos; i<size-1; i++ )
	{
		m_Array[i] = m_Array[i+1];
	}

	m_iIndex--;
}

/*
template <class CArrayType, int iArraySize>
inline void fixed_vector<CArrayType, iArraySize>::push_back( CArrayType element )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = element;
}*/


template <class CArrayType, int iArraySize>
inline CArrayType& fixed_vector<CArrayType, iArraySize>::operator[] ( const int iIndex )
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}


template <class CArrayType, int iArraySize>
inline const CArrayType& fixed_vector<CArrayType, iArraySize>::operator[] ( const int iIndex ) const
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}





template <class CPointerType, int iArraySize>
inline void fixed_pointer_vector<CPointerType, iArraySize>::push_back( CPointerType *pElement )
{
	assert( m_iIndex < iArraySize );
	m_Array[m_iIndex++] = pElement;
}


template <class CPointerType, int iArraySize>
inline CPointerType *fixed_pointer_vector<CPointerType, iArraySize>::operator[] (int iIndex)
{
	assert( iIndex < iArraySize );
	return m_Array[iIndex];
}


/// 4:21 AM 9/13/2010 - Commented out: 2 options but both have drawbacks.
/// 1. Use conversion function from string to other types. See from_string_to_x() below.
///   - Does not work in the first place because this requires template functions that differs only in return types?
/// 2. Use conv_to_x() in CTextFileScanner.hpp
///   - Need to create a temporary object and copy as many times as the number of scanned elements.
/*
/// For CTextFileScanner
template <class T, int iArraySize>
inline void conv_to_x( std::vector<std::string>& src, int& index, fixed_vector<T,iArraySize>& dest )
{
// 1.
//	dest.resize( 0 );
//	while( 1 <= (int)src.size() - index )
//	{
//		dest.push_back( from_string_to_x<T>(src[index]) );
//		index += 1;
//	}

// 2.
//	dest.resize( 0 );
//	while( 1 <= (int)src.size() - index )
//	{
//		T t;
//		conv_to_x( src, index, t );
//		dest.push_back( t );
//	}
}*/
} // amorphous



#endif  /*  __fixed_vector_HPP__  */
