#ifndef  __ConvexTest_HPP__
#define  __ConvexTest_HPP__


#include "amorphous/3DMath/Quaternion.hpp"
#include "amorphous/Graphics/fwd.hpp"
#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/Physics/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CActorHolder
{
public:
	CActorHolder() : pActor(NULL) {}
	physics::CActor *pActor;
//	Transform pose;
	CustomMesh mesh;
};


class CConvexTest : public CGraphicsTestBase
{
	physics::CScene *m_pPhysicsScene;
//	typedef binary_node<CSplitMeshNodeObjects> CMeshNode;

	std::vector<CActorHolder> m_Actors;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<FontBase> m_pFont;

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

	const char *GetAppTitle() const { return "ConvexTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

	void HandleInput( const InputData& input );
};


#endif /* __ConvexTest_HPP__ */
