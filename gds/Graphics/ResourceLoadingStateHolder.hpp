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
class CResourceLoadingStateSet
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


class CResourceLoadingState
{
//	U32 m_ID;

protected:

	boost::weak_ptr<GraphicsResourceEntry> m_pResourceEntry;

public:

	CResourceLoadingState();

	CResourceLoadingState( boost::shared_ptr<GraphicsResourceEntry> pEntry )
		:
	m_pResourceEntry( pEntry )
	{}

	virtual ~CResourceLoadingState() {}

	virtual bool IsLoaded();

	bool IsReleased();

};


class CTextureLoadingStateHolder : public CResourceLoadingState
{
public:

	CTextureLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pTextureResourceEntry )
		:
	CResourceLoadingState(pTextureResourceEntry)
	{}

	bool IsLoaded();
};


class CMeshLoadingStateHolder : public CResourceLoadingState
{
public:

	CMeshLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pMeshResourceEntry )
		:
	CResourceLoadingState(pMeshResourceEntry)
	{}

	bool IsLoaded();
};


class CShaderLoadingStateHolder : public CResourceLoadingState
{
	boost::weak_ptr<GraphicsResourceEntry> m_pShaderResourceEntry;

public:

	CShaderLoadingStateHolder( boost::shared_ptr<GraphicsResourceEntry> pShaderResourceEntry )
		:
	CResourceLoadingState(pShaderResourceEntry)
	{}

	bool IsLoaded();
};


class CResourceLoadingStateHolder
{
	std::list< boost::shared_ptr<CResourceLoadingState> > m_lstpResourceLoadingState;

public:

	/// \param pLoadingState owned reference of a loading state object
	void Add( CResourceLoadingState *pLoadingState );

	void Add( TextureHandle& texture_handle );
	void Add( MeshHandle& mesh_handle );
	void Add( ShaderHandle& shader_handle );

	void AddFromResourceEntry( boost::shared_ptr<GraphicsResourceEntry> pEntry );

	bool AreAllResourceLoaded();
};


} // namespace amorphous



#endif		/*  __ResourceLoadingStateHolder_H__  */
