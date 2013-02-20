#ifndef  __CustomMeshTest_HPP__
#define  __CustomMeshTest_HPP__


#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CCustomMeshTest : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

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
