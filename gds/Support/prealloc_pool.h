#ifndef  __gsf_prealloc_pool_H__
#define  __gsf_prealloc_pool_H__


#include <vector>
#include "SafeDeleteVector.h"


template <class CElementType>
class default_pooled_object_initializer
{
public:
	void operator()( CElementType *p ) {}
};


class pooled_object
{

	int m_PooledObjectStockIndex;
	int m_PooledObjectStockID;

public:

	pooled_object() {}

	int GetStockIndex() const { return m_PooledObjectStockIndex; }
	void SetStockIndex( int index ) { m_PooledObjectStockIndex = index; }

	int GetStockID() const { return m_PooledObjectStockID; }
	void SetStockID( int id ) { m_PooledObjectStockID = id; }
};


inline int get_prealloc_object_id()
{
	static int s_IDCounter = -1;
	s_IDCounter = ( s_IDCounter + 1 ) % INT_MAX;

	return s_IDCounter;
};


class pooled_object_handle
{
	int index;
	int id;

public:

	pooled_object_handle() : index(-1), id(-1) {}

	pooled_object_handle( int _index, int _id ) : index(_index), id(_id) {}

	template<class T> friend class prealloc_pool;
};


/**
object pool
- Class that uses prealloc_pool needs to have the following member functions.
  - default constructor
    - constructor with no args

  - void SetStockIndex( int id )
    - sets stock index
  - int GetStockIndex() const
    - returns index set by SetStockIndex() above

  - void SetStockID( int id )
    - sets stock id
  - int GetStockID() const
    - returns id set by SetStockID() above



	====================== template ======================

	class CElement
	{

		int m_StockIndex;
		int m_StockID;

	public:

		CElement() {}

		int GetStockIndex() const { return m_StockIndex; }
		void SetStockIndex( int index ) { m_StockIndex = index; }

		int GetStockID() const { return m_StockID; }
		void SetStockID( int id ) { m_StockID = id; }
	};
*/
template <class CElementType>
class prealloc_pool
{
protected:

	std::vector<int> m_VacantSlot;

	int m_VacantSlotPos;

	int m_IDCounter;

	/// objects
	std::vector<CElementType *> m_vecpStock;

public:

	prealloc_pool()
		:
	m_VacantSlotPos(-1),
	m_IDCounter(0)
	{
	}

	virtual ~prealloc_pool()
	{
		release_all();
	}

	/// deletes all the objects
	void release_all()
	{
		m_VacantSlot.resize( 0 );

		SafeDeleteVector( m_vecpStock );
		m_vecpStock.resize( 0 );
	}

	/// \param num_stocks must be much smaller than INT_MAX
	void init( int num_stocks )
	{
		init( num_stocks, default_pooled_object_initializer<CElementType>() );
	}

	template<class T>
	void init( int num_stocks, const T& initializer )
	{
		if( num_stocks < 0 )
			return;

		release_all();

		m_vecpStock.resize( num_stocks );

		m_VacantSlot.resize( num_stocks );

		int i;
		for( i=0; i<num_stocks; i++ )
		{
			m_VacantSlot[i] = i;

			m_vecpStock[i] = new CElementType();
			m_vecpStock[i]->SetStockIndex( i );
			m_vecpStock[i]->SetStockID( -1 );

			T()( m_vecpStock[i] );
		}

		m_VacantSlotPos = num_stocks - 1;
	}

	/// returns a new object
	/// - The caller must not delete the returned object. You have been warned.
	///   - Call prealloc_pool::release()
	/// Note that this function does not call new
	CElementType *get_new_object()
	{
		if( 0 <= m_VacantSlotPos )
		{
			CElementType *pObject = m_vecpStock[ m_VacantSlot[m_VacantSlotPos] ];

			m_VacantSlotPos--;

			// set the id 
			// - The counter is incremented in get_prealloc_object_id().
			pObject->SetStockID( get_prealloc_object_id() );

			return pObject;
		}
		else
		{
			// ran out of stock!
			return NULL;
		}
	}

	/// creates and returns a handle for an existing object
	/// \param pObject must be a pointer to an object obtained as a return value of prealloc_pool::get_new_object().
	pooled_object_handle get_handle( CElementType *pObject )
	{
		if( pObject )
			return pooled_object_handle( pObject->GetStockIndex(), pObject->GetStockID() );
		else
			return pooled_object_handle();
	}

	/// get a handle to a new object
	/// - returns a handle to retrieve the instance later
	///   with prealloc_pool::get( const pooled_object_handle& handle )
	pooled_object_handle get_new_handle()
	{
		return get_handle( get_new_object() );
	}

	/// does not call the destructor
	void release( CElementType *pObject )
	{
		m_VacantSlotPos++;
		m_VacantSlot[ m_VacantSlotPos ] = pObject->GetStockIndex();
		pObject->SetStockID( -1 );
	}

	/// returns an object pointed to by handle.
	/// returns NULL if
	/// - the object has already been released
	/// - the argument handle is invalid
	CElementType *get( const pooled_object_handle& handle )
	{
		if( handle.index < 0 || (int)m_vecpStock.size() <= handle.index )
			return NULL;

		CElementType *pObject = m_vecpStock[handle.index];

		if( pObject->GetStockID() == handle.id )
			return pObject;
		else
			return NULL; // the requested object has already been released
	}
};


#endif		/*  __gsf_prealloc_pool_H__  */
