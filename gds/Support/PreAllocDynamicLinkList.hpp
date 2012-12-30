
#ifndef  __PREALLOCDYNAMICLINKLIST_H__
#define  __PREALLOCDYNAMICLINKLIST_H__

#include  <assert.h>


namespace amorphous
{


struct SLinkListNode;


#include <vector>
#include <algorithm>


/*
bool (*_CompFunc)( ?? );

template<class T> 
bool TCompFunc( TCPreAllocDynamicLinkList<T>::SLinkListNode* node0,
			    TCPreAllocDynamicLinkList<T>::SLinkListNode* node1 )
{
	_CompFunc
	
}*/


template <class CElementType>
class TCPreAllocDynamicLinkList
{
protected:

	struct SLinkListNode
	{
		friend class TCPreAllocDynamicLinkList;

		CElementType element;
		SLinkListNode *m_pNext;
		SLinkListNode *m_pPrev;

	public:
		SLinkListNode() { m_pPrev = m_pNext = NULL; }
	};


	SLinkListNode *m_pUnusedList;
	SLinkListNode *m_pActiveList;

	/// used for soring
	std::vector<SLinkListNode *> m_vecpTempList;

public:

	class LinkListIterator
	{
		SLinkListNode *m_pNodePtr;

	public:
		LinkListIterator() { m_pNodePtr = NULL; }
		LinkListIterator(SLinkListNode *pNodePtr) { m_pNodePtr = pNodePtr; }

		void operator++(int)
		{	m_pNodePtr = m_pNodePtr->m_pNext;	}

		void operator--(int)
		{	m_pNodePtr = m_pNodePtr->m_pPrev;	}

		CElementType& operator*() const
		{	return m_pNodePtr->element;	}

		CElementType *operator->() const
		{	return &(m_pNodePtr->element);	}

		bool operator==(const LinkListIterator& iterator)
		{	return (m_pNodePtr == iterator.m_pNodePtr);	}

		bool operator!=(const LinkListIterator& iterator)
		{	return (!(*this == iterator));	}
	};

	LinkListIterator Begin() { return LinkListIterator(m_pActiveList); }
	LinkListIterator End() { return LinkListIterator(); }

	TCPreAllocDynamicLinkList()
	{
		m_pUnusedList = NULL;
		m_pActiveList = NULL;
	}

	~TCPreAllocDynamicLinkList() { Release(); }

	void Init( int iNumDefaultElements )
	{
		// create and link the first element to the unused list
		m_pUnusedList = new SLinkListNode;
		m_pUnusedList->m_pPrev = NULL;
		m_pUnusedList->m_pNext = NULL;

		// create elements and link to the unused list
		SLinkListNode *pNewElement;
		int i;
		for( i=0; i<iNumDefaultElements-1; i++ )
		{
			pNewElement = new SLinkListNode;
			m_pUnusedList->m_pPrev = pNewElement;
			pNewElement->m_pNext = m_pUnusedList;
			m_pUnusedList = pNewElement;
		}
	}
	
	CElementType *GetNewObject()
	{
		// get one from the unused list
		SLinkListNode *pNewElement = m_pUnusedList;
		if( pNewElement )
		{
			if( pNewElement->m_pNext )
				pNewElement->m_pNext->m_pPrev = NULL;
			m_pUnusedList = pNewElement->m_pNext;
		}
		else
		{	// no available element is left - create a new one
			pNewElement = new SLinkListNode;
		}

		// link to 'pNewElement' to the head of the active list
		if( m_pActiveList )
		{	// some are already linked to the active list
			pNewElement->m_pNext = m_pActiveList;
			m_pActiveList->m_pPrev = pNewElement;
		}
		else	// the is a first link to the active list
			pNewElement->m_pNext = NULL;

		pNewElement->m_pPrev = NULL;
		m_pActiveList = pNewElement;

		return &(pNewElement->element);
	}

	void Release( CElementType *&pElement )	// release element
	{
		SLinkListNode *p = m_pActiveList;
		while( p )
		{
			if( &(p->element) == pElement )
			{	// found requested element - release 'p->element' from the active list 
				if( p == m_pActiveList )
					m_pActiveList = p->m_pNext;	// 'p->element' is the first element in 'm_pActiveList'
	
				if( p->m_pNext )
					p->m_pNext->m_pPrev = p->m_pPrev;
				if( p->m_pPrev )
					p->m_pPrev->m_pNext = p->m_pNext;

				// re-link to the unused list
				p->m_pPrev = NULL;
				if( m_pUnusedList )
				{
					m_pUnusedList->m_pPrev = p;
					p->m_pNext = m_pUnusedList;
				}
				else
					p->m_pNext = NULL;

				m_pUnusedList = p;

				pElement = NULL;	// released pointer is invalid - clear it with NULL
				return;
			}

			p = p->m_pNext;
		}

		assert( 0 );	// invalid release call - requested element was not found in the active list
	}


	void Reset()	// link all the elements to the unused list
	{
		SLinkListNode* p = m_pActiveList;
		if( !p )
			return;		// all the elements are in the unused list

		// find the end of the active list
		while( p->m_pNext )
		{
			p = p->m_pNext;
		}

		// p is the end element in the active list
		// insert the entire active list into the head of unused list
		if( m_pUnusedList )
		{
			m_pUnusedList->m_pPrev = p;
			p->m_pNext = m_pUnusedList;
		}

		m_pUnusedList = m_pActiveList;
		m_pActiveList = NULL;	
	}

	void Release()
	{
		// link all the elements to the unused list
		Reset();

		SLinkListNode *pNext, *p = m_pUnusedList;
		while( p )
		{
			// save pointer to the next
			pNext = p->m_pNext;
			delete p;
			p = pNext;
		}

		m_pUnusedList = NULL;
	}


	void Sort( bool (*CompFunc)( CElementType*, CElementType* ) )
	{
		m_vecpTempList.resize( 0 );
        
/*		LinkListIterator itr;
		itr = Begin();

		// move all the active elements to temp list
		while( itr != End() )
		{
			m_vecpTempList.push_back( &(*itr) );
			itr++;
		}*/

		SLinkListNode *p;
		for( p = m_pActiveList; p != NULL; p = p->m_pNext )
		{
			m_vecpTempList.push_back( p );
		}

		_QSort( m_vecpTempList, 0, m_vecpTempList.size() - 1, CompFunc);

//		sort( m_vecpTempList.begin(), m_vecpTempList.end(), CompFunc );


		/// re-link objects to active list
		m_pActiveList = m_vecpTempList[0];
		m_pActiveList->m_pPrev = NULL;

		SLinkListNode *pPrev = m_pActiveList;
		int i, num = m_vecpTempList.size();
		for( i=1; i<num; i++ )
		{
			m_vecpTempList[i]->m_pPrev = pPrev;
			pPrev->m_pNext = m_vecpTempList[i];
			pPrev = pPrev->m_pNext;
		}
		pPrev->m_pNext = NULL;


	}

private:

	void _QSort( std::vector<SLinkListNode *>& v, int left, int right,
		bool (*CompFunc)( CElementType*, CElementType* ) )
	{
		// qsort
		int i, last;
		if( left >= right )
			return;

//		swap( v, left, (left + right) / 2 );
		swap( v[left], v[(left + right) / 2] );
		last = left;
		for( i = left+1; i <= right; i++ )
		{
			if( (*CompFunc)( &(v[i]->element), &(v[left]->element) ) )
//				swap( v, ++last, i );
				swap( v[++last], v[i] );
		}
//		swap( v, left, last );
		swap( v[left], v[last] );
		_QSort( v, left, last-1, CompFunc );
		_QSort( v, last+1, right, CompFunc );
	}

//	TCPreAllocDynamicLinkList();
//	~TCPreAllocDynamicLinkList() { Release(); }
//	void Init( int iNumDefaultElements )
//	CElementType *GetNewObject();
//	void Release( CElementType *pElement );	// release element
//	void Reset();	// link all the elements to the unused list
//	void Release();

};


} // amorphous



#endif		/*  __PREALLOCDYNAMICLINKLIST_H__  */
