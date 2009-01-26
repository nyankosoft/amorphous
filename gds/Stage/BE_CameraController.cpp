#include "BE_CameraController.h"
#include "BE_ScriptedCamera.h"

#include "GameMessage.h"
#include "CopyEntityDesc.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "ScreenEffectManager.h"
#include "Serialization_BaseEntityHandle.h"
#include "EntityMotionPathRequest.h"

#include "GameInput/InputHub.h"
#include "Input/InputHandler_Cutscene.h"

#include "Support/Log/DefaultLog.h"


using namespace std;
using namespace boost;


static const U32 gs_FadeoutTimeMS = 500;


CTextureRenderTarget CBE_CameraController::ms_aTextureRenderTarget[NUM_MAX_ACTIVE_CAMERAS];

bool CBE_CameraController::ms_TextureRenderTargetsInitialized = false;

uint CBE_CameraController::ms_NumAvailableTextureRenderTargets = 0;


CBE_CameraController::CBE_CameraController()
:
m_pInputHandler(NULL),
m_bEndingCutscene(false),
m_CutsceneEndStartedTime(0)
{
	m_bLighting = false;
	m_bNoClip = true;

	m_bUseCutsceneInputHandler = true;
}


CBE_CameraController::~CBE_CameraController()
{
	SafeDelete( m_pInputHandler );
}


void CBE_CameraController::Init()
{
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;

	m_pInputHandler = new CInputHandler_Cutscene( this );

	// create texture render targets
	// TODO: Do this only if necessary

	if( ms_TextureRenderTargetsInitialized == false )
	{
		for( int i=0; i<NUM_MAX_ACTIVE_CAMERAS; i++ )
		{
			bool res = ms_aTextureRenderTarget[i].InitScreenSizeRenderTarget();
			if( res )
				ms_NumAvailableTextureRenderTargets++;
		}
	}
}


void CBE_CameraController::InitCopyEntity(CCopyEntity* pCopyEnt)
{
}


/// returns the number of cameras that should be rendering the stage right now.
void CBE_CameraController::GetActiveCameraIndices( CCopyEntity* pCopyEnt,
												   TCFixedVector<uint,CCopyEntity::NUM_MAX_CHILDREN_PER_ENTITY>& active_cam_indices )
{
	const float time_in_stage = (float)m_pStage->GetElapsedTime(); // [sec]
	for( int i=0; i<pCopyEnt->GetNumChildren(); i++ )
	{
		shared_ptr<CCopyEntity> pCameraEntity = pCopyEnt->m_aChild[i].Get();

		shared_ptr<CScriptedCameraEntity> pScriptedCamEntity
			= boost::dynamic_pointer_cast<CScriptedCameraEntity,CCopyEntity>( pCameraEntity );

		if( pScriptedCamEntity->GetPath().IsAvailable( time_in_stage ) )
			active_cam_indices.push_back( (uint)i );
	}
}


void CBE_CameraController::Act(CCopyEntity* pCopyEnt)
{
	m_pInputHandler->SetEntity( pCopyEnt );

	int i=0;

	float time_in_stage = (float)m_pStage->GetElapsedTime(); // [sec]
	bool camera_active = false;
	while( i < pCopyEnt->GetNumChildren() ) // i.e. i < num_cameras
	{
		shared_ptr<CCopyEntity> pCameraEntity = pCopyEnt->m_aChild[i].Get();

		if( !pCameraEntity )
			continue;

		if( pCameraEntity->pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_SCRIPTEDCAMERA )
			continue;

		// update camera
		pCameraEntity->pBaseEntity->Act( pCameraEntity.get() );

		shared_ptr<CScriptedCameraEntity> pScriptedCamEntity
			= boost::dynamic_pointer_cast<CScriptedCameraEntity,CCopyEntity>( pCameraEntity );

		if( pScriptedCamEntity->GetPath().IsExpired( time_in_stage ) )
		{
			// remove the scripted camera
			// - pCopyEnt->GetNumChildren() is decremented.
			CCopyEntity *pCameraRawPtr = pCameraEntity.get();
			m_pStage->TerminateEntity( pCameraRawPtr );
		}
		else
		{
			// the scripted camera still has some path to follow
			camera_active |= pScriptedCamEntity->GetPath().IsAvailable( time_in_stage );
			i++;
		}

	}

	CCopyEntity *pCurrentCameraEntity = m_pStage->GetEntitySet()->GetCameraEntity();
	if( camera_active && pCurrentCameraEntity != pCopyEnt )
	{
		// The controller has one more cameras that should be activated
		// - start the cut scene
		LOG_PRINT( "- Starting a cut scene" );

		if( pCurrentCameraEntity )
			m_PrevCameraEntity = CEntityHandle<>( pCurrentCameraEntity->Self() );
		else
			m_PrevCameraEntity = CEntityHandle<>();

		// set the camera controller entity as the camera entity of the stage
		m_pStage->GetEntitySet()->SetCameraEntity( pCopyEnt );

		if( m_bUseCutsceneInputHandler )
            InputHub().PushInputHandler( m_pInputHandler );
	}
	else if( !camera_active && pCurrentCameraEntity == pCopyEnt )
	{
		// All the camera should be deactivated now
		// and the controller has been a current camera entity.
		EndCutscene( pCopyEnt );
	}
	else if( 0 < m_CutsceneEndStartedTime
		&& gs_FadeoutTimeMS < m_pStage->GetElapsedTimeMS() - m_CutsceneEndStartedTime )
	{
		// Player has chosen to skip the cutscene
		// and the fade out effect is complete.
		EndCutscene( pCopyEnt );
	}
}


void CBE_CameraController::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_SET_MOTION_PATH:
		{
		// set motion path
//		EntityMotionPathRequest *pReq = (EntityMotionPathRequest *)rGameMessage.pUserData;
//		m_Path.SetKeyPoses( pReq->vecKeyPose );
//		m_Path.SetMotionPathType( pReq->MotionPathType );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		return;
		}
	}
}


void CBE_CameraController::RenderStage( CCopyEntity* pCopyEnt )
{
/*	int num_cameras = pCopyEnt->GetNumChildren();

	if( num_cameras == 0 )
	{
		LOG_PRINT_WARNING( " - No camera entity" );
		return;
	}

	num_cameras = 1;
*/
	TCFixedVector<uint,CCopyEntity::NUM_MAX_CHILDREN_PER_ENTITY> active_cam_indices;

	GetActiveCameraIndices( pCopyEnt, active_cam_indices );
	uint num_active_cameras = (uint)active_cam_indices.size();

	if( NUM_MAX_ACTIVE_CAMERAS < num_active_cameras )
		num_active_cameras = NUM_MAX_ACTIVE_CAMERAS;

	if( 0 < num_active_cameras /*num_active_cameras == 1*/ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[active_cam_indices[0]].GetRawPtr();
		if( IsValidEntity(pCameraEntity) )
			pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );
	}
	else if( 2 <= num_active_cameras )
	{
		// render the stage to different texture render targets
		for( uint i=0; i<num_active_cameras; i++ )
		{
			CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[active_cam_indices[i]].GetRawPtr();

			if( !IsValidEntity(pCameraEntity) )
				continue;

			ms_aTextureRenderTarget[i].SetRenderTarget();

			pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );

			ms_aTextureRenderTarget[i].ResetRenderTarget();
		}
	}
/*
	for( uint i=0; i<num_active_cameras; i++ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[i].GetRawPtr();

		// render stage with the camera
//		pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );

		pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );
	}*/
}


void CBE_CameraController::CreateRenderTasks(CCopyEntity* pCopyEnt)
{
	int num_cameras = pCopyEnt->GetNumChildren();

	if( num_cameras == 0 )
	{
		LOG_PRINT_WARNING( " - No camera entity" );
		return;
	}

	num_cameras = 1;

	for( int i=0; i<num_cameras; i++ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[i].GetRawPtr();

		if( !IsValidEntity(pCameraEntity) )
			continue;

		// render stage with the camera
//		pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );

		pCameraEntity->pBaseEntity->CreateRenderTasks( pCameraEntity );
	}
}


void CBE_CameraController::SkipCutscene( CCopyEntity* pCopyEnt )
{
	m_CutsceneEndStartedTime = m_pStage->GetElapsedTimeMS();

	float fade_out_time_margin = 0.1f;
	float fade_out_time = (float)gs_FadeoutTimeMS * 0.001f + fade_out_time_margin;

	m_pStage->GetScreenEffectManager()->FadeOutTo( 0xFF000000, fade_out_time, AlphaBlend::InvSrcAlpha );
}


void CBE_CameraController::EndCutscene( CCopyEntity* pCopyEnt )
{
	// end the cut scene
	LOG_PRINT( " Ending a cut scene" );

	CCopyEntity *pPrevCamEntity = m_PrevCameraEntity.GetRawPtr();
	if( pPrevCamEntity )
	{
		// set the previous camera entity
		m_pStage->GetEntitySet()->SetCameraEntity( pPrevCamEntity );
		m_PrevCameraEntity = CEntityHandle<>();
	}

	// terminate all the camera entities
	const int num_cameras = pCopyEnt->GetNumChildren();
	for( int i=0; i<num_cameras; i++ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[i].GetRawPtr();

		if( !pCameraEntity )
			continue;

		m_pStage->TerminateEntity( pCameraEntity );
	}

	pCopyEnt->iNumChildren = 0;

	if( m_bUseCutsceneInputHandler )
	{
		// set the previous input handler
		InputHub().PopInputHandler();
	}

	m_CutsceneEndStartedTime = 0;

	m_pStage->GetScreenEffectManager()->FadeInFrom( 0xFF000000, 0.2f, AlphaBlend::InvSrcAlpha );
}


bool CBE_CameraController::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBaseEntity::LoadSpecificPropertiesFromFile( scanner );

	if( scanner.TryScanBool( "CUTSCENE", "YES/NO", m_bUseCutsceneInputHandler ) )
		return true;

	return false;
}


void CBE_CameraController::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_bUseCutsceneInputHandler;
}
