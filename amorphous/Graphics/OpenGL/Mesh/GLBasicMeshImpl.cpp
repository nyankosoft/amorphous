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
m_VBO(0),
m_IBO(0),
m_VertexFormatFlags(0),
m_VertexSize(0),
m_NumIndices(0),
m_IndexSize(sizeof(unsigned short)),
m_IndexType(GL_UNSIGNED_SHORT)
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
	glDeleteBuffers( 1, &m_VBO );
	glDeleteBuffers( 1, &m_IBO );
}


bool GLBasicMeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( filename );

	if( !loaded )
		return false;

	return LoadFromArchive( archive, filename, option_flags );
}


Result::Name GLBasicMeshImpl::InitVerticesAndIndices( C3DMeshModelArchive& archive )
{
	Release();

	CustomMesh cm;
	bool cm_loaded = cm.LoadFromArchive( archive );

	if( !cm_loaded )
	{
		LOG_PRINT_ERROR( " Failed to load the mesh as a custom mesh." );
		return Result::UNKNOWN_ERROR;
	}

	glGenBuffers( 1, &m_VBO );

	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );

	glBufferData(
		GL_ARRAY_BUFFER,
		cm.GetVertexSize() * cm.GetNumVertices(),
		cm.GetVertexBufferPtr(),
		GL_STATIC_DRAW
		);

	const CMMA_VertexSet& vertex_set = archive.GetVertexSet();

	glGenVertexArrays( 1, &m_VAO );
	glBindVertexArray( m_VAO );

	GLuint vertex_element_index = 0;

	size_t offset = 0;

	const uint vertex_flags = ToVFF( vertex_set.GetVertexFormat() );

	m_VertexFormatFlags = vertex_flags;

	m_VertexSize = cm.GetVertexSize();

	if( vertex_flags & VFF::POSITION )
	{
		m_VertexElementOffsets[VEE::POSITION] = (uint)cm.GetVertexElementOffset( VEE::POSITION );
		m_VertexElementStreamIndices[VEE::POSITION] = vertex_element_index++;
	}
	else
	{
		LOG_PRINT_ERROR( " A vertex must at least have a position." );
		return Result::INVALID_ARGS;
	}

	if( vertex_flags & VFF::NORMAL )
	{
		m_VertexElementOffsets[VEE::NORMAL] = (uint)cm.GetVertexElementOffset( VEE::NORMAL );
		m_VertexElementStreamIndices[VEE::NORMAL] = vertex_element_index++;
	}

	if( vertex_flags & VFF::DIFFUSE_COLOR )
	{
		m_VertexElementOffsets[VEE::DIFFUSE_COLOR] = (uint)cm.GetVertexElementOffset( VEE::DIFFUSE_COLOR );
		m_VertexElementStreamIndices[VEE::DIFFUSE_COLOR] = vertex_element_index++;
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
			m_VertexElementOffsets[VEE::TEXCOORD2_0 + i] = (uint)cm.GetVertexElementOffset( (VEE::ElementName)(VEE::TEXCOORD2_0 + i) );
			m_VertexElementStreamIndices[VEE::TEXCOORD2_0 + i] = vertex_element_index++;
		}
	}

	glBindVertexArray( 0 );

	// Copy indices
	glGenBuffers( 1, &m_IBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		cm.GetIndexSize() * cm.GetNumIndices(),
		cm.GetIndexBufferPtr(),
		GL_STATIC_DRAW
		);

	m_IndexSize = cm.GetIndexSize();

	switch( m_IndexSize )
	{
	case 1: m_IndexType = GL_UNSIGNED_BYTE;  break;
	case 2: m_IndexType = GL_UNSIGNED_SHORT; break;
//	case 4: m_IndexType = GL_UNSIGNED_INT;   break; // GL_UNSIGNED_INT == 4 bytes?
	default:
		LOG_PRINT_ERROR( " An unspoorted index size." );
		return Result::UNKNOWN_ERROR;
		break;
	}

	m_NumIndices = cm.GetNumIndices();

	return Result::SUCCESS;
}


bool GLBasicMeshImpl::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
//	if( !IsValidMeshModelArchive( archive ) )
//		return false;

	// copy the contents from the archive

	m_strFilename = filename;

	InitVerticesAndIndices( archive );

	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

	m_TriangleSets = archive.GetTriangleSet();

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
//	if( m_VAO == 0 )
//		return;

//	glBindVertexArray( m_VAO );

	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );

	const U32 vertex_flags = m_VertexFormatFlags;
	const uint vertex_size = m_VertexSize;

	if( vertex_flags & VFF::POSITION )
	{
		glEnableVertexAttribArray( m_VertexElementStreamIndices[VEE::POSITION] );
		glVertexAttribPointer( m_VertexElementStreamIndices[VEE::POSITION], 3, GL_FLOAT, GL_FALSE, vertex_size, 0 );
	}
	else
	{
		LOG_PRINT_ERROR( " A vertex must at least have a position." );
		return;
	}

	if( vertex_flags & VFF::NORMAL )
	{
		glEnableVertexAttribArray( m_VertexElementStreamIndices[VEE::NORMAL] );
		glVertexAttribPointer( m_VertexElementStreamIndices[VEE::NORMAL], 3, GL_FLOAT, GL_FALSE, vertex_size, (void *)m_VertexElementOffsets[VEE::NORMAL] );
	}

	if( vertex_flags & VFF::DIFFUSE_COLOR )
	{
		glEnableVertexAttribArray( m_VertexElementStreamIndices[VEE::DIFFUSE_COLOR] );
		glVertexAttribPointer( m_VertexElementStreamIndices[VEE::DIFFUSE_COLOR], 4, GL_FLOAT, GL_FALSE, vertex_size, (void *)m_VertexElementOffsets[VEE::DIFFUSE_COLOR] );
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
			glEnableVertexAttribArray( m_VertexElementStreamIndices[VEE::TEXCOORD2_0 + i] );
			glVertexAttribPointer( m_VertexElementStreamIndices[VEE::TEXCOORD2_0 + i], 2, GL_FLOAT, GL_FALSE, vertex_size, (void *)m_VertexElementOffsets[VEE::TEXCOORD2_0 + i] );
		}
	}

	if( m_TriangleSets.size() == 1 )
	{
		if( 0 < m_vecMaterial.size() )
			SetGLTextures( m_vecMaterial[0] );
		glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );
	}
	else if( 2 <= m_TriangleSets.size() )
	{
		int num_subsets_to_draw = (int)m_TriangleSets.size();
		for( int i=0; i<num_subsets_to_draw; i++ )
		{
			if( i < (int)m_vecMaterial.size() )
				SetGLTextures( m_vecMaterial[i] );

			const CMMA_TriangleSet& ts = m_TriangleSets[ i ];

			glDrawRangeElements(
				GL_TRIANGLES,                    // GLenum mode,
				0,                               // GLuint start,
				50000,                           // GLuint end,
				ts.m_iNumTriangles * 3,          // GLsizei count,
				m_IndexType,                     // GLenum type
				GL_INDEX_BUFFER_OFFSET( ts.m_iStartIndex * m_IndexSize ) // const GLvoid *indices
				);
		}
	}


//	glBindVertexArray( 0 );
}


/// renders subsets of the mesh with the current shader technique
/// - the same shader technique is used to render all the materials
void GLBasicMeshImpl::RenderSubsets( ShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
{
}


} // namespace amorphous
