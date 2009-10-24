#include "BasicMesh.hpp"
#include <gds/Support/fnop.hpp>
#include <gds/Graphics/D3DXMeshObjectBase.hpp>
using namespace fnop;


using namespace std;
using namespace boost;


//=============================================================================
// CMeshMaterial
//=============================================================================

void CMeshMaterial::LoadTextureAsync( int i )
{
	if( i < 0 || (int)Texture.size() <= i )
		return;

	CTextureResourceDesc desc = TextureDesc[i];
	desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
	Texture[i].Load( desc );
}



//=============================================================================
// CMeshImpl
//=============================================================================

Result::Name CMeshImpl::LoadMaterials( C3DMeshModelArchive& rArchive, U32 option_flags )
{
	m_AABB = rArchive.GetAABB();
	vector<CMMA_Material>& rvecSrcMaterial = rArchive.GetMaterial();

	m_NumMaterials = (int)rvecSrcMaterial.size();
	m_vecMaterial.resize( m_NumMaterials );

/*	InitMaterials( (int)rvecSrcMaterial.size() );

	// load AABBs that represent bounding boxes for each triangle set
	m_vecAABB.resize(m_NumMaterials);
	for( int mat=0; mat < m_NumMaterials; mat++ )
		m_vecAABB[mat] = rArchive.GetTriangleSet()[mat].m_AABB;

	// all triangle sets are set visible by default
	m_IsVisible.resize( m_NumMaterials + 1, 1 );
*/
	string tex_filename;

	// push the current working directory to the directory stack
	dir_stack dirstk( get_path(m_strFilename) );

	// save the current working directory
//	string orig_dir = fnop::get_cwd();
	// set the abs path of the mesh file as the working directory
//	fnop::set_wd( fnop::get_path( m_strFilename ) );

	bool loaded = false;
	for( int i=0; i<m_NumMaterials; i++ )
	{
/*		memset( &m_pMeshMaterials[i], 0, sizeof(D3DMATERIAL9) );

		float specular = rvecSrcMaterial[i].fSpecular;
		m_pMeshMaterials[i].Specular.r = specular;
		m_pMeshMaterials[i].Specular.g = specular;
		m_pMeshMaterials[i].Specular.b = specular;
		m_pMeshMaterials[i].Specular.a = specular;

		m_pMeshMaterials[i].Diffuse.r = 1.0f;
		m_pMeshMaterials[i].Diffuse.g = 1.0f;
		m_pMeshMaterials[i].Diffuse.b = 1.0f;
		m_pMeshMaterials[i].Diffuse.a = 1.0f;

		m_pMeshMaterials[i].Ambient.r = 0.25f;
		m_pMeshMaterials[i].Ambient.g = 0.25f;
		m_pMeshMaterials[i].Ambient.b = 0.25f;
		m_pMeshMaterials[i].Ambient.a = 1.00f;
*/
		// texture(s)

		const size_t num_textures = rvecSrcMaterial[i].vecTexture.size();

		m_vecMaterial[i].Texture.resize( num_textures );
		m_vecMaterial[i].TextureDesc.resize( num_textures );

		for( size_t tex=0; tex<num_textures; tex++ )
		{
			CMMA_Texture& texture_archive = rvecSrcMaterial[i].vecTexture[tex];
			tex_filename = texture_archive.strFilename;
			if( 0 < tex_filename.length() )
			{
				string tex_filepath;
				if( 4 < tex_filename.length() // valid shortest abs. path filename - "C:/a"
					&& tex_filename[1] == ':'
					&& tex_filename[2] != ':' ) // rule out database resource name - e.g., "a::b"
				{
					// absolute path
					tex_filepath = tex_filename;
				}
				else
				{
					// relative apth
					tex_filepath = fnop::get_cwd() + "/" + tex_filename;
				}

				m_vecMaterial[i].TextureDesc[tex].ResourcePath = tex_filepath;

				if( option_flags & MeshLoadOption::DO_NOT_LOAD_TEXTURES )
				{
				}
				else if( option_flags & MeshLoadOption::LOAD_TEXTURES_ASYNC )
				{
					// start the asynchronous loading now
//					m_vecMaterial[i].TextureDesc[tex].vecpGroup = vecpGroup;
					m_vecMaterial[i].TextureDesc[tex].LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
					m_vecMaterial[i].Texture[tex].Load( m_vecMaterial[i].TextureDesc[tex] );
				}
				else
				{
					// load texture(s) now
					loaded = m_vecMaterial[i].Texture[tex].Load( tex_filepath );
				}
			}
		}

/*		if( !bLoaded )
		{	// texture file was not found - create default white texture 
			DWORD dwColor = 0xFFFFFFFF;
			CTextureTool::CreateTexture( &dwColor, 1, 1, &(m_paMaterial[i].m_pSurfaceTexture) );
		}*/

		// set minimum alpha
		m_vecMaterial[i].fMinVertexDiffuseAlpha = rvecSrcMaterial[i].fMinVertexDiffuseAlpha;
	}

	// back to the original working directory
	dirstk.prevdir();
//	fnop::set_wd( orig_dir );

	return Result::SUCCESS;
}


bool CMeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
{
//	Release();

	bool loaded = false;

	if( 0 < filename.length() )
	{
		m_strFilename = filename;
	}
	else
		return false;

	C3DMeshModelArchive archive;
	bool b = archive.LoadFromFile( filename );

	if( !b )
		return false;

	loaded = LoadFromArchive( archive, filename, option_flags );

//	if( loaded )
//		m_strFilename = filename;

    return loaded;
}


void CMeshImpl::Render( CShaderManager& rShaderMgr, std::vector<CShaderTechniqueHandle>& vecShaderTechnique )
{
	const int num_shader_techniques = (int)vecShaderTechnique.size();
	const int num_loops
		= GetNumMaterials() < num_shader_techniques ? GetNumMaterials() : num_shader_techniques;

	vector<int> vecMatIndex( num_loops );
	for( int i=0; i<num_loops; i++ )
		vecMatIndex[i] = i;

	this->RenderSubsets( rShaderMgr, vecMatIndex, vecShaderTechnique );
}



//=============================================================================
// CD3DMeshFactory
//=============================================================================
/*
#include "D3DBasicMeshImpl.hpp"
#include "D3DProgressiveMeshImpl.hpp"
#include "D3DSkeletalMeshImpl.hpp"


//class CMeshFactoryImpl_D3D : public CMeshFactoryImpl
class CD3DMeshFactory : public CMeshFactory
{
public:

	boost::shared_ptr<CBasicMesh> CreateBasicMeshImpl()
	{
		shared_ptr<CD3DBasicMeshImpl> pImpl( new CD3DBasicMeshImpl );
		shared_ptr<CBasicMesh> p( new CBasicMesh(pImpl) );
		return p;
	}

	boost::shared_ptr<CProgressiveMesh> CreateProgressiveMeshImpl()
	{
		shared_ptr<CD3DProgressiveMeshImpl> pImpl( new CD3DProgressiveMeshImpl );
		shared_ptr<CProgressiveMesh> p( new CProgressiveMesh(pImpl) );
		return p;
	}

	boost::shared_ptr<CSkeletalMesh> CreateSkeletalMeshImpl()
	{
		shared_ptr<CD3DSkeletalMeshImpl> pImpl( new CD3DSkeletalMeshImpl );
		shared_ptr<CSkeletalMesh> p( new CSkeletalMesh(pImpl) );
		return p;
	}
};
*/


#include "GLBasicMeshImpl.hpp"
#include "GLProgressiveMeshImpl.hpp"
#include "GLSkeletalMeshImpl.hpp"


//class CMeshFactoryImpl_GL : public CMeshFactoryImpl
class CGLMeshFactory : public CMeshFactory
{
public:

//	boost::shared_ptr<CBasicMesh> CreateBasicMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateBasicMeshImpl()
	{
		shared_ptr<CGLBasicMeshImpl> pImpl( new CGLBasicMeshImpl );
		return pImpl;
//		shared_ptr<CBasicMesh> p( new CBasicMesh(pImpl) );
//		return p;
	}

//	boost::shared_ptr<CProgressiveMesh> CreateProgressiveMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateProgressiveMeshImpl()
	{
		shared_ptr<CGLProgressiveMeshImpl> pImpl( new CGLProgressiveMeshImpl );
		return pImpl;
//		shared_ptr<CProgressiveMesh> p( new CProgressiveMesh(pImpl) );
//		return p;
	}

//	boost::shared_ptr<CSkeletalMesh> CreateSkeletalMeshImpl()
	boost::shared_ptr<CMeshImpl> CreateSkeletalMeshImpl()
	{
		shared_ptr<CGLSkeletalMeshImpl> pImpl( new CGLSkeletalMeshImpl );
		return pImpl;
//		shared_ptr<CSkeletalMesh> p( new CSkeletalMesh(pImpl) );
//		return p;
	}
};

/*
extern void InitMeshFactory( boost::shared_ptr<CMeshFactory>& pMeshFactory )
{
	if( 
}
*/


#include "ProgressiveMesh.hpp"
#include "SkeletalMesh.hpp"


CBasicMesh::CBasicMesh()
{
	CMeshImpl *pImpl = MeshImplFactory()->CreateBasicMeshImpl();
	m_pImpl = shared_ptr<CMeshImpl>( pImpl );
}



//=============================================================================
// CMeshFactory
//=============================================================================

CBasicMesh *CMeshFactory::CreateMeshInstance( CMeshType::Name mesh_type )
{
	switch( mesh_type )
	{
	case CMeshType::BASIC:
		return CreateBasicMeshInstance();
	case CMeshType::PROGRESSIVE:
		return CreateProgressiveMeshInstance();
	case CMeshType::SKELETAL:
		return CreateSkeletalMeshInstance();
	default:
		return NULL;
	}

	return NULL;
}


shared_ptr<CBasicMesh> CMeshFactory::CreateMesh( CMeshType::Name mesh_type )
{
	return shared_ptr<CBasicMesh>( CreateMeshInstance( mesh_type ) );
}


CBasicMesh *CMeshFactory::CreateBasicMeshInstance() { return new CBasicMesh; }
CProgressiveMesh *CMeshFactory::CreateProgressiveMeshInstance() { return new CProgressiveMesh; } 
CSkeletalMesh *CMeshFactory::CreateSkeletalMeshInstance() { return new CSkeletalMesh; } 

shared_ptr<CBasicMesh> CMeshFactory::CreateBasicMesh() { shared_ptr<CBasicMesh> pMesh( CreateBasicMeshInstance() ); return pMesh; }
shared_ptr<CProgressiveMesh> CMeshFactory::CreateProgressiveMesh() { shared_ptr<CProgressiveMesh> pMesh( CreateProgressiveMeshInstance() ); return pMesh; } 
shared_ptr<CSkeletalMesh> CMeshFactory::CreateSkeletalMesh() { shared_ptr<CSkeletalMesh> pMesh( CreateSkeletalMeshInstance() ); return pMesh; } 


CBasicMesh* CMeshFactory::LoadMeshObjectFromFile( const std::string& filepath,
												  U32 load_option_flags,
												  CMeshType::Name mesh_type )
{
	CBasicMesh* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromFile( filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}


CBasicMesh* CMeshFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	CMeshType::Name mesh_type )
{
	CBasicMesh* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}
