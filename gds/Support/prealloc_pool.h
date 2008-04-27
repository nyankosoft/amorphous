#ifndef  __gsf_prealloc_pool_H__
#define  __gsf_prealloc_pool_H__


#include <stack>
#include <vector>
using namespace std;

#include <Support/SafeDeleteVector.h>

/**
object pool
class that uses prealloc_pool needs to have the following member function
  - defualt ctor()
    - ctor() with no args
  - void SetStockID( int id )
    returns id set by SetStockID() above
  - int GetStockID() const


	====================== template ======================

	class CElement
	{

		int m_StockID;

	public:

		CElement() {}

		int GetStockID() const { return m_StockID; }
		void SetStockID( int id ) { m_StockID = id; }
	};
*/
template <class CElementType>
class prealloc_pool
{
protected:

//	std::stack<int> m_VacantSlot; // stack
	std::vector<int> m_VacantSlot; // vector

	int m_VacantSlotPos; // vector

	/// objects
	std::vector<CElementType *> m_vecpStock;

public:

	prealloc_pool()
	{
	}

	virtual ~prealloc_pool()
	{
		release_all();
	}

	void release_all()
	{
//		while( !m_VacantSlot.empty() )
//			m_VacantSlot.pop();

		m_VacantSlot.resize( 0 );

		SafeDeleteVector( m_vecpStock );
		m_vecpStock.resize( 0 );
	}

	void init( int num_stocks )
	{
		if( num_stocks < 0 )
			return;

		release_all();

		m_vecpStock.resize( num_stocks );

		m_VacantSlot.resize( num_stocks ); // vector

		int i;
		for( i=0; i<num_stocks; i++ )
		{
//			m_VacantSlot.push( i ); // stack
			m_VacantSlot[i] = i; // vector

			m_vecpStock[i] = new CElementType();
			m_vecpStock[i]->SetStockID( i );
		}

		m_VacantSlotPos = num_stocks - 1;
	}

	/// does not call new 
	CElementType *get()
	{
//		if( 0 < m_VacantSlot.size() ) // stack
		if( 0 <= m_VacantSlotPos )
		{
			// take one from the vacant stack
//			CElementType *pObject = m_vecpStock[ m_VacantSlot.top() ]; // stack
			CElementType *pObject = m_vecpStock[ m_VacantSlot[m_VacantSlotPos] ]; // vector

//			m_VacantSlot.pop(); // stack
			m_VacantSlotPos--; // vector

			return pObject;
		}
		else
		{
			return NULL;
		}
	}

	// does not call the destructor
	void release( CElementType *pObject )
	{
//		m_VacantSlot.push( pObject->GetStockID() ); // stack

		m_VacantSlotPos++;
		m_VacantSlot[ m_VacantSlotPos ] = pObject->GetStockID();
	}
};


#endif		/*  __gsf_prealloc_pool_H__  */
