#ifndef  __CustomMeshTest_HPP__
#define  __CustomMeshTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/GUI/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CCustomMeshTest : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	std::string m_TextBuffer;

	CustomMesh m_Mesh;

	MeshHandle m_RegularMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	CCustomMeshTest();

	~CCustomMeshTest();

	const char *GetAppTitle() const { return "CustomMeshTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __CustomMeshTest_HPP__ */
