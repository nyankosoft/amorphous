#include "GraphicsResources.hpp"

#include "Graphics/Mesh/BasicMesh.hpp"
#include "Graphics/Mesh/CustomMesh.hpp" // For GetCustomMesh()
#include "Graphics/Mesh/MeshFactory.hpp"
#include "Graphics/MeshGenerators.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/ShaderGenerator.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/lfs.hpp"
#include "Support/ImageArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"
using namespace GameLib1::Serialization;

using std::string;
using namespace boost;


//==================================================================================================
// CGraphicsResource
//==================================================================================================

CGraphicsResource::CGraphicsResource()
:
m_OptionFlags(0),
m_LastModifiedTimeOfFile(0),
m_State(GraphicsResourceState::RELEASED),
m_IsCachedResource(false),
m_Index(0)
{}


CGraphicsResource::~CGraphicsResource()
{
//	Release();	// LNK2019 - why???
}


/*
void CGraphicsResource::Release()
{}
*/


/**
  Release and reload resource if the file has been modified since the last time is was load
   - This method does not change the reference count
 */
void CGraphicsResource::Refresh()
{
	const string& resource_path = GetDesc().ResourcePath;
	string filepath, keyname;

	if( is_db_filepath_and_keyname( resource_path ) )
		decompose_into_db_filepath_and_keyname( resource_path, filepath, keyname );
	else
		filepath = resource_path;

	const time_t current_last_mod_time = lfs::get_last_modified_time( filepath );
	if( m_LastModifiedTimeOfFile < current_last_mod_time )
	{
		LOG_PRINT( " Refreshing " + filepath + "..." );
		Release();
		Load();
	}
}


/// Starting point of the synchronous loading.
bool CGraphicsResource::Load()
{
	if( IsDiskResource() )
	{
		return LoadFromDisk();
	}
	else
	{
		// create resource from non-disk resource
		// - e.g.) empty texture that gets filled by user-defined a routine
		// - Simply create an empty texture if no loader is set to desc. This happens
		//   when an unused cached texture needs to be loaded after being released
		//   in order to reconfigure graphics devices for resolution changes or something.
		return CreateFromDesc();
	}
}


void CGraphicsResource::ReleaseNonChachedResource()
{
	if( !m_IsCachedResource )
		Release();
}


void CGraphicsResource::ReleaseCachedResource()
{
	if( m_IsCachedResource )
		Release();
}


bool CGraphicsResource::LoadFromDisk()
{
	bool loaded = false;
	string target_filepath;
	const string resource_path = GetDesc().ResourcePath;

	if( is_db_filepath_and_keyname( resource_path ) )
	{
		// decompose the string
		string db_filename, keyname;
		decompose_into_db_filepath_and_keyname( resource_path, db_filename, keyname );

		string cwd = lfs::get_cwd();

		CBinaryDatabase<string> db;
		bool db_open = db.Open( db_filename );
		if( !db_open )
			return false; // the database is being used by someone else - retry later

		loaded = LoadFromDB( db, keyname );

		target_filepath = db_filename;
	}
	else
	{
		loaded = LoadFromFile( resource_path );

		target_filepath = resource_path;
	}


	if( loaded )
	{
		SetState( GraphicsResourceState::LOADED );

		// record the time of last modification of the texture file
		m_LastModifiedTimeOfFile = lfs::get_last_modified_time(target_filepath);
	}

	return loaded;
}


bool CGraphicsResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	if( !GetDesc().Sharable || !desc.Sharable )
		return false;

	if( GetResourceType() != desc.GetResourceType() )
		return false;

	if( 0 < GetDesc().ResourcePath.length()
	 && 0 < desc.ResourcePath.length()
	 && GetDesc().ResourcePath == desc.ResourcePath )
	{
		// Treat as sharable only if both of the resources have resource paths
		// and they are the same ones.
		return true;
	}
	else
		return false;
}


void CGraphicsResource::GetStatus( char *pDestBuffer )
{
	const CGraphicsResourceDesc& desc = GetDesc();

	sprintf( pDestBuffer, " / %s",	desc.ResourcePath.c_str() );
}



//==================================================================================================
// CTextureResource
//==================================================================================================

CTextureResource::CTextureResource( const CTextureResourceDesc *pDesc )
{
	if( pDesc )
		m_TextureDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


CTextureResource::~CTextureResource()
{
	Release();
}


bool CTextureResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	return CGraphicsResource::CanBeSharedAsSameResource( desc );
}


int CTextureResource::CanBeUsedAsCache( const CGraphicsResourceDesc& desc )
{
	if( GetState() != GraphicsResourceState::RELEASED )
		return 0;

	return desc.CanBeUsedAsTextureCache( m_TextureDesc );
}


void CTextureResource::UpdateDescForCachedResource( const CGraphicsResourceDesc& desc )
{
	desc.UpdateCachedTextureResourceDesc( m_TextureDesc );
}


bool CTextureResource::GetLockedTexture( shared_ptr<CLockedTexture>& pLockedTexture )
{
	// TODO: increment the ref count when the async loading process is started
//	m_iRefCount++;

	if( !m_pLockedTexture )
		return false;

	pLockedTexture = m_pLockedTexture;
	return true;
}


void CTextureResource::Release()
{
	SetState( GraphicsResourceState::RELEASED );
}


bool CTextureResource::IsDiskResource() const
{
	if( m_TextureDesc.pLoader
	 || m_TextureDesc.ResourcePath.length() == 0
	 || m_TextureDesc.UsageFlags & UsageFlag::RENDER_TARGET )
	{
		// Treat as non-disk resource if any one of the following is true
		// - Has a texture loader, or an algorithm to fill the texture
		// - Has no resource path
		// - Sepcified as a render target texture
		return false;
	}
	else
		return true;

//	if( 0 < m_TextureDesc.Width
//	 && 0 < m_TextureDesc.Height
//	 && m_TextureDesc.Format != TextureFormat::Invalid )
/*	if( m_TextureDesc.pLoader )
	{
		return false;
	}
	else
		return true;*/
}


void CTextureResource::GetStatus( char *pDestBuffer )
{
	CGraphicsResource::GetStatus( pDestBuffer );

	char buffer[256];
	const CTextureResourceDesc& desc = m_TextureDesc;

	sprintf( buffer, " / %d x %d", desc.Width, desc.Height );
	strcat( pDestBuffer, buffer );
}



//==================================================================================================
// CMeshResource
//==================================================================================================

shared_ptr<CCustomMesh> GetCustomMesh( CBasicMesh& src_mesh )
{
	shared_ptr<CMeshImpl> pMeshImpl = src_mesh.m_pImpl;
	if( !pMeshImpl )
		return shared_ptr<CCustomMesh>();

	shared_ptr<CCustomMesh> pCustomMesh = boost::dynamic_pointer_cast<CCustomMesh,CMeshImpl>( pMeshImpl );

	return pCustomMesh;
}


shared_ptr<CCustomMesh> GetCustomMesh( shared_ptr<CBasicMesh> pSrcMesh )
{
	if( !pSrcMesh )
		return shared_ptr<CCustomMesh>();

	return GetCustomMesh( *pSrcMesh );
}

/*
// Need to include MeshObjectHandle.hpp to define this
// Should move this to MeshObjectHandle.cpp?
shared_ptr<CCustomMesh> GetCustomMesh( CMeshObjectHandle& src_mesh )
{
	return GetCustomMesh( src_mesh.GetMesh() );
}*/


CMeshResource::CMeshResource( const CMeshResourceDesc *pDesc )
{
	if( pDesc )
		m_MeshDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An incompatible resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();

	for( int i=0; i<numof(m_aSubResourceState); i++ )
		m_aSubResourceState[i] = GraphicsResourceState::RELEASED;
}


CMeshResource::~CMeshResource()
{
	Release();
}


bool CMeshResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	m_pMeshObject.reset();

	string mesh_archive_key = keyname;

	// retrieve mesh archive from db
	C3DMeshModelArchive mesh_archive;
	db.GetData( mesh_archive_key, mesh_archive );

	CMeshFactory factory;
	CBasicMesh *pMesh = factory.LoadMeshObjectFromArchive( mesh_archive, keyname, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );
	m_pMeshObject = boost::shared_ptr<CBasicMesh>( pMesh );

	return ( m_pMeshObject ? true : false );
}


bool CMeshResource::LoadFromFile( const std::string& filepath )
{
	m_pMeshObject.reset();

	CMeshFactory factory;
	CBasicMesh *pMeshObject
		= factory.LoadMeshObjectFromFile( filepath, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<CBasicMesh>( pMeshObject );
		return true;
	}
	else
		return false;
}


void CMeshResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	m_pMeshObject.reset();

	SetState( GraphicsResourceState::RELEASED );
}


bool CMeshResource::IsDiskResource() const
{
	if( m_MeshDesc.pMeshGenerator )
	{
		return false;
	}
	else
		return true;
}


bool CMeshResource::CreateFromDesc()
{
	if( !m_MeshDesc.pMeshGenerator )
		return false;

	Result::Name res = m_MeshDesc.pMeshGenerator->Generate();
	if( res != Result::SUCCESS )
		return false;

	CMeshFactory factory;
	CBasicMesh *pMeshObject
		= factory.LoadMeshObjectFromArchive( m_MeshDesc.pMeshGenerator->MeshArchive(),
		                                     m_MeshDesc.ResourcePath,
											 m_MeshDesc.LoadOptionFlags,
											 m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<CBasicMesh>( pMeshObject );
		SetState( GraphicsResourceState::LOADED );
		return true;
	}
	else
		return false;
}


bool CMeshResource::CanBeSharedAsSameResource( const CGraphicsResourceDesc& desc )
{
	return desc.CanBeSharedAsSameMeshResource( m_MeshDesc );
}


int CMeshResource::CanBeUsedAsCache( const CGraphicsResourceDesc& desc )
{
	if( GetState() != GraphicsResourceState::RELEASED )
		return 0;

	return desc.CanBeUsedAsMeshCache( m_MeshDesc );
}

/*
struct IDAndString
{
	int id;
	const char* text;
};


#define ID_AND_STRING(id) { id, #id }


IDAndString g_MeshTypes[] =
{
	ID_AND_STRING(CMeshType::BASIC),
	ID_AND_STRING(CMeshType::PROGRESSIVE),
	ID_AND_STRING(CMeshType::SKELETAL)
};
*/


bool CMeshResource::Create()
{
/*	LPD3DXMESH pMesh;

	HRESULT hr;
	hr = D3DXCreateMesh(
			m_MeshDesc.NumIndices / 3,       // DWORD NumFaces,
			m_MeshDesc.NumVertices,          // DWORD NumVertices,
			0,                               // DWORD Options,
			&(m_MeshDesc.vecVertElement[0]), // CONST LPD3DVERTEXELEMENT9 * pDeclaration,
			DIRECT3D9.GetDevice(),           // LPDIRECT3DDEVICE9 pD3DDevice,
			&pMesh
		);
*/
	CMeshFactory factory;
	m_pMeshObject = factory.CreateMesh( m_MeshDesc.MeshType );

	if( !m_pMeshObject )
		return false;

	if( m_MeshDesc.NumVertices == 0 || m_MeshDesc.NumIndices == 0 )
	{
		// Assume that the user requested to make an empty mesh
		// Also, don't set the state as loaded, since it may be loaded later
		// from the archive.
		return true;
	}

	bool mesh_created = m_pMeshObject->CreateMesh(
		m_MeshDesc.NumVertices,
		m_MeshDesc.NumIndices,
		0,
		m_MeshDesc.vecVertElement
		);

	if( mesh_created )
	{
		SetState( GraphicsResourceState::LOADED );
		return true;
	}
	else
		return false;
}


void CMeshResource::SetSubResourceState( CMeshSubResource::Name subresource,
										 GraphicsResourceState::Name state )
{
	m_aSubResourceState[subresource] = state;

	bool loaded_so_far = true;
	for( int i=0; i<numof(m_aSubResourceState); i++ )
	{
		if( m_aSubResourceState[i] != GraphicsResourceState::LOADED )
		{
			loaded_so_far = false;
			break;
		}
	}

	// mark the resource loaded only if all the subresource are loaded
	if( loaded_so_far )
		SetState( GraphicsResourceState::LOADED );
}


void CMeshResource::CreateMeshAndLoadNonAsyncResources( C3DMeshModelArchive& archive )
{
	lfs::dir_stack dirstk( lfs::get_parent_path(m_MeshDesc.ResourcePath) );

	if( m_pMeshObject )
		m_pMeshObject->LoadNonAsyncResources( archive, m_MeshDesc.LoadOptionFlags );
//		m_pMeshObject->LoadNonAsyncResources( archive, MeshLoadOption::LOAD_TEXTURES_ASYNC, m_MeshDesc.vecpGroup );

	dirstk.pop_and_chdir();


//	m_pMeshObject->GetTexture( 0, 0 ).GetEntry()->GetMeshDesc
}


bool CMeshResource::LoadMeshFromArchive( C3DMeshModelArchive& mesh_archive )
{
	if( GetState() == GraphicsResourceState::LOADED )
		return false;

	if( m_pMeshObject )
	{
		bool res = m_pMeshObject->LoadFromArchive(
			mesh_archive,
			m_MeshDesc.ResourcePath,
			MeshLoadOption::LOAD_TEXTURES_ASYNC
			);

		if( res )
			SetState( GraphicsResourceState::LOADED );
		else
			LOG_PRINT_ERROR( "Failed to load a mesh '" + m_MeshDesc.ResourcePath + "'from archive." );

		return res;
	}
	else
		return false;

}


void CMeshResource::GetStatus( char *pDestBuffer )
{
	CGraphicsResource::GetStatus( pDestBuffer );

	char buffer[256];
	const CMeshResourceDesc& desc = m_MeshDesc;

	sprintf( buffer, " / %d", desc.MeshType );
	strcat( pDestBuffer, buffer );
}


//==================================================================================================
// CShaderResource
//==================================================================================================

CShaderResource::CShaderResource( const CShaderResourceDesc *pDesc )
:
m_pShaderManager(NULL)
{
	if( pDesc )
		m_ShaderDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


CShaderResource::~CShaderResource()
{
//	LOG_FUNCTION_SCOPE();

	Release();
}


bool CShaderResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SafeDelete( m_pShaderManager );

	LOG_PRINT_ERROR( " - Not implemented!" );

	return false;

	// retrieve mesh archive from db
//	db.GetData( keyname, shader_archive );

//	m_pShaderManager = CreateShaderFromArchive( shader_archive );

//	return ( m_pShaderManager ? true : false );
}


bool CShaderResource::Create()
{
	SafeDelete( m_pShaderManager );
	m_pShaderManager = CreateShaderManager();

	return true;
}


bool CShaderResource::LoadFromFile( const std::string& filepath )
{
	SafeDelete( m_pShaderManager );

	// load a shader file
	m_pShaderManager = CreateShaderManager();
	bool loaded = m_pShaderManager->LoadShaderFromFile( filepath );

	if( loaded )
		SetState( GraphicsResourceState::LOADED );

	return loaded;
}


void CShaderResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SafeDelete( m_pShaderManager );

	SetState( GraphicsResourceState::RELEASED );
}


bool CShaderResource::IsDiskResource() const
{
	if( m_ShaderDesc.ShaderType == CShaderType::PROGRAMMABLE
	 && 0 < m_ShaderDesc.ResourcePath.length() )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CShaderResource::CreateFromDesc()
{
	if( m_ShaderDesc.pShaderGenerator )
	{
		string shader_content;
		m_ShaderDesc.pShaderGenerator->GetShader( shader_content );

		// Need to convert to stream_buffer
		stream_buffer buffer;

		// Since stream_buffer is not a null terminated string buffer, but a generic char buffer,
		// we need to add +1 to the source string size to copy its terminating null char.
		buffer.write( (void *)shader_content.c_str(), shader_content.length() + 1 );

		return CreateShaderFromTextBuffer( buffer );
	}
	else if( m_ShaderDesc.ShaderType == CShaderType::NON_PROGRAMMABLE )
	{
		SafeDelete( m_pShaderManager );

		// create pseudo shader manager for fixed function pipleline.
		m_pShaderManager = CreateFixedFunctionPipelineManager();
		return true;
	}
	else
		return false;
}


bool CShaderResource::CreateShaderFromTextBuffer( stream_buffer& buffer )
{
	if( !m_pShaderManager )
		m_pShaderManager = CreateShaderManager();

	bool loaded = m_pShaderManager->LoadShaderFromText( buffer );

	if( loaded )
		SetState( GraphicsResourceState::LOADED );

	return loaded;
}
