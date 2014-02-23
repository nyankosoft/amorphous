#ifndef  __GraphicsResourceEntries_HPP__
#define  __GraphicsResourceEntries_HPP__


#include <sys/stat.h>
#include <string>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"


namespace amorphous
{
using namespace serialization;


/**
 - base class of the graphics resources
   - derived classes are,
     - CTextureEntry: texture resource
	 - MeshResource: mesh (3D model) resource
	 - ShaderManagerEntry: shader resource
   - each derived class has derived class instance of resource desc as one of their member variables

*/
class GraphicsResourceEntry
{
public:

	enum State
	{
		STATE_RESERVED,
		STATE_RELEASED,
		NUM_STATES
	};

private:

	State m_State;

	int m_iRefCount;

	boost::shared_ptr<GraphicsResource> m_pResource;

	/// cache derived class pointers to avoid dynamic casting during runtime
	boost::shared_ptr<TextureResource> m_pTextureResource;
	boost::shared_ptr<MeshResource> m_pMeshResource;
	boost::shared_ptr<ShaderResource> m_pShaderResource;

	/// holds a copy of desc
	boost::shared_ptr<GraphicsResourceDesc> m_pDesc;

public:

	inline GraphicsResourceEntry();

	inline void IncRefCount();

	inline void DecRefCount();

	int GetRefCount() const { return m_iRefCount; }

	State GetState() const { return m_State; }

	boost::shared_ptr<GraphicsResource> GetResource() { return m_pResource; }

	inline void SetResource( boost::shared_ptr<GraphicsResource> pResource );

	boost::shared_ptr<TextureResource> GetTextureResource() { return m_pTextureResource; }
	boost::shared_ptr<MeshResource> GetMeshResource() { return m_pMeshResource; }
	boost::shared_ptr<ShaderResource> GetShaderResource() { return m_pShaderResource; }

	const boost::shared_ptr<TextureResource> GetTextureResource() const { return m_pTextureResource; }
	const boost::shared_ptr<MeshResource> GetMeshResource() const { return m_pMeshResource; }
	const boost::shared_ptr<ShaderResource> GetShaderResource() const { return m_pShaderResource; }

	void GetStatus( std::string& dest_buffer );

	friend class GraphicsResourceManager;

//	boost::shared_ptr<T> GetResource() { return m_pResource; }
//	void SetResource( boost::shared_ptr<T> pResource ) { m_pResource = pResource; }

};


//---------------------------- inline implementations ---------------------------- 

//================================================================================
// GraphicsResourceEntry
//================================================================================

inline GraphicsResourceEntry::GraphicsResourceEntry()
:
m_State(STATE_RELEASED),
m_iRefCount(0)
{
}


/**
 Load the resource when the reference count is incremented from 0 to 1
*/
inline void GraphicsResourceEntry::IncRefCount()
{
	m_iRefCount++;

/*	if( m_iRefCount == 0 )
	{
		bool res = Load();
		if( res )
			m_iRefCount = 1;
	}
	else
	{
		// resource has already been loaded - just increment the reference count
		m_iRefCount++;
	}*/
}


/**
 Release the resource when the reference count is decremented from 1 to 0
*/
inline void GraphicsResourceEntry::DecRefCount()
{
	if( m_iRefCount == 0 )
	{
		std::string resource_path_info;
		if( GetResource() )
			resource_path_info = "(resource name: " + GetResource()->GetDesc().ResourcePath + ")";
		
		LOG_PRINT_WARNING( " - A redundant call: ref count is already 0 " + resource_path_info );
		return;	// error
	}

	m_iRefCount--;

	if( m_iRefCount == 0 )
	{
		if( GetResource() ) // Do not release cached resources
			GetResource()->ReleaseNonChachedResource();

		m_State = STATE_RELEASED;
	}
}


inline void GraphicsResourceEntry::SetResource( boost::shared_ptr<GraphicsResource> pResource )
{
	m_pResource = pResource;

	m_pTextureResource = boost::dynamic_pointer_cast<TextureResource,GraphicsResource>(pResource);
	m_pMeshResource    = boost::dynamic_pointer_cast<MeshResource,GraphicsResource>(pResource);
	m_pShaderResource  = boost::dynamic_pointer_cast<ShaderResource,GraphicsResource>(pResource);
}


} // namespace amorphous



#endif  /* __GraphicsResourceEntries_HPP__ */
