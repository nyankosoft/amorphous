#include "BasicMesh.hpp"
#include "MeshFactory.hpp" // ctor of CBasicMesh needs the mesh impl factory 
#include "../MeshModel/3DMeshModelArchive.hpp"
#include "../MeshModel/MeshBone.hpp"
#include "../TextureGenerators/SingleColorTextureGenerator.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;


void SetSingleColorTextureDesc( CTextureResourceDesc& desc,
							   const SFloatRGBAColor& color = SFloatRGBAColor::White(),
							   int tex_width = 1,
							   int tex_height = 1 )
{
	desc.Width  = tex_width;
	desc.Height = tex_height;
	desc.Format = TextureFormat::A8R8G8B8;
	desc.pLoader.reset( new CSingleColorTextureGenerator(SFloatRGBAColor::White()) );
}



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

CMeshImpl::CMeshImpl()
:
m_bViewFrustumTest(false)
{
}


const CMeshBone& CMeshImpl::GetBone( const std::string& bone_name ) const
{
	return CMeshBone::NullBone();
}


const CMeshBone& CMeshImpl::GetRootBone() const
{
	return CMeshBone::NullBone();
}


Result::Name CMeshImpl::LoadMaterialsFromArchive( C3DMeshModelArchive& rArchive, U32 option_flags )
{
	const vector<CMMA_Material>& rvecSrcMaterial = rArchive.GetMaterial();

//	InitMaterials( (int)rvecSrcMaterial.size() );

	const int num_materials = (int)rvecSrcMaterial.size();
	m_vecMaterial.resize( num_materials );

	// create list of material indices
	// - used by Render() to render all the materials in the default order
	m_vecFullMaterialIndices.resize( num_materials );
	for( int i=0; i<num_materials; i++ )
		m_vecFullMaterialIndices[i] = i;

	// load AABBs
	m_AABB = rArchive.GetAABB(); // aabb for the mesh
	m_vecAABB.resize(num_materials);
	for( int mat=0; mat < num_materials; mat++ )
		m_vecAABB[mat] = rArchive.GetTriangleSet()[mat].m_AABB; // AABBs that represent bounding boxes for each triangle set

	// all triangle sets are set visible by default
	m_IsVisible.resize( num_materials + 1, 1 );

	string tex_filename;

	bool loaded = false;
	for( int i=0; i<num_materials; i++ )
	{
		// name

		m_vecMaterial[i].Name = rvecSrcMaterial[i].Name;

		// materials

		m_vecMaterial[i].m_Mat = rvecSrcMaterial[i].m_Params;

		// texture(s)

		const size_t num_textures = rvecSrcMaterial[i].vecTexture.size();

		m_vecMaterial[i].Texture.resize( num_textures );
		m_vecMaterial[i].TextureDesc.resize( num_textures );

		for( size_t tex=0; tex<num_textures; tex++ )
		{
			const CMMA_Texture& texture_archive = rvecSrcMaterial[i].vecTexture[tex];
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
				else if( tex_filename.find( "::" ) != string::npos
					  && tex_filename.find( "::" ) != 0 )
				{
					// database::keyname
					tex_filepath = tex_filename;
				}
				else
				{
					// relative apth
					path filepath = path(m_strFilename).parent_path() / tex_filename;
					tex_filepath = filepath.string();
				}

				m_vecMaterial[i].TextureDesc[tex].ResourcePath = tex_filepath;
			}
			else if( texture_archive.type == CMMA_Texture::SINGLECOLOR
				&& 0 < texture_archive.vecfTexelData.size_x() )
			{
				CTextureResourceDesc& current_desc = m_vecMaterial[i].TextureDesc[tex];

				current_desc.pLoader.reset( new CSingleColorTextureGenerator( texture_archive.vecfTexelData(0,0) ) );

				current_desc.Width  = texture_archive.vecfTexelData.size_x();
				current_desc.Height = texture_archive.vecfTexelData.size_y();
				current_desc.Format = TextureFormat::A8R8G8B8;

				// TODO: define a function to set texture resource id string
				static int s_texcount = 0;
				current_desc.ResourcePath = "<Texture>" + to_string(s_texcount);

			}

			if( m_vecMaterial[i].TextureDesc[tex].IsValid() )
			{
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
					m_vecMaterial[i].TextureDesc[tex].LoadingMode = CResourceLoadingMode::SYNCHRONOUS;
					loaded = m_vecMaterial[i].Texture[tex].Load( m_vecMaterial[i].TextureDesc[tex] );
/*					if( !loaded )
					{
						SetSingleColorTextureDesc( m_vecMaterial[i].TextureDesc[tex], SFloatRGBAColor::White() );
						m_vecMaterial[i].Texture[tex].Load( m_vecMaterial[i].TextureDesc[tex] );
					}*/
				}
			}
		}

		// set minimum alpha
		m_vecMaterial[i].fMinVertexDiffuseAlpha = rvecSrcMaterial[i].fMinVertexDiffuseAlpha;
	}

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



CBasicMesh::CBasicMesh()
{
	CMeshImpl *pImpl = MeshImplFactory()->CreateBasicMeshImpl();
	m_pImpl.reset( pImpl );
}


} // namespace amorphous
