#ifndef  __ClothTest_HPP__
#define  __ClothTest_HPP__


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


class CRigidBodyObject
{
	physics::CScene *m_pScene;

public:
	physics::CActor *m_pActor;
	CMeshObjectHandle m_Mesh;

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
	CCustomMesh m_Mesh;

	CClothObject() : m_pScene(NULL), m_pCloth(NULL) {}

	void CreateFromMesh( physics::CScene *pScene, const std::string& mesh_filepath, const Matrix34& world_pose = Matrix34Identity() );

	void Release();

	void Render();
};


class CTestMeshHolder
{
public:
	CMeshObjectHandle m_Handle;
	Matrix34 m_Pose;

	CMeshResourceDesc m_MeshDesc;

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


class CClothTest : public CGraphicsTestBase, public CGraphicsComponent
{
	enum UIID
	{
		UIID_DLG_ROOT = 1000,
		UIID_DLG_SLIDERS,
		UIID_DLG_RESOLUTION,
		UIID_LBX_RESOLUTION,
		UIID_DLG_LISTBOXGROUP,
		UIID_OTHER
	};

	enum Params
	{
		TEXT_BUFFER_SIZE = 4096
	};

//	std::vector<CMeshObjectHandle> m_vecMesh;
	std::vector<CTestMeshHolder> m_vecMesh;

	CShaderHandle m_Shader;

	CShaderTechniqueHandle m_MeshTechnique;

	boost::shared_ptr<CInputHandler_Dialog> m_pUIInputHandler;

	boost::shared_ptr<CFontBase> m_pFont;

	CGM_DialogManagerSharedPtr m_pSampleUI;

	physics::CScene *m_pPhysScene;

	Scalar m_PhysTimestep;

	Scalar m_PhysOverlapTime;

	std::vector<CRigidBodyObject> m_RigidBodies;

	CClothObject m_Cloth;
//	std::vector<physics::CCloth *> m_pCloths;

//	CCustomMesh m_ClothMesh;

	char m_TextBuffer[TEXT_BUFFER_SIZE];

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

	int Init();

	void Release() {};

	void Update( float dt );

	void Render();

//	virtual void RenderBase();

	virtual void HandleInput( const SInputData& input );

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


#endif /* __ClothTest_HPP__ */
