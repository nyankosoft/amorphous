#ifndef  __ClothTest_HPP__
#define  __ClothTest_HPP__


#include "amorphous/Graphics/MeshObjectHandle.hpp"
#include "amorphous/Graphics/ShaderHandle.hpp"
#include "amorphous/Graphics/Shader/ShaderTechniqueHandle.hpp"
#include "amorphous/Graphics/Mesh/CustomMesh.hpp"
#include "amorphous/GUI/fwd.hpp"
#include "amorphous/3DMath/CriticalDamping.hpp"
#include "amorphous/Physics/fwd.hpp"

#include "../../_Common/GraphicsTestBase.hpp"


class CRigidBodyObject
{
	physics::CScene *m_pScene;

public:
	physics::CActor *m_pActor;
	MeshHandle m_Mesh;

	CRigidBodyObject() : m_pScene(NULL), m_pActor(NULL) {}

	void Release();

	void Render();

	void InitBox(          physics::CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose = Matrix34Identity(), float mass = 1.0f, const SFloatRGBAColor& color = SFloatRGBAColor::White() );
	void InitStaticBox(    physics::CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose = Matrix34Identity(), const SFloatRGBAColor& color = SFloatRGBAColor::White() );
	void InitKinematicBox( physics::CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose = Matrix34Identity(), const SFloatRGBAColor& color = SFloatRGBAColor::White() );

	void InitStaticCapsule( physics::CScene *pScene, float radius, float length, const Matrix34& world_pose = Matrix34Identity(), const SFloatRGBAColor& color = SFloatRGBAColor::White() );
};


class CClothObject
{
	physics::CScene *m_pScene;

public:
	physics::CCloth *m_pCloth;
	CustomMesh m_Mesh;

	CClothObject() : m_pScene(NULL), m_pCloth(NULL) {}

	void CreateFromMesh( physics::CScene *pScene, const std::string& mesh_filepath, const Matrix34& world_pose = Matrix34Identity() );

	void Release();

	void Render();
};


class CTestMeshHolder
{
public:
	MeshHandle m_Handle;
	Matrix34 m_Pose;

	MeshResourceDesc m_MeshDesc;

	enum LoadingStyleName
	{
		LOAD_SYNCHRONOUSLY,
		LOAD_MESH_AND_TEX_TOGETHER,
		LOAD_MESH_AND_TEX_SEPARATELY,
		NUM_LOADING_STYLES
	};

	LoadingStyleName m_LoadingStyle;

	CTestMeshHolder();

	CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose );

	void Load();
};


class CClothTest : public CGraphicsTestBase
{
//	std::vector<MeshHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	ShaderHandle m_Shader;

	ShaderTechniqueHandle m_MeshTechnique;

//	std::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	physics::CScene *m_pPhysScene;

	Scalar m_PhysTimestep;

	Scalar m_PhysOverlapTime;

	std::vector<CRigidBodyObject> m_RigidBodies;

	CClothObject m_Cloth;
//	std::vector<physics::CCloth *> m_pCloths;

//	CustomMesh m_ClothMesh;

	cdv<Vector3> m_vWindForce;

	bool m_StartPhysicsSimulation;

private:

	void CreateSampleUI();

	bool InitShader();

	void CreateCachedResources();

	void RenderMeshes();

	void SetLights();

	void InitPhysicsEngine();

	void UpdatePhysics( float dt );

	void MoveClothHolderActor( const Vector3& dist );

public:

	CClothTest();

	~CClothTest();

	const char *GetAppTitle() const { return "ClothTest"; }

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const InputData& input );
};


#endif /* __ClothTest_HPP__ */
