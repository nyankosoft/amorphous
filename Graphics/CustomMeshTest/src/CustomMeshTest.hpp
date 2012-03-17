#ifndef  __CustomMeshTest_HPP__
#define  __CustomMeshTest_HPP__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CCustomMeshTest : public CGraphicsTestBase
{
//	std::vector<CMeshObjectHandle> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	std::string m_TextBuffer;

	CCustomMesh m_Mesh;

	CMeshObjectHandle m_RegularMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	CCustomMeshTest();

	~CCustomMeshTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const SInputData& input );
};


#endif /* __CustomMeshTest_HPP__ */
