#include "BasicMesh.hpp"
#include "MeshFactory.hpp" // ctor of BasicMesh needs the mesh impl factory 
#include "../MeshModel/3DMeshModelArchive.hpp"
#include "../MeshModel/MeshBone.hpp"
#include "../TextureUtilities.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;
using namespace boost::filesystem;


//=============================================================================
// MeshMaterial
//=============================================================================

void MeshMaterial::LoadTextureAsync( int i )
{
	if( i < 0 || (int)Texture.size() <= i )
		return;

	TextureResourceDesc desc = TextureDesc[i];
	desc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
	Texture[i].Load( desc );
}



//=============================================================================
// MeshImpl
//=============================================================================

MeshImpl::MeshImpl()
:
m_bViewFrustumTest(false)
{
}


const MeshBone& MeshImpl::GetBone( const std::string& bone_name ) const
{
	return MeshBone::NullBone();
}


const MeshBone& MeshImpl::GetRootBone() const
{
	return MeshBone::NullBone();
}


Result::Name MeshImpl::LoadMaterialsFromArchive( C3DMeshModelArchive& rArchive, U32 option_flags )
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
			const TextureResourceDesc& texture_archive = rvecSrcMaterial[i].vecTexture[tex];
			tex_filename = texture_archive.ResourcePath;
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
			else
			{
				TextureResourceDesc& current_desc = m_vecMaterial[i].TextureDesc[tex];

				current_desc = texture_archive;

				// TODO: define a function to set texture resource id string
				static int s_texcount = 0;
				if( current_desc.ResourcePath.length() == 0 )
				{
					current_desc.ResourcePath = "<Texture>" + to_string(s_texcount);
					s_texcount += 1;
				}

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


bool MeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
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


void MeshImpl::Render( ShaderManager& rShaderMgr, std::vector<ShaderTechniqueHandle>& vecShaderTechnique )
{
	const int num_shader_techniques = (int)vecShaderTechnique.size();
	const int num_loops
		= GetNumMaterials() < num_shader_techniques ? GetNumMaterials() : num_shader_techniques;

	vector<int> vecMatIndex( num_loops );
	for( int i=0; i<num_loops; i++ )
		vecMatIndex[i] = i;

	this->RenderSubsets( rShaderMgr, vecMatIndex, vecShaderTechnique );
}


BasicMesh::BasicMesh()
{
	MeshImpl *pImpl = GetMeshImplFactory()->CreateBasicMeshImpl();
	m_pImpl.reset( pImpl );
}


} // namespace amorphous
