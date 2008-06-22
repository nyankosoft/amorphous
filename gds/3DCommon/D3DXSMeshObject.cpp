
#include "D3DXSMeshObject.h"

#include "Support/Log/DefaultLog.h"
#include "Support/memory_helpers.h"
#include "Support/fnop.h"
using namespace fnop;


CD3DXSMeshObject::CD3DXSMeshObject()
: m_iNumBones(0), m_paBoneMatrix(NULL), m_pRootBone(NULL)
{
}


CD3DXSMeshObject::CD3DXSMeshObject( const std::string& filename )
: m_iNumBones(0), m_paBoneMatrix(NULL), m_pRootBone(NULL)
{
	bool loaded = CD3DXMeshObjectBase::LoadFromFile( filename );

	if( !loaded )
		LOG_PRINT_ERROR( "Failed to load a mesh object from file" + filename );
}


CD3DXSMeshObject::~CD3DXSMeshObject()
{
	Release();
}


void CD3DXSMeshObject::Release()
{
	SafeDeleteArray( m_paBoneMatrix );
	m_iNumBones = 0;
	m_vecpBone.resize(0);

	SafeDelete( m_pRootBone );

	CD3DXPMeshObject::Release();
}


bool CD3DXSMeshObject::LoadSkeletonFromArchive( C3DMeshModelArchive& archive )
{
	// load bones
	m_iNumBones = archive.GetNumBones();
	if( 1 < m_iNumBones )
	{
		m_paBoneMatrix = new D3DXMATRIX [m_iNumBones];
		for( int j=0; j<m_iNumBones; j++ )
			D3DXMatrixIdentity( &m_paBoneMatrix[j] );

		int iMatrixIndex = 0;
		m_pRootBone = new CMM_Bone();
		m_pRootBone->LoadBone_r( archive.GetSkeletonRootBone(), m_paBoneMatrix, iMatrixIndex );

		// set pointers to bones to a single array
		// so that they can be accessed with indices
		m_vecpBone.clear();
		m_pRootBone->SetBoneToArray_r( m_vecpBone );

		// init local transform caches
		m_vecLocalTransformCache.resize( m_iNumBones, Matrix34Identity() );

		return true;
	}
	else
	{
		LOG_PRINT_WARNING( " - The mesh archive has no bone" );
		m_iNumBones = 0;
		return false;
	}

	return true;
}


bool CD3DXSMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename )
{
	// create mesh from archive
	// - turn off mulitple progress meshes by default
	return LoadFromArchive( archive, filename, 1 ); 
}


bool CD3DXSMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, int num_pmeshes )
{
	bool pmesh_loaded = CD3DXPMeshObject::LoadFromArchive( archive, filename, num_pmeshes );

	// load skeleton hierarchy
	LoadSkeletonFromArchive( archive );

	return pmesh_loaded;
}
