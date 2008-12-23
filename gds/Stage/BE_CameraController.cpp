
#include "BE_CameraController.h"
#include "BE_ScriptedCamera.h"

#include "GameMessage.h"
#include "CopyEntityDesc.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "ScreenEffectManager.h"
#include "Serialization_BaseEntityHandle.h"

#include "GameInput/InputHub.h"
#include "Input/InputHandler_Cutscene.h"

#include "Support/Log/DefaultLog.h"


static const U32 gs_FadeoutTimeMS = 500;


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
//	Init3DModel();

//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;

	m_pPrevCameraEntity = NULL;

	m_pInputHandler = new CInputHandler_Cutscene( this );
}


void CBE_CameraController::InitCopyEntity(CCopyEntity* pCopyEnt)
{
}



void CBE_CameraController::Act(CCopyEntity* pCopyEnt)
{
	m_pInputHandler->SetEntity( pCopyEnt );

	int i, num_cameras = pCopyEnt->GetNumChildren();

	bool camera_active = false;
	for( i=0; i<num_cameras; i++ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[i].GetRawPtr();

		if( !IsValidEntity(pCameraEntity) )
			continue;

		if( pCameraEntity->pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_SCRIPTEDCAMERA )
			continue;

		// update camera
		pCameraEntity->pBaseEntity->Act( pCameraEntity );

		CBE_ScriptedCamera *pCameraBaseEntity = (CBE_ScriptedCamera *)(pCameraEntity->pBaseEntity);
		CBEC_ScriptedCameraExtraData& ex = pCameraBaseEntity->GetExtraData(pCameraEntity);

		camera_active |= ex.Path.IsAvailable( (float)m_pStage->GetElapsedTime() );
	}

	CCopyEntity *pCurrentCameraEntity = m_pStage->GetEntitySet()->GetCameraEntity();
	if( camera_active && pCurrentCameraEntity != pCopyEnt )
	{
		// start the cut scene
		LOG_PRINT( "- Starting a cut scene" );

		m_pPrevCameraEntity = pCurrentCameraEntity;

		// set the camera controller entity as the camera entity of the stage
		m_pStage->GetEntitySet()->SetCameraEntity( pCopyEnt );

		if( m_bUseCutsceneInputHandler )
            InputHub().PushInputHandler( m_pInputHandler );
	}
	else if( !camera_active && pCurrentCameraEntity == pCopyEnt )
	{
		EndCutscene( pCopyEnt );
	}
	else if( 0 < m_CutsceneEndStartedTime
		&& gs_FadeoutTimeMS < m_pStage->GetElapsedTimeMS() - m_CutsceneEndStartedTime )
	{
		// player has chosen to skip the cutscene
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

		pCameraEntity->pBaseEntity->RenderStage( pCameraEntity );
	}
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
	g_Log.Print( "CBE_CameraController::EndCutscene() - ending a cut scene" );

	// set the previous camera entity
	m_pStage->GetEntitySet()->SetCameraEntity( m_pPrevCameraEntity );
	m_pPrevCameraEntity = NULL;

	// terminate all the camera entities
	const int num_cameras = pCopyEnt->GetNumChildren();
	for( int i=0; i<num_cameras; i++ )
	{
		CCopyEntity *pCameraEntity = pCopyEnt->m_aChild[i].GetRawPtr();

		if( !IsValidEntity(pCameraEntity) )
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
