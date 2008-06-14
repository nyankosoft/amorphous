#include "GraphicsResourceManager.h"
//#include "GraphicsResourceHandle.h"
//#include "TextureHandle.h"

#include "3DCommon/Direct3D9.h"

#include "Support/Log/DefaultLog.h"
#include "Support/SafeDelete.h"
#include "Support/SafeDeleteVector.h"

#include "Support/Serialization/BinaryDatabase.h"

using namespace std;


//==================================================================================================
// CGraphicsResourceManager
//==================================================================================================

// define the singleton instance
///CGraphicsResourceManager CGraphicsResourceManager::ms_SingletonInstance_;
CSingleton<CGraphicsResourceManager> CGraphicsResourceManager::m_obj;


CGraphicsResourceManager::CGraphicsResourceManager()
{
	m_AsyncLoadingAllowed = false;
}


CGraphicsResourceManager::~CGraphicsResourceManager()
{
	Release();
}


//void CGraphicsResourceManager::Init()
//{}


void CGraphicsResourceManager::Release()
{
	ReleaseGraphicsResources();

	SafeDeleteVector( m_vecpResourceEntry );
}


CGraphicsResourceEntry *CGraphicsResourceManager::CreateGraphicsResourceEntry( const CGraphicsResourceDesc& desc )
{
	switch( desc.ResourceType )
	{
	case CGraphicsResourceDesc::RT_TEXTURE:
		return new CTextureEntry();
	case CGraphicsResourceDesc::RT_MESHOBJECT:
		return new CMeshObjectEntry(desc.MeshType);
	case CGraphicsResourceDesc::RT_SHADERMANAGER:
		return new CShaderManagerEntry();
	default:
		LOG_PRINT_WARNING( " - invalid resource type" );
		return NULL;
	}
}


int CGraphicsResourceManager::LoadGraphicsResource( const CGraphicsResourceDesc& desc )
{
	LOG_FUNCTION_SCOPE();

	if( desc.Filename.length() == 0 )
		return -1;	// invalid filename

	size_t i, num_resources = m_vecpResourceEntry.size();
	for( i=0; i<num_resources; i++ )
	{
		// check if the requested can be shared with one already registered to the graphics resource manager
		if( m_vecpResourceEntry[i]->CanBeSharedAsSameResource( desc ) )
		{
			// requested resource was found in the list
			// - no need to add a new resource.
			// - resource gets loaded if reference count is incremented from 0 to 1
			m_vecpResourceEntry[i]->IncRefCount();
			return (int)i;
		}
	}

	// not found in the list - need to load as a new texture
	m_vecpResourceEntry.push_back( CreateGraphicsResourceEntry(desc) );
	m_vecpResourceEntry.back()->SetFilename( desc.Filename );

	m_vecpResourceEntry.back()->IncRefCount();	// increment the reference count - this will load the resource

	if( m_vecpResourceEntry.back()->GetRefCount() == 1 )
	{
		LOG_PRINT( " - Created a graphics resource: " + desc.Filename );

		// new texture has been successfully loaded
		return (int)i;
	}
	else
	{
		LOG_PRINT_WARNING( "Failed to create a graphics resource: " + desc.Filename );

		SafeDelete( m_vecpResourceEntry.back() );
		m_vecpResourceEntry.pop_back();
		return -1;	// texture file was not found
	}
}


/// called from handle
int CGraphicsResourceManager::LoadTexture( std::string filename )
{
	CGraphicsResourceDesc desc = CGraphicsResourceDesc(CGraphicsResourceDesc::RT_TEXTURE);
	desc.Filename = filename;
	return LoadGraphicsResource( desc );
}


/// called from handle
int CGraphicsResourceManager::LoadMeshObject( std::string filename, int mesh_type )
{
	CGraphicsResourceDesc desc = CGraphicsResourceDesc(CGraphicsResourceDesc::RT_MESHOBJECT);
	desc.Filename = filename;
	desc.MeshType = mesh_type;
	return LoadGraphicsResource( desc );
}


/// called from handle
int CGraphicsResourceManager::LoadShaderManager( std::string filename )
{
	CGraphicsResourceDesc desc = CGraphicsResourceDesc(CGraphicsResourceDesc::RT_SHADERMANAGER);
	desc.Filename = filename;
	return LoadGraphicsResource( desc );
}


void CGraphicsResourceManager::IncResourceRefCount( int entry_id )
{
	if( entry_id < 0 || (int)m_vecpResourceEntry.size() <= entry_id )
		return;	// invalid ID

	m_vecpResourceEntry[entry_id]->IncRefCount();
}


void CGraphicsResourceManager::DecResourceRefCount( int entry_id )
{
	if( entry_id < 0 || (int)m_vecpResourceEntry.size() <= entry_id )
		return;	// invalid ID

	m_vecpResourceEntry[entry_id]->DecRefCount();
}


void CGraphicsResourceManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		m_vecpResourceEntry[i]->Load();
	}
}


void CGraphicsResourceManager::ReleaseGraphicsResources()
{
//	g_Log.Print( "CGraphicsResourceManager::ReleaseGraphicsResources()" );

	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		m_vecpResourceEntry[i]->Release();
	}
}


void CGraphicsResourceManager::Refresh()
{
	size_t i, num_entries = m_vecpResourceEntry.size();
	for( i=0; i<num_entries; i++ )
	{
		m_vecpResourceEntry[i]->Refresh();
	}
}
