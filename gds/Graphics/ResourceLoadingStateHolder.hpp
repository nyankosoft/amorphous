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

	boost::weak_ptr<CGraphicsResourceEntry> m_pResourceEntry;

public:

	CResourceLoadingState();

	CResourceLoadingState( boost::shared_ptr<CGraphicsResourceEntry> pEntry )
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

	CTextureLoadingStateHolder( boost::shared_ptr<CGraphicsResourceEntry> pTextureResourceEntry )
		:
	CResourceLoadingState(pTextureResourceEntry)
	{}

	bool IsLoaded();
};


class CMeshLoadingStateHolder : public CResourceLoadingState
{
public:

	CMeshLoadingStateHolder( boost::shared_ptr<CGraphicsResourceEntry> pMeshResourceEntry )
		:
	CResourceLoadingState(pMeshResourceEntry)
	{}

	bool IsLoaded();
};


class CShaderLoadingStateHolder : public CResourceLoadingState
{
	boost::weak_ptr<CGraphicsResourceEntry> m_pShaderResourceEntry;

public:

	CShaderLoadingStateHolder( boost::shared_ptr<CGraphicsResourceEntry> pShaderResourceEntry )
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

	void Add( CTextureHandle& texture_handle );
	void Add( CMeshObjectHandle& mesh_handle );
	void Add( CShaderHandle& shader_handle );

	void AddFromResourceEntry( boost::shared_ptr<CGraphicsResourceEntry> pEntry );

	bool AreAllResourceLoaded();
};


} // namespace amorphous



#endif		/*  __ResourceLoadingStateHolder_H__  */
