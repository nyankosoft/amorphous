#include "GraphicsResources.hpp"

#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp" // For GetCustomMesh()
#include "amorphous/Graphics/Mesh/MeshFactory.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/ShaderGenerator.hpp"
#include "amorphous/Support/SafeDelete.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/ImageArchive.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Support/Serialization/BinaryDatabase.hpp"


#ifdef __ANDROID__
namespace amorphous
{
AAssetManager *sg_pAssetManager = nullptr;
void SetAssetManager(AAssetManager *pAssetManager)
{
	LOG_PRINT(" entered.");
	if(!pAssetManager)
		LOG_PRINT("null pAssetManager");

	sg_pAssetManager = pAssetManager;
}
}
#endif // __ANDROID__

namespace amorphous
{
using namespace serialization;
using namespace std;


//==================================================================================================
// GraphicsResource
//==================================================================================================

GraphicsResource::GraphicsResource()
:
m_OptionFlags(0),
m_LastModifiedTimeOfFile(0),
m_State(GraphicsResourceState::RELEASED),
m_IsCachedResource(false),
m_Index(0)
{}


GraphicsResource::~GraphicsResource()
{
//	Release();	// LNK2019 - why???
}


/*
void GraphicsResource::Release()
{}
*/


/**
  Release and reload resource if the file has been modified since the last time is was load
   - This method does not change the reference count
 */
void GraphicsResource::Refresh()
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
bool GraphicsResource::Load()
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


void GraphicsResource::ReleaseNonChachedResource()
{
	if( !m_IsCachedResource )
		Release();
}


void GraphicsResource::ReleaseCachedResource()
{
	if( m_IsCachedResource )
		Release();
}


bool GraphicsResource::LoadFromDisk()
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
#ifdef __ANDROID__
	else if(resource_path.find("asset://") == 0)
	{
		if( !sg_pAssetManager )
		{
			LOG_PRINT_ERROR("asset_manager == null");
			return false;
		}

		const string asset_pathname = resource_path.substr(string("asset://").length());
		LOG_PRINT("asset_pathname: " + asset_pathname);
		
		AAsset *asset = AAssetManager_open(sg_pAssetManager,asset_pathname.c_str(),AASSET_MODE_BUFFER);

		if(!asset)
		{
			LOG_PRINTF_ERROR(("AAssetManager_open() returned null. File: %s",asset_pathname.c_str()));
			return false;
		}

		const int buffer_size = 300000;
		unsigned char buffer[buffer_size];

		int num_bytes_read = AAsset_read(asset,buffer,(size_t)buffer_size);

		LOG_PRINTF(("num_bytes_read: %d", num_bytes_read));

		if(num_bytes_read < 0)
		{
			return false;
		}

		string output_pathname = "/storage/emulated/0/Download/" + asset_pathname;
		FILE *fp = fopen(output_pathname.c_str(),"wb");
		if(fp)
		{
			size_t written = fwrite(buffer,1,num_bytes_read,fp);
			LOG_PRINTF(("written: %d",(int)written));
			fclose(fp);
		}

		loaded = LoadFromMemory(buffer, num_bytes_read);

		AAsset_close(asset);

		asset = nullptr;

		//loaded = LoadFromFile( output_pathname );
	}
#endif /* __ANDROID__ */
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


bool GraphicsResource::CanBeSharedAsSameResource( const GraphicsResourceDesc& desc )
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


void GraphicsResource::GetStatus( std::string& dest_buffer )
{
	dest_buffer += ( " / " + GetDesc().ResourcePath );
}



//==================================================================================================
// TextureResource
//==================================================================================================

TextureResource::TextureResource( const TextureResourceDesc *pDesc )
{
	if( pDesc )
		m_TextureDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


TextureResource::~TextureResource()
{
	Release();
}


bool TextureResource::CanBeSharedAsSameResource( const GraphicsResourceDesc& desc )
{
	return GraphicsResource::CanBeSharedAsSameResource( desc );
}


int TextureResource::CanBeUsedAsCache( const GraphicsResourceDesc& desc )
{
	if( GetState() != GraphicsResourceState::RELEASED )
		return 0;

	return desc.CanBeUsedAsTextureCache( m_TextureDesc );
}


void TextureResource::UpdateDescForCachedResource( const GraphicsResourceDesc& desc )
{
	desc.UpdateCachedTextureResourceDesc( m_TextureDesc );
}


bool TextureResource::GetLockedTexture( shared_ptr<LockedTexture>& pLockedTexture )
{
	// TODO: increment the ref count when the async loading process is started
//	m_iRefCount++;

	if( !m_pLockedTexture )
		return false;

	pLockedTexture = m_pLockedTexture;
	return true;
}


void TextureResource::Release()
{
	SetState( GraphicsResourceState::RELEASED );
}


bool TextureResource::IsDiskResource() const
{
	return m_TextureDesc.IsDiskResource();

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


void TextureResource::GetStatus( std::string& dest_buffer )
{
	GraphicsResource::GetStatus( dest_buffer );

	const TextureResourceDesc& desc = m_TextureDesc;

	dest_buffer += ( " / " + to_string(desc.Width) + " x " + to_string(desc.Height) );
}



//==================================================================================================
// MeshResource
//==================================================================================================

shared_ptr<CustomMesh> GetCustomMesh( BasicMesh& src_mesh )
{
	shared_ptr<MeshImpl> pMeshImpl = src_mesh.m_pImpl;
	if( !pMeshImpl )
		return shared_ptr<CustomMesh>();

	shared_ptr<CustomMesh> pCustomMesh = std::dynamic_pointer_cast<CustomMesh,MeshImpl>( pMeshImpl );

	return pCustomMesh;
}


shared_ptr<CustomMesh> GetCustomMesh( shared_ptr<BasicMesh> pSrcMesh )
{
	if( !pSrcMesh )
		return shared_ptr<CustomMesh>();

	return GetCustomMesh( *pSrcMesh );
}

/*
// Need to include MeshObjectHandle.hpp to define this
// Should move this to MeshObjectHandle.cpp?
shared_ptr<CustomMesh> GetCustomMesh( MeshHandle& src_mesh )
{
	return GetCustomMesh( src_mesh.GetMesh() );
}*/


MeshResource::MeshResource( const MeshResourceDesc *pDesc )
{
	if( pDesc )
		m_MeshDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An incompatible resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();

	for( int i=0; i<numof(m_aSubResourceState); i++ )
		m_aSubResourceState[i] = GraphicsResourceState::RELEASED;
}


MeshResource::~MeshResource()
{
	Release();
}


bool MeshResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	m_pMeshObject.reset();

	string mesh_archive_key = keyname;

	// retrieve mesh archive from db
	C3DMeshModelArchive mesh_archive;
	db.GetData( mesh_archive_key, mesh_archive );

	MeshFactory factory;
	BasicMesh *pMesh = factory.LoadMeshObjectFromArchive( mesh_archive, keyname, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );
	m_pMeshObject = std::shared_ptr<BasicMesh>( pMesh );

	return ( m_pMeshObject ? true : false );
}


bool MeshResource::LoadFromFile( const std::string& filepath )
{
	m_pMeshObject.reset();

	MeshFactory factory;
	BasicMesh *pMeshObject
		= factory.LoadMeshObjectFromFile( filepath, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<BasicMesh>( pMeshObject );
		return true;
	}
	else
	{
		LOG_PRINT_WARNING( "Failed to load the mesh: " + filepath );
		return false;
	}
}


bool MeshResource::LoadFromMemory(const unsigned char *buffer, int size_in_bytes)
{
//	LOG_FUNCTION_SCOPE();

	m_pMeshObject.reset();
	
	MeshFactory factory;
	LOG_PRINT("Loading mesh from memory.");
	BasicMesh *pMeshObject
		= factory.LoadMeshFromMemory( buffer, size_in_bytes, m_MeshDesc.ResourcePath, m_MeshDesc.LoadOptionFlags, m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<BasicMesh>( pMeshObject );
		return true;
	}
	else
	{
		LOG_PRINT_WARNING( "Failed to load the mesh from memory: " + m_MeshDesc.ResourcePath );
		return false;
	}
}


void MeshResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	m_pMeshObject.reset();

	SetState( GraphicsResourceState::RELEASED );
}


bool MeshResource::IsDiskResource() const
{
	if( m_MeshDesc.pMeshGenerator )
	{
		return false;
	}
	else
		return true;
}


bool MeshResource::CreateFromDesc()
{
	if( !m_MeshDesc.pMeshGenerator )
		return false;

	Result::Name res = m_MeshDesc.pMeshGenerator->Generate();
	if( res != Result::SUCCESS )
		return false;

	MeshFactory factory;
	BasicMesh *pMeshObject
		= factory.LoadMeshObjectFromArchive( m_MeshDesc.pMeshGenerator->MeshArchive(),
		                                     m_MeshDesc.ResourcePath,
											 m_MeshDesc.LoadOptionFlags,
											 m_MeshDesc.MeshType );

	if( pMeshObject )
	{
		m_pMeshObject = shared_ptr<BasicMesh>( pMeshObject );
		SetState( GraphicsResourceState::LOADED );
		return true;
	}
	else
		return false;
}


bool MeshResource::CanBeSharedAsSameResource( const GraphicsResourceDesc& desc )
{
	return desc.CanBeSharedAsSameMeshResource( m_MeshDesc );
}


int MeshResource::CanBeUsedAsCache( const GraphicsResourceDesc& desc )
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
	ID_AND_STRING(MeshType::BASIC),
	ID_AND_STRING(MeshType::PROGRESSIVE),
	ID_AND_STRING(MeshType::SKELETAL)
};
*/


bool MeshResource::Create()
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
	MeshFactory factory;
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

	bool mesh_created = false;
//	if( 0 < m_MeshDesc.vecVertElement.size() )
	if( false )
	{
//		mesh_created = m_pMeshObject->CreateMesh(
//		m_MeshDesc.NumVertices,
//		m_MeshDesc.NumIndices,
//		0,
//		m_MeshDesc.vecVertElement
//		);
	}
	else
	{
		mesh_created = m_pMeshObject->CreateMesh(
		m_MeshDesc.NumVertices,
		m_MeshDesc.NumIndices,
		0,
		m_MeshDesc.VertexFormatFlags
		);
	}

	if( mesh_created )
	{
		SetState( GraphicsResourceState::LOADED );
		return true;
	}
	else
		return false;
}


void MeshResource::SetSubResourceState( MeshSubResource::Name subresource,
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


void MeshResource::CreateMeshAndLoadNonAsyncResources( C3DMeshModelArchive& archive )
{
	lfs::dir_stack dirstk( lfs::get_parent_path(m_MeshDesc.ResourcePath) );

	if( m_pMeshObject )
		m_pMeshObject->LoadNonAsyncResources( archive, m_MeshDesc.LoadOptionFlags );
//		m_pMeshObject->LoadNonAsyncResources( archive, MeshLoadOption::LOAD_TEXTURES_ASYNC, m_MeshDesc.vecpGroup );

	dirstk.pop_and_chdir();


//	m_pMeshObject->GetTexture( 0, 0 ).GetEntry()->GetMeshDesc
}


bool MeshResource::LoadMeshFromArchive( C3DMeshModelArchive& mesh_archive )
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


void MeshResource::ReleaseGraphicsResources()
{
	if( !m_pMeshObject )
		return;

	if( m_pMeshObject->IsCustomMesh() )
	{
		// Since the mesh stores all the data using generic containers
		// and does not use data type specpfic to a graphics library,
		// we simply skip the releasing/re-creating process.
		return;
	}
	else
	{
		ReleaseNonChachedResource();
	}
}


void MeshResource::LoadGraphicsResources()
{
	if(m_pMeshObject && m_pMeshObject->IsCustomMesh())
	{
		return;
	}
	else
	{
		Load();
	}
}


void MeshResource::GetStatus( std::string& dest_buffer )
{
	GraphicsResource::GetStatus( dest_buffer );

	const MeshResourceDesc& desc = m_MeshDesc;

	dest_buffer += " / " + to_string((int)desc.MeshType);
}


//==================================================================================================
// ShaderResource
//==================================================================================================

ShaderResource::ShaderResource( const ShaderResourceDesc *pDesc )
:
m_pShaderManager(nullptr)
{
	if( pDesc )
		m_ShaderDesc = *pDesc;
	else
		LOG_PRINT_ERROR( "An invalid resource desc" );

	m_IsCachedResource = pDesc->IsCachedResource();
}


ShaderResource::~ShaderResource()
{
//	LOG_FUNCTION_SCOPE();

	Release();
}


bool ShaderResource::LoadFromDB( CBinaryDatabase<std::string>& db, const std::string& keyname )
{
	SafeDelete( m_pShaderManager );

	LOG_PRINT_ERROR( " - Not implemented!" );

	return false;

	// retrieve mesh archive from db
//	db.GetData( keyname, shader_archive );

//	m_pShaderManager = CreateShaderFromArchive( shader_archive );

//	return ( m_pShaderManager ? true : false );
}


bool ShaderResource::Create()
{
	SafeDelete( m_pShaderManager );
	m_pShaderManager = CreateShaderManager();

	return true;
}


bool ShaderResource::LoadFromFile( const std::string& filepath )
{
	SafeDelete( m_pShaderManager );

	// load a shader file
	m_pShaderManager = CreateShaderManager();
	bool loaded = m_pShaderManager->LoadShaderFromFile( filepath );

	if( loaded )
		SetState( GraphicsResourceState::LOADED );

	return loaded;
}


bool ShaderResource::CanBeSharedAsSameResource( const GraphicsResourceDesc& desc )
{
	return desc.CanBeSharedAsSameShaderResource( m_ShaderDesc );
}


void ShaderResource::Release()
{
//	LOG_FUNCTION_SCOPE();

	SafeDelete( m_pShaderManager );

	SetState( GraphicsResourceState::RELEASED );
}


bool ShaderResource::IsDiskResource() const
{
	if( m_ShaderDesc.ShaderType == ShaderTypeName::PROGRAMMABLE
	 && 0 < m_ShaderDesc.ResourcePath.length() )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool ShaderResource::CreateFromDesc()
{
	if( m_ShaderDesc.pShaderGenerator )
	{
		string shader_content;
		m_ShaderDesc.pShaderGenerator->GetShader( shader_content );

		if( shader_content.length() == 0 )
		{
			// We assume that the shader generator separately generates
			// the vertex & fragment shaders.

			string vs, fs;
			m_ShaderDesc.pShaderGenerator->GetVertexShader( vs );
			m_ShaderDesc.pShaderGenerator->GetPixelShader( fs );

			return CreateProgramFromSource( vs, fs );
		}

		// Need to convert to stream_buffer
		stream_buffer buffer;

		// Since stream_buffer is not a null terminated string buffer, but a generic char buffer,
		// we need to add +1 to the source string size to copy its terminating null char.
		buffer.write( (void *)shader_content.c_str(), shader_content.length() + 1 );

		return CreateShaderFromTextBuffer( buffer );
	}
	else if( m_ShaderDesc.ShaderType == ShaderTypeName::NON_PROGRAMMABLE )
	{
		SafeDelete( m_pShaderManager );

		// create pseudo shader manager for fixed function pipleline.
		m_pShaderManager = CreateFixedFunctionPipelineManager();
		return true;
	}
	else
		return false;
}


bool ShaderResource::CreateShaderFromTextBuffer( stream_buffer& buffer )
{
	if( !m_pShaderManager )
		m_pShaderManager = CreateShaderManager();

	if( !m_pShaderManager )
		return false;

	bool loaded = m_pShaderManager->LoadShaderFromText( buffer );

	if( loaded )
		SetState( GraphicsResourceState::LOADED );

	return loaded;
}


bool ShaderResource::CreateProgramFromSource( const std::string& vertex_shader, const std::string& fragment_shader )
{
	if( !m_pShaderManager )
		m_pShaderManager = CreateShaderManager();

	if( !m_pShaderManager )
	{
		LOG_PRINT_ERROR("!m_pShaderManager");
		return false;
	}

	bool loaded = m_pShaderManager->LoadShaderFromText( vertex_shader, fragment_shader );

	if( loaded )
		SetState( GraphicsResourceState::LOADED );
	else
	{
		LOG_PRINT_ERROR("!loaded");
	}

	return loaded;
}


} // namespace amorphous
