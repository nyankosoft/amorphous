#ifndef  __AsyncResourceLoader_H__
#define  __AsyncResourceLoader_H__


#include <queue>
#include <boost/thread.hpp>
#include "GraphicsResourceManager.h"

#include "Support/Singleton.h"
using namespace NS_KGL;


class CAsyncResourceLoader;


class CResourceLoadRequest
{
public:

	enum Type
	{
		LoadFromDisk,
		CopyToGraphicsMemory,
		NumRequestTypes
	};

private:

	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

//	int	m_ResourceEntryIndex;

	/// all the loaders have resource desc?
	boost::shared_ptr<CGraphicsResourceLoader> m_pLoader;

	Type m_RequestType;

public:

	CResourceLoadRequest( Type type,
		boost::weak_ptr<CGraphicsResourceEntry> pEntry//,
//		int entry_index
		)
		:
	m_RequestType(type),
	m_pResourceEntry(pEntry)
//	m_ResourceEntryIndex(entry_index)
	{}

	Type GetRequestType() const { return m_RequestType; }

//	boost::weak_ptr<CGraphicsResourceEntry> GetResourceEntry() { return m_pResourceEntry; }

	friend class CAsyncResourceLoader;
	friend class CGraphicsResourceManager;
};


/// request to lock/unlock graphics resource memory
/// - e.g.) vertex buffer, index buffer, texture, etc.
class CGraphicsDeviceRequest
{
public:

	enum Type
	{
		Lock,
		Unlock,
		NumRequestTypes
	};

	Type m_RequestType;

	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

	/// all the loaders have resource desc?
	boost::shared_ptr<CGraphicsResourceLoader> m_pLoader;

public:

	CGraphicsDeviceRequest( CGraphicsDeviceRequest::Type type, boost::weak_ptr<CGraphicsResourceEntry> pEntry )
		:
	m_RequestType(type),
	m_pResourceEntry(pEntry)
	{}


	friend class CAsyncResourceLoader;
};


class CAsyncResourceLoader
{
	boost::mutex m_Mutex;

	std::queue<CResourceLoadRequest> m_ResourceLoadRequestQueue;

	boost::mutex m_IOMutex;

	std::queue<CGraphicsDeviceRequest> m_GraphicsDeviceRequestQueue;

	boost::mutex m_GraphicsDeviceMutex;

	boost::shared_ptr<boost::thread> m_pIOThread;

	bool m_bEndIOThread;

private:

	class CIOThreadStarter
	{
		CAsyncResourceLoader *m_pAsyncLoader;

	public:

		CIOThreadStarter( CAsyncResourceLoader *pAsyncLoader )
			:
		m_pAsyncLoader(pAsyncLoader)
		{}

		void operator()()
		{
			// start the thread
			m_pAsyncLoader->IOThreadMain();
		}
	};

private:

	/// Called by resource IO thread
	void ProcessResourceLoadRequests();

protected:

	/// singleton
	static CSingleton<CAsyncResourceLoader> m_obj;

public:

	CAsyncResourceLoader();

	/// Release the resource IO thread
	void Release();

	static CAsyncResourceLoader* Get() { return m_obj.get(); }

	bool AddResourceLoadRequest( const CResourceLoadRequest& req );

	/// Main loop for the resource IO thread
	void IOThreadMain();

	/// Called by the render thread
	void ProcessGraphicsDeviceRequests();
};


inline CAsyncResourceLoader& AsyncResourceLoader()
{
	return (*CAsyncResourceLoader::Get());
}


#endif  /* __AsyncResourceLoader_H__ */
