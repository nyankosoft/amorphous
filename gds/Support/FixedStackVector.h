
#ifndef __TFIXEDSTACKVECTOR_H__
#define __TFIXEDSTACKVECTOR_H__

#include <assert.h>

template <class CArrayType, int iArraySize>
class TCFixedStackVector
{
	CArrayType m_Array[iArraySize];
	int m_aiIndexStack[iArraySize];	// list of indices to empty(unused) elements
	int m_iIndex;

public:

	TCFixedStackVector()
	{
		m_iIndex = 0;
		for( int i=0; i<iArraySize; i++ )
			m_aiIndexStack[i] = i;
	}

	// invalidate all the elements
	// CAUTION: when Clear() is called, all the indices to elements will become invalid
	inline void Clear() { m_iIndex = 0; }

	// get one of available(empty) elements and returns its index
	inline int GetNewElementIndex()
	{
		assert( m_iIndex < iArraySize );	// check if an available element is left
		return m_aiIndexStack[m_iIndex++];
	}

	// returns reference to the i-th element
	inline CArrayType& operator[]( int iIndex )
	{
		assert( 0<=iIndex && iIndex<iArraySize );
		return m_Array[iIndex];
	}

	// releases the i-th element
	inline void Release( int iIndex )
	{
		m_iIndex--;
		assert( 0<=m_iIndex );
		m_aiIndexStack[m_iIndex] = iIndex;	// add to the list of indices to empty elements
	}

};


#endif  /*  __TFIXEDSTACKVECTOR_H__  */