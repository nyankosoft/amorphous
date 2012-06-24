#ifndef  __ConvexTest_HPP__
#define  __ConvexTest_HPP__


#include "gds/3DMath/Quaternion.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/Physics/fwd.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CActorHolder
{
public:
	CActorHolder() : pActor(NULL) {}
	physics::CActor *pActor;
//	Transform pose;
	CCustomMesh mesh;
};


class CConvexTest : public CGraphicsTestBase
{
	physics::CScene *m_pPhysicsScene;
//	typedef binary_node<CSplitMeshNodeObjects> CMeshNode;

	std::vector<CActorHolder> m_Actors;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CFontBase> m_pFont;

//	boost::shared_ptr<CCameraController> m_pSplitPlaneController;

	std::string m_TextBuffer;

private:

	bool InitShader();

	void RenderMeshes();

	void SaveTexturesAsImageFiles();

	void Reset();

	bool SetLight();

	int InitPhysics( const std::vector< std::pair<std::string,int> >& mesh_and_quantity_pairs );

public:

	CConvexTest();

	~CConvexTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const SInputData& input );
};


#endif /* __ConvexTest_HPP__ */
