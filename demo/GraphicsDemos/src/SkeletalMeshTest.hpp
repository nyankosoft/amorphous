#ifndef  __SkeletalMeshTest_HPP__
#define  __SkeletalMeshTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


//class CVehicleController;


class CSkeletalMeshTest : public CGraphicsTestBase
{
//	std::string m_MeshFilepath;

	MeshHandle m_Mesh;

//	float m_fDetailLevel;

	ShaderHandle m_Shader;

//	boost::shared_ptr<CVehicleController> m_pVehicleController;

private:

	Result::Name LoadShader();

	Result::Name SetLight( ShaderManager& shader_mgr );

	void RenderMesh();

	Result::Name UpdateVertexBlendTransformsFromLocalTransforms( SkeletalMesh& skeletal_mesh, const std::vector<Transform>& local_transforms );

	Result::Name SetUniformRotations( float angle, unsigned int axis );

	int GetRotationAxis();

public:

	CSkeletalMeshTest();

	~CSkeletalMeshTest();

	const char *GetAppTitle() const { return "SkeletalMeshTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __SkeletalMeshTest_HPP__ */
