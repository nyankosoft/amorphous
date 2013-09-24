#include "D3DXPMeshObject.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/memory_helpers.hpp"


namespace amorphous
{


CD3DXPMeshObject::CD3DXPMeshObject()
:
m_iPMeshCur(0)
{
}


CD3DXPMeshObject::CD3DXPMeshObject( const std::string& filename, int num_pmeshes )
:
m_iPMeshCur(0)
{
	bool loaded = LoadFromFile( filename, num_pmeshes );

	if( !loaded )
		LOG_PRINT_ERROR( "Cannot to load a mesh object from file: " + filename );
}


CD3DXPMeshObject::~CD3DXPMeshObject()
{
	Release();
}


void CD3DXPMeshObject::Release()
{
	for( int iPMesh = 0; iPMesh < m_vecpPMesh.size(); iPMesh++ )
		SAFE_RELEASE( m_vecpPMesh[iPMesh] );

	m_vecpPMesh.clear();

	CD3DXMeshObjectBase::Release();
}


bool CD3DXPMeshObject::LoadFromFile( const std::string& filename, int num_pmeshes )
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
		HRESULT hr = LoadFromXFile( filename, num_pmeshes );
	}
	else
	{
		C3DMeshModelArchive archive;
		bool b = archive.LoadFromFile( filename );

		if( !b )
			return false;

		// TODO: Support load option flags for LoadFromFile() functions
		loaded = LoadFromArchive( archive, filename, 0 );
	}

//	if( loaded )
//		m_strFilename = filename;

    return loaded;
}


HRESULT CD3DXPMeshObject::LoadFromXFile( const std::string& filename )
{
	return LoadFromXFile( filename, NUM_DEFAULT_PMESHES );
}


HRESULT CD3DXPMeshObject::LoadFromXFile( const std::string& filename, int num_pmeshes )
{
	// save the filename
	m_strFilename = filename;

//	m_NumPMeshes = num_pmeshes;

	if( NUM_MAX_PMESHES < num_pmeshes )
		num_pmeshes = NUM_MAX_PMESHES;

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();
	HRESULT hr;

	LPD3DXMESH   pMesh = NULL;
	LPD3DXBUFFER pAdjacencyBuffer = NULL;

	hr = LoadD3DXMeshAndMaterialsFromXFile( filename, pMesh, pAdjacencyBuffer );

	hr = CreatePMeshFromMesh( pMesh, pAdjacencyBuffer, num_pmeshes );


	SAFE_RELEASE( pAdjacencyBuffer );
	SAFE_RELEASE( pMesh );

	if( FAILED(hr) )
	{
		LOG_PRINT_ERROR( " - Cannot create a progressive mesh from:" + filename );
	}

	return hr;
}


bool CD3DXPMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	return LoadFromArchive( archive, filename, option_flags, NUM_DEFAULT_PMESHES );
}


bool CD3DXPMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes )
{
	Release();

	m_strFilename = filename;

	// copy the contents from the archive

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	LPD3DXMESH pMesh = LoadD3DXMeshFromArchive( archive );

	Result::Name res = LoadMaterialsFromArchive( archive, option_flags );

	HRESULT hr = SetAttributeTable( pMesh, archive.GetTriangleSet() );

	LPD3DXBUFFER pAdjacencyBuffer = NULL;

	// get adjacency info
	DWORD buffer_size_factor = 300;	// 3 at least
	DWORD num_faces = pMesh->GetNumFaces();
	hr = D3DXCreateBuffer( sizeof(DWORD) * pMesh->GetNumFaces() * buffer_size_factor, &pAdjacencyBuffer );
	if( FAILED(hr) )
	{
		LOG_PRINT_WARNING( "D3DXCreateBuffer() failed for " + filename );
		return false;
	}

	DWORD *pBufferPtr = (DWORD *)pAdjacencyBuffer->GetBufferPointer();

	memset( pBufferPtr, 0, sizeof(DWORD) * pMesh->GetNumFaces() * buffer_size_factor );

	hr = pMesh->GenerateAdjacency( 0.01f, pBufferPtr );

	if( FAILED(hr) )
	{
		Release();
		LOG_PRINT_WARNING( " - GenerateAdjacency() failed for " + filename );
		return false;
	}

/*	hr = D3DXValidMesh( pMesh, pBufferPtr, NULL );

	if( hr == D3DXERR_INVALIDMESH )
	{
		assert( !"invalid mesh" );
		return hr;
	}*/


	hr = CreatePMeshFromMesh( pMesh, pAdjacencyBuffer, num_pmeshes );

	SAFE_RELEASE( pAdjacencyBuffer );
	SAFE_RELEASE( pMesh );

	if( FAILED(hr) )
	{
		Release();
	}

	// check the attribute tables (for debugging)
	if( 0 < m_vecpPMesh.size() && m_vecpPMesh[0] )
		PeekAttribTables( m_vecpPMesh[0] );

	return true;
}


HRESULT CD3DXPMeshObject::CreatePMeshFromMesh( LPD3DXMESH pMesh,
                                               LPD3DXBUFFER pAdjacencyBuffer,
											   int num_mesh_divisions )
{

	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	HRESULT hr;
	DWORD        dw32BitFlag;
	LPD3DXPMESH  pPMesh = NULL;
	UINT iPMesh;
	DWORD cVerticesMin, cVerticesMax, cVerticesPerMesh;

	// get adjacency info
//	D3DXCreateBuffer( sizeof(DWORD) * pMesh->GetNumFaces() * 3, &pAdjacencyBuffer );
//	pMesh->GenerateAdjacency( 0.001f, pAdjacencyBuffer );

	// ====== get the number of the attribute tables in the original mesh ======
	DWORD dwOrigAttribTableSize;
	pMesh->GetAttributeTable( NULL, &dwOrigAttribTableSize );

	dw32BitFlag = ( pMesh->GetOptions() & D3DXMESH_32BIT );

	LPVOID pAdjacencyBufferPtr = pAdjacencyBuffer->GetBufferPointer();

	// Perform simple cleansing operations on mesh
	LPD3DXMESH pTempMesh;
	hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION,
		                pMesh,
						(DWORD*)pAdjacencyBufferPtr,
						&pTempMesh,
                        (DWORD*)pAdjacencyBufferPtr,
						NULL );
	if( FAILED(hr) )
	{
		goto End;
	}


	PeekAttribTables( pTempMesh );


//	SAFE_RELEASE( pMesh );
	pMesh = pTempMesh;


	// Perform a weld to try and remove excess vertices.
	// Weld the mesh using all epsilons of 0.0f.  A small epsilon like 1e-6 works well too
	/*	D3DXWELDEPSILONS Epsilons;
	ZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
	if( FAILED( hr = D3DXWeldVertices( pMesh, 0, &Epsilons,
										(DWORD*)pAdjacencyBuffer->GetBufferPointer(),
										(DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL ) ) )
	{
		goto End;
	}
	*/

	// Verify validity of mesh for simplification
	if( FAILED( hr = D3DXValidMesh( pMesh, (DWORD*)pAdjacencyBufferPtr, NULL ) ) )
	{
		goto End;
	}

	PeekAttribTables( pMesh );

//	CreateLocalBoundingSphereFromD3DXMesh( pMesh );


	int i;
	bool has_normals;
	has_normals = false;

	DWORD dwFVF;
	dwFVF = pMesh->GetFVF();
	D3DVERTEXELEMENT9 declaration[MAX_FVF_DECL_SIZE];
	if( dwFVF & D3DFVF_NORMAL )
	{
		has_normals = true;
	}
	else
	{
		memset( declaration, 0, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE );
//		D3DVERTEXELEMENT9 decl_end[1] = D3DDECL_END();
		pMesh->GetDeclaration( declaration );
//		while( declaration[i] != decl_end[0] )
		for( i=0; i<MAX_FVF_DECL_SIZE; i++ )
		{
			if( declaration[i].Usage == D3DDECLUSAGE_NORMAL )
			{
				has_normals = true;
				break;
			}
		}
		if( i == MAX_FVF_DECL_SIZE )
		{
			LOG_PRINT_ERROR( "Mesh normals were not found - Non-FVF mesh must have normals." );
			goto End;
		}
	}

	// If the mesh is missing normals, generate them.
//	if ( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
	if ( !has_normals )
	{
		hr = pMesh->CloneMeshFVF( dw32BitFlag | D3DXMESH_MANAGED, pMesh->GetFVF() | D3DFVF_NORMAL,
									pd3dDevice, &pTempMesh );
		if( FAILED(hr) )
			goto End;

		D3DXComputeNormals( pTempMesh, NULL );

		pMesh->Release();
		pMesh = pTempMesh;
	}


	PeekAttribTables( pMesh );


	// ====== generate progressive meshes ======

	// The created progressive mesh does not have the same attribute table(s).
	// - The two attribute tables of the original mesh gets reduced to only one
	//   in the created mesh...
	// Fixed: Added code to set attribute IDs for each face in SetAttributeTable()
	hr = D3DXGeneratePMesh( pMesh, (DWORD*)pAdjacencyBufferPtr,
							NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &pPMesh );
	if( FAILED(hr) )
		goto End;


	// check the attribute tables in the progressive mesh
	PeekAttribTables( pPMesh );


	cVerticesMin = pPMesh->GetMinVertices();
	cVerticesMax = pPMesh->GetMaxVertices();

	cVerticesPerMesh = ( cVerticesMax - cVerticesMin + 10 ) / num_mesh_divisions;

	UINT num_pmeshes;
	num_pmeshes = take_max( (DWORD)1, (DWORD)ceil( (cVerticesMax - cVerticesMin + 1) / (float)cVerticesPerMesh ) );
	m_vecpPMesh.resize( num_pmeshes, NULL );

/*	if( m_vecpPMesh == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto End;
	}
	ZeroMemory( m_vecpPMesh, sizeof(LPD3DXPMESH) * num_pmeshes );


	// Clone full size pmesh
	hr = pPMesh->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, pPMesh->GetFVF(), pd3dDevice, &m_pPMeshFull );
	if( FAILED(hr) )
		goto End;
*/
	pPMesh->GetDeclaration( declaration );

	// Clone all the separate pmeshes
	for( iPMesh = 0; iPMesh < num_pmeshes; iPMesh++ )
	{
		if( pPMesh->GetFVF() != 0 )
            hr = pPMesh->ClonePMeshFVF( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, pPMesh->GetFVF(), pd3dDevice, &m_vecpPMesh[iPMesh] );
		else
			hr = pPMesh->ClonePMesh( D3DXMESH_MANAGED | D3DXMESH_VB_SHARE, declaration, pd3dDevice, &m_vecpPMesh[iPMesh] );

		if( FAILED(hr) )
			goto End;

		// Trim to appropriate space
		hr = m_vecpPMesh[iPMesh]->TrimByVertices( cVerticesMin + cVerticesPerMesh * iPMesh, cVerticesMin + cVerticesPerMesh * (iPMesh+1), NULL, NULL);
		if( FAILED(hr) )
			goto End;

		hr = m_vecpPMesh[iPMesh]->OptimizeBaseLOD( D3DXMESHOPT_VERTEXCACHE, NULL );
		if( FAILED(hr) )
			goto End;
	}

	// Set current to be maximum number of vertices
	m_iPMeshCur = num_pmeshes - 1;
	hr = m_vecpPMesh[m_iPMeshCur]->SetNumVertices( cVerticesMax );
	if( FAILED(hr) )
		goto End;


End:
	SAFE_RELEASE( pPMesh );

	if( FAILED(hr) )
	{
		Release();

		num_pmeshes = 0;
	}

	return hr;
}


void CD3DXPMeshObject::SetNumVertices( DWORD dwNumVertices )
{
    // If current pm valid for desired value, then set the number of vertices directly
    if( ( dwNumVertices >= m_vecpPMesh[m_iPMeshCur]->GetMinVertices() ) &&
        ( dwNumVertices <= m_vecpPMesh[m_iPMeshCur]->GetMaxVertices() ) )
    {
        m_vecpPMesh[m_iPMeshCur]->SetNumVertices( dwNumVertices );
    }
    else  // Search for the right one
    {
        m_iPMeshCur = m_vecpPMesh.size() - 1;

        // Look for the correct "bin"
        while( m_iPMeshCur > 0 )
        {
            // If number of vertices is less than current max then we found one to fit
            if( dwNumVertices >= m_vecpPMesh[m_iPMeshCur]->GetMinVertices() )
                break;

            m_iPMeshCur -= 1;
        }

        // Set the vertices on the newly selected mesh
        m_vecpPMesh[m_iPMeshCur]->SetNumVertices( dwNumVertices );
    }
}


} // namespace amorphous
