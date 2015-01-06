#ifndef  __MiscShaderDemo_HPP__
#define  __MiscShaderDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class MiscShaderDemo : public CGraphicsTestBase
{
	std::vector<MeshHandle> m_Meshes;

//	ShaderHandle m_Shader;
	std::vector<ShaderHandle> m_Shaders;

	ShaderTechniqueHandle m_MeshTechnique;

//	CustomMesh m_Mesh;

//	MeshHandle m_RegularMesh;

	unsigned int m_CurrentShader;

private:

	bool InitShaders();

	void RenderMeshes();

	ShaderHandle GetCurrentShader();

public:

	MiscShaderDemo();

	~MiscShaderDemo();

	const char *GetAppTitle() const { return "MiscShaderDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __MiscShaderDemo_HPP__ */
