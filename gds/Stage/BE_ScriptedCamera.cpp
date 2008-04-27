#include "BE_ScriptedCamera.h"

#include "GameMessage.h"
#include "CopyEntityDesc.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "ScreenEffectManager.h"

#include "3DCommon/Direct3D9.h"
#include "3DCommon/3DGameMath.h"

#include "Support/Log/DefaultLog.h"
#include "Support/Vec3_StringAux.h"
#include "Support/macro.h"


void FocusTargetFrameSet::UpdateFocusTargetEntities( CEntitySet *pEntitySet )
{
	size_t i, num_key_frames = m_vecKeyFrame.size();
	for( i=0; i<num_key_frames; i++ )
	{
		if( 0 < m_vecKeyFrame[i].val.m_TargetName.length() )
		{
			CCopyEntity* pTarget = pEntitySet->GetEntityByName( m_vecKeyFrame[i].val.m_TargetName.c_str() );
			if( IsValidEntity( pTarget ) )
				m_vecKeyFrame[i].val.m_pTarget = pTarget;
		}
	}
}

/*
void CBEC_ScriptedCameraExtraData::InitializedAtCutsceneStart()
{
	FocusTargetFrameSet& rFocusTarget = KeyFrames.Camera.FocusTarget;

	rFocusTarget.UpdateForcusTargetEntities( ms_pS);
}*/


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
//	Init3DModel();

//	m_ActorDesc.iCollisionGroup = ENTITY_COLL_GROUP_OTHER_ENTITIES;
//	m_ActorDesc.ActorFlag = JL_ACTOR_APPLY_NO_IMPULSE;

	m_vecExtraData.reserve( 4 );
}


void CBE_ScriptedCamera::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	pCopyEnt->iExtraDataIndex = GetNewExtraDataIndex();

	CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt);
}



void CBE_ScriptedCamera::Act(CCopyEntity* pCopyEnt)
{
	CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt);

	if( ex.Path.IsAvailable( (float)m_pStage->GetElapsedTime() ) )
	{
		if( !ex.m_InitializedAtCutsceneStart )
		{
			ex.KeyFrames.Camera.FocusTarget.UpdateFocusTargetEntities( m_pStage->GetEntitySet() );
			ex.m_InitializedAtCutsceneStart = true;
		}

		UpdateScriptedMotionPath( pCopyEnt, ex.Path );
		PERIODICAL( 100, g_Log.Print( "CBE_ScriptedCamera::Act() - updated motion path ... pos: " +
			to_string(pCopyEnt->Position(), 2) ) );

		ex.Camera.SetPose( pCopyEnt->GetWorldPose() );
		ex.Camera.UpdateVFTreeForWorldSpace();

		UpdateCameraParams( pCopyEnt );
	}

//	if( m_pStage->GetEntitySet()->GetCameraEntity() == pCopyEnt )
//	{
//		m_Camera.SetPose( pCopyEnt->GetWorldPose() );
//	}
}


void CBE_ScriptedCamera::RenderStage(CCopyEntity* pCopyEnt)
{
	CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt);

	PERIODICAL( 100, g_Log.Print( "CBE_ScriptedCamera::RenderStage() - camera pos: " +
		to_string(ex.Camera.GetPosition(), 2) ) );


	CScreenEffectManager *pScreenEffectManager = m_pStage->GetScreenEffectManager();

	// save the original settings
	int orig_effect_flag = pScreenEffectManager->GetEffectFlag();

	if( 7.5f < m_pStage->GetElapsedTime() )
		int break_here = 1;

	const CPPEffectParams& effect = ex.PPEffectParams;
	int effect_flag = effect.flag;
//	int effect_flag = effect.flag | ScreenEffect::MonochromeColor;
	pScreenEffectManager->SetEffectFlag( effect_flag );
	pScreenEffectManager->SetBlurEffect( effect.blur_x, effect.blur_y );

	if( effect_flag & ScreenEffect::PseudoMotionBlur )
		pScreenEffectManager->SetMotionBlurWeight( effect.motion_blur_strength );

//	pScreenEffectManager->RaiseEffectFlag( ScreenEffect::PseudoMotionBlur );	
//	pScreenEffectManager->SetMotionBlurWeight( 0.6f );

	if( effect_flag & ScreenEffect::PseudoBlur )
		PERIODICAL( 10, g_Log.Print( "blur: %f, %f", effect.blur_x, effect.blur_y ) );
//	pScreenEffectManager->SetGlareLuminanceThreshold( effect.glare_threshold );

//	m_pStage->Render( m_Camera );
	m_pStage->Render( ex.Camera );

	// restore the original effect settings
	pScreenEffectManager->SetEffectFlag( orig_effect_flag );
}


void CBE_ScriptedCamera::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_SET_MOTION_PATH:
	  {
		// set motion path
		EntityMotionPathRequest *pReq = (EntityMotionPathRequest *)rGameMessage.pUserData;
		CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt_Self);
		ex.Path.SetKeyPoses( pReq->vecKeyPose );
		ex.Path.SetMotionPathType( pReq->MotionPathType );

		g_Log.Print( "CBE_ScriptedCamera::MessageProcedure() - added motion path to scripted camera" );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		return;
	  }

	case GM_SET_DEFAULT_CAMERA_PARAM:
	  {
		// set motion path
		CameraParam *pParam = (CameraParam *)rGameMessage.pUserData;
		CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt_Self);
		ex.DefaultParam = *pParam;

		// TODO: change camera params dynamically
		ex.Camera.SetNearClip( ex.DefaultParam.nearclip );
		ex.Camera.SetFarClip( ex.DefaultParam.farclip );
		ex.Camera.SetFOV( ex.DefaultParam.fov );
		ex.Camera.SetAspectRatio( ex.DefaultParam.aspect_ratio );

		g_Log.Print( "CBE_ScriptedCamera::MessageProcedure() - set default camera params" );

//		MsgBoxFmt( "set motion path for enemy entity: %s", pCopyEnt_Self->GetName().c_str() );
		return;
	  }

	case GM_SET_SCRIPTCAMERAKEYFRAMES:
	  {
		CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt_Self);

		// set camera effects
		ex.KeyFrames = *(CScriptCameraKeyFrames *)rGameMessage.pUserData;

		g_Log.Print( "CBE_ScriptedCamera::MessageProcedure() - set camera effect params" );

		return;
	  }
	}
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


void CBE_ScriptedCamera::AddExtraData()
{
	m_vecExtraData.push_back( CBEC_ScriptedCameraExtraData() );
	CBEC_ScriptedCameraExtraData& ex = m_vecExtraData.back();

	ex.m_bInUse = true;

}


void CBE_ScriptedCamera::UpdateCameraParams( CCopyEntity* pCopyEnt )
{
	CBEC_ScriptedCameraExtraData& ex = GetExtraData(pCopyEnt);

	float current_time = (float)m_pStage->GetElapsedTime();
	bool res;

	// camera params
	CCameraProperty& cam = ex.KeyFrames.Camera;

	// camera pose (focus target)
	// overwrite camera orientation if it's focusing on an entity / a position
	if( cam.FocusTarget.IsAvailable( current_time ) )
	{
		cam.FocusTarget.UpdateFocusTargetPositions( current_time );

//		Vector3 vPosToLookAt = Vector3(0,0,0);
		CameraTargetHolder cam_target;
		res = cam.FocusTarget.CalcFrame( current_time, cam_target );
		if( res )
		{
			Vector3 vPosToLookAt = cam_target.m_vTargetPos;
			Vector3 vCamDir = vPosToLookAt - ex.Camera.GetPosition();
			Vec3Normalize( vCamDir, vCamDir );

			ex.Camera.SetOrientation( CreateOrientFromFwdDir( vCamDir ) );
		}
	}

//	float current_time = m_pStage->GetElapsedTime();
//	bool res
	float fov, nc, fc;
	res = cam.fov.CalcFrame( current_time, fov );
	if( res )
		ex.Camera.SetFOV( fov );

	res = cam.nearclip.CalcFrame( current_time, nc );
	if( res )
		ex.Camera.SetNearClip( nc );

	res = cam.farclip.CalcFrame( current_time, fc );
	if( res )
		ex.Camera.SetFarClip( fc );


	//
	// update effect params
	//

	CScreenEffectProperty& effect = ex.KeyFrames.Effect;
	CPPEffectParams& rEffectParams = ex.PPEffectParams;

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
