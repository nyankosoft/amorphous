#include "GL2BasicMeshImpl.hpp"
#include "../GLExtensions.hpp"
#include "../GLGraphicsDevice.hpp" // for LOG_GL_ERROR() macro
#include "../GLTextureResourceVisitor.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Support/Log/DefaultLog.hpp"

#include "Graphics/OpenGL/Mesh/GLProgressiveMeshImpl.hpp" // Used by CGLMeshImplFactory
#include "Graphics/OpenGL/Mesh/GLSkeletalMeshImpl.hpp"	  // Used by CGLMeshImplFactory


namespace amorphous
{

using namespace std;
//using namespace boost;


#define GL_INDEX_BUFFER_OFFSET(i) ((char *)NULL + (i))


/*
// TUTORIAL
// Based Off Of Code Supplied At OpenGL.org
bool IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}
//~TUTORIAL
*/


static void SetGLTextures( MeshMaterial& mat )
{
	vector<TextureHandle>& vecTex = mat.Texture;
	for( uint j=0; j<(uint)vecTex.size(); j++ )
	{
		SetTextureGL_FFP( j, vecTex[j] );
	}
}


CGL2BasicMeshImpl::CGL2BasicMeshImpl()
:
m_NumIndices(0),

m_PositionBuffer(0),
m_NormalBuffer(0),
m_DiffuseColorBuffer(0),	
m_TexCoordBuffer(0),
m_IndexBuffer(0)
{
	m_VBOSupported = true;
}


CGL2BasicMeshImpl::CGL2BasicMeshImpl( const std::string& filename )
{
	bool loaded = LoadFromFile(filename);

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load a mesh file" + filename );
	}
}


CGL2BasicMeshImpl::~CGL2BasicMeshImpl()
{
	Release();
}


void CGL2BasicMeshImpl::Release()
{
	if( glGenBuffers )
	{
		glDeleteBuffers( 1, &m_PositionBuffer );
		glDeleteBuffers( 1, &m_NormalBuffer );
		glDeleteBuffers( 1, &m_DiffuseColorBuffer );
		glDeleteBuffers( 1, &m_TexCoordBuffer );
		glDeleteBuffers( 1, &m_IndexBuffer );
	}
	else
	{
		// ARB
		glDeleteBuffersARB( 1, &m_PositionBuffer );
		glDeleteBuffersARB( 1, &m_NormalBuffer );
		glDeleteBuffersARB( 1, &m_DiffuseColorBuffer );
		glDeleteBuffersARB( 1, &m_TexCoordBuffer );
		glDeleteBuffersARB( 1, &m_IndexBuffer );
	}

	m_NumIndices = 0;
}


bool CGL2BasicMeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( filename );

	if( !loaded )
		return false;

	return LoadFromArchive( archive, filename, option_flags );
}


bool IsValidMeshModelArchive( C3DMeshModelArchive& archive )
{
	if( !archive.GetVertexIndex().empty()
	 && 0 < archive.GetVertexSet().GetNumVertices() )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CGL2BasicMeshImpl::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	if( !IsValidMeshModelArchive( archive ) )
		return false;

	// copy the contents from the archive

	m_strFilename = filename;

	if( glGenBuffers )
		BuildVBOs( archive );
	else
		BuildVBOs_ARB( archive );

	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

	m_vecTriangleSet = archive.GetTriangleSet();

	return true;
}


bool CGL2BasicMeshImpl::CreateMesh( int num_vertices, int num_indices, U32 option_flags, U32 vertex_format_flags )
{
	LOG_PRINT_ERROR( " Not implemented." );
	return false;
}


void CGL2BasicMeshImpl::BuildVBOs( C3DMeshModelArchive& archive )
{
//	C3DMeshModelArchive archive;
	CMMA_VertexSet& vert_set = archive.GetVertexSet();
	int num_vertices = vert_set.GetNumVertices();

	const U32 vffs = vert_set.GetVertexFormat();
/*	if( vffs &  )
	{

	}
*/
	// Generate and bind the vertex buffer
	glGenBuffers( 1, &m_PositionBuffer );							// Get A Valid Name
	glBindBuffer( GL_ARRAY_BUFFER, m_PositionBuffer );			// Bind The Buffer

	// Load The Data
	glBufferData( GL_ARRAY_BUFFER, num_vertices*3*sizeof(float), &(vert_set.vecPosition[0]), GL_STATIC_DRAW );


	if( vffs & CMMA_VertexSet::VF_NORMAL )
	{
		glGenBuffers( 1, &m_NormalBuffer );							// Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_NormalBuffer );			// Bind The Buffer

		// Load The Data
		glBufferData( GL_ARRAY_BUFFER, num_vertices*3*sizeof(float), &(vert_set.vecNormal[0]), GL_STATIC_DRAW );
	}

	if( vffs & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		glGenBuffers( 1, &m_DiffuseColorBuffer );							// Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_DiffuseColorBuffer );			// Bind The Buffer

		// Load The Data
		glBufferData( GL_ARRAY_BUFFER, num_vertices*4*sizeof(float), &(vert_set.vecDiffuseColor[0]), GL_STATIC_DRAW );
	}

	if( vffs & CMMA_VertexSet::VF_2D_TEXCOORD0
	 && !vert_set.vecTex[0].empty() )
	{
		// Generate And Bind The Texture Coordinate Buffer
		glGenBuffers( 1, &m_TexCoordBuffer );							// Get A Valid Name
		glBindBuffer( GL_ARRAY_BUFFER, m_TexCoordBuffer );		// Bind The Buffer

		// Load The Data
		glBufferData( GL_ARRAY_BUFFER, num_vertices*2*sizeof(float), &(vert_set.vecTex[0][0]), GL_STATIC_DRAW );
	}

	// Mesh data has been copied to the graphics memory

	m_NumIndices = (int)archive.GetVertexIndex().size();

	// index buffer
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

	// uint indices
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_NumIndices, &(archive.GetVertexIndex()[0]), GL_STATIC_DRAW);

	// ushort indices
	vector<ushort> vecUShortIndex;
	vecUShortIndex.resize( m_NumIndices );
	for( int i=0; i<m_NumIndices; i++ )
		vecUShortIndex[i] = (ushort)archive.GetVertexIndex()[i];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort) * m_NumIndices, &(vecUShortIndex[0]), GL_STATIC_DRAW);

	m_vecIndex = vecUShortIndex;

	// when drawing...

	/* setup your gl*Pointers as usual */
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer );
//	glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );

	// Only call the last glBindBuffer function if you want to disable buffering.
	// Otherwise, just leave it as it is and bind the new buffers as and when needed.
}


void CGL2BasicMeshImpl::BuildVBOs_ARB( C3DMeshModelArchive& archive )
{
//	C3DMeshModelArchive archive;
	CMMA_VertexSet& vert_set = archive.GetVertexSet();
	int num_vertices = vert_set.GetNumVertices();

	const U32 vffs = vert_set.GetVertexFormat();
/*	if( vffs &  )
	{

	}
*/
	// Generate and bind the vertex buffer
	glGenBuffersARB( 1, &m_PositionBuffer );							// Get A Valid Name
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_PositionBuffer );			// Bind The Buffer

	// Load The Data
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, num_vertices*3*sizeof(float), &(vert_set.vecPosition[0]), GL_STATIC_DRAW_ARB );


	if( vffs & CMMA_VertexSet::VF_NORMAL )
	{
		glGenBuffersARB( 1, &m_NormalBuffer );							// Get A Valid Name
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_NormalBuffer );			// Bind The Buffer

		// Load The Data
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, num_vertices*3*sizeof(float), &(vert_set.vecNormal[0]), GL_STATIC_DRAW_ARB );
	}

	if( vffs & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		glGenBuffersARB( 1, &m_DiffuseColorBuffer );							// Get A Valid Name
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DiffuseColorBuffer );			// Bind The Buffer

		// Load The Data
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, num_vertices*4*sizeof(float), &(vert_set.vecDiffuseColor[0]), GL_STATIC_DRAW_ARB );
	}

	if( vffs & CMMA_VertexSet::VF_2D_TEXCOORD0 )
	{
		// Generate And Bind The Texture Coordinate Buffer
		glGenBuffersARB( 1, &m_TexCoordBuffer );							// Get A Valid Name
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_TexCoordBuffer );		// Bind The Buffer

		// Load The Data
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, num_vertices*2*sizeof(float), &(vert_set.vecTex[0][0]), GL_STATIC_DRAW_ARB );
	}

	// Mesh data has been copied to the graphics memory

	m_NumIndices = (int)archive.GetVertexIndex().size();

	// index buffer
	glGenBuffersARB(1, &m_IndexBuffer);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer);

	// uint indices
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_NumIndices, &(archive.GetVertexIndex()[0]), GL_STATIC_DRAW);

	// ushort indices
	vector<ushort> vecUShortIndex;
	vecUShortIndex.resize( m_NumIndices );
	for( int i=0; i<m_NumIndices; i++ )
		vecUShortIndex[i] = (ushort)archive.GetVertexIndex()[i];
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(ushort) * m_NumIndices, &(vecUShortIndex[0]), GL_STATIC_DRAW);

	// when drawing...

	/* setup your gl*Pointers as usual */
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer );
//	glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );

	// Only call the last glBindBuffer function if you want to disable buffering.
	// Otherwise, just leave it as it is and bind the new buffers as and when needed.
}


/// renders subsets of the mesh
/// - use different shader techniques for each material
void CGL2BasicMeshImpl::RenderSubsets( ShaderManager& rShaderMgr,
		                     const std::vector<int>& vecMaterialIndex,
							 std::vector<ShaderTechniqueHandle>& vecShaderTechnique )
{
	if( !glDrawRangeElements )
		return;

	if( m_vecIndex.empty() )
		return;

	const int num_subsets_to_draw = (int)vecMaterialIndex.size();
	for( int i=0; i<num_subsets_to_draw; i++ )
	{
		int subset_index = vecMaterialIndex[i];
		if( subset_index < 0 || (int)m_vecTriangleSet.size() <= subset_index )
			continue;

		const CMMA_TriangleSet& ts = m_vecTriangleSet[ subset_index ];

		if( (int)m_vecIndex.size() < ts.m_iStartIndex + ts.m_iNumTriangles * 3 )
			continue;

		if( subset_index < (int)m_vecMaterial.size() )
			SetGLTextures( m_vecMaterial[subset_index] );

		// setup your gl*Pointers as usual
//		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer ); // Is this eeded? pointer to the indices is set in glDrawRangeElements() 

		LOG_GL_ERROR( "glBindBuffer() failed." );

		glDrawRangeElements(
			GL_TRIANGLES,                    // GLenum mode,
			0,                               // GLuint start,
			50000,                           // GLuint end,
			ts.m_iNumTriangles * 3,          // GLsizei count,
			GL_UNSIGNED_SHORT,               // GLenum type
			GL_INDEX_BUFFER_OFFSET( ts.m_iStartIndex * sizeof(unsigned short) ) // const GLvoid *indices
			);

		LOG_GL_ERROR( "glDrawRangeElements() failed." );

//		glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );

		LOG_GL_ERROR( "glBindBuffer() for release failed." );
	}
}


void CGL2BasicMeshImpl::Render( ShaderManager& rShaderMgr )
{
	rShaderMgr.Begin();

	Render();
}


void CGL2BasicMeshImpl::RenderSubset( ShaderManager& rShaderMgr, int material_index )
{
}


/*
void CGL2BasicMeshImpl::Render_Debug()
{
}
*/


void CGL2BasicMeshImpl::Render()
{
	// Enable Pointers
	glEnableClientState( GL_VERTEX_ARRAY );	            // Enable Vertex Arrays
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );      // Enable Texture Coord Arrays
	glEnableClientState( GL_NORMAL_ARRAY );             // Enable Normal Arrays
	glEnableClientState( GL_COLOR_ARRAY );              // Enable Color Arrays

	// Set Pointers To Our Data
	if( m_VBOSupported )
	{
		if( glGenBuffers )
	  {
		glBindBuffer( GL_ARRAY_BUFFER, m_PositionBuffer );
		glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );		// Set The Vertex Pointer To The Vertex Buffer

		glBindBuffer( GL_ARRAY_BUFFER, m_TexCoordBuffer );
		glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );		// Set The TexCoord Pointer To The TexCoord Buffer

		// color (diffuse color?)
		glBindBuffer( GL_ARRAY_BUFFER, m_DiffuseColorBuffer );
		glColorPointer( 4,             // GLint size,
                        GL_FLOAT,      // GLenum type,
                        0,             // GLsizei stride, ( 0 == tightly packed color array)
                        (char *) NULL  // const GLvoid *pointer
                      );

		// vertex normal?
		if( m_NormalBuffer != 0 )
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_NormalBuffer );
			glNormalPointer( GL_FLOAT,     // GLenum type,
							 0,            // GLsizei	stride,
							 (char *) NULL // const GLvoid *pointer
						   );
		}

		// index buffer
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
	  }
	  else
		  BindBuffers_ARB();
	}
/*	else
	{
		glVertexPointer( 3, GL_FLOAT, 0, m_pVertices ); // Set The Vertex Pointer To Our Vertex Data
		glTexCoordPointer( 2, GL_FLOAT, 0, m_pTexCoords ); // Set The Vertex Pointer To Our TexCoord Data
	}
*/

	if( m_vecTriangleSet.size() == 1 )
	{
		if( 0 < m_vecMaterial.size() )
			SetGLTextures( m_vecMaterial[0] );

		// Render
	//	glDrawArrays( GL_TRIANGLES, 0, m_nVertexCount );	// Draw all of the triangles at once
		glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );
	}
	else if( 1 < m_vecTriangleSet.size() )
	{
		vector<ShaderTechniqueHandle> shader_techniques( m_vecTriangleSet.size() );
		RenderSubsets( FixedFunctionPipelineManager(), m_vecFullMaterialIndices, shader_techniques );
	}

	// Disable Pointers
	glDisableClientState( GL_VERTEX_ARRAY );					// Disable Vertex Arrays
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );				// Disable Texture Coord Arrays
	glDisableClientState( GL_NORMAL_ARRAY );                    // Disable Normal Arrays
	glDisableClientState( GL_COLOR_ARRAY );                     // Disable Color Arrays

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}


void CGL2BasicMeshImpl::BindBuffers_ARB()
{
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_PositionBuffer );
	glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );		// Set The Vertex Pointer To The Vertex Buffer

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_TexCoordBuffer );
	glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );		// Set The TexCoord Pointer To The TexCoord Buffer

	// color (diffuse color?)
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DiffuseColorBuffer );
	glColorPointer( 4,             // GLint size,
                    GL_FLOAT,      // GLenum type,
                    0,             // GLsizei stride, ( 0 == tightly packed color array)
                    (char *) NULL  // const GLvoid *pointer
                  );

	// vertex normal?
	glNormalPointer( GL_FLOAT,     // GLenum type,
                     0,            // GLsizei	stride,
                     (char *) NULL // const GLvoid *pointer
                   );
}


/// renders subsets of the mesh with the current shader technique
/// - the same shader technique is used to render all the materials
void CGL2BasicMeshImpl::RenderSubsets( ShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
{
}


MeshImpl* CGLMeshImplFactory::CreateBasicMeshImpl() { return new CGL2BasicMeshImpl; }
MeshImpl* CGLMeshImplFactory::CreateProgressiveMeshImpl() { return new CGLProgressiveMeshImpl; }
MeshImpl* CGLMeshImplFactory::CreateSkeletalMeshImpl() { return new CGLSkeletalMeshImpl; }


} // namespace amorphous
