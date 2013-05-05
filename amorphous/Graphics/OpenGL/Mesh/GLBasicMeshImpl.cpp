#include "GLBasicMeshImpl.hpp"
#include "../GLExtensions.hpp"
#include "../GLGraphicsDevice.hpp" // for LOG_GL_ERROR() macro
#include "../GLTextureResourceVisitor.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Mesh/CustomMesh.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;
//using namespace boost;


#define GL_INDEX_BUFFER_OFFSET(i) ((char *)NULL + (i))


static void SetGLTextures( MeshMaterial& mat )
{
	vector<TextureHandle>& vecTex = mat.Texture;
	for( uint j=0; j<(uint)vecTex.size(); j++ )
	{
		SetTextureGL_FFP( j, vecTex[j] );
	}
}


GLBasicMeshImpl::GLBasicMeshImpl()
:
m_VAO(0),
m_NumIndices(0),
//m_PositionBuffer(0),
//m_NormalBuffer(0),
//m_DiffuseColorBuffer(0),	
//m_TexCoordBuffer(0),
m_IndexBuffer(0)
{
	m_VBOSupported = true;

	for( int i=0; i<VEE::NUM_VERTEX_ELEMENTS; i++ )
	{
		m_VertexElementOffsets[i] = 0;
	}
}


GLBasicMeshImpl::GLBasicMeshImpl( const std::string& filename )
{
	bool loaded = LoadFromFile(filename);

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load a mesh file" + filename );
	}
}


GLBasicMeshImpl::~GLBasicMeshImpl()
{
	Release();
}


void GLBasicMeshImpl::Release()
{
//	glDeleteBuffers( 1, &m_PositionBuffer );
}


bool GLBasicMeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( filename );

	if( !loaded )
		return false;

	return LoadFromArchive( archive, filename, option_flags );
}


Result::Name GLBasicMeshImpl::InitVerticesAndIndices( const CMMA_VertexSet& vertex_set )
{
	glGenVertexArrays( 1, &m_VAO );
	glBindVertexArray( m_VAO );

	GLuint vertex_element_index = 0;

	size_t offset = 0;

	const uint vertex_flags = vertex_set.m_VertexFormatFlag;

	if( vertex_set.m_VertexFormatFlag & CMMA_VertexSet::VF_POSITION )
	{
		glEnableVertexAttribArray( vertex_element_index );
		glVertexAttribPointer( vertex_element_index++, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		offset += sizeof(float) * 3;
	}
	else
	{
		LOG_PRINT_ERROR( " A vertex must at least have a position." );
		return Result::INVALID_ARGS;
	}

	if( vertex_set.m_VertexFormatFlag & CMMA_VertexSet::VF_NORMAL )
	{
		m_VertexElementOffsets[VEE::NORMAL] = offset;
		glEnableVertexAttribArray( vertex_element_index );
		glVertexAttribPointer( vertex_element_index++, 3, GL_FLOAT, GL_FALSE, 0, (void *)offset );
		offset += sizeof(float) * 3;
	}

	if( vertex_set.m_VertexFormatFlag & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		m_VertexElementOffsets[VEE::DIFFUSE_COLOR] = offset;
		glEnableVertexAttribArray( vertex_element_index );
		glVertexAttribPointer( vertex_element_index++, 4, GL_FLOAT, GL_FALSE, 0, (void *)offset );
		offset += sizeof(float) * 4;
	}

	U32 texcoord2_element_flags[] =
	{
		VFF::TEXCOORD2_0,
		VFF::TEXCOORD2_1,
		VFF::TEXCOORD2_2,
		VFF::TEXCOORD2_3
	};

	for( int i=0; i<numof(texcoord2_element_flags); i++ )
	{
		if( vertex_flags & texcoord2_element_flags[i] )
		{
			m_VertexElementOffsets[VEE::TEXCOORD2_0 + i] = offset;
			glEnableVertexAttribArray( vertex_element_index );
			glVertexAttribPointer( vertex_element_index++, 2, GL_FLOAT, GL_FALSE, 0, (void *)offset );
			offset += sizeof(TEXCOORD2);
		}
	}

	glBindVertexArray( 0 );

	return Result::SUCCESS;
}


bool GLBasicMeshImpl::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
//	if( !IsValidMeshModelArchive( archive ) )
//		return false;

	// copy the contents from the archive

	m_strFilename = filename;

	CustomMesh cm;
	cm.LoadFromArchive( archive );

	InitVerticesAndIndices( archive.GetVertexSet() );

	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

	m_vecTriangleSet = archive.GetTriangleSet();

	return true;
}


bool GLBasicMeshImpl::CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags )
{
	LOG_PRINT_ERROR( " Not implemented." );
	return false;
}


/// renders subsets of the mesh
/// - use different shader techniques for each material
void GLBasicMeshImpl::RenderSubsets( ShaderManager& rShaderMgr,
		                     const std::vector<int>& vecMaterialIndex,
							 std::vector<ShaderTechniqueHandle>& vecShaderTechnique )
{
}


void GLBasicMeshImpl::Render( ShaderManager& rShaderMgr )
{
	rShaderMgr.Begin();

	Render();
}


void GLBasicMeshImpl::RenderSubset( ShaderManager& rShaderMgr, int material_index )
{
}


void GLBasicMeshImpl::Render()
{
	if( m_VAO == 0 )
		return;

	glBindVertexArray( m_VAO );

	glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );

	glBindVertexArray( 0 );
}


/// renders subsets of the mesh with the current shader technique
/// - the same shader technique is used to render all the materials
void GLBasicMeshImpl::RenderSubsets( ShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
{
}


} // namespace amorphous
