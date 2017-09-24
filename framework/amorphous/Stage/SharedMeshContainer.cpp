#include "SharedMeshContainer.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/GameCommon/GameMathMisc.hpp"
#include "amorphous/GameCommon/MeshBoneControllerBase.hpp"
//#include "App/GameApplicationBase.hpp"
#include "amorphous/Task/GameTaskManager.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/StringAux.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"
#include "amorphous/Utilities/TextFileScannerExtensions.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using std::map;
using boost::shared_ptr;

//===============================================================================
// SharedMeshContainer
//===============================================================================

SharedMeshContainer::SharedMeshContainer()
	:
	m_PropertyFlags(0)
{
	m_ShaderTechnique.resize(1,1);
	m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	m_MeshDesc.MeshType = MeshType::BASIC;
}


SharedMeshContainer::SharedMeshContainer( const string& filename )
	:
	m_PropertyFlags(0)
{
	m_ShaderTechnique.resize(1,1);
	m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	m_MeshDesc.MeshType = MeshType::BASIC;
	m_MeshDesc.ResourcePath = filename;
}


SharedMeshContainer::~SharedMeshContainer()
{
	// texture is released in the destructor of 'TextureHandle'
	//	m_SpecTex.Release();
}


void SharedMeshContainer::ValidateShaderTechniqueTable()
{
	BasicMesh *pMeshObject = m_MeshObjectHandle.GetMesh().get();
	if( !pMeshObject )
		return;

	if( m_ShaderTechnique.size_y() == 0 )
	{
		m_ShaderTechnique.resize(1,1);
		m_ShaderTechnique(0,0).SetTechniqueName( "NoShader" );
	}
	else if( m_ShaderTechnique.size_y() == 1 && 2 <= pMeshObject->GetNumMaterials() )
	{
		// only one technique and 2 or more materials
		// - assumes the user wants to render all the materials with the same shader technique 
		m_PropertyFlags |= PF_USE_SINGLE_TECHNIQUE_FOR_ALL_MATERIALS;
	}
	else if( m_ShaderTechnique.size_y() < pMeshObject->GetNumMaterials() )
	{
		int num_orig_rows = m_ShaderTechnique.size_y();

		// (the number of techniques) < (the number of materials)
		// - copy the last technique and make sure there are as many techniques as materials

		// increase the columns to cover all the materials
		m_ShaderTechnique.increase_y( pMeshObject->GetNumMaterials() - m_ShaderTechnique.size_y() );

		// overwrite increased rows with the last technique for each LOD
		int lod, num_lods = m_ShaderTechnique.size_x(); // columns: for LODs
		int row, num_rows = m_ShaderTechnique.size_y(); // rows: for materials
		for( lod=0; lod<num_lods; lod++ )
		{
			for( row=num_orig_rows; row<num_rows; row++ )
			{
				m_ShaderTechnique( lod, row )
					= m_ShaderTechnique( lod, num_orig_rows - 1 );
			}
		}
	}
}


bool SharedMeshContainer::LoadMeshObject()
{
	m_MeshObjectHandle.Load( m_MeshDesc );

	// validate shader technique table
	BasicMesh *pMeshObject = m_MeshObjectHandle.GetMesh().get();
	if( pMeshObject )
	{
		ValidateShaderTechniqueTable();
	}

	/*
	load mesh (standard, progressive or skeletal)
	support .x file
	...

	if( 0 < m_SpecTex.filename.length() )
	{
	m_SpecTex.Load();
	}
	*/
	return true;
}


void SharedMeshContainer::Release()
{
	SafeDeleteVector( m_vecpMeshBoneController );

	m_vecTargetMaterialIndex.resize( 0 );
}


} // namespace amorphous
