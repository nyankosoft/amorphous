#include "D3DCGraphicsResourceLoaders.hpp"
#include "../AsyncResourceLoader.hpp"
#include "Mesh/D3DXMeshObjectBase.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"


namespace amorphous
{

using std::vector;
using namespace boost;

/*
CD3DMeshLoader::CD3DMeshLoader()
:
m_pVertexBufferContent(NULL)
{
}
*/

CD3DMeshLoader::~CD3DMeshLoader()
{
	SafeDelete( m_pVertexBufferContent );
}


void CD3DMeshLoader::LoadMeshSubresources()
{
	if( m_pArchive )
	{
		m_MeshDesc.NumVertices = m_pArchive->GetVertexSet().GetNumVertices();
		m_MeshDesc.NumIndices  = (int)m_pArchive->GetNumVertexIndices();

		m_MeshDesc.VertexFormatFlags = m_pArchive->GetVertexSet().m_VertexFormatFlag;

		vector<D3DVERTEXELEMENT9> vertex_elements;

		LoadVerticesForD3DXMesh(
			m_pArchive->GetVertexSet(),
			vertex_elements, // m_MeshDesc.vecVertElement,
			m_MeshDesc.VertexSize,
			m_pVertexBufferContent
			);

		m_MeshDesc.VertexFormatFlags = ToVFF( m_pArchive->GetVertexSet().m_VertexFormatFlag );

//		SafeDelete( pVertexBufferContent );

		LoadIndices( *(m_pArchive.get()), m_vecIndexBufferContent );

		GetAttributeTableFromTriangleSet( m_pArchive->GetTriangleSet(), m_vecAttributeRange );
	}
}


/// Called by I/O thread after the mesh archive is loaded and stored to 'm_pArchive'
/// - Usually loaded from disk
void CD3DMeshLoader::OnLoadingCompleted( std::shared_ptr<GraphicsResourceLoader> pSelf )
{
	// change this to true if async loading is fixed
	bool preferred_async_loading_method = false;

	if( !preferred_async_loading_method )
	{
		LOG_PRINT( "Sending a LoadToGraphicsMemoryByRenderThread request for a mesh: " + m_MeshDesc.ResourcePath );
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::LoadToGraphicsMemoryByRenderThread, pSelf, GetResourceEntry() );
		GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );
		return;
	}

	// - send a lock request
	//   - Actually does not lock. Send the lock request to call AcquireResource.
	//   - Must be processed before the lock requrests of the subresources below.
	CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, pSelf, GetResourceEntry() );
	GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );

	// create subresource loaders
	shared_ptr<CD3DXMeshLoaderBase> apLoader[3];
	apLoader[0].reset( new CD3DXMeshVerticesLoader(GetResourceEntry()) );
	apLoader[1].reset( new CD3DXMeshIndicesLoader(GetResourceEntry()) );
	apLoader[2].reset( new CD3DXMeshAttributeTableLoader(GetResourceEntry()) );

	for( int i=0; i<3; i++ )
	{
		// set the pointer to mesh archive to each subresource loader
		apLoader[i]->m_pArchive   = m_pArchive;
//		apLoader[i]->m_pMeshEntry = m_pMeshEntry;

		apLoader[i]->m_pMeshLoader = boost::dynamic_pointer_cast<CD3DMeshLoader,MeshLoader>( m_pSelf.lock() );

		// add requests to load subresource from the mesh archive
//		ResourceLoadRequest req( ResourceLoadRequest::LoadFromDisk, apLoader[i], GetResourceEntry() );
//		GetAsyncResourceLoader().AddResourceLoadRequest( req );

		// subresources have been loaded
		// - send lock requests for each mesh sub resource
		CGraphicsDeviceRequest req( CGraphicsDeviceRequest::Lock, apLoader[i], GetResourceEntry() );
		GetAsyncResourceLoader().AddGraphicsDeviceRequest( req );
	}

	// create mesh instance
	// load resources that needs to be loaded synchronously
//	GetResourceEntry()->GetMeshResource()->CreateMeshAndLoadNonAsyncResources( *(m_pArchive.get()) );
}


void CD3DMeshLoader::OnResourceLoadedOnGraphicsMemory()
{
	// Do nothing
	// - Avoid setting the resource state to GraphicsResourceState::LOADED
	// - Need to wait until all the subresources are locked, copied, and unlocked.
}



//===================================================================================
// CD3DXMeshLoaderBase
//===================================================================================

CD3DXMeshObjectBase *CD3DXMeshLoaderBase::GetD3DMeshImpl()
{
	return dynamic_cast<CD3DXMeshObjectBase *>( GetMesh() );
}


//===================================================================================
// CD3DXMeshVerticesLoader
//===================================================================================

Result::Name CD3DXMeshLoaderBase::Load()
{
	return ( LoadFromArchive() ? Result::SUCCESS : Result::UNKNOWN_ERROR );
}



//===================================================================================
// CD3DXMeshVerticesLoader
//===================================================================================

bool CD3DXMeshVerticesLoader::LoadFromArchive()
{
/*	// load the vertices from the mesh archive (m_pArchive), and store it
	// to the buffer (m_pVertexBufferContent).
	GetMesh()->LoadVertices( m_pVertexBufferContent, *(m_pArchive.get()) );

//	LoadVerticesForD3DXMesh( m_pArchive->GetVertexSet(), elems, size, dest_buffer );

	m_pMeshLoader->RaiseStateFlags( MeshLoader::VERTICES_LOADED );
	m_pMeshLoader->SendLockRequestIfAllSubresourcesHaveBeenLoaded();
*/
	return true;
}


bool CD3DXMeshVerticesLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( m_pLockedVertexBuffer )
	{
//		memcpy( m_pLockedVertexBuffer, m_pVertexBufferContent, GetMesh()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );

		memcpy( m_pLockedVertexBuffer,
			m_pMeshLoader->VertexBufferContent(),
			GetD3DMeshImpl()->GetVertexSize() * m_pArchive->GetVertexSet().GetNumVertices() );
	}

	return true;
}


bool CD3DXMeshVerticesLoader::Lock()
{
	LOG_PRINT( "" );

	CD3DXMeshObjectBase *pMesh = GetD3DMeshImpl();
	if( pMesh )
		return pMesh->LockVertexBuffer( m_pLockedVertexBuffer );
	else
		return false;
}


bool CD3DXMeshVerticesLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockVertexBuffer();
	m_pLockedVertexBuffer = NULL;
	return unlocked;
}


void CD3DXMeshVerticesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( MeshSubResource::VERTEX, GraphicsResourceState::LOADED );
}


bool CD3DXMeshVerticesLoader::IsReadyToLock() const
{
	return true;
}



//===================================================================================
// CD3DXMeshIndicesLoader
//===================================================================================

bool CD3DXMeshIndicesLoader::LoadFromArchive()
{
/*	unsigned short *pIBData;
	GetMesh()->LoadIndices( pIBData, *(m_pArchive.get()) );
	m_pIndexBufferContent = (void *)pIBData;

	m_pMeshLoader->RaiseStateFlags( MeshLoader::INDICES_LOADED );
	m_pMeshLoader->SendLockRequestIfAllSubresourcesHaveBeenLoaded();
*/
	return true;
}


bool CD3DXMeshIndicesLoader::Lock()
{
	LOG_PRINT( "" );

	CD3DXMeshObjectBase *pMesh = GetD3DMeshImpl();
	if( pMesh )
		return pMesh->LockIndexBuffer( m_pLockedIndexBuffer );
	else
		return false;
}


bool CD3DXMeshIndicesLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockIndexBuffer();
	m_pLockedIndexBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshIndicesLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( m_pLockedIndexBuffer )
	{
//		memcpy( m_pLockedIndexBuffer, m_pIndexBufferContent, m_IndexBufferSize );

		memcpy( m_pLockedIndexBuffer,
			&(m_pMeshLoader->IndexBufferContent()[0]),
			m_pMeshLoader->IndexBufferContent().size() );
	}


	return true;
}


void CD3DXMeshIndicesLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( MeshSubResource::INDEX, GraphicsResourceState::LOADED );
}


bool CD3DXMeshIndicesLoader::IsReadyToLock() const
{
	return GetSubResourceState( MeshSubResource::VERTEX ) == GraphicsResourceState::LOADED;
}



//===================================================================================
// CD3DXMeshAttributeTableLoader
//===================================================================================

bool CD3DXMeshAttributeTableLoader::Lock()
{
	LOG_PRINT( "" );

	shared_ptr<CD3DXMeshObjectBase> pMesh
		= boost::dynamic_pointer_cast<CD3DXMeshObjectBase,MeshImpl>( GetMesh()->m_pImpl );

	if( !pMesh )
		return false;

	LPD3DXMESH pD3DXMesh = pMesh->GetMesh();

	if( !pD3DXMesh )
		return false;

	HRESULT hr = pD3DXMesh->SetAttributeTable(
		&(m_pMeshLoader->AttributeTable()[0]),
		(DWORD)(m_pMeshLoader->AttributeTable().size()) );

	bool locked = GetD3DMeshImpl()->LockAttributeBuffer( m_pLockedAttributeBuffer );
	return locked;
}


bool CD3DXMeshAttributeTableLoader::Unlock()
{
	LOG_PRINT( "" );

	bool unlocked = GetD3DMeshImpl()->UnlockAttributeBuffer();
	m_pLockedAttributeBuffer = NULL;
	return unlocked;
}


bool CD3DXMeshAttributeTableLoader::CopyLoadedContentToGraphicsResource()
{
	LOG_PRINT( "" );

	if( !m_pLockedAttributeBuffer )
		return false;

	const vector<CMMA_TriangleSet>& vecTriangleSet = m_pArchive->GetTriangleSet();

	DWORD *pdwBuffer = m_pLockedAttributeBuffer;
	DWORD face = 0;
	const int num_materials = GetMesh()->GetNumMaterials();
	for( int i=0; i<num_materials; i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i];

		DWORD face_start = triangle_set.m_iStartIndex / 3;
		DWORD num_faces = triangle_set.m_iNumTriangles;
		for( face=face_start; face<face_start + num_faces; face++ )
		{
			pdwBuffer[face] = i;
		}
	}

	return true;
}


void CD3DXMeshAttributeTableLoader::OnResourceLoadedOnGraphicsMemory()
{
	SetSubResourceState( MeshSubResource::ATTRIBUTE_TABLE, GraphicsResourceState::LOADED );
}


bool CD3DXMeshAttributeTableLoader::IsReadyToLock() const
{
	return GetSubResourceState( MeshSubResource::INDEX ) == GraphicsResourceState::LOADED;
}


} // namespace amorphous
