#ifndef  __OBBTreeTest_HPP__
#define  __OBBTreeTest_HPP__


#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class OBBTreeTest : public CGraphicsTestBase
{
	boost::shared_ptr<OBBTree> m_pOBBTree;

	int m_DrawLevel;

	int m_NumDrawLevels;

//	MeshHandle m_SkyboxMesh;

	MeshHandle m_Mesh;

//	std::vector<MeshHandle> m_vecpMeshes;

	ShaderTechniqueHandle m_MeshTechnique;

//	ShaderTechniqueHandle m_SkyboxTechnique;

	ShaderTechniqueHandle m_DefaultTechnique;

	ShaderHandle m_Shader;

private:

	void InitOBBTree( C3DMeshModelArchive& mesh_archive );

public:

	OBBTreeTest();

	~OBBTreeTest();

	const char *GetAppTitle() const { return "OBBTreeTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __OBBTreeTest_HPP__ */
