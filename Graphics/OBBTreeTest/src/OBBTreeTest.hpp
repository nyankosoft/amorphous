#ifndef  __OBBTreeTest_HPP__
#define  __OBBTreeTest_HPP__


#include "Graphics/fwd.hpp"
#include "Graphics/MeshObjectHandle.hpp"
#include "Graphics/TextureHandle.hpp"
#include "Graphics/ShaderHandle.hpp"
#include "Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "Input/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"

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
