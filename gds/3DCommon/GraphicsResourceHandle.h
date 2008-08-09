#ifndef  __GraphicsResourceHandle_H__
#define  __GraphicsResourceHandle_H__

#include <string>

#include "GraphicsResource.h"
#include "GraphicsResourceDescs.h"

class CGraphicsResourceHandle
{
protected:

	int m_EntryID;

	virtual void IncResourceRefCount() = 0;
	virtual void DecResourceRefCount() = 0;

	inline void copy( const CGraphicsResourceHandle& handle );

public:

	inline CGraphicsResourceHandle() : m_EntryID(-1) {}

	/// Release() is called in the dtor of each derived handle class
	virtual ~CGraphicsResourceHandle() {}

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	bool IsLoaded() { return ( 0 <= m_EntryID ); }

	/// does not clear the filename
	/// - user can reload the resource after Release() by calling Load()
	virtual void Release() = 0;

	/// reload any updated file since the last load
//	void Refresh();

	inline const CGraphicsResourceHandle &operator=( const CGraphicsResourceHandle& handle );

	inline CGraphicsResourceHandle( const CGraphicsResourceHandle& handle );

//	static const CGraphicsResourceHandle ms_NullHandle;
//	static const CGraphicsResourceHandle& Null() { return ms_NullHandle; }

	friend class CGraphicsResourceManager;
};


//=================================== inline implementations ===================================

/*
inline void CGraphicsResourceHandle::Release()
{
	if( 0 <= m_EntryID )
	{
		DecResourceRefCount();
		m_EntryID = -1;
	}
}
*/

inline void CGraphicsResourceHandle::copy( const CGraphicsResourceHandle& handle )
{
	if( 0 <= m_EntryID )
	{
		// decrement the reference count of the current texture
		DecResourceRefCount();
	}

	m_EntryID = handle.m_EntryID;

	if( 0 <= m_EntryID )
	{
		// increment the reference count of the newly assigned texture
		IncResourceRefCount();
	}
}

inline const CGraphicsResourceHandle &CGraphicsResourceHandle::operator=( const CGraphicsResourceHandle& handle )
{
	copy( handle );

	return *this;
}


inline CGraphicsResourceHandle::CGraphicsResourceHandle( const CGraphicsResourceHandle& handle )
{
	copy( handle );
}


#endif  /* __GraphicsResourceHandle_H__ */
