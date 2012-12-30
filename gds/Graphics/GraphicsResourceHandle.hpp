#ifndef  __GraphicsResourceHandle_H__
#define  __GraphicsResourceHandle_H__


#include <string>

#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceEntries.hpp"


namespace amorphous
{


class CGraphicsResourceHandle
{
protected:

	boost::shared_ptr<CGraphicsResourceEntry> m_pResourceEntry;

//	void IncResourceRefCount();
//	void DecResourceRefCount();

	inline void copy( const CGraphicsResourceHandle& handle );

public:

	inline CGraphicsResourceHandle() {}

	/// Release() is called in the dtor of each derived handle class
	virtual ~CGraphicsResourceHandle() {}

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	boost::shared_ptr<CGraphicsResourceEntry> GetEntry() { return m_pResourceEntry; }

	const boost::shared_ptr<CGraphicsResourceEntry> GetEntry() const { return m_pResourceEntry; }

	inline bool IsLoaded() const;

	virtual GraphicsResourceState::Name GetEntryState();

	inline void Release();

	/// reload any updated file since the last load
//	void Refresh();

	inline const CGraphicsResourceHandle &operator=( const CGraphicsResourceHandle& handle );

	inline CGraphicsResourceHandle( const CGraphicsResourceHandle& handle );

//	static const CGraphicsResourceHandle ms_NullHandle;
//	static const CGraphicsResourceHandle& Null() { return ms_NullHandle; }

	friend class CGraphicsResourceManager;
};


//=================================== inline implementations ===================================


inline void CGraphicsResourceHandle::Release()
{
	if( m_pResourceEntry )
	{
		m_pResourceEntry->DecRefCount();
		m_pResourceEntry.reset();
	}
}


inline bool CGraphicsResourceHandle::IsLoaded() const
{
	if( GetEntry()
	 && GetEntry()->GetResource()
	 && GetEntry()->GetResource()->GetState() == GraphicsResourceState::LOADED )
	{
		return true;
	}
	else
		return false;
}


inline void CGraphicsResourceHandle::copy( const CGraphicsResourceHandle& handle )
{
	if( m_pResourceEntry )
        m_pResourceEntry->DecRefCount(); // decrement the reference count of the current resource

	m_pResourceEntry = handle.m_pResourceEntry;

	if( m_pResourceEntry )
        m_pResourceEntry->IncRefCount(); // increment the reference count of the newly assigned texture
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

} // namespace amorphous



#endif  /* __GraphicsResourceHandle_H__ */
