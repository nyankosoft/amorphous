#include "MeshObjectHandle.hpp"
#include "GraphicsResourceManager.hpp"
#include "Mesh/SkeletalMesh.hpp"
#include "Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


//==================================================================================================
// MeshHandle
//==================================================================================================

const MeshHandle MeshHandle::ms_NullHandle;


boost::shared_ptr<CMeshResource> MeshHandle::GetMeshResource()
{
	if( GetEntry() )
		return GetEntry()->GetMeshResource();
	else
		return boost::shared_ptr<CMeshResource>();
}


CMeshType::Name MeshHandle::GetMeshType()
{
	if( GetEntry()
	 && GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMeshType();
	else
		return CMeshType::INVALID; // TODO: return a value that means invalid request
}


bool MeshHandle::Load( const std::string& resource_path )
{
	CMeshResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


bool MeshHandle::Load( const CMeshResourceDesc& desc )
{
	// if currently holding a mesh, release it
	Release();

	if( desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		m_pResourceEntry = GraphicsResourceManager().LoadMesh( desc );
	}
	else
	{
		m_pResourceEntry = GraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;
	else
		return false;
}


boost::shared_ptr<SkeletalMesh> MeshHandle::GetSkeletalMesh()
{
	boost::shared_ptr<BasicMesh> pBasicMesh = GetMesh();
	if( pBasicMesh )
		return boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pBasicMesh);
	else
		return boost::shared_ptr<SkeletalMesh>();
}



/*
bool MeshHandle::LoadAsync( int priority )
{
	CMeshResourceDesc desc;
	desc.Filename = filename;


	return true;
}
*/
/*
void MeshHandle::Serialize( IArchive& ar, const unsigned int version )
{
	GraphicsResourceHandle::Serialize( ar, version );
}
*/

} // namespace amorphous
