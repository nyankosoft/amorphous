#ifndef  __MeshSplitterDemo_HPP__
#define  __MeshSplitterDemo_HPP__


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


class SplitMeshNodeObjects
{
public:
	boost::shared_ptr<CustomMesh> pMesh;
	Vector3 shift;

	SplitMeshNodeObjects()
		:
	shift(Vector3(0,0,0))
	{}
};


class MeshSplitterDemo : public CGraphicsTestBase
{
	typedef binary_node<SplitMeshNodeObjects> SplitMeshNode;

	SplitMeshNode m_RootMeshNode;

	Matrix34 m_MeshPose;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	bool m_ControlSplitPlane;

	boost::shared_ptr<amorphous::CameraController> m_pSplitPlaneController;

private:

	bool InitShader();

	void RenderMeshes( SplitMeshNode& node, ShaderManager& shader_mgr, const Matrix34& parent_transform );

	void RenderMeshes();

	void RenderSplitPlane();

	void SplitMeshesAtLeafNodes( SplitMeshNode& node, const Plane& split_plane );

	void SplitMesh();

	void Reset();

	void UpdateSplitPlaneControllerState();

	bool SetLight();

public:

	MeshSplitterDemo();

	~MeshSplitterDemo();

	const char *GetAppTitle() const { return "MeshSplitterDemo"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	virtual void HandleInput( const InputData& input );
};


#endif /* __MeshSplitterDemo_HPP__ */
