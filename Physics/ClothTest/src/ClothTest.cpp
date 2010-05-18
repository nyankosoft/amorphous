#include "ClothTest.hpp"
#include <boost/foreach.hpp>
#include "gds/Graphics.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Graphics/VertexFormat.hpp"
#include "gds/Graphics/Mesh/CustomMeshRenderer.hpp"
#include "gds/Support/Timer.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"
#include "gds/Support/MTRand.hpp"
#include "gds/GUI.hpp"
#include "gds/Physics.hpp"
#include "SceneUtility.hpp"

using namespace std;
using namespace boost;
using namespace physics;


static int gs_TextureMipLevels = 1;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CClothTest();
}


extern const std::string GetAppTitle()
{
	return string("ClothTest");
}


void CRigidBodyObject::Release()
{
	if( m_pScene )
		m_pScene->ReleaseActor( m_pActor );
}


CMeshResourceDesc CreateBoxMeshDesc( Vector3 edge_lengths, const SFloatRGBAColor& color )
{
	// mesh
	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( edge_lengths );
	pBoxMeshGenerator->SetDiffuseColor( color );
	CMeshResourceDesc mesh_desc;
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
//	m_vecMesh.back().m_MeshDesc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	return mesh_desc;
}


void CRigidBodyObject::InitStaticBox( CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
	m_pScene = pScene;

	CMeshResourceDesc mesh_desc( CreateBoxMeshDesc( edge_lengths, color ) );
	m_Mesh.Load( mesh_desc );

	// actor
	CSceneUtility util(pScene);
	m_pActor = util.CreateStaticBoxActor( edge_lengths * 0.5f, world_pose );
}


void CRigidBodyObject::InitKinematicBox( CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
	m_pScene = pScene;

	CMeshResourceDesc mesh_desc( CreateBoxMeshDesc( edge_lengths, color ) );
	m_Mesh.Load( mesh_desc );

	// actor
	CSceneUtility util(pScene);
	m_pActor = util.CreateKinematicBoxActor( edge_lengths * 0.5f, world_pose );
}


void CRigidBodyObject::InitStaticCapsule( CScene *pScene, float radius, float length, const Matrix34& world_pose, const SFloatRGBAColor& color )
{
	m_pScene = pScene;

//	CMeshResourceDesc mesh_desc( CreateCapsuleMeshDesc( edge_lengths, color ) );
//	m_Mesh.Load( mesh_desc );

	// actor
	CSceneUtility util(pScene);
	m_pActor = util.CreateStaticCapsuleActor( radius, length, world_pose );
}


void CRigidBodyObject::InitBox( CScene *pScene, Vector3 edge_lengths, const Matrix34& world_pose, float mass, const SFloatRGBAColor& color )
{
	m_pScene = pScene;

	CMeshResourceDesc mesh_desc( CreateBoxMeshDesc( edge_lengths, color ) );
	m_Mesh.Load( mesh_desc );

	// actor
	CSceneUtility util(pScene);
	m_pActor = util.CreateBoxActor( edge_lengths * 0.5f, world_pose, mass );
}


void CRigidBodyObject::Render()
{
	if( !m_pActor )
		return;

	Matrix34 pose = m_pActor->GetWorldPose();

	FixedFunctionPipelineManager().SetWorldTransform( pose );

	shared_ptr<CBasicMesh> pMesh = m_Mesh.GetMesh();
	if( pMesh )
		pMesh->Render();
}


void CClothObject::Release()
{
//	CMeshData mesh_data;
//	m_pCloth->SetMeshData( mesh_data );

	if( m_pScene )
		m_pScene->ReleaseCloth( m_pCloth );
}


void CClothObject::CreateFromMesh( physics::CScene *pScene, const std::string& mesh_filepath, const Matrix34& world_pose )
{
	m_pScene = pScene;

	CSceneUtility util(m_pScene);

	bool mesh_loaded = m_Mesh.LoadFromFile( mesh_filepath );
	if( mesh_loaded )
		m_pCloth = util.CreateClothFromMesh( m_Mesh, world_pose, true );
}


void CClothObject::Render()
{
//	if( !m_pCloth )
//		return;

	Matrix34 pose = Matrix34Identity();//m_pCloth->GetWorldPose();

	FixedFunctionPipelineManager().SetWorldTransform( pose );

	GetCustomMeshRenderer().RenderMesh( m_Mesh );
}



CTestMeshHolder::CTestMeshHolder()
:
m_LoadingStyle(LOAD_SYNCHRONOUSLY),
m_Pose(Matrix34Identity())
{
}


CTestMeshHolder::CTestMeshHolder( const std::string& filepath, LoadingStyleName loading_style, const Matrix34& pose )
:
m_LoadingStyle(loading_style),
m_Pose(pose)
{
	m_MeshDesc.ResourcePath = filepath;

	if( loading_style == LOAD_MESH_AND_TEX_TOGETHER )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::ASYNCHRONOUS;
		m_MeshDesc.LoadOptionFlags = MeshLoadOption::LOAD_TEXTURES_ASYNC;
	}
	else if( loading_style == LOAD_MESH_AND_TEX_SEPARATELY )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::SYNCHRONOUS;
		m_MeshDesc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;
		m_Handle.Load( m_MeshDesc );
	}
	else if( loading_style == LOAD_SYNCHRONOUSLY )
	{
		m_MeshDesc.LoadingMode = CResourceLoadingMode::SYNCHRONOUS;
		m_Handle.Load( m_MeshDesc );
	}
//	else
//	{
//	}
}


void CTestMeshHolder::Load()
{
	m_Handle.Load( m_MeshDesc );
}



CClothTest::CClothTest()
:
m_pPhysScene(NULL),
m_PhysTimestep(0.02f),
m_PhysOverlapTime(0),
m_StartPhysicsSimulation(false)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );

	g_Camera.SetPosition( Vector3( 0, 2, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );

	m_vWindForce.current = Vector3(0,0,0);
	m_vWindForce.target  = Vector3(0,0,0);
	m_vWindForce.vel     = Vector3(0,0,0);
	m_vWindForce.smooth_time = 1.0f;
}


CClothTest::~CClothTest()
{
	if( m_pPhysScene )
	{
		for( size_t i=0; i<m_RigidBodies.size(); i++ )
			m_RigidBodies[i].Release();
//		m_RigidBody.Release();
		m_Cloth.Release();

		PhysicsEngine().ReleaseScene( m_pPhysScene );
	}
}


void CClothTest::CreateSampleUI()
{
}


void CClothTest::SetLights()
{
//	CShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	CShaderManager *pShaderMgr = &FixedFunctionPipelineManager();
	if( !pShaderMgr )
		return;

	CShaderLightManager *pShaderLightMgr = pShaderMgr->GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	CDirectionalLight dir_light;
	dir_light.DiffuseColor = SFloatRGBColor(1,1,1);
	dir_light.fIntensity = 1.0f;
	dir_light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -2.5f, -0.9f ) );
	pShaderLightMgr->SetDirectionalLight( dir_light );
	pShaderLightMgr->CommitChanges();

	bool set_pnt_light = true;
	if( set_pnt_light )
	{
		CPointLight pnt_light;
		pnt_light.DiffuseColor = SFloatRGBColor(1,1,1);
		pnt_light.fIntensity = 1.0f;
		pnt_light.vPosition = Vector3( 0.3f, 3.5f, -1.9f );
		pnt_light.fAttenuation[0] = 1.0f;
		pnt_light.fAttenuation[1] = 1.0f;
		pnt_light.fAttenuation[2] = 0.1f;
		pShaderLightMgr->SetPointLight( pnt_light );
		pShaderLightMgr->CommitChanges();
	}

/*	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );
*/}


bool CClothTest::InitShader()
{
	// initialize shader
/*	bool shader_loaded = m_Shader.Load( "shaders/glsl_test.vert|shaders/glsl_test.frag" );
	if( !shader_loaded )
		return false;
*/

//	string shader_filepath = LoadParamFromFile<string>( "config", "Shader" );

	SetLights();

	Matrix44 matProj
		= Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );

	if( m_Shader.GetShaderManager() )
		m_Shader.GetShaderManager()->SetProjectionTransform( matProj );

	return true;
}

/*
CActor *CreateBoxActor( const Vector3& edge_lengths, Scalar mass, const Matrix34& world_pose )
{
}
*/


void AttachClothToActor( CCloth& cloth, CActor& actor )
{
	for( int i=0; i<actor.GetNumShapes(); i++ )
		cloth.AttachToShape( actor.GetShape(i), 0 );
}


void CClothTest::InitPhysicsEngine()
{
	physics::PhysicsEngine().Init();

	CSceneDesc scene_desc;
	m_pPhysScene = PhysicsEngine().CreateScene( scene_desc );
	if( !m_pPhysScene )
		return;

	CMaterialDesc mat_desc;
	mat_desc.StaticFriction  = 1.5f;
	mat_desc.DynamicFriction = 1.2f;
	mat_desc.Restitution     = 0.5f;
	physics::CMaterial *pMat = m_pPhysScene->CreateMaterial( mat_desc );
	int mat_id = pMat->GetMaterialID();

	m_pPhysScene->SetGravity( Vector3(  0.0f, -9.8f, 0.0f ) );

	/* -------- collision group and attaching of cloth to actor --------

	               cloth   coll obj   attach obj  regular obj
	------------------------------------------------------------
	0) cloth       No      Yes        No          Yes(*)
	1) coll obj    ---     No         No          No
	2) attach obj  ---     ---        No          No
	3) regular obj ---     ---        ---         Yes

	 - A cloth is attached to an actor even if they do not belong to the same collision group
	   as long as they overlap
	 - (*) In actual game application, this depends on the type of cloth
	   - cloth thats should intract with objects in game world, e.g. flags. -> Yes
	   - cloth that are local to each entity, e.g. a character's hair, cloth, etc. -> No
	*/
	m_pPhysScene->SetGroupCollisionFlag( 0, 0, false );
	m_pPhysScene->SetGroupCollisionFlag( 0, 1, true );
	m_pPhysScene->SetGroupCollisionFlag( 0, 2, false );
	m_pPhysScene->SetGroupCollisionFlag( 0, 3, true );
	m_pPhysScene->SetGroupCollisionFlag( 1, 1, false );
	m_pPhysScene->SetGroupCollisionFlag( 1, 2, false );
	m_pPhysScene->SetGroupCollisionFlag( 1, 3, false );
	m_pPhysScene->SetGroupCollisionFlag( 2, 2, false );
	m_pPhysScene->SetGroupCollisionFlag( 2, 3, false );
	m_pPhysScene->SetGroupCollisionFlag( 3, 3, true );

//	m_pCloths.resize( 2 );

	m_RigidBodies.resize( 7 );
//	m_RigidBodies[0].InitStaticBox( m_pPhysScene, Vector3(1,1,1) );

	// ground
	m_RigidBodies[1].InitStaticBox( m_pPhysScene, Vector3(50.0f,1.0f,50.0f), Matrix34( Vector3(0.0f,-0.5f,0.0f), Matrix33Identity() ) );
	m_RigidBodies[1].m_pActor->SetCollisionGroup( 3 );

	// collision tests with cloth
	m_RigidBodies[2].InitStaticBox( m_pPhysScene, Vector3(0.3f,0.3f,0.3f), Matrix34( Vector3(0.0f,1.0f,0.0f), Matrix33Identity() ) );
	m_RigidBodies[2].m_pActor->SetCollisionGroup( 1 );

	m_RigidBodies[3].InitBox( m_pPhysScene, Vector3(1,1,1), Matrix34( Vector3(5, 5,0), Matrix33Identity() ), 1.0f, SFloatRGBAColor::Red() );
	m_RigidBodies[4].InitBox( m_pPhysScene, Vector3(1,1,1), Matrix34( Vector3(5, 7,0), Matrix33Identity() ), 1.0f, SFloatRGBAColor::Green() );
	m_RigidBodies[5].InitBox( m_pPhysScene, Vector3(1,1,1), Matrix34( Vector3(5, 9,0), Matrix33Identity() ), 1.0f, SFloatRGBAColor::Blue() );
	m_RigidBodies[6].InitBox( m_pPhysScene, Vector3(1,1,1), Matrix34( Vector3(5,11,0), Matrix33Identity() ), 1.0f, SFloatRGBAColor::Yellow() );
	m_RigidBodies[3].m_pActor->SetCollisionGroup( 3 );
	m_RigidBodies[4].m_pActor->SetCollisionGroup( 3 );
	m_RigidBodies[5].m_pActor->SetCollisionGroup( 3 );
	m_RigidBodies[6].m_pActor->SetCollisionGroup( 3 );

	CSceneUtility util(m_pPhysScene);
/*	CActor *pBoxActor = util.CreateStaticBoxActor( Vector3(1,1,1), Matrix34Identity() );
	InitStaticBox*/
//	pBoxActor->SetWorldPose( Matrix34Identity() );

	// create cloth

	// horizontal
//	Vector3 box_edges = Vector3(0.2f,0.2f,0.2f);
//	Matrix34 box_pose = Matrix34( Vector3(0.0f,2.0f,0.0f), Matrix33Identity() );
//	Matrix33 cloth_rotation = Matrix33RotationX( (float)PI * 0.5f );
//	Matrix34 cloth_pose = Matrix34( Vector3(0,2,0), cloth_rotation );

	// vertical
	Vector3 box_edges = Vector3(1.2f,0.2f,0.2f);
	Matrix34 box_pose = Matrix34( Vector3(0.0f,2.0f,0.0f), Matrix33Identity() );
	Matrix34 cloth_pose
		= Matrix34( Vector3(0,2.0f,0), Matrix33Identity() )
		* Matrix34( Vector3(0,0,0), Matrix33RotationX(deg_to_rad(45.0f)) )
		* Matrix34( Vector3(0,-0.5f,0), Matrix33Identity() );

	// cloth and box to attach the cloth to
	string mesh_filepath = LoadParamFromFile<string>( "params.txt", "ClothMesh" );
	m_Cloth.CreateFromMesh( m_pPhysScene, "models/tex_cloth-10x10.msh", cloth_pose );
	m_RigidBodies[0].InitKinematicBox( m_pPhysScene, box_edges, box_pose );
	m_RigidBodies[0].m_pActor->SetCollisionGroup( 2 );

	if( m_Cloth.m_pCloth )
	{
		m_Cloth.m_pCloth->SetWindAcceleration( Vector3(0,1,0) );
		m_Cloth.m_pCloth->SetExternalAcceleration( Vector3( -3.0f,  0.0f, 0.0f ) );

		m_Cloth.m_pCloth->SetGroup( 0 );
	}

	if( m_Cloth.m_pCloth && m_RigidBodies[0].m_pActor )
	{
		AttachClothToActor( *(m_Cloth.m_pCloth), *(m_RigidBodies[0].m_pActor) );
	}

//	m_pCloths[0]->AttachToShape( NULL, 0 );
}


int CClothTest::Init()
{
//	m_pFont = shared_ptr<CFontBase>( new CFont( "Bitstream Vera Sans Mono", 16, 16 ) );
	shared_ptr<CTextureFont> pTexFont( new CTextureFont );
	pTexFont->InitFont( GetBuiltinFontData( "BitstreamVeraSansMono-Bold-256" ) );
	pTexFont->SetFontSize( 8, 16 );
	m_pFont = pTexFont;

	m_vecMesh.push_back( CTestMeshHolder() );
	shared_ptr<CBoxMeshGenerator> pBoxMeshGenerator( new CBoxMeshGenerator() );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) );
	pBoxMeshGenerator->SetDiffuseColor( SFloatRGBAColor::White() );
	m_vecMesh.back().m_MeshDesc.pMeshGenerator = pBoxMeshGenerator;
//	m_vecMesh.back().m_MeshDesc.OptionFlags |= GraphicsResourceOption::DONT_SHARE;
	m_vecMesh.back().Load();

/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

/*	string mesh_file[] =
	{
		"./models/sample_level_00.msh", // manually load textures
		"./models/FlakySlate.msh",      // load mesh and texture asnchronously
		"./models/HighAltitude.msh",
		"./models/RustPeel.msh",
		"./models/SmashedGrayMarble.msh"
	};

	BOOST_FOREACH( const string& filepath, mesh_file )
	{
		m_vecMesh.push_back( CMeshObjectHandle() );

		CMeshResourceDesc desc;
		desc.ResourcePath = filepath;

		if( m_TestAsyncLoading )
			desc.LoadOptionFlags = MeshLoadOption::DO_NOT_LOAD_TEXTURES;

		m_vecMesh.back().Load( desc );
	}
*/
/*
	m_vecMesh.push_back( CTestMeshHolder( "./models/sample_level_00.msh",   CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY, Matrix34( Vector3(0,0,0), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/FlakySlate.msh",        CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1, 100), Matrix33Identity() ) ) );
	m_vecMesh.push_back( CTestMeshHolder( "./models/HighAltitude.msh",      CTestMeshHolder::LOAD_MESH_AND_TEX_SEPARATELY,   Matrix34( Vector3(-25,1, 100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/RustPeel.msh",          CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3( 25,1,-100), Matrix33Identity() ) ) );
//	m_vecMesh.push_back( CTestMeshHolder( "./models/SmashedGrayMarble.msh", CTestMeshHolder::LOAD_MESH_AND_TEX_TOGETHER,   Matrix34( Vector3(-25,1,-100), Matrix33Identity() ) ) );
*/

	// init shader
	InitShader();

//	if( m_TestAsyncLoading )
//		CreateCachedResources();

	InitPhysicsEngine();

	return 0;
}

void CClothTest::UpdatePhysics( float dt )
{
	if( !m_StartPhysicsSimulation )
		return;

	if( !m_pPhysScene )
		return;

	Scalar total_time = dt + m_PhysOverlapTime;

	if (total_time > 0.1f)
		total_time = 0.1f;

	// split the timestep into fixed size chunks

	int num_loops = (int) (total_time / m_PhysTimestep);
	Scalar timestep = m_PhysTimestep;

	if ( false /*m_allow_smaller_timesteps*/ )
	{
		if (num_loops == 0)
			num_loops = 1;
		timestep = total_time / num_loops;
	}

	m_PhysOverlapTime = total_time - num_loops * timestep;
	for(int i=0; i<num_loops ; ++i)
	{
//		PROFILE_SCOPE( "Physics Simulation" );

		// handle the motions and collisions of rigid body entities
		m_pPhysScene->Simulate( timestep );

		while( !m_pPhysScene->FetchResults( physics::SimulationStatus::RigidBodyFinished ) )
		{}
	}
}


void CClothTest::Update( float dt )
{
	if( m_pSampleUI )
		m_pSampleUI->Update( dt );

	float pitch = 0, yaw = 0;
	GaussianRand(pitch,yaw);
	m_vWindForce.target
		= Matrix33RotationY(yaw)
		* Matrix33RotationX(pitch)
		* Vector3(0,0,1);

	Vec3Normalize( m_vWindForce.target, m_vWindForce.target );
	m_vWindForce.target *= 5.0f;

	m_vWindForce.Update( dt );

	if( m_Cloth.m_pCloth )
		m_Cloth.m_pCloth->SetWindAcceleration( m_vWindForce.current );

	UpdatePhysics( dt );

/*
	if( true //m_TestAsyncLoading/ )
	{
//		LoadTexturesAsync();

		BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
		{
			//if( holder.m_LoadingStyle != CTestMeshHolder::LOAD_SYNCHRONOUSLY )
				LoadResourcesAsync( holder );
		}
	}

	if( !GraphicsResourceManager().IsAsyncLoadingAllowed() )
	{
		// async loading is not enabled
		// - The primary thread (this thread) loads the resources from the disk/memory.
		AsyncResourceLoader().ProcessResourceLoadRequest();
	}*/
}


void CClothTest::RenderMeshes()
{
	LPDIRECT3DDEVICE9 pd3dDevice = DIRECT3D9.GetDevice();

	GraphicsDevice().SetRenderState( RenderStateType::DEPTH_TEST, true );
/*
//	CShaderManager *pShaderManager = m_Shader.GetShaderManager();
	if( !pShaderManager )
		return;

	// render the scene

	pShaderManager->SetViewerPosition( g_Camera.GetPosition() );
*/
	ShaderManagerHub.PushViewAndProjectionMatrices( g_Camera );

	for( size_t i=0; i<m_RigidBodies.size(); i++ )
		m_RigidBodies[i].Render();

	m_Cloth.Render();

/*
	pShaderManager->SetTechnique( m_MeshTechnique );
//	BOOST_FOREACH( CMeshObjectHandle& mesh, m_vecMesh )
	BOOST_FOREACH( CTestMeshHolder& holder, m_vecMesh )
	{
		if( holder.m_Handle.GetEntryState() == GraphicsResourceState::LOADED )
		{
			// set world transform
			const Matrix34 mesh_world_pose = holder.m_Pose;
//			FixedPipelineManager().SetWorldTransform( mesh_world_pose );
			pShaderManager->SetWorldTransform( mesh_world_pose );

			shared_ptr<CBasicMesh> pMesh = holder.m_Handle.GetMesh();

			if( pMesh )
				pMesh->Render( *pShaderManager );
		}
	}
*/
	ShaderManagerHub.PopViewAndProjectionMatrices_NoRestore();
}


void CClothTest::Render()
{
	PROFILE_FUNCTION();

	RenderMeshes();

	if( m_pSampleUI )
		m_pSampleUI->Render();

	Vector2 vTopLeft(     (float)GetWindowWidth() / 4,  (float)16 );
	Vector2 vBottomRight( (float)GetWindowWidth() - 16, (float)GetWindowHeight() * 3 / 2 );
/*	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
	rect.Draw();
*/
	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
	m_pFont->DrawText( m_TextBuffer, vTopLeft );

	Vector3 vCamPos = g_Camera.GetPosition();
	m_pFont->DrawText(
		fmt_string( "x: %f\ny: %f\nz: %f\n", vCamPos.x, vCamPos.y, vCamPos.z ),
		Vector2( 20, 300 ) );
}



void CClothTest::MoveClothHolderActor( const Vector3& dist )
{
	CActor *pActor = m_RigidBodies[0].m_pActor;
	if( !pActor )
		return;

	Matrix34 pose = pActor->GetWorldPose();
	pose.vPosition += dist;
	pActor->SetWorldPose( pose );
}


void CClothTest::HandleInput( const SInputData& input )
{
	if( m_pUIInputHandler )
	{
//		CInputHandler::ProcessInput() does not take const SInputData&
		SInputData input_copy = input;
		m_pUIInputHandler->ProcessInput( input_copy );

		if( m_pUIInputHandler->PrevInputProcessed() )
			return;
	}

	static int m_CurrentMouseX = -1;
	static int m_CurrentMouseY = -1;
	static bool m_MouseLButtonPressed = false;
	static bool m_MouseRButtonPressed = false;
	static bool m_ShiftKeyPressed = false;

	int x=0,y=0;//,dx=0,dy=0;

	switch( input.iGICode )
	{
	case 'C':
	case GIC_LSHIFT:
		m_ShiftKeyPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		m_CurrentMouseX = -1;
		m_CurrentMouseY = -1;
		break;
	case GIC_MOUSE_BUTTON_L:
		m_MouseLButtonPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		break;
	case GIC_MOUSE_BUTTON_R:
		m_MouseRButtonPressed = ( input.iType == ITYPE_KEY_PRESSED ) ? true : false;
		break;
	case GIC_MOUSE_AXIS_X:
		if( !m_StartPhysicsSimulation || !m_ShiftKeyPressed )
		{
			m_CurrentMouseX = x;
			m_CurrentMouseY = y;
			break;
		}

		x = (int)input.GetParamH16();
		if( m_CurrentMouseX < 0 )
			m_CurrentMouseX = x; // init
		else
		{
			MoveClothHolderActor( Vector3( (float)(x - m_CurrentMouseX) * 0.005f, 0.0f, 0.0f ) );
			m_CurrentMouseX = x;
		}
		break;
	case GIC_MOUSE_AXIS_Y:
		if( !m_StartPhysicsSimulation || !m_ShiftKeyPressed )
		{
			m_CurrentMouseX = x;
			m_CurrentMouseY = y;
			break;
		}

		y = (int)input.GetParamL16();
		if( m_CurrentMouseY < 0 )
			m_CurrentMouseY = y; // init
		else
		{
			MoveClothHolderActor( Vector3( 0.0f, (float)(y - m_CurrentMouseY) * 0.005f, 0.0f ) );
			m_CurrentMouseY = y;
		}
		break;
	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			SaveTexturesAsImageFiles();
		}
		break;
	case GIC_SPACE:
		if( input.iType == ITYPE_KEY_PRESSED )
			m_StartPhysicsSimulation = true;
		break;
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	default:
		break;
	}
}


void CClothTest::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void CClothTest::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
//	CreateSampleUI();
}
