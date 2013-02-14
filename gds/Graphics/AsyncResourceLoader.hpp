#ifndef  __AsyncResourceLoader_H__
#define  __AsyncResourceLoader_H__


#include <queue>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include "fwd.hpp"
#include "GraphicsResourceManager.hpp"

#include "../Support/singleton.hpp"


namespace amorphous
{


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

	boost::weak_ptr<GraphicsResourceEntry> m_pResourceEntry;

	/// all the loaders have resource desc?
	boost::shared_ptr<GraphicsResourceLoader> m_pLoader;

	Type m_RequestType;

public:

	CResourceLoadRequest( Type type,
		boost::shared_ptr<GraphicsResourceLoader> pLoader,
		boost::weak_ptr<GraphicsResourceEntry> pEntry
		)
		:
	m_RequestType(type),
	m_pLoader(pLoader),
	m_pResourceEntry(pEntry)
	{}

	Type GetRequestType() const { return m_RequestType; }

//	boost::weak_ptr<GraphicsResourceEntry> GetResourceEntry() { return m_pResourceEntry; }

	friend class AsyncResourceLoader;
	friend class GraphicsResourceManager;
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
		LoadToGraphicsMemoryByRenderThread, ///< not a preferred way of async loading. used by mesh loading
		NumRequestTypes
	};

	Type m_RequestType;

	boost::weak_ptr<GraphicsResourceEntry> m_pResourceEntry;

	/// all the loaders have resource desc?
	boost::shared_ptr<GraphicsResourceLoader> m_pLoader;

public:

	CGraphicsDeviceRequest(
		CGraphicsDeviceRequest::Type type,
		boost::shared_ptr<GraphicsResourceLoader> pLoader,
		boost::weak_ptr<GraphicsResourceEntry> pEntry )
		:
	m_RequestType(type),
	m_pLoader(pLoader),
	m_pResourceEntry(pEntry)
	{}


	friend class AsyncResourceLoader;
};


class AsyncResourceLoader
{
	std::queue<CResourceLoadRequest> m_ResourceLoadRequestQueue;

	boost::mutex m_IOMutex;

	std::queue<CGraphicsDeviceRequest> m_GraphicsDeviceRequestQueue;

	boost::mutex m_GraphicsDeviceMutex;

	boost::shared_ptr<boost::thread> m_pIOThread;

	bool m_bEndIOThread;

private:

	class CIOThreadStarter
	{
		AsyncResourceLoader *m_pAsyncLoader;

	public:

		CIOThreadStarter( AsyncResourceLoader *pAsyncLoader )
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
	static singleton<AsyncResourceLoader> m_obj;

public:

	AsyncResourceLoader();

	~AsyncResourceLoader();

	/// Release the resource IO thread
	void Release();

	static AsyncResourceLoader* Get() { return m_obj.get(); }

	bool AddResourceLoadRequest( const CResourceLoadRequest& req );

	bool AddGraphicsDeviceRequest( const CGraphicsDeviceRequest& req );

	/// Main loop for the resource IO thread
	void IOThreadMain();

	/// Called by the render thread
	void ProcessGraphicsDeviceRequests();


	/// Processes a single request
	/// - Called by the system when the async loading is enabled.
	///   - The user must not call this method.
	/// - The user is responsible for calling this method in the main loop if the async loading is disabled.
	void ProcessResourceLoadRequest();
};


inline AsyncResourceLoader& GetAsyncResourceLoader()
{
	return (*AsyncResourceLoader::Get());
}

} // namespace amorphous



#endif  /* __AsyncResourceLoader_H__ */
