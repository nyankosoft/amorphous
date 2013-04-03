#include "D3DXSMeshObject.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/memory_helpers.hpp"


namespace amorphous
{


CD3DXSMeshObject::CD3DXSMeshObject()
: m_iNumBones(0), m_paWorldTransforms(NULL), m_pRootBone(NULL)
{
}


CD3DXSMeshObject::CD3DXSMeshObject( const std::string& filename )
: m_iNumBones(0), m_paWorldTransforms(NULL), m_pRootBone(NULL)
{
	bool loaded = CD3DXMeshObjectBase::LoadFromFile( filename );

	if( !loaded )
		LOG_PRINT_ERROR( " - Failed to load a mesh object from a file: " + filename );
}


CD3DXSMeshObject::~CD3DXSMeshObject()
{
	Release();
}


void CD3DXSMeshObject::Release()
{
	SafeDeleteArray( m_paWorldTransforms );
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
		m_paWorldTransforms = new Transform [m_iNumBones];
		Transform transform;
		transform.SetIdentity();
		for( int j=0; j<m_iNumBones; j++ )
			m_paWorldTransforms[j]  = transform;

		int iMatrixIndex = 0;
		m_pRootBone = new MeshBone();
		m_pRootBone->LoadBone_r( archive.GetSkeletonRootBone(), m_paWorldTransforms, iMatrixIndex );

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


bool CD3DXSMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags )
{
	// create mesh from archive
	// - turn off mulitple progress meshes by default
	return LoadFromArchive( archive, filename, option_flags, 1 ); 
}


bool CD3DXSMeshObject::LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes )
{
	bool pmesh_loaded = CD3DXPMeshObject::LoadFromArchive( archive, filename, option_flags, num_pmeshes );

	// load skeleton hierarchy
	LoadSkeletonFromArchive( archive );

	return pmesh_loaded;
}


void CD3DXSMeshObject::DumpSkeletonToTextFile( const std::string& output_filepath ) const
{
	if( !m_pRootBone )
		return;

	FILE *fp = fopen(output_filepath.c_str(),"w");
	if( !fp )
		return;

	fprintf( fp, "%d total bones\n---\n", m_iNumBones );

	m_pRootBone->DumpToTextFile( fp, 0 );

	fclose(fp);
}


} // namespace amorphous
