#ifndef  __GraphicsResourceEntries_HPP__
#define  __GraphicsResourceEntries_HPP__


#include <sys/stat.h>
#include <string>
#include <d3dx9tex.h>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include "fwd.hpp"
#include "GraphicsResources.hpp"
#include "GraphicsResourceDescs.hpp"

#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

//template<class T>class CBinaryDatabase<T>;


/**
 - base class of the graphics resources
   - derived classes are,
     - CTextureEntry: texture resource
	 - CMeshResource: mesh (3D model) resource
	 - CShaderManagerEntry: shader resource
   - each derived class has derived class instance of resource desc as one of their member variables

*/
class CGraphicsResourceEntry
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

	boost::shared_ptr<CGraphicsResource> m_pResource;

	/// cache derived class pointers to avoid dynamic casting during runtime
	boost::shared_ptr<CTextureResource> m_pTextureResource;
	boost::shared_ptr<CMeshResource> m_pMeshResource;
	boost::shared_ptr<CShaderResource> m_pShaderResource;

	/// holds a copy of desc
	boost::shared_ptr<CGraphicsResourceDesc> m_pDesc;

public:

	inline CGraphicsResourceEntry();

	inline void IncRefCount();

	inline void DecRefCount();

	int GetRefCount() const { return m_iRefCount; }

	State GetState() const { return m_State; }

	boost::shared_ptr<CGraphicsResource> GetResource() { return m_pResource; }

	inline void SetResource( boost::shared_ptr<CGraphicsResource> pResource );

	boost::shared_ptr<CTextureResource> GetTextureResource() { return m_pTextureResource; }
	boost::shared_ptr<CMeshResource> GetMeshResource() { return m_pMeshResource; }
	boost::shared_ptr<CShaderResource> GetShaderResource() { return m_pShaderResource; }

	const boost::shared_ptr<CTextureResource> GetTextureResource() const { return m_pTextureResource; }
	const boost::shared_ptr<CMeshResource> GetMeshResource() const { return m_pMeshResource; }
	const boost::shared_ptr<CShaderResource> GetShaderResource() const { return m_pShaderResource; }

	void GetStatus( char *pDestBuffer );

	friend class CGraphicsResourceManager;

//	boost::shared_ptr<T> GetResource() { return m_pResource; }
//	void SetResource( boost::shared_ptr<T> pResource ) { m_pResource = pResource; }

};


//---------------------------- inline implementations ---------------------------- 

//================================================================================
// CGraphicsResourceEntry
//================================================================================

inline CGraphicsResourceEntry::CGraphicsResourceEntry()
:
m_State(STATE_RELEASED),
m_iRefCount(0)
{
}


/**
 Load the resource when the reference count is incremented from 0 to 1
*/
inline void CGraphicsResourceEntry::IncRefCount()
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
inline void CGraphicsResourceEntry::DecRefCount()
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


inline void CGraphicsResourceEntry::SetResource( boost::shared_ptr<CGraphicsResource> pResource )
{
	m_pResource = pResource;

	m_pTextureResource = boost::dynamic_pointer_cast<CTextureResource,CGraphicsResource>(pResource);
	m_pMeshResource    = boost::dynamic_pointer_cast<CMeshResource,CGraphicsResource>(pResource);
	m_pShaderResource  = boost::dynamic_pointer_cast<CShaderResource,CGraphicsResource>(pResource);
}



#endif  /* __GraphicsResourceEntries_HPP__ */
