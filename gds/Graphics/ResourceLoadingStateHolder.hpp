#ifndef __ResourceLoadingStateHolder_H__
#define __ResourceLoadingStateHolder_H__


#include "fwd.hpp"
#include "Graphics/fwd.hpp"
#include "../base.hpp"
#include <list>
#include <boost/weak_ptr.hpp>


namespace amorphous
{


// draft
class ResourceLoadingStateSet
{
public:
	enum Name
	{
		NOT_READY,
		ALL_LOADED,
		NO_RESOURCE_LOADING_STATE_HOLDER,
		NUM_STATE_SETS
	};
};


class ResourceLoadingState
{
//	U32 m_ID;

protected:

	boost::weak_ptr<GraphicsResourceEntry> m_pResourceEntry;

public:

	ResourceLoadingState();

	ResourceLoadingState( boost::shared_ptr<GraphicsResourceEntry> pEntry )
		:
	m_pResourceEntry( pEntry )
	{}

	virtual ~ResourceLoadingState() {}

	virtual bool IsLoaded();

	bool IsReleased();

};


class TextureLoadingStateHolder : public ResourceLoadingState
{
public:

	TextureLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pTextureResourceEntry )
		:
	ResourceLoadingState(pTextureResourceEntry)
	{}

	bool IsLoaded();
};


class MeshLoadingStateHolder : public ResourceLoadingState
{
public:

	MeshLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pMeshResourceEntry )
		:
	ResourceLoadingState(pMeshResourceEntry)
	{}

	bool IsLoaded();
};


class ShaderLoadingStateHolder : public ResourceLoadingState
{
	boost::weak_ptr<GraphicsResourceEntry> m_pShaderResourceEntry;

public:

	ShaderLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pShaderResourceEntry )
		:
	ResourceLoadingState(pShaderResourceEntry)
	{}

	bool IsLoaded();
};


class ResourceLoadingStateHolder
{
	std::list< boost::shared_ptr<ResourceLoadingState> > m_lstpResourceLoadingState;

public:

	/// \param pLoadingState owned reference of a loading state object
	void Add( ResourceLoadingState *pLoadingState );

	void Add( TextureHandle& texture_handle );
	void Add( MeshHandle& mesh_handle );
	void Add( ShaderHandle& shader_handle );

	void AddFromResourceEntry( boost::shared_ptr<GraphicsResourceEntry> pEntry );

	bool AreAllResourceLoaded();
};


} // namespace amorphous



#endif		/*  __ResourceLoadingStateHolder_H__  */
