#ifndef  __GraphicsResourceHandle_H__
#define  __GraphicsResourceHandle_H__

#include <string>

#include "GraphicsResource.h"

#include "Support/Serialization/ArchiveObjectBase.h"
using namespace GameLib1::Serialization;


class CGraphicsResourceHandle : public IArchiveObjectBase
{
protected:

	int m_EntryID;

	void IncResourceRefCount();
	void DecResourceRefCount();

	inline void copy( const CGraphicsResourceHandle& handle );

public:

	std::string filename;

public:

	inline CGraphicsResourceHandle() : m_EntryID(-1) {}

	virtual ~CGraphicsResourceHandle() { Release(); }

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	virtual bool Load() = 0;

	/// priority 0 - 256
	virtual bool LoadAsync( int priority );

	bool IsLoaded() { return ( 0 <= m_EntryID ); }

	/// does not clear the filename
	/// - user can reload the resource after Release() by calling Load()
	/// - ( 0 <= m_EntryID ) check is done in DecResourceRefCount()
	///   but also added to this function in order to avoid non-inline calls when unused handle is realeased
	inline void Release();

	/// reload any updated file since the last load
//	void Refresh();

	inline const CGraphicsResourceHandle &operator=( const CGraphicsResourceHandle& handle );

	inline CGraphicsResourceHandle( const CGraphicsResourceHandle& handle );

	virtual void Serialize( IArchive& ar, const unsigned int version );

//	static const CGraphicsResourceHandle ms_NullHandle;
//	static const CGraphicsResourceHandle& Null() { return ms_NullHandle; }
};


//=================================== inline implementations ===================================


inline void CGraphicsResourceHandle::Release()
{
	if( 0 <= m_EntryID )
	{
		DecResourceRefCount();
		m_EntryID = -1;
	}
}


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

	filename = handle.filename;
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
