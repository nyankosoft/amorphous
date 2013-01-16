#ifndef __MeshBonesUpdateCallback_HPP__
#define __MeshBonesUpdateCallback_HPP__


#include "GraphicsResourcesUpdateCallback.hpp"
#include "../Graphics/Shader/BlendTransformsLoader.hpp"
#include "../Graphics/MeshObjectHandle.hpp"
#include "../Graphics/Mesh/SkeletalMesh.hpp"


namespace amorphous
{


/// Calculates the blend transforms for a skeletal mesh
class CMeshBonesUpdateCallback : public CGraphicsResourcesUpdateCallback
{
	std::vector<Transform> m_LocalTransforms;

	MeshHandle m_TargetSkeletalMesh;

	boost::shared_ptr<CBlendTransformsLoader> m_pBlendTransformsLoader;

public:

	CMeshBonesUpdateCallback() {}

	CMeshBonesUpdateCallback( boost::shared_ptr<CBlendTransformsLoader> pBlendTransformsLoader )
		:
	m_pBlendTransformsLoader(pBlendTransformsLoader)
	{}

	Result::Name SetSkeletalMesh( MeshHandle target_skeletal_mesh )
	{
		boost::shared_ptr<BasicMesh> pMesh = target_skeletal_mesh.GetMesh();
		if( !pMesh )
			return Result::INVALID_ARGS;

		boost::shared_ptr<SkeletalMesh> pSMesh = boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pMesh);
		if( !pSMesh )
			return Result::INVALID_ARGS;

		m_TargetSkeletalMesh = target_skeletal_mesh;

		return Result::SUCCESS;
	}

	void SetBlendTransformsLoader( boost::shared_ptr<CBlendTransformsLoader> pBlendTransformsLoader ) { m_pBlendTransformsLoader = pBlendTransformsLoader; }

	/// Update the blend transforms stored in the blend transforms loader.
	void UpdateGraphics()
	{
		using namespace boost;

		if( !m_pBlendTransformsLoader )
			return;

		std::vector<Transform>& dest_blend_transforms = m_pBlendTransformsLoader->BlendTransforms();
		dest_blend_transforms.resize( m_LocalTransforms.size() );

		boost::shared_ptr<BasicMesh> pMesh = m_TargetSkeletalMesh.GetMesh();
		if( !pMesh )
			return;

		boost::shared_ptr<SkeletalMesh> pSMesh = boost::dynamic_pointer_cast<SkeletalMesh,BasicMesh>(pMesh);
		if( !pSMesh )
			return;

		pSMesh->CalculateBlendTransforms( m_LocalTransforms, dest_blend_transforms );
	}

	std::vector<Transform>&  MeshBoneLocalTransforms() { return m_LocalTransforms; }
};


//void SetGraphicsUpdateCallbackForSkeletalMesh( CGamteItem& item )
//{
//}

} // amorphous



#endif /* __MeshBonesUpdateCallback_HPP__ */
