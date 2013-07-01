#ifndef  __MultiShapeActorsTest_HPP__
#define  __MultiShapeActorsTest_HPP__


#include <boost/shared_ptr.hpp>
#include "gds/3DMath/Matrix34.hpp"
#include "gds/Graphics/fwd.hpp"
#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/MeshObjectHandle.hpp"
#include "gds/Graphics/ShaderHandle.hpp"
#include "gds/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "gds/Input/fwd.hpp"
#include "gds/GUI/fwd.hpp"
#include "gds/GameCommon/CriticalDamping.hpp"
#include "gds/Physics/fwd.hpp"
#include "gds/Graphics/Mesh/CustomMesh.hpp"

#include "../../../_Common/GraphicsTestBase.hpp"


class CMultiShapeActorsTest : public CGraphicsTestBase, public CGraphicsComponent
{
//	std::vector<CTestMeshHolder> m_vecMesh;
	std::vector<CMeshObjectHandle> m_Meshes;

	std::vector<physics::CActor *> m_pActors;

	std::vector<uint> m_ActorMeshIDs;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

//	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	physics::CScene *m_pPhysScene;

	Scalar m_PhysTimestep;

	Scalar m_PhysOverlapTime;

//	std::vector<CRigidBodyObject> m_RigidBodies;

	cdv<Vector3> m_vWindForce;

	bool m_StartPhysicsSimulation;

	int m_DefaultMaterialID;

	static const float ms_fActorStartInitHeight;

	float m_fActorInitHeight;

private:

	void CreateSampleUI();

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

	void SetLights();

	void InitPhysicsEngine();

	void UpdatePhysics( float dt );

	void MoveClothHolderActor( const Vector3& dist );

	void CreateActors( const std::string& model, int quantity );
public:

	CMultiShapeActorsTest();

	~CMultiShapeActorsTest();

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __MultiShapeActorsTest_HPP__ */
