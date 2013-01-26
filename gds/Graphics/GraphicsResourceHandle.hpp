#ifndef  __GraphicsResourceHandle_H__
#define  __GraphicsResourceHandle_H__


#include <string>

#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceEntries.hpp"


namespace amorphous
{


class GraphicsResourceHandle
{
protected:

	boost::shared_ptr<GraphicsResourceEntry> m_pResourceEntry;

//	void IncResourceRefCount();
//	void DecResourceRefCount();

	inline void copy( const GraphicsResourceHandle& handle );

public:

	inline GraphicsResourceHandle() {}

	/// Release() is called in the dtor of each derived handle class
	virtual ~GraphicsResourceHandle() {}

	virtual GraphicsResourceType::Name GetResourceType() const = 0;

	boost::shared_ptr<GraphicsResourceEntry> GetEntry() { return m_pResourceEntry; }

	const boost::shared_ptr<GraphicsResourceEntry> GetEntry() const { return m_pResourceEntry; }

	inline bool IsLoaded() const;

	virtual GraphicsResourceState::Name GetEntryState();

	inline void Release();

	/// reload any updated file since the last load
//	void Refresh();

	inline const GraphicsResourceHandle &operator=( const GraphicsResourceHandle& handle );

	inline GraphicsResourceHandle( const GraphicsResourceHandle& handle );

//	static const GraphicsResourceHandle ms_NullHandle;
//	static const GraphicsResourceHandle& Null() { return ms_NullHandle; }

	friend class GraphicsResourceManager;
};


//=================================== inline implementations ===================================


inline void GraphicsResourceHandle::Release()
{
	if( m_pResourceEntry )
	{
		m_pResourceEntry->DecRefCount();
		m_pResourceEntry.reset();
	}
}


inline bool GraphicsResourceHandle::IsLoaded() const
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


inline void GraphicsResourceHandle::copy( const GraphicsResourceHandle& handle )
{
	if( m_pResourceEntry )
        m_pResourceEntry->DecRefCount(); // decrement the reference count of the current resource

	m_pResourceEntry = handle.m_pResourceEntry;

	if( m_pResourceEntry )
        m_pResourceEntry->IncRefCount(); // increment the reference count of the newly assigned texture
}


inline const GraphicsResourceHandle &GraphicsResourceHandle::operator=( const GraphicsResourceHandle& handle )
{
	copy( handle );

	return *this;
}


inline GraphicsResourceHandle::GraphicsResourceHandle( const GraphicsResourceHandle& handle )
{
	copy( handle );
}

} // namespace amorphous



#endif  /* __GraphicsResourceHandle_H__ */
