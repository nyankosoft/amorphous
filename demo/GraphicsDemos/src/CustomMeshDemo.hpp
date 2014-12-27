#ifndef  __CustomMeshDemo_HPP__
#define  __CustomMeshDemo_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CCustomMeshDemo : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	CustomMesh m_Mesh;

	MeshHandle m_RegularMesh;

private:

	bool InitShader();

	void RenderMeshes();

	void SetLights( bool use_hemespheric_light );

public:

	CCustomMeshDemo();

	~CCustomMeshDemo();

	const char *GetAppTitle() const { return "CustomMeshDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __CustomMeshDemo_HPP__ */
