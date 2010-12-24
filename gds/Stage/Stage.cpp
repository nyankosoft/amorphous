#include "Stage/Stage.hpp"
#include "Stage/BSPMap.hpp"
#include "Stage/StaticGeometry.hpp"
#include "Stage/EntitySet.hpp"
#include "Stage/EntityRenderManager.hpp"
#include "Stage/trace.hpp"
#include "Stage/ViewFrustumTest.hpp"
#include "Stage/CopyEntityDesc.hpp"
#include "Stage/ScreenEffectManager.hpp"
#include "Stage/SurfaceMaterialManager.hpp"
#include "Stage/Input/InputHandler_StageDebug.hpp"
#include "Script/PyModule_3DMath.hpp"
#include "Script/PyModule_Graphics.hpp"
#include "Script/PyModule_sound.hpp"
#include "Script/PyModule_stage.hpp"
#include "Script/PyModule_StageUtility.hpp"
#include "Script/PyModule_visual_effect.hpp"
#include "Script/PyModules.hpp"
#include "Script/ScriptManager.hpp"
#include "Support/Timer.hpp"
#include "Support/macro.h"
#include "Support/memory_helpers.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/lfs.hpp"

#include "Physics/PhysicsEngine.hpp"
#include "Physics/SceneDesc.hpp"
#include "Physics/Scene.hpp"
#include "Physics/MaterialDesc.hpp"
#include "Physics/Material.hpp"
#include "Sound/SoundManager.hpp"
#include "Input/InputHub.hpp"


static uint gs_DebugInputHandlerIndex = 0;


using namespace std;
using namespace boost;
using namespace physics;


void SetStageForScriptCallback( CStage* pStage )
{
	SetStageForStageScriptCallback( pStage );
	gsf::py::entity::SetStageForEntityScriptCallback( pStage );
}


CStage::CStage()
:
//m_pPhysicsManager(NULL),
m_pPhysicsScene(NULL),
//m_pPhysicsVisualizer(NULL),
m_pMaterialManager(NULL),
m_pScriptManager(NULL),
m_pStaticGeometry(NULL),
m_pCamera(NULL),
m_pStageDebugInputHandler(NULL)
{
	// debug - see the thread id
	boost::thread::id thread_id = boost::this_thread::get_id();

//	m_pTimer = new PrecisionTimer();
	m_pTimer = new CTimer();
	PauseTimer();	// don't start until the initialization is complete

	m_pScreenEffectManager.reset( new CScreenEffectManager );
	m_pScreenEffectManager->Init();

	m_pEntitySet = new CEntitySet( this );

	m_pScreenEffectManager->SetTargetSceneRenderer( m_pEntitySet->GetRenderManager().get() );

	CBSPTreeForAABB::Initialize();

	// create the script manager so that an application can add custom modules
	// before calling CStage::Initialize() and running scripts for initialization
	m_pScriptManager = new CScriptManager;
}


CStage::~CStage()
{
	LOG_FUNCTION_SCOPE();

	for( size_t i=0; i<m_vecpMaterial.size(); i++ )
		m_pPhysicsScene->ReleaseMaterial( m_vecpMaterial[i] );

	SafeDelete( m_pStaticGeometry );
	SafeDelete( m_pEntitySet );
	m_pScreenEffectManager.reset();
	SafeDelete( m_pScriptManager );
//	SafeDelete( m_pPhysicsManager );
	PhysicsEngine().ReleaseScene( m_pPhysicsScene );
	SafeDelete( m_pMaterialManager );

//	SafeDelete( m_pPhysicsVisualizer );

	// release the input handler registered for debugging
	InputHub().RemoveInputHandler( gs_DebugInputHandlerIndex, m_pStageDebugInputHandler );
	SafeDelete( m_pStageDebugInputHandler );

	SafeDelete( m_pTimer );
}


CStaticGeometryBase *CreateStaticGeometry( CStage* pStage, const string& filename )
{
	string ext = lfs::get_ext( filename );

	if( ext == "bspx" ) // bsp level file (deprecated)
		return new CBSPMap( pStage );
	else if( ext == "sga" ) // static geometry archive
//		return new CStaticGeometryFG( pStage );
		return new CStaticGeometry( pStage );
	else
	{
		LOG_PRINT_WARNING( "an invalid static geometry filename: " + filename );
		return NULL;
	}
}


#include "Physics/Actor.hpp"
#include "Physics/Shape.hpp"

class TestTriggerReport : public physics::CUserTriggerReport
{
public:
	void OnTrigger( const CTriggerEvent& trigger_event )
	{
		CShape *pShape0 = trigger_event.pTriggerShape;
		CShape *pShape1 = trigger_event.pOtherShape;
		CActor& actor0 = pShape0->GetActor();
		CActor& actor1 = pShape1->GetActor();
		U32 status = trigger_event.StatusFlags;
		CCopyEntity *pEntity0 = (CCopyEntity *)actor0.m_pFrameworkData;
		CCopyEntity *pEntity1 = (CCopyEntity *)actor1.m_pFrameworkData;

		if( pEntity0 && pEntity1 )
			pEntity0->OnPhysicsTrigger( *pShape0, *pEntity1, *pShape1, status );
	}
};


class TestContactReport : public physics::CUserContactReport
{
public:
	void OnContactNotify( CContactPair& pair, U32 events )
	{
		CActor *pActor0 = pair.pActors[0];
		CActor *pActor1 = pair.pActors[1];
		if( !pActor0 || !pActor1 )
			return;

		CCopyEntity *pEntity0 = (CCopyEntity *)pActor0->m_pFrameworkData;
		CCopyEntity *pEntity1 = (CCopyEntity *)pActor1->m_pFrameworkData;

		if( pEntity0 && pEntity1 )
		{
			pEntity0->OnPhysicsContact( pair, *pEntity1 );
			pEntity1->OnPhysicsContact( pair, *pEntity0 );
		}
	}
};


bool CStage::InitPhysicsManager()
{
	static TestContactReport s_TCR;
	static TestTriggerReport s_TTR;

	// initialize physics scene
	CSceneDesc phys_scene_desc;
	m_pPhysicsScene = PhysicsEngine().CreateScene( phys_scene_desc );
	m_pPhysicsScene->SetUserContactReport( &s_TCR );
	m_pPhysicsScene->SetActorGroupPairFlags(0,0,ContactPairFlag::NOTIFY_ALL);
//	m_pPhysicsScene->SetActorGroupPairFlags(0,0,NX_NOTIFY_ON_START_TOUCH|NX_NOTIFY_ON_TOUCH|NX_NOTIFY_ON_END_TOUCH);

	m_pPhysicsScene->SetUserTriggerReport( &s_TTR );

	// set pairs that don't collide with each other
/*	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_STATICGEOMETRY,ENTITY_COLL_GROUP_STATICGEOMETRY, false );
	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_PLAYER,		ENTITY_COLL_GROUP_ITEM, false );
	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_DOOR,			ENTITY_COLL_GROUP_STATICGEOMETRY, false );
	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_DOOR,			ENTITY_COLL_GROUP_DOOR, false );
	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_PLAYER,		ENTITY_COLL_GROUP_PLAYER, false );
	m_pPhysicsManager->SetCollisionGroupState( ENTITY_COLL_GROUP_NOCLIP,		false );
*/

	// Register a material as a default
	CMaterialDesc mat_desc;
	mat_desc.StaticFriction  = 1.5f;
	mat_desc.DynamicFriction = 1.2f;
	mat_desc.Restitution     = 0.5f;
	physics::CMaterial *pDefaultMaterial = m_pPhysicsScene->CreateMaterial( mat_desc );
	int mat_id = pDefaultMaterial->GetMaterialID();

//	MsgBoxFmt( "physics visualizer created: %d", m_pPhysicsVisualizer );

	return true;
}


bool CStage::LoadBaseEntity( CBaseEntityHandle& base_entity_handle )
{
	return m_pEntitySet->LoadBaseEntity( base_entity_handle );
}


// render stage from the view of an entity which has been specifed as the camera entity
void CStage::Render()
{
	PROFILE_FUNCTION();

	// since no camera has been specified from the client, get the current camera from entity set
	// 9:27 PM 8/26/2007 - changed: camera entity renders the stage using its own rendering routine
	// for example,
	// - camera entity == player
	//   > render the scene and HUD
	// - camera entity == scripted camera for script event (cut scene)
	//   > render the scene

	m_pEntitySet->UpdateCamera();

	CCopyEntity* pCameraEntity = m_pEntitySet->GetCameraEntity();
	if( pCameraEntity )
	{
		pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );
		return;
	}
/*
	CCamera *pCurrentCamera = m_pEntitySet->GetCurrentCamera();
	if( pCurrentCamera )
	{
		Render( *pCurrentCamera );
//		m_pEntitySet->GetCameraEntity()->RenderStage( this );
	}
	else
	{
		m_pCamera = NULL;
		return;	// stage cannot be rendered without a camera
	}*/
}

/*
// render stage by directly providing the camera
void CStage::Render( CCamera& rCam )
{
	// save the current camera
	// the camera must not be released / destroyed untill the Render() finishes
	m_pCamera = &rCam;

///	g_PerformanceCheck.iNumDrawPrimCalls = 0;
//	g_PerformanceCheck.iNumRenderedCopyEntites = 0;

	// set transforms and effect flags
	m_pScreenEffectManager->BeginRender( rCam );

	// entity set uses shader manager held by screen effect manager
	m_pScreenEffectManager->SetShaderManager();

	// next, render the objects in the world (entities)
	// Here, 'entites' also include
	// - static geometry
	// - skybox
	m_pEntitySet->Render( rCam );

	// the scene has been rendered
	// perform post-process effects on the scene
	m_pScreenEffectManager->RenderPostProcessEffects();

	// render physics-related debug info
	// e.g. bounding boxes, contact positions & normals
//	if( m_pPhysicsVisualizer )
//		m_pPhysicsVisualizer->RenderVisualInfo();

	m_pScreenEffectManager->EndRender();

	m_pCamera = NULL;
}*/


// render stage by directly providing the camera
void CStage::Render( CCamera& rCam )
{
	m_pCamera = &rCam;

	m_pScreenEffectManager->Render( rCam );
}


/// Called from external module that wants to let the current camera entity render the stage.
/// Create render tasks necessary for the current camera entity to render the stage,
/// and add them to render task list.
void CStage::CreateRenderTasks()
{
	CCopyEntity* pCameraEntity = m_pEntitySet->GetCameraEntity();
	if( pCameraEntity )
	{
		pCameraEntity->pBaseEntity->CreateRenderTasks( pCameraEntity );
		return;
	}
}


/// called from base entity.
/// Module that wants to render the stage by directly calling CStage::Render()
/// needs to call this function in advance
void CStage::CreateStageRenderTasks( CCamera *pCamera )
{
	// creates render tasks for
	// - envmap
	// - shadowmap

	m_pEntitySet->GetRenderManager()->UpdateCamera( pCamera );

	m_pScreenEffectManager->CreateRenderTasks();

/*	if( m_pScreenEffectManager->UsesExtraRenderTarget() )
	{
		// screen effects are applied after rendering the scene
		// - register the scene rendering as a separate render task
		m_pEntitySet->GetRenderManager()->CreateRenderTasks( *pCamera, true );

		m_pScreenEffectManager->CreateRenderTasks();
	}
	else
	{
		// no screen effects that require texture render target
		// - final scene is rendered in CStage::Render() path
		//   and not registered as a separate render task
		m_pEntitySet->GetRenderManager()->CreateRenderTasks( *pCamera, false );
	}*/
}


void CStage::ClipTrace(STrace& tr)
{
	PROFILE_FUNCTION();

	if( *tr.pvStart == *tr.pvGoal )
	{
		tr.vEnd = *tr.pvStart;
///		tr.fFraction = 0;			// commented out at 17:22 2007/04/15

		return;
	}

	tr.SetAABB();
	tr.vEnd = *tr.pvStart + (*tr.pvGoal - *tr.pvStart) * tr.fFraction;

	if( m_pStaticGeometry && !(tr.sTraceType & TRACETYPE_IGNORE_MAP) )
		m_pStaticGeometry->ClipTrace( tr );

	if( !(tr.sTraceType & TRACETYPE_IGNORE_ALL_ENTITIES) )
		m_pEntitySet->ClipTrace( tr );
}

/*
void CStage::ClipTrace( CJL_LineSegment& segment )
{
	m_pPhysicsManager->ClipLineSegment( segment );
}
*/

//check if a bounding volume is in a valid position
//if it is in a solid position, turn 'tr.in_solid' to 'true' and return
void CStage::CheckPosition(STrace& tr)
{
	// 'tr.aabb_swept' represents the bounding volume of the source entity in world coordinate
	if(tr.bvType == BVTYPE_DOT)
		tr.aabb_swept.SetMaxAndMin( tr.vEnd, tr.vEnd );	//swept volume is just a dot
	else
		tr.aabb_swept.TransformCoord( tr.aabb, tr.vEnd );

	// Check position against map
	if( m_pStaticGeometry && !(tr.sTraceType & TRACETYPE_IGNORE_MAP) )
		m_pStaticGeometry->CheckPosition( tr );

	// Check position against entities
	if( !(tr.sTraceType & TRACETYPE_IGNORE_ALL_ENTITIES) )
		m_pEntitySet->CheckPosition( tr );
}


void CStage::CheckCollision(CTrace& tr)
{
	if( tr.GetBVType() == BVTYPE_SPHERE )
	{	// not checked against map. collision check with entities only.
		if( tr.GetStartPosition() == tr.GetGoalPosition() )
		{
			// check position against entities
			m_pEntitySet->CheckPosition( tr );
		}
	}
	else if( tr.GetBVType() == BVTYPE_DOT )
	{
		if(tr.GetStartPosition() == tr.GetGoalPosition())
			return;
		else
		{
			STrace tr2;
			tr2.bvType = BVTYPE_DOT;
			tr2.pvStart = &tr.GetStartPosition();
			tr2.pvGoal  = &tr.GetGoalPosition();
			tr2.aabb.SetMaxAndMin( Vector3(0,0,0), Vector3(0,0,0) );
			tr2.SetAABB();
			tr2.sTraceType = tr.GetTraceType();
			
			// Check collision with the map (static geometry)
			if( m_pStaticGeometry )
                m_pStaticGeometry->ClipTrace(tr2);

			// Check collision with other entities
			if( !(tr2.sTraceType & TRACETYPE_IGNORE_ALL_ENTITIES) )
				m_pEntitySet->ClipTrace(tr2);

			tr.SetEndFraction( tr2.fFraction );
			tr.SetEndPosition( tr2.vEnd );
			if( tr2.pTouchedEntity )
				tr.AddTouchEntity( tr2.pTouchedEntity );
		}
	}
		return;
}


void CStage::GetVisibleEntities( CViewFrustumTest& vf_test )
{
	static vector<CCopyEntity *> s_vecpTempVisibleEntityBuffer;
	s_vecpTempVisibleEntityBuffer.resize(0);

	// save the original buffer
	vector<CCopyEntity *> *pvecpOrigVisibleEntityBuffer	= vf_test.m_pvecpVisibleEntity;

	// set temporary buffer
	vf_test.SetBuffer( s_vecpTempVisibleEntityBuffer );

	// get potentially visible entities
	m_pEntitySet->GetVisibleEntities(vf_test);

	// check if the potentially visible entities are occluded by the map or other entities
	STrace tr;
	Vector3 vStart = vf_test.GetCamera()->GetPosition();
	Vector3 vGoal;
	int i, iNumPotentiallyVisibles = vf_test.GetNumVisibleEntities();
	if( 300 < iNumPotentiallyVisibles )
		int iTooManyVisibleEntities = 1;

	tr.pvStart = &vStart;
	tr.pvGoal  = &vGoal;
	tr.bvType = BVTYPE_DOT;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

	for( i=0; i<iNumPotentiallyVisibles; i++ )
	{
		vGoal = vf_test.GetEntity(i)->GetWorldPosition();
		tr.fFraction = 1.0f;
		tr.pTouchedEntity = NULL;
//		m_pStaticGeometry->ClipTrace( tr );
		ClipTrace( tr );

//		if( tr.fFraction == 1.0f )
		if( tr.pTouchedEntity == vf_test.GetEntity(i) )
			pvecpOrigVisibleEntityBuffer->push_back( vf_test.GetEntity(i) );	// entity is not obstructed
	}

	// restore original buffer
	vf_test.SetBuffer( *pvecpOrigVisibleEntityBuffer );
}

/*
char CStage::IsCurrentlyVisibleCell(short sCellIndex)
{
	return m_pStaticGeometry->IsCurrentlyVisibleCell( sCellIndex );
}
*/

void CStage::Update( float dt )
{
	PROFILE_FUNCTION();

//	ONCE( MsgBoxFmt( "CStage::Update() - 0x%x", this ) );
	ONCE( g_Log.Print( "CStage::Update() - 0x%x", this ) );

	// update timer for frame time (delta time)
	m_pTimer->UpdateFrameTime();

	ONCE( g_Log.Print( "CStage::Update() - updating all entities" ) );

	// update states of entities
	m_pEntitySet->UpdateAllEntities( dt );

	ONCE( g_Log.Print( "CStage::Update() - updated all entities" ) );

	// event script
	SetStageForScriptCallback( this );
	m_pScriptManager->Update();
	SetStageForScriptCallback( NULL );

//	if( m_pTextMessageManager )
//		m_pTextMessageManager->Update( dt );

	m_pScreenEffectManager->Update( dt );
}


double CStage::GetElapsedTime()
{
//	return m_pTimer->CurrentTime();
	return m_pTimer->GetTime();
}


unsigned long CStage::GetElapsedTimeMS()
{
//	return (unsigned long)(m_pTimer->CurrentTime() * 1000.0f);
	return m_pTimer->GetTimeMS();
}


void CStage::PauseTimer()
{
	if( m_pTimer )
        m_pTimer->Stop();
}


void CStage::ResumeTimer()
{
	if( m_pTimer )
        m_pTimer->Start();
}


void CStage::ReleasePhysicsActor( CActor*& pPhysicsActor )
{
	m_pPhysicsScene->ReleaseActor( pPhysicsActor );
}


Vector3 CStage::GetGravityAccel() const
{
	return Vector3( 0, -9.8f, 0 );
}


CEntityHandle<> CStage::LoadStaticGeometryFromFile( const std::string filename )
{
	SafeDelete( m_pStaticGeometry );
	m_pStaticGeometry = CreateStaticGeometry( this, filename );

	if( !m_pStaticGeometry )
		return CEntityHandle<>();

	// register the static geometry as an entity
	// - the entity is used to render the static geometry

	CBaseEntityHandle baseentity_handle;
	baseentity_handle.SetBaseEntityName( "StaticGeometry" );
	CCopyEntityDesc desc;
	desc.strName = filename;
	desc.pBaseEntityHandle = &baseentity_handle;
	desc.pUserData = m_pStaticGeometry;

	CCopyEntity *pStaticGeometryEntity = CreateEntity( desc );

	if( !pStaticGeometryEntity )
		return CEntityHandle<>();

//	shared_ptr<CStaticGeometryEntity> pEntity( new CStaticGeometryEntity );
//	pEntity->SetStaticGeometry( m_pStaticGeometry );
//	CEntityHandle<CStaticGeometryEntity> entity
//		= CreateEntity( pEntity, baseentity_handle );

	m_pEntitySet->WriteEntityTreeToFile( "debug/entity_tree - loaded static geometry.txt" );

	// load the static geometry from file
	this->PauseTimer();
	bool loaded = m_pStaticGeometry->LoadFromFile( filename );
	this->ResumeTimer();

	return CEntityHandle<>( pStaticGeometryEntity->Self() );
//	return entity;
}


bool CStage::LoadMaterial( /* const string& material_filename */)
{
//	string material_filename = m_strStageFilename;
//	lfs::change_ext( material_filename, "mat" );

	string material_filename = "./Stage/material.bin";

	SafeDelete( m_pMaterialManager );

	m_pMaterialManager = new CSurfaceMaterialManager;
	
	bool b = m_pMaterialManager->LoadFromFile( material_filename );

	if( !b )
		return false;

	// register materials to physics simulator
	int i, iNumMaterials = m_pMaterialManager->GetNumMaterials();
	m_vecpMaterial.resize( iNumMaterials );
	for( i=0; i<iNumMaterials; i++ )
	{
		CSurfaceMaterial& src_material = m_pMaterialManager->GetSurfaceMaterial(i);

		CMaterialDesc desc;
		desc.SetDefault();
		desc.StaticFriction  = src_material.GetPhysicsMaterial().fStaticFriction;
		desc.DynamicFriction = src_material.GetPhysicsMaterial().fDynamicFriction;
		desc.Restitution     = src_material.GetPhysicsMaterial().fElasticity;

		m_vecpMaterial[i] = m_pPhysicsScene->CreateMaterial( desc );
	}

	return true;
}


CSurfaceMaterial& CStage::GetMaterial( int index )
{
	return m_pMaterialManager->GetSurfaceMaterial( index );
}


bool CStage::InitEventScriptManager( const string& script_archive_filename )
{
	LOG_FUNCTION_SCOPE();

	m_ScriptArchiveFilename = script_archive_filename;

	if( CScriptManager::ms_UseBoostPythonModules )
	{
		RegisterPythonModule_math3d();
		RegisterPythonModule_gfx();
		RegisterPythonModule_sound();
		RegisterPythonModule_stage();
		RegisterPythonModule_visual_effect();
		stage_util::RegisterPythonModule_stage_util();
	}
	else
	{
		m_pScriptManager->AddModule( "PlayerInfo",	g_PyModulePlayerMethod );
		m_pScriptManager->AddModule( "Stage",		g_PyModuleStageMethod );
		m_pScriptManager->AddModule( "TextMessage",	g_PyModuleTextMessageMethod );
		m_pScriptManager->AddModule( "Sound",		g_PyModuleSoundMethod );
		m_pScriptManager->AddModule( "Entity",		gsf::py::entity::g_PyModuleEntityMethod );
		m_pScriptManager->AddModule( "HUD",			g_PyModuleHUDMethod );
		m_pScriptManager->AddModule( "cam",			gsf::py::cam::g_PyModuleCameraMethod );
		m_pScriptManager->AddModule( "StageGraph",	g_PyModuleStageGraphMethod );
		m_pScriptManager->AddModule( "gr",			g_PyModuleGraphicsElementMethod );
		m_pScriptManager->AddModule( "gre",			g_PyModuleAnimatedGraphicsMethod );
		m_pScriptManager->AddModule( "Task",			gsf::py::task::g_PyModuleTaskMethod );
		m_pScriptManager->AddModule( "Light",		gsf::py::light::g_PyModuleLightMethod );
		m_pScriptManager->AddModule( "VisualEffect",	gsf::py::ve::g_PyModuleVisualEffectMethod );
	}


	bool res = m_pScriptManager->LoadScriptArchiveFile( script_archive_filename );

	if( !res )
	{
		LOG_PRINT_ERROR( "unable to load script: " + script_archive_filename );
		return false;
	}

	// run scripts for initialization
	ResumeTimer();
	SetStageForScriptCallback( this );
	m_pScriptManager->InitScripts();
	m_pScriptManager->Update();
	SetStageForScriptCallback( NULL );
	PauseTimer();

	return true;
}


bool CStage::Initialize( const string& script_archive_filename )
{
	LOG_FUNCTION_SCOPE();

	// nullify all the copy-entities in 'm_pEntitySet'
	m_pEntitySet->ReleaseAllEntities();

	// creates a physics manager for the stage
	// rigid-body entities are registered to physics engine when created
	InitPhysicsManager();

	// load scripts initialize
	// also run scripts that initializes stage
	// - static geometry is loaded by init script
	// - entities can be created in this call
	InitEventScriptManager( script_archive_filename );

	if( m_pStaticGeometry )
	{
		// make EntityTree from static geometry
		CBSPTree bsptree;
		m_pStaticGeometry->MakeEntityTree( bsptree );
		m_pEntitySet->MakeEntityTree( &bsptree );

		// set dynamic light manager for static geometry
		m_pStaticGeometry->SetDynamicLightManager( m_pEntitySet );

		// m_pEntitySet->RegisterStaticGeometry( m_pStaticGeometry );

		// set shader to the light manager
		// because shader is managed by static geometry
//		m_pEntitySet->GetLightEntityManager()->Init( m_pEntitySet );
	}

	// load the binary material file that has the same body filename as the stage file
	// and different suffix "mat"
	LoadMaterial();

	// register input handler for debugging the stage
	m_pStageDebugInputHandler = new CInputHandler_StageDebug( m_pSelf );
	if( InputHub().GetInputHandler(gs_DebugInputHandlerIndex) )
		InputHub().GetInputHandler(gs_DebugInputHandlerIndex)->AddChild( m_pStageDebugInputHandler );
	else
		InputHub().PushInputHandler( gs_DebugInputHandlerIndex, m_pStageDebugInputHandler );

	// stage has been initialized - start the timer
	ResumeTimer();

	double time = m_pTimer->GetTime();

	return true;
}


void CStage::NotifyEntityTerminationToEventManager( CCopyEntity* pEntity )
{
//	m_pScriptManager->OnCopyEntityDestroyed( pEntity );
}


void CStage::UpdateListener()
{
	Matrix34 cam_pose;
	CCamera *pCamera;

	// get the pose of the current camera
	pCamera = this->GetCurrentCamera();
	if( pCamera )
	{
		pCamera->GetPose( cam_pose );
	}
	else
	{
		CCopyEntity* pEntity = this->GetEntitySet()->GetCameraEntity();
		if( pEntity )
			cam_pose = pEntity->GetWorldPose();
		else
			cam_pose.Identity();
	}

	// update listener for sound manager
	SoundManager().SetListenerPose( cam_pose ); 

	CCopyEntity *pCameraEntity = this->GetEntitySet()->GetCameraEntity();
	if( pCameraEntity )
		SoundManager().SetListenerVelocity( pCameraEntity->Velocity() ); 

	SoundManager().CommitDeferredSettings();
}



/*
CJL_PhysicsVisualizer_D3D *CStage::GetPhysicsVisualizer()
{
	return m_pPhysicsVisualizer;
}
*/

/*
void CStage::PlaySound3D( char* pcSoundName, Vector3& rvPosition )
{
	SoundManager().PlayAt( pcSoundName, rvPosition );
}


void CStage::PlaySound3D( int iIndex, Vector3& rvPosition )
{
	SoundManager().PlayAt( iIndex, rvPosition );
}


void CStage::PlaySound3D( CSoundHandle &rSoundHandle, Vector3& rvPosition )
{
	SoundManager().PlayAt( rSoundHandle, rvPosition );
}
*/


/*
void CStage::SaveCurrentState(FILE* fp)
{
	m_pEntitySet->SaveCurrentCopyEntities( fp );
}

void CStage::LoadSavedData(FILE* fp)
{
	m_pEntitySet->LoadCopyEntitiesFromSavedData( fp );
}
*/
