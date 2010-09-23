#ifndef  __CustomMeshTest_HPP__
#define  __CustomMeshTest_HPP__


#include <boost/shared_ptr.hpp>
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CCustomMeshTest : public CGraphicsTestBase
{
	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

//	std::vector<CMeshObjectHandle> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

	CCustomMesh m_Mesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights();

public:

	CCustomMeshTest();

	~CCustomMeshTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );
};


#endif /* __CustomMeshTest_HPP__ */
