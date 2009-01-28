#include "BE_ScriptedCamera.h"

#include "GameMessage.h"
#include "CopyEntityDesc.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "ScreenEffectManager.h"

#include "Graphics/Direct3D9.h"
#include "Graphics/3DGameMath.h"

#include "Support/Log/DefaultLog.h"
#include "Support/Vec3_StringAux.h"
#include "Support/Macro.h"
#include "Support/VectorRand.h"


inline static short& HasExpired(CCopyEntity* pCameraEntity) { return pCameraEntity->s1; };


void FocusTargetFrameSet::UpdateFocusTargetEntities( CEntitySet *pEntitySet )
{
	size_t i, num_key_frames = m_vecKeyFrame.size();
	for( i=0; i<num_key_frames; i++ )
	{
		if( 0 < m_vecKeyFrame[i].val.m_TargetName.length() )
		{
			CCopyEntity* pTarget = pEntitySet->GetEntityByName( m_vecKeyFrame[i].val.m_TargetName.c_str() );
			if( IsValidEntity( pTarget ) )
				m_vecKeyFrame[i].val.m_Target = CEntityHandle<>( pTarget->Self() );
		}
	}
}


void CScriptedCameraEntity::Update( float dt )
{
	if( m_Path.IsAvailable( (float)GetStage()->GetElapsedTime() ) )
	{
		if( !m_InitializedAtCutsceneStart )
		{
			m_KeyFrames.Camera.FocusTarget.UpdateFocusTargetEntities( GetStage()->GetEntitySet() );
			m_InitializedAtCutsceneStart = true;
		}

		pBaseEntity->UpdateScriptedMotionPath( this, m_Path );

		PERIODICAL( 100, g_Log.Print( "CScriptedCameraEntity::Update() - updated motion path ... pos: " +
			to_string(this->Position(), 2) ) );

		m_Camera.SetPose( this->GetWorldPose() );
		m_Camera.UpdateVFTreeForWorldSpace();

		UpdateCameraParams();
	}
	else if( m_Path.IsExpired( (float)GetStage()->GetElapsedTime() ) )
	{
		HasExpired(this) = 1;
	}

//	if( m_pStage->GetEntitySet()->GetCameraEntity() == this )
//	{
//		m_Camera.SetPose( this->GetWorldPose() );
//	}
}


//void CBE_ScriptedCamera::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
void CScriptedCameraEntity::HandleMessage( SGameMessage& msg )
{
	switch( msg.iEffect )
	{
	case GM_SET_MOTION_PATH:
	  {
		// set motion path
		EntityMotionPathRequest *pReq = (EntityMotionPathRequest *)msg.pUserData;
		m_Path.SetKeyPoses( pReq->vecKeyPose );
		m_Path.SetMotionPathType( pReq->MotionPathType );

		m_Initialized = true;

		LOG_PRINT( " - added motion path to scripted camera" );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		return;
	  }

	case GM_SET_DEFAULT_CAMERA_PARAM:
	  {
		// set motion path
		CameraParam *pParam = (CameraParam *)msg.pUserData;
		m_DefaultParam = *pParam;

		// TODO: change camera params dynamically
		m_Camera.SetNearClip( m_DefaultParam.nearclip );
		m_Camera.SetFarClip( m_DefaultParam.farclip );
		m_Camera.SetFOV( m_DefaultParam.fov );
		m_Camera.SetAspectRatio( m_DefaultParam.aspect_ratio );

		LOG_PRINT( " - set default camera params" );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		return;
	  }

	case GM_SET_SCRIPTCAMERAKEYFRAMES:
		// set camera effects
		m_KeyFrames = *(CScriptCameraKeyFrames *)msg.pUserData;

		LOG_PRINT( " - set camera effect params" );

		return;
	}
}

void CScriptedCameraEntity::RenderStage()
{
	PERIODICAL( 100, LOG_PRINT( "Camera pos: " + to_string(m_Camera.GetPosition(), 2) ) );

	if( 7.5f < GetStage()->GetElapsedTime() )
		int break_here = 1;

	CScreenEffectManager *pScreenEffectManager = GetStage()->GetScreenEffectManager();

	// save the original settings
	int orig_effect_flag = pScreenEffectManager->GetEffectFlag();

/*	const CPPEffectParams& effect = m_PPEffectParams;
	int effect_flag = effect.flag;
//	int effect_flag = effect.flag | ScreenEffect::MonochromeColor;
	pScreenEffectManager->SetEffectFlag( effect_flag );
	pScreenEffectManager->SetBlurEffect( effect.blur_x, effect.blur_y );

	if( effect_flag & ScreenEffect::PseudoMotionBlur )
		pScreenEffectManager->SetMotionBlurWeight( effect.motion_blur_strength );

	if( effect_flag & ScreenEffect::PseudoBlur )
		PERIODICAL( 10, g_Log.Print( "blur: %f, %f", effect.blur_x, effect.blur_y ) );

//	pScreenEffectManager->SetGlareLuminanceThreshold( effect.glare_threshold );
*/
	GetStage()->Render( m_Camera );

	// restore the original effect settings
	pScreenEffectManager->SetEffectFlag( orig_effect_flag );
}

/*
bool CBE_ScriptedCamera::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_PhysicsBaseEntity::LoadSpecificPropertiesFromFile( scanner );

	return false;
}

void CBE_ScriptedCamera::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_PhysicsBaseEntity::Serialize( ar, version );
}
*/


void CScriptedCameraEntity::CreateRenderTasks()
{
	// add render tasks necessary to render the stage
	GetStage()->CreateStageRenderTasks( &m_Camera );
}


void CScriptedCameraEntity::UpdateCameraOrientationByFocusTarget( float current_time )
{
	CCameraProperty& cam = m_KeyFrames.Camera;

	if( !cam.FocusTarget.IsAvailable( current_time ) )
		return; // no focus target is set for the current time

	cam.FocusTarget.UpdateFocusTargetPositions( current_time );

//	Vector3 vPosToLookAt = Vector3(0,0,0);
	CameraTargetHolder cam_target;
	bool res = cam.FocusTarget.CalcFrame( current_time, cam_target );
	if( res )
	{
		if( true /* shake */ )
		{
			Vector3 vPosToLookAt
				= cam_target.m_vTargetPos;
				//+ Vec3RandDir() * ;
			Vector3 vCamDir = vPosToLookAt - m_Camera.GetPosition();
			Vec3Normalize( vCamDir, vCamDir );

			float frametime = GetStage()->GetFrameTime();

			Matrix33 orient
				= CreateOrientFromFwdDir( vCamDir )
				* Matrix33RotationX( RangedRand(-0.1f, 0.1f) )
				* Matrix33RotationY( RangedRand(-0.1f, 0.1f) );

			m_CamOrient.target = orient;

			m_CamOrient.Update( frametime );

			m_CamOrient.current.Orthonormalize();

			m_Camera.SetOrientation( m_CamOrient.current );
//				m_Camera.SetOrientation( orient );
		}
		else
		{
			Vector3 vPosToLookAt = cam_target.m_vTargetPos;
			Vector3 vCamDir = vPosToLookAt - m_Camera.GetPosition();
			Vec3Normalize( vCamDir, vCamDir );

			m_Camera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) );
		}
	}
}


void CScriptedCameraEntity::UpdateCameraParams()
{
	float current_time = (float)GetStage()->GetElapsedTime();
	bool res;

	// camera params
	CCameraProperty& cam = m_KeyFrames.Camera;

	// camera pose (focus target)
	// overwrite camera orientation if it's focusing on an entity / a position
	UpdateCameraOrientationByFocusTarget( current_time );

	float fov, nc, fc;
	res = cam.fov.CalcFrame( current_time, fov );
	if( res )
		m_Camera.SetFOV( fov );

	res = cam.nearclip.CalcFrame( current_time, nc );
	if( res )
		m_Camera.SetNearClip( nc );

	res = cam.farclip.CalcFrame( current_time, fc );
	if( res )
		m_Camera.SetFarClip( fc );


	//
	// update effect params
	//

	CScreenEffectProperty& effect = m_KeyFrames.Effect;
	CPPEffectParams& rEffectParams = m_PPEffectParams;

	rEffectParams.Clear();

	int effect_flag = 0;
	Vector2 vBlur = Vector2(0,0);

	res = effect.Blur.CalcFrame( current_time, vBlur );
	if( res )
	{
		rEffectParams.blur_x = vBlur.x;
		rEffectParams.blur_y = vBlur.y;
		effect_flag |= ScreenEffect::PseudoBlur;
	}

	res = effect.MotionBlurStrength.CalcFrame( current_time, rEffectParams.motion_blur_strength );
	if( res )
		effect_flag |= ScreenEffect::PseudoMotionBlur;

	res = effect.MonochromeColorOffset.CalcFrame( current_time, rEffectParams.monocrhome_color_offset );
	if( res )
		effect_flag |= ScreenEffect::MonochromeColor;

	res = effect.GlareThreshold.CalcFrame( current_time, rEffectParams.glare_threshold );
	if( res )
		effect_flag |= ScreenEffect::Glare;

	if( effect_flag
	& ( ScreenEffect::Glare
	  | ScreenEffect::PseudoBlur
	  | ScreenEffect::PseudoMotionBlur
	  | ScreenEffect::MonochromeColor
	  | ScreenEffect::PseudoBlur ) )
	  effect_flag |= ScreenEffect::PostProcessEffects;

	rEffectParams.flag = effect_flag;

/*	SFloatRGBColor fade_color;
	res = effect.FadeColor.CalcFrame( current_time, fade_color );
	if( res )
	{
		effect_flag |= ScreenEffect::Fade;
		rEffectParams.fade_color = fade_color;
	}*/

//	Noise;
//	Stripe;
}

//===============================================================================================
// 
//===============================================================================================

CBE_ScriptedCamera::CBE_ScriptedCamera()
{
	m_bLighting = false;
	m_bNoClip = true;
}


void CBE_ScriptedCamera::Init()
{
//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;
}


void CBE_ScriptedCamera::InitCopyEntity(CCopyEntity* pCopyEnt)
{

//	D3DXCreateTexture( DIRECT3D9.GetDevice(),
//	ex.pSceneTexture;
}


void CBE_ScriptedCamera::Act( CCopyEntity* pCopyEnt )
{
	pCopyEnt->Update( (float)m_pStage->GetElapsedTime() );

	// commented out - the camera controller terminates scripted cameras in its loop to update child entities
//	if( HasExpired(pCopyEnt) )
//		m_pStage->TerminateEntity( pCopyEnt );
}


void CBE_ScriptedCamera::RenderStage( CCopyEntity* pCopyEnt )
{
	pCopyEnt->RenderStage();
}

void CBE_ScriptedCamera::CreateRenderTasks( CCopyEntity* pCopyEnt )
{
	pCopyEnt->CreateRenderTasks();
}


/*
void CBEC_ScriptedCameraExtraData::InitializedAtCutsceneStart()
{
	FocusTargetFrameSet& rFocusTarget = KeyFrames.Camera.FocusTarget;

	rFocusTarget.UpdateForcusTargetEntities( ms_pS);
}*/

