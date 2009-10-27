#include "D3DXMeshObject.hpp"

#include "Graphics/Shader/Shader.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "Graphics/FVF_BumpVertex.h"
#include "Graphics/FVF_BumpWeightVertex.h"
#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/FVF_TextureVertex.h"
#include "Graphics/FVF_NormalVertex.h"
#include "Graphics/FVF_WeightVertex.h"
#include "Graphics/FVF_ShadowVertex.h"

#include "Graphics/MeshModel/MeshBone.hpp"
using namespace MeshModel;

#include "Support/Log/DefaultLog.hpp"


CD3DXMeshObject::CD3DXMeshObject()
: m_pMesh(NULL)
{
}


CD3DXMeshObject::CD3DXMeshObject( const std::string& filename )
: m_pMesh(NULL)
{
	bool loaded = LoadFromFile(filename);

	if( !loaded )
	{
		LOG_PRINT_ERROR( " - Failed to load a mesh file" + filename );
	}
}


CD3DXMeshObject::~CD3DXMeshObject()
{
	Release();
}


void CD3DXMeshObject::Release()
{
	// release materials
	CD3DXMeshObjectBase::Release();

    if( m_pMesh != NULL )
	{
        m_pMesh->Release();
		m_pMesh = NULL;
	}
}


HRESULT CD3DXMeshObject::LoadFromXFile( const std::string& filename )
{
	LPD3DXBUFFER pAdjacencyBuffer = NULL;
	HRESULT hr = LoadD3DXMeshAndMaterialsFromXFile( filename, m_pMesh, pAdjacencyBuffer );

	SAFE_RELEASE( pAdjacencyBuffer );

	return hr;
}


bool CD3DXMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	// copy the contents from the archive

	m_strFilename = filename;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	HRESULT hr;

	LPD3DXMESH pMesh = LoadD3DXMeshFromArchive( archive );

	if( !pMesh )
		return false;

	// load surface materials & textures
	LoadMaterialsFromArchive( archive, option_flags );

	hr = SetAttributeTable( pMesh, archive.GetTriangleSet() );

	if( FAILED(hr) )
		return false;

	m_pMesh = pMesh;
	pMesh = NULL;

	PeekAttribTables( m_pMesh );

	return true;
}


bool CD3DXMeshObject::CreateMesh( int num_vertices, int num_indices, U32 option_flags,
								  std::vector<D3DVERTEXELEMENT9>& vecVertexElement )
{
	HRESULT hr;
	hr = D3DXCreateMesh(
			num_indices / 3,         // DWORD NumFaces,
			num_vertices,            // DWORD NumVertices,
			option_flags,            // DWORD Options,
			&(vecVertexElement[0]),  // CONST LPD3DVERTEXELEMENT9 * pDeclaration,
			DIRECT3D9.GetDevice(),   // LPDIRECT3DDEVICE9 pD3DDevice,
			&m_pMesh
		);

	if( FAILED(hr) )
		LOG_PRINT_ERROR( fmt_string( " D3DXCreateMesh() failed. (%d faces, %d vertices, flags: %d)", num_indices / 3, num_vertices, option_flags ) );

	return (m_pMesh != NULL);
}
