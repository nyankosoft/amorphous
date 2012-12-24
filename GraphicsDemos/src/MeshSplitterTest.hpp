#ifndef  __MeshSplitterTest_HPP__
#define  __MeshSplitterTest_HPP__


#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Input/fwd.hpp"
#include "binary_tree.hpp"

#include "../../_Common/GraphicsTestBase.hpp"

/*
class CSplitMeshNode
{
public:

	boost::shared_ptr<CCustomMesh> m_pMesh;

	std::vector<CSplitMeshNode> m_Children;
};*/


class CSplitMeshNodeObjects
{
public:
	boost::shared_ptr<CCustomMesh> pMesh;
	Vector3 shift;

	CSplitMeshNodeObjects()
		:
	shift(Vector3(0,0,0))
	{}
};


class CCameraController;


class CMeshSplitterTest : public CGraphicsTestBase
{
	typedef binary_node<CSplitMeshNodeObjects> CMeshNode;

//	CSplitMeshNode m_RootMeshNode;
//	binary_node<CCustomMesh> m_RootMeshNode;
	CMeshNode m_RootMeshNode;


	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

	bool m_ControlSplitPlane;

	boost::shared_ptr<CCameraController> m_pSplitPlaneController;

	std::string m_TextBuffer;

private:

	bool InitShader();

	void RenderMeshes( binary_node<CSplitMeshNodeObjects>& node, CShaderManager& shader_mgr, const Matrix34& parent_transform );

	void RenderMeshes();

	void RenderSplitPlane();

	void SaveTexturesAsImageFiles();

	void SplitMeshesAtLeafNodes( CMeshNode& node, const Plane& split_plane );

	void SplitMesh();

	void Reset();

	void UpdateSplitPlaneControllerState();

	bool SetLight();

public:

	CMeshSplitterTest();

	~CMeshSplitterTest();

	const char *GetAppTitle() const { return "MeshSplitterTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const SInputData& input );
};


#endif /* __MeshSplitterTest_HPP__ */
