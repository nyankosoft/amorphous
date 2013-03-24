#ifndef  __OBBTreeTest_HPP__
#define  __OBBTreeTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

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

	boost::shared_ptr<FontBase> m_pFont;

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
