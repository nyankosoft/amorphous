#ifndef  __MultiShapeActorsTest_HPP__
#define  __MultiShapeActorsTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Input/fwd.hpp"
#include "amorphous/GUI/fwd.hpp"
#include "amorphous/GameCommon/CriticalDamping.hpp"
#include "amorphous/Physics/fwd.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CMultiShapeActorsTest : public CGraphicsTestBase
{
//	std::vector<TestMeshHolder> m_vecMesh;
	std::vector<MeshHandle> m_Meshes;

	std::vector<physics::CActor *> m_pActors;

	std::vector<uint> m_ActorMeshIDs;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

//	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<FontBase> m_pFont;

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

	const char *GetAppTitle() const { return "MultiShapeActorsTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );
};


#endif /* __MultiShapeActorsTest_HPP__ */
