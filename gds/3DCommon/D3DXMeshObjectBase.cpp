#include "D3DXMeshObjectBase.h"

#include "../base.h"
#include "3DCommon/FVF_BumpVertex.h"
#include "3DCommon/FVF_BumpWeightVertex.h"
#include "3DCommon/FVF_ColorVertex.h"
#include "3DCommon/FVF_TextureVertex.h"
#include "3DCommon/FVF_NormalVertex.h"
#include "3DCommon/FVF_WeightVertex.h"
#include "3DCommon/FVF_ShadowVertex.h"
#include "3DCommon/Camera.h"
#include "3DCommon/Shader/ShaderManager.h"

#include "Support/StringAux.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Profile.h"
#include "Support/memory_helpers.h"
#include "Support/Macro.h"
#include "Support/fnop.h"
using namespace fnop;

#include <direct.h>



/**
Terms
----------------------------
- subset, material, and surface are used interchangeably

Tips
----------------------------
- Idally, A mesh should has a single material
  - Switching materials comes with certain performance penalty
    - Consider using the following tricks to texture to achieve different surface properties
      with a single material
      - store per-pixel specularity / glossines to texture, esp. in alpha channel
      - vertex diffuse colors

Details
----------------------------
- 2 categorizations for rendering modes of the mesh
  1. single / non-single shader technique
    - single shader technique
      - set shader technique and the call Render()
    - individual shader techniques for each material
  2. render all materials or some of the mesh materials
    - all materials
	  - Render()
	- some of the materials
	  - RenderSubsets()
*/

void CD3DXMeshObjectBase::Release()
{
	m_vecAABB.resize(0);

	m_IsVisible.resize(0);

	SafeDeleteArray( m_paVertexElements );

	SAFE_RELEASE( m_pVertexDecleration );

	SafeDeleteArray( m_pMeshMaterials );

	for( int i=0; i<m_NumMaterials; i++ )
	{
		// texture handles are released automatically
	}
}


bool CD3DXMeshObjectBase::LoadFromFile( const std::string& filename, U32 opiton_flags )
{
	Release();

	bool loaded = false;

	if( 0 < filename.length() )
	{
		m_strFilename = filename;
	}
	else
		return false;

	if( filename.substr(filename.length()-2,2) == ".x" )
	{
		// .x file
		// - load mesh and store it to CD3DXMeshObject::m_pMesh
		// - load material(s) and store them to CD3DXMeshObjectBase::m_pMeshMaterials
		HRESULT hr = LoadFromXFile( filename );

		loaded = SUCCEEDED(hr) ? true : false;
	}
	else
	{
		// file extension is not ".x" - load as a mesh archive
		C3DMeshModelArchive archive;
		bool b = archive.LoadFromFile( filename );

		if( !b )
			return false;

		loaded = LoadFromArchive( archive, filename, opiton_flags );
	}

//	if( loaded )
//		m_strFilename = filename;

    return loaded;
}


void CD3DXMeshObjectBase::InitMaterials( int num_materials )
{
	m_NumMaterials = num_materials;

	SafeDeleteArray( m_pMeshMaterials );
	m_pMeshMaterials = new D3DMATERIAL9[m_NumMaterials];

	m_vecMaterial.resize( m_NumMaterials );

	// create list of material indices
	// - used by Render() to render all the materials in the default order
	m_vecFullMaterialIndices.resize( m_NumMaterials );
	for( int i=0; i<m_NumMaterials; i++ )
		m_vecFullMaterialIndices[i] = i;
}


HRESULT CD3DXMeshObjectBase::LoadMaterialsFromArchive( C3DMeshModelArchive& rArchive, U32 option_flags )
{
	m_AABB = rArchive.GetAABB();
	vector<CMMA_Material>& rvecSrcMaterial = rArchive.GetMaterial();

	InitMaterials( (int)rvecSrcMaterial.size() );

	// load AABBs that represent bounding boxes for each triangle set
	m_vecAABB.resize(m_NumMaterials);
	for( int mat=0; mat < m_NumMaterials; mat++ )
		m_vecAABB[mat] = rArchive.GetTriangleSet()[mat].m_AABB;

	// all triangle sets are set visible by default
	m_IsVisible.resize( m_NumMaterials + 1, 1 );

	string tex_filename;

	// save the current working directory
//	string orig_dir = fnop::get_cwd();
	// set the abs path of the mesh file as the working directory
//	fnop::set_wd( fnop::get_path( m_strFilename ) );

	m_vecMaterial.resize( m_NumMaterials );

	// push the current working directory to the directory stack
	dir_stack DirStack;
	DirStack.setdir( get_path(m_strFilename) );

	bool loaded = false;
	for( int i=0; i<m_NumMaterials; i++ )
	{
		memset( &m_pMeshMaterials[i], 0, sizeof(D3DMATERIAL9) );

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

		// texture(s)

		const size_t num_textures = rvecSrcMaterial[i].vecTexture.size();

		m_vecMaterial[i].Texture.resize( num_textures );
		m_vecMaterial[i].TextureDesc.resize( num_textures );

		for( size_t tex=0; tex<num_textures; tex++ )
		{
			tex_filename = rvecSrcMaterial[i].vecTexture[tex].strFilename;
			if( 0 < tex_filename.length() )
			{
				string tex_filepath = fnop::get_cwd() + "/" + tex_filename;
				m_vecMaterial[i].TextureDesc[tex].ResourcePath = tex_filepath;

				if( !(option_flags & MeshLoadOption::DO_NOT_LOAD_TEXTURES) )
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


	// restore the previous working directory
//	fnop::set_wd( orig_dir );

	// back to the original working directory
	DirStack.prevdir();

	return S_OK;
}


#define NUM_MAX_VERTEX_ELEMENTS 64


bool CD3DXMeshObjectBase::CreateVertexDeclaration()
{
	if( m_paVertexElements )
	{
		HRESULT hr = DIRECT3D9.GetDevice()->CreateVertexDeclaration( m_paVertexElements, &m_pVertexDecleration );
		return SUCCEEDED(hr) ? true : false;
	}
	else
		return false;
}


LPD3DXMESH CD3DXMeshObjectBase::LoadD3DXMeshFromArchive( C3DMeshModelArchive& archive )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	void *pDestVBData, *pSrcVBData = NULL;

	// load vertex data from the rArchive
	// - Need to do this before D3DXCreateMesh() to determine vertex declarations
	LoadVertices( pSrcVBData, archive );

	// vertex elements array has been updated in LoadVertices() - create the vertex declaration
	CreateVertexDeclaration();

	DWORD num_vertices = (DWORD)archive.GetVertexSet().GetNumVertices();


	LPD3DXMESH pMesh = NULL;

	{
		LOG_SCOPE( "D3DXCreateMesh" );

	hr = D3DXCreateMesh( (DWORD)archive.GetVertexIndex().size() / 3,
		                 num_vertices,
//						 0,
						 D3DXMESH_MANAGED,
						 m_paVertexElements,
						 pd3dDev,
						 &pMesh );
	}

	if( hr == E_OUTOFMEMORY )
	{
		LOG_PRINT_ERROR( " - D3DXCreateMesh() failed. (E_OUTOFMEMORY)" );
	}

	if( FAILED(hr) )
		return NULL;

	// copy vertices to vertex buffer
	hr = pMesh->LockVertexBuffer( 0, &pDestVBData );

	memcpy( pDestVBData, pSrcVBData, m_iVertexSize * num_vertices );

	pMesh->UnlockVertexBuffer();

	SafeDeleteArray( pSrcVBData );

	FillIndexBuffer( pMesh, archive );

	return pMesh;
}


static inline D3DVERTEXELEMENT9 D3DVertexElement( 
    WORD    Stream,     // Stream index
    WORD    Offset,     // Offset in the stream in bytes
    BYTE    Type,       // Data type
    BYTE    Method,     // Processing method
    BYTE    Usage,      // Semantics
    BYTE    UsageIndex  // Semantic index
	)
{
	D3DVERTEXELEMENT9 elem;
	elem.Stream     = Stream;
	elem.Offset     = Offset;
	elem.Type       = Type;
	elem.Method     = Method;
	elem.Usage      = Usage;
	elem.UsageIndex = UsageIndex;
	return elem;
}

/**
 \param pVBData [in,out] reference to the pointer that points to the address of the buffer to hold vertex data
 \param pVertexElements [in,out] pointer to the buffer for vertex element declarations

*/
void CD3DXMeshObjectBase::LoadVertices( void*& pVBData,
								        C3DMeshModelArchive& archive )
{
	CMMA_VertexSet& rVertexSet = archive.GetVertexSet();

	int i;
	const int iNumVertices = rVertexSet.GetNumVertices();

	int vert_size = 0;
	int num_vertex_decs = 0;
	D3DVERTEXELEMENT9 aVertexDeclaration[NUM_MAX_VERTEX_ELEMENTS];
	size_t pos_offset = 0, normal_offset = 0, binormal_offset = 0, tangent_offset = 0;
	size_t weight_index_offset = 0, weight_offset = 0;
	size_t color_offset = 0;
	size_t tex_offset[] = {0,0,0,0};
	const uint vert_fmt_flags = rVertexSet.GetVertexFormat();

	if( vert_fmt_flags & CMMA_VertexSet::VF_POSITION )
	{
		pos_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 );
		vert_size += sizeof(float) * 3;
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_NORMAL )
	{
		normal_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 );
		vert_size += sizeof(float) * 3;
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_BUMPMAP )
	{
		// needs binormal and tangent
		tangent_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 );
		vert_size += sizeof(float) * 3;
		binormal_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,0 );
		vert_size += sizeof(float) * 3;
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		color_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0 );
		vert_size += 4;
	}

	uint tex_flag[] = {
		CMMA_VertexSet::VF_2D_TEXCOORD0,
		CMMA_VertexSet::VF_2D_TEXCOORD1,
		CMMA_VertexSet::VF_2D_TEXCOORD2,
		CMMA_VertexSet::VF_2D_TEXCOORD3 };

	BYTE usage_index = 0;
	for( i=0; i<numof(tex_flag); i++ )
	{
		if( vert_fmt_flags & tex_flag[i] )
		{
			tex_offset[i] = vert_size;
			aVertexDeclaration[num_vertex_decs++]
			= D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     usage_index );
			vert_size += sizeof(float) * 2;
			usage_index++;
		}
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_WEIGHT )
	{
		weight_index_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 );
		vert_size += 4;
		weight_offset = vert_size;
		aVertexDeclaration[num_vertex_decs++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0 );
		vert_size += sizeof(float) * 4;
	}

	// add terminating element to the vertex declaration
	D3DVERTEXELEMENT9 end_element[] = { D3DDECL_END() };
	aVertexDeclaration[num_vertex_decs++] = end_element[0];

//	DIRECT3D9.GetDevice()->CreateVertexDeclaration( aVertexDeclaration, &m_pVertexDecleration );

	// the size of the each vertex (in bytes)
	m_iVertexSize = vert_size;

	// allocate memory to store all the vertices of the specified format
	unsigned char *pBuffer = new unsigned char [vert_size * iNumVertices];

	// copy the vertices to the buffer
	if( vert_fmt_flags & CMMA_VertexSet::VF_POSITION )
	{
		for( i=0; i<iNumVertices; i++ )
			memcpy( pBuffer + i * vert_size + pos_offset, &rVertexSet.vecPosition[i], sizeof(Vector3) );
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_NORMAL )
	{
		for( i=0; i<iNumVertices; i++ )
			memcpy( pBuffer + i * vert_size + normal_offset, &rVertexSet.vecNormal[i], sizeof(Vector3) );
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_BUMPMAP )
	{
		for( i=0; i<iNumVertices; i++ )
		{
			memcpy( pBuffer + i * vert_size + tangent_offset,  &rVertexSet.vecTangent[i],  sizeof(Vector3) );
			memcpy( pBuffer + i * vert_size + binormal_offset, &rVertexSet.vecBinormal[i], sizeof(Vector3) );
		}
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_DIFFUSE_COLOR )
	{
		for( i=0; i<iNumVertices; i++ )
		{
			DWORD color = rVertexSet.vecDiffuseColor[i].GetARGB32();
			memcpy( pBuffer + i * vert_size + color_offset, &color, sizeof(DWORD) );
		}
	}

	for( int t=0; t<numof(tex_flag); t++ )
	{
		if( rVertexSet.vecTex.size() <= t )
			break;

		if( vert_fmt_flags & tex_flag[t] )
		{
			for( i=0; i<iNumVertices; i++ )
				memcpy( pBuffer + i * vert_size + tex_offset[t], &rVertexSet.vecTex[t][i], sizeof(float) * 2 );
		}
	}

	if( vert_fmt_flags & CMMA_VertexSet::VF_WEIGHT )
	{
		unsigned char Indices[4];
		float weights[4];
		for( i=0; i<iNumVertices; i++ )
		{
			rVertexSet.GetBlendMatrixIndices_4Bytes( i, Indices );
			DWORD indices = D3DCOLOR_ARGB( Indices[0], Indices[1], Indices[2], Indices[3] );
			rVertexSet.GetBlendMatrixWeights_4Floats( i, weights );

			memcpy( pBuffer + i * vert_size + weight_index_offset,  &indices,  sizeof(DWORD) );
			memcpy( pBuffer + i * vert_size + weight_offset,        weights,   sizeof(float) * 4 );
		}
	}

	pVBData = pBuffer;

	// update vertex elements
	SafeDeleteArray( m_paVertexElements );
	m_paVertexElements = new D3DVERTEXELEMENT9 [num_vertex_decs];
	memcpy( m_paVertexElements, aVertexDeclaration, sizeof(D3DVERTEXELEMENT9) * num_vertex_decs );
}


HRESULT CD3DXMeshObjectBase::LoadMaterials( D3DXMATERIAL* d3dxMaterials, int num_materials )
{
	// allocate material buffers, etc.
	InitMaterials( num_materials );

	// Change the current directory to the mesh's directory so we can
	// find the textures.
	size_t LastSlashPos = m_strFilename.find( "\\" );
	if( LastSlashPos == std::string::npos )
		LastSlashPos = m_strFilename.find( "/" );

	std::string current_dir;
	if( LastSlashPos != std::string::npos )
		current_dir = m_strFilename.substr( 0, LastSlashPos );
	else
		current_dir = "./";

	char str[MAX_PATH], strCWD[MAX_PATH];
	getcwd( strCWD, MAX_PATH );
	chdir( current_dir.c_str() );

	// Copy the materials and load the textures
	for( int i = 0; i < m_NumMaterials; i++ )
	{
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		// Find the path to the texture and create that texture
//		MultiByteToWideChar( CP_ACP, 0, d3dxMaterials[i].pTextureFilename, -1, str, MAX_PATH );

		m_vecMaterial[i].Texture.resize(1);

		if( d3dxMaterials[i].pTextureFilename )
		{
			strcpy( str, d3dxMaterials[i].pTextureFilename );
			str[MAX_PATH - 1] = L'\0';

			m_vecMaterial[i].Texture[0].Load( str );

//			m_pMeshTextures[i].filename = str;
//			m_pMeshTextures[i].Load();
		}
	}

	// Restore the current directory
//	SetCurrentDirectory( strCWD );
	chdir( strCWD );

	return S_OK;
}


void CD3DXMeshObjectBase::PeekAttribTables( LPD3DXBASEMESH pMesh )
{
	D3DXATTRIBUTERANGE attrib_table[128];
	DWORD attrib_table_size;
	pMesh->GetAttributeTable( attrib_table, &attrib_table_size );
}


HRESULT CD3DXMeshObjectBase::LoadD3DXMeshAndMaterialsFromXFile( const std::string& xfilename,
															   LPD3DXMESH& rpMesh,
															   LPD3DXBUFFER& rpAdjacencyBuffer )
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;

	// temporary buffer to hold the materials of the model
	// each polygon has 2 attributes, 'material of the surface' and 'texture'
	// These 2 attributes are stored in this buffer.
    LPD3DXBUFFER pD3DXMtrlBuffer;

	LPD3DXMESH pMesh;

	DWORD num_materials = 0;

    // Load the mesh from the specified file
	if( FAILED( D3DXLoadMeshFromX( xfilename.c_str(), D3DXMESH_MANAGED, 
                                   pd3dDevice, &rpAdjacencyBuffer, 
                                   &pD3DXMtrlBuffer, NULL, 
								   &num_materials,
								   &pMesh ) ) )
    {
		pMesh = NULL;
        return E_FAIL;
    }

	// check the attribute tables (for debugging)
	PeekAttribTables( pMesh );

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	hr = LoadMaterials( d3dxMaterials, (int)num_materials );

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

	return hr;
}


/// This doesn't have to be a member of CD3DXMeshObjectBase, does it?
/// - CD3DXMeshObjectBase may want right to choose index format (U16 / U32). This is always U16 for now.
///   - Then, the argument type has to be void
bool CD3DXMeshObjectBase::LoadIndices( unsigned short*& pIBData, C3DMeshModelArchive& archive )
{
	if( !pIBData )
		return false;

	const vector<unsigned int>& rvecVertexIndex = archive.GetVertexIndex();
	const int num_indices = archive.GetNumVertexIndices();

	pIBData = new unsigned short [num_indices];

	for( int i=0; i<num_indices; i++ )
		pIBData[i] = (unsigned short)rvecVertexIndex[i];

	return true;
}


bool CD3DXMeshObjectBase::FillIndexBuffer( LPD3DXMESH pMesh, C3DMeshModelArchive& archive )
{
	if( !pMesh )
		return false;

	const vector<unsigned int>& rvecVertexIndex = archive.GetVertexIndex();
	const int num_indices = archive.GetNumVertexIndices();

	unsigned short* pusIBData;
	HRESULT hr;

	// copy the index data to the index buffer
    if( FAILED( hr = pMesh->LockIndexBuffer( 0, (VOID**)&pusIBData ) ) )
		return false;

	for( int i=0; i<num_indices; i++ )
		pusIBData[i] = (unsigned short)rvecVertexIndex[i];

	hr = pMesh->UnlockIndexBuffer();

	return SUCCEEDED(hr) ? true : false;
}


HRESULT CD3DXMeshObjectBase::SetAttributeTable( LPD3DXMESH pMesh,
											     const vector<CMMA_TriangleSet>& vecTriangleSet )
{
	// convert triangle sets to attribute ranges
	D3DXATTRIBUTERANGE *paAttribTable = new D3DXATTRIBUTERANGE [ m_NumMaterials ];

	for( int i=0; i<m_NumMaterials; i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i];

		paAttribTable[i].AttribId		= i;
//		paAttribTable[i].AttribId		= i + 1;
//		paAttribTable[i].FaceStart		= triangle_set.m_iStartIndex;
		paAttribTable[i].FaceStart		= triangle_set.m_iStartIndex / 3;
		paAttribTable[i].FaceCount		= triangle_set.m_iNumTriangles;
		paAttribTable[i].VertexStart	= triangle_set.m_iMinIndex;
		paAttribTable[i].VertexCount	= triangle_set.m_iNumVertexBlocksToCover;
	}

	HRESULT hr = pMesh->SetAttributeTable( paAttribTable, m_NumMaterials );

	SafeDeleteArray( paAttribTable );

	// set attribute IDs for each face
	DWORD *pdwBuffer = NULL;
	pMesh->LockAttributeBuffer( 0, &pdwBuffer );
	DWORD face = 0;
	for( int i=0; i<m_NumMaterials; i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i];

		DWORD face_start = triangle_set.m_iStartIndex / 3;
		DWORD num_faces = triangle_set.m_iNumTriangles;
		for( face=face_start; face<face_start + num_faces; face++ )
		{
			pdwBuffer[face] = i;
		}
	}

	pMesh->UnlockAttributeBuffer();

	return S_OK;
}


void CD3DXMeshObjectBase::UpdateVisibility( const CCamera& cam )
{
	if( !cam.ViewFrustumIntersectsWith( m_AABB ) )
	{
		// the entire mesh is outside the view frustum
		m_IsVisible.assign( GetNumMaterials() + 1, 0 );
		return;
	}
	else
		m_IsVisible[GetNumMaterials()] = 1;

	const int num_materials = GetNumMaterials();
	for( int mat=0; mat<num_materials; mat++ )
	{
		if( cam.ViewFrustumIntersectsWith( m_vecAABB[mat] ) )
			m_IsVisible[mat] = 1;
		else
			m_IsVisible[mat] = 0;
	}
}


void CD3DXMeshObjectBase::RenderSubsets( CShaderManager& rShaderMgr,
										 const std::vector<int>& vecMaterialIndex )
{
	PROFILE_FUNCTION();

	vector<CShaderTechniqueHandle> empty_shader_technique_array;
	RenderSubsets( rShaderMgr, vecMaterialIndex, empty_shader_technique_array );
}


void CD3DXMeshObjectBase::RenderSubsets( CShaderManager& rShaderMgr,
										 const std::vector<int>& vecMaterialIndex,
										 std::vector<CShaderTechniqueHandle>& vecShaderTechnique )
{
	bool single_shader_technique = ( vecShaderTechnique.size() == 0 ) ? true : false;

	if( m_bViewFrustumTest && !IsMeshVisible() )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
	if( !pEffect )
		return;

	UINT cPasses;
//	pEffect->Begin( &cPasses, 0 );

	LPD3DXBASEMESH pMesh = GetBaseMesh();
	if( !pMesh )
		return;

	hr = pd3dDevice->SetVertexDeclaration( GetVertexDeclaration() );

	// Meshes are divided into subsets by materials. Render each subset in a loop
//	const int num_materials = GetNumMaterials();
//	for( int mat=0; mat<num_materials; mat++ )
	const size_t num_materials_to_render = vecMaterialIndex.size();
	for( size_t i=0; i<num_materials_to_render; i++ )
	{
		int mat = vecMaterialIndex[i];

		if( m_bViewFrustumTest && !IsMeshVisible(mat) )
			continue;

		if( !single_shader_technique )
			rShaderMgr.SetTechnique( vecShaderTechnique[i] );

		const int num_textures_for_material = (int)m_vecMaterial[mat].Texture.size();
		for( int tex=0; tex<num_textures_for_material; tex++ )
			rShaderMgr.SetTexture( tex, GetTexture( mat, tex ) );

		pEffect->CommitChanges();

		pEffect->Begin( &cPasses, 0 );
		for( UINT p = 0; p < cPasses; ++p )
		{
			pEffect->BeginPass( p );

			// Draw the mesh subset
			pMesh->DrawSubset( mat );

			pEffect->EndPass();
		}
		pEffect->End();
	}

//	pEffect->End();
}

/*
// called by the render thread
// create mesh
// lock VB & IB?
// Vertex elements must be retrieved before this function is called and set to m_aVertexElements,
// which means CD3DXMeshObjectBase::LoadVertices() must also be called before this,
// which, in turn, means vertex buffer content gets loaded to memory before this because
// LoadVertices() load vertex data to memory as well as determining vertex elements
bool CD3DXMeshObjectBase::CreateMesh( vb_size, ib_size )
{
	LPD3DXBASEMESH pMesh;

	D3DVERTEXELEMENT9 m_aVertexElements; // must be ready by the time this function is called by the render thread

	archive;
	hr = D3DXCreateMesh( (DWORD)archive.GetVertexIndex().size() / 3,
		                 archive.GetVertexSet().GetNumVertices(),//num_vertices,
//						 0,
						 D3DXMESH_MANAGED,
						 m_aVertexElements,
						 DIRECT3D9.GetDevice(),
						 &pMesh );

	hr = pMesh->LockVertexBuffer( 0, &pDestVBData );

    if( FAILED( hr = pMesh->LockIndexBuffer( 0, (VOID**)&pusIBData ) ) )
		return hr;

}
*/

HRESULT CD3DXMeshObjectBase::CreateLocalBoundingSphereFromD3DXMesh( LPD3DXMESH pMesh )
{
	// Lock the vertex buffer, to generate a simple bounding sphere
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	pVertexBuffer = NULL;
	HRESULT hr;
	void* pVertices;
	hr = pMesh->GetVertexBuffer( &pVertexBuffer );
	if( FAILED(hr) )
		return hr;

	hr = pVertexBuffer->Lock( 0, 0, &pVertices, D3DLOCK_NOSYSLOCK );
	if( FAILED(hr) )
		return hr;

	D3DXVECTOR3 m_vObjectCenter;
	hr = D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, pMesh->GetNumVertices(),
									D3DXGetFVFVertexSize(pMesh->GetFVF()),
									&m_vObjectCenter, &m_LocalShpere.radius );
	pVertexBuffer->Unlock();
	SAFE_RELEASE( pVertexBuffer );

	m_LocalShpere.vCenter = m_vObjectCenter;

	if( FAILED(hr) )
		return hr;

/*	{
		D3DXMatrixTranslation( &m_mWorldCenter, -m_vObjectCenter.x,
												-m_vObjectCenter.y,
												-m_vObjectCenter.z );
		D3DXMATRIXA16 m;
		D3DXMatrixScaling( &m, 2.0f / m_fObjectRadius,
							2.0f / m_fObjectRadius,
							2.0f / m_fObjectRadius );
		D3DXMatrixMultiply( &m_mWorldCenter, &m_mWorldCenter, &m );
	}
*/

	return hr;
}

bool CD3DXMeshObjectBase::LockVertexBuffer( void*& pLockedVertexBuffer )
{
	if( GetBaseMesh() )
	{
		HRESULT hr = GetBaseMesh()->LockVertexBuffer( 0, &pLockedVertexBuffer );
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


bool CD3DXMeshObjectBase::LockIndexBuffer( void*& pLockedIndexBuffer )
{
	if( GetBaseMesh() )
	{
		HRESULT hr = GetBaseMesh()->LockIndexBuffer( 0, &pLockedIndexBuffer );
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


bool CD3DXMeshObjectBase::LockAttributeBuffer( DWORD*& pLockedAttributeBuffer )
{
	if( GetMesh() ) // need ID3DXMesh - LockAttributeBuffer() is not a member of in ID3DXBaseMesh
	{
		HRESULT hr = GetMesh()->LockAttributeBuffer( 0, &pLockedAttributeBuffer );
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


bool CD3DXMeshObjectBase::UnlockVertexBuffer()
{
	if( GetBaseMesh() )
	{
		HRESULT hr = GetBaseMesh()->UnlockVertexBuffer();
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


bool CD3DXMeshObjectBase::UnlockIndexBuffer()
{
	if( GetBaseMesh() )
	{
		HRESULT hr = GetBaseMesh()->UnlockIndexBuffer();
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


bool CD3DXMeshObjectBase::UnlockAttributeBuffer()
{
	if( GetMesh() ) // need ID3DXMesh - LockAttributeBuffer() is not a member of in ID3DXBaseMesh
	{
		HRESULT hr = GetMesh()->UnlockAttributeBuffer();
		return (SUCCEEDED(hr)) ? true : false;
	}
	else
		return false;
}


#include "D3DXMeshObject.h"
#include "D3DXSMeshObject.h"



static CD3DXMeshObjectBase* CreateMeshInstance( CMeshType::Name mesh_type )
{
	switch( mesh_type )
	{
	case CMeshType::BASIC:
		return new CD3DXMeshObject();
	case CMeshType::PROGRESSIVE:
		return new CD3DXPMeshObject();
	case CMeshType::SKELETAL:
		return new CD3DXSMeshObject();
	default:
		return NULL;
	}

	return NULL;
}


CD3DXMeshObjectBase* CMeshObjectFactory::LoadMeshObjectFromFile( const std::string& filepath,
																 U32 load_option_flags,
																 CMeshType::Name mesh_type )
{
	CD3DXMeshObjectBase* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromFile( filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}
}


CD3DXMeshObjectBase* CMeshObjectFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	CMeshType::Name mesh_type )
{
	CD3DXMeshObjectBase* pMesh = CreateMeshInstance( mesh_type );

	bool loaded = pMesh->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	if( loaded )
		return pMesh;
	else
	{
		SafeDelete( pMesh );
		return NULL;
	}

/*	if( mesh_type != CMeshType::BASIC )
		return false;

	CD3DXMeshObject *pMeshObject = new CD3DXMeshObject();

	U32 load_option_flags = 0;
	pMeshObject->LoadFromArchive( mesh_archive, filepath, load_option_flags );

	return pMeshObject; */
}
