#ifndef  __gsf_shared_prealloc_pool_H__
#define  __gsf_shared_prealloc_pool_H__


#include "prealloc_pool.h"
#include <boost/shared_ptr.hpp>


/**
  See comments at prealloc_pool.h
*/
template <class CElementType>
class shared_prealloc_pool
{
protected:

	typedef boost::shared_ptr<CElementType> ElementPointer;

	std::vector<int> m_VacantSlot;

	int m_VacantSlotPos;

	int m_IDCounter;

	/// objects
	std::vector<ElementPointer> m_vecpStock;

public:

	shared_prealloc_pool()
		:
	m_VacantSlotPos(-1),
	m_IDCounter(0)
	{
	}

	virtual ~shared_prealloc_pool()
	{
		release_all();
	}

	/// deletes all the objects
	void release_all()
	{
		m_VacantSlot.resize( 0 );

		m_vecpStock.resize( 0 );
	}

	/// \param num_stocks must be much smaller than INT_MAX
	void init( int num_stocks )
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

			m_vecpStock[i] = boost::shared_ptr<CElementType>( new CElementType() );
			m_vecpStock[i]->SetStockIndex( i );
			m_vecpStock[i]->SetStockID( -1 );
		}

		m_VacantSlotPos = num_stocks - 1;
	}

	/// returns a new object
	/// - The caller must not delete the returned object. You have been warned.
	///   - Call shared_prealloc_pool::release()
	/// Note that this function does not call new
	ElementPointer get_new_object()
	{
		if( 0 <= m_VacantSlotPos )
		{
			ElementPointer pObject = m_vecpStock[ m_VacantSlot[m_VacantSlotPos] ];

			m_VacantSlotPos--;

			// set the id 
			// - The counter is incremented in get_prealloc_object_id().
			pObject->SetStockID( get_prealloc_object_id() );

			return pObject;
		}
		else
		{
			// ran out of stock!
			return ElementPointer();
		}
	}

	/// creates and returns a handle for an existing object
	/// \param pObject must be a pointer to an object obtained as a return value of shared_prealloc_pool::get_new_object().
	pooled_object_handle get_handle( ElementPointer pObject )
	{
		if( pObject )
			return pooled_object_handle( pObject->GetStockIndex(), pObject->GetStockID() );
		else
			return pooled_object_handle();
	}

	/// get a handle to a new object
	/// - returns a handle to retrieve the instance later
	///   with shared_prealloc_pool::get( const pooled_object_handle& handle )
	pooled_object_handle get_new_handle()
	{
		return get_handle( get_new_object() );
	}

	/// does not call the destructor
	void release( ElementPointer pObject )
	{
		m_VacantSlotPos++;
		m_VacantSlot[ m_VacantSlotPos ] = pObject->GetStockIndex();
		pObject->SetStockID( -1 );
	}

	/// returns an object pointed to by handle.
	/// returns NULL if
	/// - the object has already been released
	/// - the argument handle is invalid
	ElementPointer get( const pooled_object_handle& handle )
	{
		if( handle.index < 0 || (int)m_vecpStock.size() <= handle.index )
			return NULL;

		ElementPointer pObject = m_vecpStock[handle.index];

		if( pObject->GetStockID() == handle.id )
			return pObject;
		else
			return NULL; // the requested object has already been released
	}
};


#endif		/*  __gsf_shared_prealloc_pool_H__  */
