#include "GLBasicMeshImpl.hpp"
#include "../GLExtensions.hpp"

#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "Graphics/MeshModel/MeshBone.hpp"
using namespace MeshModel;

#include "Support/Log/DefaultLog.hpp"

using namespace std;
//using namespace boost;

/*
// VBO Extension Definitions, From glext.h
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);
*/


/*
// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;					// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;					// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;					// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;			// VBO Deletion Procedure



void InitVBO()
{
	m_VBOSupported = IsExtensionSupported( "GL_ARB_vertex_buffer_object" );
	if( m_VBOSupported )
	{
		// Get Pointers To The GL Functions
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
		// Load Vertex Data Into The Graphics Card Memory
//		g_pMesh->BuildVBOs();									// Build The VBOs
	}
//#else // NO_VBOS
//	m_VBOSupported = false;
//#endif
}



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


CGLBasicMeshImpl::CGLBasicMeshImpl()
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


CGLBasicMeshImpl::CGLBasicMeshImpl( const std::string& filename )
{
	bool loaded = LoadFromFile(filename);

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load a mesh file" + filename );
	}
}


CGLBasicMeshImpl::~CGLBasicMeshImpl()
{
	Release();
}


void CGLBasicMeshImpl::Release()
{
}


bool CGLBasicMeshImpl::LoadFromFile( const std::string& filename, U32 option_flags )
{
	C3DMeshModelArchive archive;
	bool loaded = archive.LoadFromFile( filename );

	if( !loaded )
		return false;

	return LoadFromArchive( archive, filename, option_flags );
}


bool CGLBasicMeshImpl::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	// copy the contents from the archive

	m_strFilename = filename;

	BuildVBOs( archive );

/*
//	LPD3DXMESH pMesh = LoadD3DXMeshFromArchive( archive );

	if( !pMesh )
		return false;
*/
	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

/*	hr = SetAttributeTable( pMesh, archive.GetTriangleSet() );

	if( FAILED(hr) )
		return false;

	m_pMesh = pMesh;
	pMesh = NULL;

	PeekAttribTables( m_pMesh );

*/
	return true;
}


bool CGLBasicMeshImpl::CreateMesh( int num_vertices, int num_indices, U32 option_flags,
								  std::vector<D3DVERTEXELEMENT9>& vecVertexElement )
{
/*	HRESULT hr;
	hr = D3DXCreateMesh(
			num_indices / 3,         // DWORD NumFaces,
			num_vertices,            // DWORD NumVertices,
			option_flags,            // DWORD Options,
			&(vecVertexElement[0]),  // CONST LPD3DVERTEXELEMENT9 * pDeclaration,
			DIRECT3D9.GetDevice(),   // LPDIRECT3DDEVICE9 pD3DDevice,
			&m_pMesh
		);

	return (m_pMesh != NULL);
*/
	return false;
}


void CGLBasicMeshImpl::BuildVBOs( C3DMeshModelArchive& archive )
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
	glGenBuffers/*ARB*/( 1, &m_PositionBuffer );							// Get A Valid Name
	glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_PositionBuffer );			// Bind The Buffer

	// Load The Data
	glBufferData/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, num_vertices*3*sizeof(float), &(vert_set.vecPosition[0]), GL_STATIC_DRAW/*_ARB*/ );


	if( vffs & CMMA_VertexSet::VF_NORMAL )
	{
		glGenBuffers/*ARB*/( 1, &m_NormalBuffer );							// Get A Valid Name
		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_NormalBuffer );			// Bind The Buffer

		// Load The Data
		glBufferData/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, num_vertices*3*sizeof(float), &(vert_set.vecNormal[0]), GL_STATIC_DRAW/*_ARB*/ );
	}

	if( vffs & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		glGenBuffers/*ARB*/( 1, &m_DiffuseColorBuffer );							// Get A Valid Name
		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_DiffuseColorBuffer );			// Bind The Buffer

		// Load The Data
		glBufferData/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, num_vertices*4*sizeof(float), &(vert_set.vecDiffuseColor[0]), GL_STATIC_DRAW/*_ARB*/ );
	}

	if( vffs & CMMA_VertexSet::VF_2D_TEXCOORD0 )
	{
		// Generate And Bind The Texture Coordinate Buffer
		glGenBuffers/*ARB*/( 1, &m_TexCoordBuffer );							// Get A Valid Name
		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_TexCoordBuffer );		// Bind The Buffer

		// Load The Data
		glBufferData/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, num_vertices*2*sizeof(float), &(vert_set.vecTex[0][0]), GL_STATIC_DRAW/*_ARB*/ );
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

	// when drawing...

	/* setup your gl*Pointers as usual */
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer );
//	glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );

	// Only call the last glBindBuffer function if you want to disable buffering.
	// Otherwise, just leave it as it is and bind the new buffers as and when needed.
}


/// renders subsets of the mesh
/// - use different shader techniques for each material
void CGLBasicMeshImpl::RenderSubsets( CShaderManager& rShaderMgr,
		                     const std::vector<int>& vecMaterialIndex,
							 std::vector<CShaderTechniqueHandle>& vecShaderTechnique )
{
/*	CMMA_TriangleSet ts;

	// setup your gl*Pointers as usual
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBuffer );

	glDrawRangeElements(
		GL_TRIANGLES,
		0,        // start,
		100000,      // end,
		ts.m_iNumTriangles,       // *3???  // count,
		GL_UNSIGNED_SHORT,        // type
		0
		);

	glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );
*/

/*
	glDrawRangeElements(
        GL_TRIANGLES, // GLenum  	mode,
 		// GLuint  	start,
 		// GLuint  	end,
 		// GLsizei  	count,
 		// GLenum  	type,
 		// const GLvoid *  	indices);
*/
}


void CGLBasicMeshImpl::Render( CShaderManager& rShaderMgr )
{
}


void CGLBasicMeshImpl::RenderSubset( CShaderManager& rShaderMgr, int material_index )
{
}


/*
void CGLBasicMeshImpl::Render_Debug()
{
}
*/


void CGLBasicMeshImpl::Render()
{
	// Enable Pointers
	glEnableClientState( GL_VERTEX_ARRAY );	            // Enable Vertex Arrays
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );      // Enable Texture Coord Arrays
	glEnableClientState( GL_NORMAL_ARRAY );             // Enable Normal Arrays
	glEnableClientState( GL_COLOR_ARRAY );              // Enable Color Arrays

	// Set Pointers To Our Data
	if( m_VBOSupported )
	{
		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_PositionBuffer );
		glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );		// Set The Vertex Pointer To The Vertex Buffer

		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_TexCoordBuffer );
		glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );		// Set The TexCoord Pointer To The TexCoord Buffer

		// color (diffuse color?)
		glBindBuffer/*ARB*/( GL_ARRAY_BUFFER/*_ARB*/, m_DiffuseColorBuffer );
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

		// index buffer
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER/*_ARB*/, m_IndexBuffer );
	}
/*	else
	{
		glVertexPointer( 3, GL_FLOAT, 0, m_pVertices ); // Set The Vertex Pointer To Our Vertex Data
		glTexCoordPointer( 2, GL_FLOAT, 0, m_pTexCoords ); // Set The Vertex Pointer To Our TexCoord Data
	}
*/
	// FIXME: support multiple subsets
	if( 0 < m_vecMaterial.size() )
	{
		vector<CTextureHandle>& vecTex = m_vecMaterial[0].Texture;
		for( int j=0; j<(int)vecTex.size(); j++ )
			glBindTexture( GL_TEXTURE_2D, vecTex[j].GetGLTextureID() );
	}

	// Render
//	glDrawArrays( GL_TRIANGLES, 0, m_nVertexCount );	// Draw all of the triangles at once
	glDrawElements( GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_SHORT, 0 );

	// Disable Pointers
	glDisableClientState( GL_VERTEX_ARRAY );					// Disable Vertex Arrays
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );				// Disable Texture Coord Arrays

/*
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	LPD3DXMESH pMesh = m_pMesh;

	//We use only the first texture stage (stage 0)
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// color arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	// alpha arguments on texture stage 0
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	// alpha-blending settings 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Meshes are divided into subsets by materials. Render each subset in a loop
//	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTex = NULL;
    for( int i=0; i<m_NumMaterials; i++ )
    {
        // Set the material and texture for this subset
		pd3dDevice->SetMaterial( &m_pMeshMaterials[i] );

//		if( FAILED(hr) ) MessageBox(NULL, "SetMaterial() Failed", "Error", MB_OK|MB_ICONWARNING);

		if( pTex = GetTexture(i,0).GetTexture() )
		{
			// blend color & alpha of vertex & texture
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		}
		else
		{
			// no texture for this material - use only the vertx color & alpha
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		}

		pd3dDevice->SetTexture( 0, pTex );

        // Draw the mesh subset
        m_pMesh->DrawSubset( i );
    }
*/
}


/// renders subsets of the mesh with the current shader technique
/// - the same shader technique is used to render all the materials
void CGLBasicMeshImpl::RenderSubsets( CShaderManager& rShaderMgr,
		                        const std::vector<int>& vecMaterialIndex /* some option to specify handles for texture */)
{
}

#include "Graphics/OpenGL/Mesh/GLProgressiveMeshImpl.hpp"
#include "Graphics/OpenGL/Mesh/GLSkeletalMeshImpl.hpp"


CMeshImpl* CGLMeshImplFactory::CreateBasicMeshImpl() { return new CGLBasicMeshImpl; }
CMeshImpl* CGLMeshImplFactory::CreateProgressiveMeshImpl() { return new CGLProgressiveMeshImpl; }
CMeshImpl* CGLMeshImplFactory::CreateSkeletalMeshImpl() { return new CGLSkeletalMeshImpl; }
