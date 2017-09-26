#include "MeshObjectHandle.hpp"
#include "GraphicsResourceManager.hpp"
#include "Mesh/SkeletalMesh.hpp"
#include "amorphous/Support/Log/DefaultLog.hpp"


namespace amorphous
{

using namespace std;


//==================================================================================================
// MeshHandle
//==================================================================================================

const MeshHandle MeshHandle::ms_NullHandle;


std::shared_ptr<MeshResource> MeshHandle::GetMeshResource()
{
	if( GetEntry() )
		return GetEntry()->GetMeshResource();
	else
		return std::shared_ptr<MeshResource>();
}


MeshType::Name MeshHandle::GetMeshType()
{
	if( GetEntry()
	 && GetEntry()->GetMeshResource() )
		return GetEntry()->GetMeshResource()->GetMeshType();
	else
		return MeshType::INVALID; // TODO: return a value that means invalid request
}


bool MeshHandle::Load( const std::string& resource_path )
{
	MeshResourceDesc desc;
	desc.ResourcePath = resource_path;
	return Load( desc );
}


bool MeshHandle::Load( const MeshResourceDesc& desc )
{
	// if currently holding a mesh, release it
	Release();

	if( desc.LoadingMode == CResourceLoadingMode::SYNCHRONOUS )
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadMesh( desc );
	}
	else
	{
		m_pResourceEntry = GetGraphicsResourceManager().LoadAsync( desc );
	}

	if( m_pResourceEntry )
		return true;
	else
		return false;
}


std::shared_ptr<SkeletalMesh> MeshHandle::GetSkeletalMesh()
{
	std::shared_ptr<BasicMesh> pBasicMesh = GetMesh();
	if( pBasicMesh )
		return std::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pBasicMesh);
	else
		return std::shared_ptr<SkeletalMesh>();
}



/*
bool MeshHandle::LoadAsync( int priority )
{
	MeshResourceDesc desc;
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
