#ifndef  __MeshSplitterTest_HPP__
#define  __MeshSplitterTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/Input/fwd.hpp"
#include "binary_tree.hpp"

#include "../../_Common/GraphicsTestBase.hpp"

/*
class CSplitMeshNode
{
public:

	boost::shared_ptr<CustomMesh> m_pMesh;

	std::vector<CSplitMeshNode> m_Children;
};*/


class CSplitMeshNodeObjects
{
public:
	boost::shared_ptr<CustomMesh> pMesh;
	Vector3 shift;

	CSplitMeshNodeObjects()
		:
	shift(Vector3(0,0,0))
	{}
};


class CMeshSplitterTest : public CGraphicsTestBase
{
	typedef binary_node<CSplitMeshNodeObjects> CMeshNode;

//	CSplitMeshNode m_RootMeshNode;
//	binary_node<CustomMesh> m_RootMeshNode;
	CMeshNode m_RootMeshNode;

	Matrix34 m_MeshPose;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	bool m_ControlSplitPlane;

	boost::shared_ptr<amorphous::CameraController> m_pSplitPlaneController;

private:

	bool InitShader();

	void RenderMeshes( binary_node<CSplitMeshNodeObjects>& node, ShaderManager& shader_mgr, const Matrix34& parent_transform );

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

	virtual void HandleInput( const InputData& input );
};


#endif /* __MeshSplitterTest_HPP__ */
