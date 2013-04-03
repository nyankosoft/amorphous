#include "D3DXMeshObjectBase.hpp"

#include "amorphous/base.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/CgEffectBase.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/Direct3D/Conversions.hpp"

#include "Support/StringAux.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Profile.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Macro.h"

#include "D3DXMeshObject.hpp"  // Used by CD3DMeshImplFactory
#include "D3DXSMeshObject.hpp" // Used by CD3DMeshImplFactory

#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;


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

//=============================================================================
// CD3DXMeshObjectBase
//=============================================================================

void CD3DXMeshObjectBase::Release()
{
	m_vecAABB.resize(0);

	m_IsVisible.resize(0);

	SafeDeleteArray( m_paVertexElements );

	SAFE_RELEASE( m_pVertexDecleration );

	SafeDeleteArray( m_pMeshMaterials );

	m_vecMaterial.resize( 0 );
}


bool CD3DXMeshObjectBase::LoadFromFile( const std::string& filename, U32 option_flags )
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

		loaded = LoadFromArchive( archive, filename, option_flags );
	}

//	if( loaded )
//		m_strFilename = filename;

    return loaded;
}


bool CD3DXMeshObjectBase::LoadNonAsyncResources( C3DMeshModelArchive& rArchive, U32 option_flags )
{
	Result::Name res = LoadMaterialsFromArchive( rArchive, option_flags );

	return (res == Result::SUCCESS) ? true : false;
}


#define NUM_MAX_VERTEX_ELEMENTS 64


bool CD3DXMeshObjectBase::CreateVertexDeclaration()
{
//	if( m_paVertexElements )
	if( m_paVertexElements )
	{
		HRESULT hr = DIRECT3D9.GetDevice()->CreateVertexDeclaration( m_paVertexElements, &m_pVertexDecleration );
		return SUCCEEDED(hr) ? true : false;
	}
	else
		return false;
}


HRESULT CD3DXMeshObjectBase::LoadD3DMaterialsFromArchive( C3DMeshModelArchive& archive )
{
	const vector<CMMA_Material>& rvecSrcMaterial = archive.GetMaterial();

	const int num_materials = (int)rvecSrcMaterial.size();
	if( num_materials == 0 )
		return E_FAIL;

	SafeDeleteArray( m_pMeshMaterials );
	m_pMeshMaterials = new D3DMATERIAL9[num_materials];

	for( int i=0; i<num_materials; i++ )
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
	}

	return S_OK;
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
	DWORD num_indices  = (DWORD)archive.GetVertexIndex().size();

	if( num_indices < 0xFFFF )
	{
		LOG_PRINT( "Creating a mesh with 16-bit indices." );
		m_IndexSizeInBits = 16;
	}
	else
	{
		LOG_PRINT( "Creating a mesh with 32-bit indices." );
		m_IndexSizeInBits = 32;
	}

	LPD3DXMESH pMesh = NULL;
	
	DWORD options = D3DXMESH_MANAGED;

	if( m_IndexSizeInBits == 32 )
		options |= D3DXMESH_32BIT;

	{
		LOG_SCOPE( "D3DXCreateMesh" );

	hr = D3DXCreateMesh( num_indices / 3,
		                 num_vertices,
//						 0,
						 options,
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

	// load materials (platform-independent)
	// - Need to allocate texturse before calling LoadD3DMaterialsFromArchive().
	LoadMaterialsFromArchive( archive, 0 );

	// load materials (D3D-specific)
	LoadD3DMaterialsFromArchive( archive );

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

/*
void GetD3DVertexDeclaration( U32 src_vertex_format_flags,
							  D3DVERTEXELEMENT9* pDestD3DVertElems,
							  int num_max_dest_elems )
{
	typedef CMMA_VertexSet VFF;
	const U32 vf_flags = src_vertex_format_flags;
	const int max_elems = num_max_dest_elems;
	D3DVERTEXELEMENT9 aVertDec[NUM_MAX_VERTEX_ELEMENTS];
	int i = 0;
	if( vf_flags & VFF::VF_POSITION      && i < max_elems )
		aVertDec[i++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0 );

	if( vf_flags & VFF::VF_NORMAL        && i < max_elems )
		aVertDec[i++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0 );

	if( vf_flags & VFF::VF_BUMPMAP       && i < max_elems )
	{
		aVertDec[i++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0 );
		aVertDec[i++] = D3DVertexElement( 0, vert_size, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,0 );
	}

	if( vf_flags & VFF::VF_DIFFUSE_COLOR && i < max_elems )
		pDestD3DVertElems[i++] = 

	if( vert_fmt_flags & CMMA_VertexSet:: && i < max_elems ) pDestD3DVertElems[i++] = 
	if( vert_fmt_flags & CMMA_VertexSet:: && i < max_elems ) pDestD3DVertElems[i++] = 
	if( vert_fmt_flags & CMMA_VertexSet:: && i < max_elems ) pDestD3DVertElems[i++] = 
	if( vert_fmt_flags & CMMA_VertexSet:: && i < max_elems ) pDestD3DVertElems[i++] = 
	if( vert_fmt_flags & CMMA_VertexSet:: && i < max_elems ) pDestD3DVertElems[i++] = 
}
*/


/**
 \param rVertexSet      [in] source vertex set
 \param pVBData         [in,out] reference to the pointer that points to the address of the buffer to hold vertex data
 \param vecVertElement  [in,out] the buffer for vertex element declarations
 \param vertex_size     [in,out] the size of the each vertex (in bytes)

*/
void LoadVerticesForD3DXMesh( const CMMA_VertexSet& rVertexSet,           // [in]
						      vector<D3DVERTEXELEMENT9>& vecVertElement,  // [out]
						      int &vertex_size,                           // [out]
						      void*& pVBData                              // [out]
							  )
{
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


	//
	// copy the data to the dest variables
	//

	vertex_size = vert_size;

	pVBData = pBuffer;

	vecVertElement.resize( num_vertex_decs );
	for( int i = 0; i<num_vertex_decs; i++ )
	{
		vecVertElement[i] = aVertexDeclaration[i];
	}
}


void CD3DXMeshObjectBase::LoadVertices( void*& pVBData,
								        C3DMeshModelArchive& archive )
{
	vector<D3DVERTEXELEMENT9> vecVertElement; // buffer to temporarily hold vertex elements

	LoadVerticesForD3DXMesh(
		archive.GetVertexSet(), // [in]
		vecVertElement,         // [out]
		m_iVertexSize,          // [out]
		pVBData                 // [out]
		);

	// update vertex elements
	SafeDeleteArray( m_paVertexElements );
	m_paVertexElements = new D3DVERTEXELEMENT9 [vecVertElement.size()];
	memcpy( m_paVertexElements, &vecVertElement[0], sizeof(D3DVERTEXELEMENT9) * vecVertElement.size() );
}


HRESULT CD3DXMeshObjectBase::LoadMaterials( D3DXMATERIAL* d3dxMaterials, int num_materials )
{
	// allocate material buffers, etc.
//	InitMaterials( num_materials );

	if( m_vecMaterial.size() == 0 )
		m_vecMaterial.resize( num_materials );

	SafeDeleteArray( m_pMeshMaterials );
	m_pMeshMaterials = new D3DMATERIAL9 [num_materials];

	// Copy the materials and load the textures
	for( int i = 0; i < num_materials; i++ )
	{
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_vecMaterial[i].Texture.resize(1);

		if( d3dxMaterials[i].pTextureFilename )
		{
			path tex_filepath = path(m_strFilename).parent_path() / string(d3dxMaterials[i].pTextureFilename);
			m_vecMaterial[i].Texture[0].Load( tex_filepath.string() );
		}
	}

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
///   - Changed: always use U16
bool LoadIndices( C3DMeshModelArchive& archive, vector<U16>& vecIBData )
{
	const vector<unsigned int>& rvecVertexIndex = archive.GetVertexIndex();
	const int num_indices = archive.GetNumVertexIndices();

	vecIBData.resize( num_indices );

	for( int i=0; i<num_indices; i++ )
		vecIBData[i] = (U16)rvecVertexIndex[i];

	return true;
}


bool CD3DXMeshObjectBase::FillIndexBuffer( LPD3DXMESH pMesh, C3DMeshModelArchive& archive )
{
	if( !pMesh )
		return false;

	const vector<unsigned int>& rvecVertexIndex = archive.GetVertexIndex();
	const int num_indices = archive.GetNumVertexIndices();

	HRESULT hr;
	void *pIB = NULL;

	// copy the index data to the index buffer
    if( FAILED( hr = pMesh->LockIndexBuffer( 0, (VOID**)&pIB ) ) )
		return false;

	if( m_IndexSizeInBits == 16 )
	{
		unsigned short *pusIBData = (unsigned short *)pIB;
		for( int i=0; i<num_indices; i++ )
			pusIBData[i] = (unsigned short)rvecVertexIndex[i];
	}
	else
	{
		U32 *pIBData = (U32 *)pIB;
		for( int i=0; i<num_indices; i++ )
			pIBData[i] = (U32)rvecVertexIndex[i];
	}

	hr = pMesh->UnlockIndexBuffer();

	return SUCCEEDED(hr) ? true : false;
}


/// \param [in] vecTriangleSet
/// \param [out] vecAttributeRange
void GetAttributeTableFromTriangleSet( const vector<CMMA_TriangleSet>& vecTriangleSet,
									   std::vector<D3DXATTRIBUTERANGE>& vecAttributeRange )
{
	vecAttributeRange.resize( vecTriangleSet.size() );

	const int num_materials = (int)vecTriangleSet.size();
	for( int i=0; i<num_materials; i++ )
	{
		const CMMA_TriangleSet& triangle_set = vecTriangleSet[i]; // src
		D3DXATTRIBUTERANGE& attrib_range = vecAttributeRange[i];  // dest

		attrib_range.AttribId		= i;
//		attrib_range.AttribId		= i + 1;
//		attrib_range.FaceStart		= triangle_set.m_iStartIndex;
		attrib_range.FaceStart		= triangle_set.m_iStartIndex / 3;
		attrib_range.FaceCount		= triangle_set.m_iNumTriangles;
		attrib_range.VertexStart	= triangle_set.m_iMinIndex;
		attrib_range.VertexCount	= triangle_set.m_iNumVertexBlocksToCover;
	}
}


HRESULT CD3DXMeshObjectBase::SetAttributeTable( LPD3DXMESH pMesh,
											     const vector<CMMA_TriangleSet>& vecTriangleSet )
{
	// convert triangle sets to attribute ranges
	vector<D3DXATTRIBUTERANGE> vecAttributeRange;
	GetAttributeTableFromTriangleSet( vecTriangleSet, vecAttributeRange );

	const int num_materials = (int)m_vecMaterial.size();
	HRESULT hr = pMesh->SetAttributeTable( &(vecAttributeRange[0]), (DWORD)num_materials );

	// set attribute IDs for each face
	DWORD *pdwBuffer = NULL;
	pMesh->LockAttributeBuffer( 0, &pdwBuffer );
	DWORD face = 0;
	for( int i=0; i<num_materials; i++ )
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


void CD3DXMeshObjectBase::UpdateVisibility( const Camera& cam )
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

/*
void CD3DXMeshObjectBase::RenderSubsets( ShaderManager& rShaderMgr,
										 const std::vector<int>& vecMaterialIndex )
{
	PROFILE_FUNCTION();

	vector<ShaderTechniqueHandle> empty_shader_technique_array;
	RenderSubsets( rShaderMgr, vecMaterialIndex, empty_shader_technique_array );
}
*/

void CD3DXMeshObjectBase::RenderSubsets( ShaderManager& rShaderMgr,
		                        const int *paMaterialIndex /* some option to specify handles for texture */,
								int num_indices )
{
	PROFILE_FUNCTION();

//	vector<ShaderTechniqueHandle> empty_shader_technique_array;
//	RenderSubsets( rShaderMgr, vecMaterialIndex, empty_shader_technique_array );
	RenderSubsets( rShaderMgr, paMaterialIndex, NULL, num_indices );
}

/*
void CD3DXMeshObjectBase::RenderSubsets( ShaderManager& rShaderMgr,
										 const std::vector<int>& vecMaterialIndex,
										 std::vector<ShaderTechniqueHandle>& vecShaderTechnique )
{}
*/

void CD3DXMeshObjectBase::RenderSubsets( ShaderManager& rShaderMgr,
										 const int *paMaterialIndex,
										 ShaderTechniqueHandle *paShaderTechnique,
										 int num_indices )
										 // Also need MeshMaterial *pMaterials = NULL?
										 // rationale: render the same model with different materials
{
//	bool single_shader_technique = ( vecShaderTechnique.size() == 0 ) ? true : false;
	bool single_shader_technique = ( paShaderTechnique == NULL ) ? true : false;

	if( m_bViewFrustumTest && !IsMeshVisible() )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	LPD3DXBASEMESH pMesh = GetBaseMesh();
	if( !pMesh )
		return;

	LPD3DXEFFECT pEffect = rShaderMgr.GetEffect();
	if( !pEffect )
		return RenderSubsetsCg( pMesh, rShaderMgr, paMaterialIndex, paShaderTechnique, num_indices );

	UINT cPasses;
//	pEffect->Begin( &cPasses, 0 );

	hr = pd3dDevice->SetVertexDeclaration( GetVertexDeclaration() );

	// Meshes are divided into subsets by materials. Render each subset in a loop
//	const int num_materials = GetNumMaterials();
//	for( int mat=0; mat<num_materials; mat++ )
	const size_t num_materials_to_render = num_indices;//vecMaterialIndex.size();
	for( size_t i=0; i<num_materials_to_render; i++ )
	{
		int mat = paMaterialIndex[i];//vecMaterialIndex[i];

		if( m_bViewFrustumTest && !IsMeshVisible(mat) )
			continue;

		if( !single_shader_technique )
//		 && i < vecShaderTechnique.size() )
		{
//			rShaderMgr.SetTechnique( vecShaderTechnique[i] );
			rShaderMgr.SetTechnique( paShaderTechnique[i] );
		}

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

void CD3DXMeshObjectBase::RenderSubsetsCg( LPD3DXBASEMESH pMesh,
										   ShaderManager& rShaderMgr,
										   const int *paMaterialIndex,
										   ShaderTechniqueHandle *paShaderTechnique,
										   int num_indices )
										   // Also need MeshMaterial *pMaterials = NULL?
										   // rationale: render the same model with different materials
{
	bool single_shader_technique = ( paShaderTechnique == NULL ) ? true : false;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	CCgEffectBase *pCgEffectMgr = dynamic_cast<CCgEffectBase *>(&rShaderMgr);
	if( !pCgEffectMgr )
		return;

	CCgEffectBase& cg_effect_mgr = *pCgEffectMgr;

//	pEffect->Begin( &cPasses, 0 );

	hr = pd3dDevice->SetVertexDeclaration( GetVertexDeclaration() );

	// Meshes are divided into subsets by materials. Render each subset in a loop
//	const int num_materials = GetNumMaterials();
//	for( int mat=0; mat<num_materials; mat++ )
	const size_t num_materials_to_render = num_indices;//vecMaterialIndex.size();
	for( size_t i=0; i<num_materials_to_render; i++ )
	{
		int mat = paMaterialIndex[i];//vecMaterialIndex[i];

		if( m_bViewFrustumTest && !IsMeshVisible(mat) )
			continue;

		if( !single_shader_technique )
//		 && i < vecShaderTechnique.size() )
		{
//			rShaderMgr.SetTechnique( vecShaderTechnique[i] );
			cg_effect_mgr.SetTechnique( paShaderTechnique[i] );
		}

		const int num_textures_for_material = (int)m_vecMaterial[mat].Texture.size();
		for( int tex=0; tex<num_textures_for_material; tex++ )
			rShaderMgr.SetTexture( tex, GetTexture( mat, tex ) );

		for( CGpass pass = cg_effect_mgr.GetFirstPass();
			 pass;
			 pass = cgGetNextPass(pass) )
		{
			bool valid_pass = cgIsPass( pass );

			cgSetPassState(pass);

			// Draw the mesh subset
			hr = pMesh->DrawSubset( mat );

			cgResetPassState(pass);
		}
	}
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the object
//-----------------------------------------------------------------------------
// Should change this to 
//void CD3DXMeshObjectBase::RenderSubsets( int *pSubsetIndices, int num_indices )?
// rationale: support subset rendering in fixed function pipeline mode.
void CD3DXMeshObjectBase::Render()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
//	LPD3DXMESH pMesh = m_pMesh;
	LPD3DXBASEMESH pMesh = GetBaseMesh();

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetPixelShader( NULL );

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
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Meshes are divided into subsets by materials. Render each subset in a loop
//	HRESULT hr;
	LPDIRECT3DTEXTURE9 pTex = NULL;
	const int num_materials = (int)m_vecMaterial.size();
    for( int i=0; i<num_materials; i++ )
    {
        // Set the material and texture for this subset
		pd3dDevice->SetMaterial( &m_pMeshMaterials[i] );

//		if( FAILED(hr) ) MessageBox(NULL, "SetMaterial() Failed", "Error", MB_OK|MB_ICONWARNING);

//		if( pTex = GetTexture(i,0).GetTexture() )
		if( 0 < m_vecMaterial[i].Texture.size() )
		{
			pTex = m_vecMaterial[i].Texture[0].GetTexture();

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
        pMesh->DrawSubset( i );
    }

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

	m_LocalShpere.center = ToVector3( m_vObjectCenter );

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


unsigned int CD3DXMeshObjectBase::GetNumVertices() const
{
	return GetBaseMesh() ? (GetBaseMesh()->GetNumVertices()) : 0;
}


unsigned int CD3DXMeshObjectBase::GetNumTriangles() const
{
	return GetBaseMesh() ? (GetBaseMesh()->GetNumFaces()) : 0;
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


void CD3DXMeshObjectBase::SetVertexDeclaration()
{
	DIRECT3D9.GetDevice()->SetVertexDeclaration( GetVertexDeclaration() );
}


/*
MeshImpl* CD3DMeshImplFactory::CreateMeshImpl( MeshType::Name mesh_type )
{
	switch( mesh_type )
	{
	case MeshType::BASIC:
		return new CD3DXMeshObject();
	case MeshType::PROGRESSIVE:
		return new CD3DXPMeshObject();
	case MeshType::SKELETAL:
		return new CD3DXSMeshObject();
	default:
		return NULL;
	}

	return NULL;
}
*/


MeshImpl* CD3DMeshImplFactory::CreateBasicMeshImpl()       { return new CD3DXMeshObject(); }
MeshImpl* CD3DMeshImplFactory::CreateProgressiveMeshImpl() { return new CD3DXPMeshObject(); }
MeshImpl* CD3DMeshImplFactory::CreateSkeletalMeshImpl()    { return new CD3DXSMeshObject(); }

/*
shared_ptr<CD3DXMeshObjectBase> CMeshObjectFactory::CreateMesh( MeshType::Name mesh_type )
{
	shared_ptr<CD3DXMeshObjectBase> pMesh( CreateMeshInstance( mesh_type ) );
	return pMesh;
}


CD3DXMeshObjectBase* CMeshObjectFactory::LoadMeshObjectFromFile( const std::string& filepath,
																 U32 load_option_flags,
																 MeshType::Name mesh_type )
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


shared_ptr<MeshImpl> CD3DMeshImplFactory::CreateMesh( MeshType::Name mesh_type )
{
	shared_ptr<CD3DXMeshObjectBase> pMesh( CreateMeshInstance( mesh_type ) );
	return pMesh;
}


MeshImpl* CD3DMeshImplFactory::LoadMeshObjectFromFile( const std::string& filepath,
																 U32 load_option_flags,
																 MeshType::Name mesh_type )
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


CD3DXMeshObjectBase* CD3DMeshImplFactory::LoadMeshObjectFromArchive( C3DMeshModelArchive& mesh_archive,
																    const std::string& filepath,
																    U32 load_option_flags,
																	MeshType::Name mesh_type )
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
}
*/


} // namespace amorphous
