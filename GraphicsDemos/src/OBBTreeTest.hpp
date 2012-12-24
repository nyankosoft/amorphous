#ifndef  __OBBTreeTest_HPP__
#define  __OBBTreeTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/TextureHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"

using namespace MeshModel;


class COBBTree;


class COBBTreeTest : public CGraphicsTestBase
{
	boost::shared_ptr<COBBTree> m_pOBBTree;

	int m_DrawLevel;

	int m_NumDrawLevels;

//	CMeshObjectHandle m_SkyboxMesh;

	CMeshObjectHandle m_Mesh;

//	std::vector<CMeshObjectHandle> m_vecpMeshes;

	CShaderTechniqueHandle m_MeshTechnique;

//	CShaderTechniqueHandle m_SkyboxTechnique;

	CShaderTechniqueHandle m_DefaultTechnique;

	CShaderHandle m_Shader;

	boost::shared_ptr<CFontBase> m_pFont;

private:

	void InitOBBTree( C3DMeshModelArchive& mesh_archive );

public:

	COBBTreeTest();

	~COBBTreeTest();

	const char *GetAppTitle() const { return "OBBTreeTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input );


//	void UpdateViewTransform( const Matrix44& matView );

//	void UpdateProjectionTransform( const Matrix44& matProj );

//	void OnKeyPressed( KeyCode::Code key_code );
};


#endif /* __OBBTreeTest_HPP__ */
