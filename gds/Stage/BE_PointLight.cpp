#include "BE_PointLight.hpp"

#include "LightEntity.hpp"
#include "GameMessage.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "Graphics/Direct3D9.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "3DMath/MathMisc.hpp"

#include "Support/Log/DefaultLog.hpp"


using namespace std;


CBE_PointLight::CBE_PointLight()
{
	m_bNoClip = true;

	m_PointLight.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	m_PointLight.Attribute.LowerDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	m_PointLight.vPosition = Vector3( 1.0f, 1.0f, 1.0f );
	m_PointLight.SetAttenuation( 0.1f, 0.1f, 0.1f );

	m_fGlareDiameter = 0.1f;

	SetBillboardType( CBEC_Billboard::TYPE_RECT_ARRAY_AND_INDICES );

	m_fTimer = 0.0f;
}


void CBE_PointLight::Init()
{
	CBE_Light::Init();

/*	if( 0 < m_GlareTextureFilepath.length() )
		m_GlareTexture.Load( m_GlareTextureFilepath );
*/

	CBEC_Billboard::Init();

//	float r = m_fGlareDiameter / 2.0f;

	// calc texture coords & vertex colors since they don't change during runtime
	// positions are calculated every frame
	int i,j;
	for(i=0; i<NUM_MAX_BILLBOARDS_PER_ENTITY; i++)
	{
		m_avBillboardRect[i*4+0].tex = TEXCOORD2(0,0);
		m_avBillboardRect[i*4+1].tex = TEXCOORD2(1,0);
		m_avBillboardRect[i*4+2].tex = TEXCOORD2(1,1);
		m_avBillboardRect[i*4+3].tex = TEXCOORD2(0,1);

		for(j=0; j<4; j++)
//			m_avBillboardRect[i*4+j].color = 0x40FFFFFF;
			m_avBillboardRect[i*4+j].color = 0x30FFFFFF;
	}

/*
	m_avGlareRect[0].vPosition = D3DXVECTOR3(-r, r, 0);
	m_avGlareRect[1].vPosition = D3DXVECTOR3( r, r, 0);
	m_avGlareRect[2].vPosition = D3DXVECTOR3( r,-r, 0);
	m_avGlareRect[3].vPosition = D3DXVECTOR3(-r,-r, 0);

		
	// set the local coordinate of the billboard rectangle
	m_avBillboardRect[0].tex = TEXCOORD2(0,0);
	m_avBillboardRect[1].tex = TEXCOORD2(1,0);
	m_avBillboardRect[2].tex = TEXCOORD2(1,1);
	m_avBillboardRect[3].tex = TEXCOORD2(0,1);


	for(int i=0; i<4; i++)
		m_avGlareRect[i].color = 0x40FFFFFF;
*/
}


void CBE_PointLight::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->EntityFlag |= BETYPE_USE_ZSORT;

	float& rfFadeoutTime = FadeoutTime(pCopyEnt);
	rfFadeoutTime = 0.0f;
/*
	if( !(m_TypeFlag & (TYPE_STATIC|TYPE_DYNAMIC)) )
	{	// neither static nor dynamic
		// - created only for pseudo glare effect. mainly put together with items
		int glare_only_light = 1;

		MsgBox( "registered pseudo light" );
	}
	else if( m_TypeFlag & TYPE_STATIC )
	{
		// register static light to the stage
		...
	}
	else
	{
		// register dynamic light to the stage
		...
	}

	if( m_TypeFlag & TYPE_TIMER )
	{
		float& rfTimeLeft = TimeLeft(pCopyEnt);;
		rfTimeLeft = m_fTimer;
	}
*/
	if( pCopyEnt->GetParent() )
	{
		pCopyEnt->s1 |= CEDL_HAS_PARENT;
	}
}


void CBE_PointLight::DeleteDynamicLight( CCopyEntity* pCopyEnt )
{
//	LOG_PRINT( "Removing a light entity - id: " to_string(pCopyEnt->iExtraDataIndex) );
/*
	if( 0 <= pCopyEnt->iExtraDataIndex )
	{
		m_pStage->GetEntitySet()->DeleteLight( pCopyEnt->iExtraDataIndex, CLE_LIGHT_DYNAMIC );
		CheckEntitiesInLightRange( pCopyEnt, pCopyEnt->iExtraDataIndex );
	}
*/
	m_pStage->TerminateEntity( pCopyEnt );
}


void CBE_PointLight::Act( CCopyEntity* pCopyEnt )
{
	// test dynamic light
/*	if( m_TypeFlag & TYPE_DYNAMIC )
	{
		if( pCopyEnt->iExtraDataIndex == -1 )
		{
			// invalid light
			// probably failed to create because of the shortage of dynamic lights
			DeleteDynamicLight( pCopyEnt );
			return;
		}

		if( m_TypeFlag & TYPE_TIMER )
		{
			float& rfTimeLeft = TimeLeft(pCopyEnt);
			rfTimeLeft -= m_pStage->GetFrameTime();

			if( rfTimeLeft <= 0 )
			{
				DeleteDynamicLight( pCopyEnt );
				return;
			}

			//if( m_TypeFlag & TYPE_FADEOUT )
			//{
			//	Vector3 vColor = pCopyEnt->v1 * rfTimeLeft / m_fTimer;
			//	short sLightIndex = (short)pCopyEnt->iExtraDataIndex;
			//	m_pStage->GetEntitySet()->GetLightEntity( sLightIndex )->SetDiffuseColor( (float *)&vColor );
			//}
		}

		if( pCopyEnt->s1 & CEDL_HAS_PARENT && !pCopyEnt->GetParent() )
		{	// terminate this dynamic light
			DeleteDynamicLight( pCopyEnt );
			return;
		}
		else
		{
			if( pCopyEnt->GetParent() )
			{	// just follow the parent
				pCopyEnt->Position() = pCopyEnt->GetParent()->Position() + D3DXVECTOR3(0.0f, 0.5f, 0.0f);

			}

			if( !IsSensible(pCopyEnt->Position()) )
				int iError = 1;

			// update the position of the light entity
			m_pStage->GetEntitySet()->GetLightEntityManager()->UpdateLightPosition( (short)pCopyEnt->iExtraDataIndex,
			                                                                  pCopyEnt->Position() );
			CheckEntitiesInLightRange( pCopyEnt, (short)pCopyEnt->iExtraDataIndex );
		}
	}*/
}


void CBE_PointLight::CheckEntitiesInLightRange( CCopyEntity* pCopyEnt, int light_index )
{
	CTrace tr, tr2;
	static vector<CCopyEntity *> s_vecpEntityBuffer;
	static vector<CCopyEntity *> s_vecpEntityBuffer2;
	
	s_vecpEntityBuffer.resize( 0 );
	s_vecpEntityBuffer2.resize( 0 );

	tr.SetTouchEntityBuffer( &s_vecpEntityBuffer );
	tr.SetSphere( pCopyEnt->Position(), 100.0f );
	m_pStage->CheckCollision( tr );

///	tr2.SetTraceType( TRACETYPE_IGNORE_NOCLIP_ENTITIES );
	tr2.SetTraceType( TRACETYPE_IGNORE_ALL_ENTITIES );	// check only the static geometry
	tr2.SetTouchEntityBuffer( &s_vecpEntityBuffer2 );

	int i, iNumOvelappingEntities = (int)tr.GetNumTouchEntities();
	CCopyEntity *pTargetEntity;///, *pTouchedEntity;
	for( i=0; i<iNumOvelappingEntities; i++ )
	{
		pTargetEntity = tr.GetTouchEntity(i);

		if( !(pTargetEntity->EntityFlag & BETYPE_LIGHTING) )
			continue;	// no need to update lighting for this entity

		if( pTargetEntity == pCopyEnt )
			continue;	// myself

		tr2.ClearTouchEntity();

		// check if there is any obstacle between the light center and the target entity
		tr2.SetLineSegment( pCopyEnt->Position(), pTargetEntity->Position() );
		m_pStage->CheckCollision( tr2 );

///		if( tr2.GetNumTouchEntities() == 0 )
///			continue;

///		pTouchedEntity = tr2.GetTouchEntity(0);

///		if( pTouchedEntity != pTargetEntity )
		if( tr2.GetEndFraction() < 1.0f )	// hit the static geometry
			continue;	// obstacle between light and entity

//		if( pTouchedEntity->bNoClip )
//			continue;

//		pTargetEntity->AddLightIndex( sLightIndex );

		// invalidate the current lighting information
		// and make sure the lighting info will be re-calculated in the next update routine for the entity
		pTargetEntity->sState |= CESTATE_LIGHT_INFORMATION_INVALID;

	}
}



bool CBE_PointLight::CheckRayToCamera( CCopyEntity* pCopyEnt )
{
	// cast a ray from this point light to the current camera
	CCopyEntity *pCameraEntity = m_pStage->GetEntitySet()->GetCameraEntity();
	if( !pCameraEntity )
		return false;	// camera entity was not found

/*	CCamera *pCamera = pCameraEntity->pBaseEntity->GetCamera();
	if( !pCamera )
		return false;

	D3DXVECTOR3 vCameraPos = pCamera->GetPosition();*/

///	D3DXVECTOR3 vCameraPos = pCameraEntity->Position();

	D3DXVECTOR3 vCameraPos;
	CCamera *pCamera = pCameraEntity->pBaseEntity->GetCamera();
	if( pCamera )
		vCameraPos = pCamera->GetPosition();
	else
		vCameraPos = pCameraEntity->Position();

	if( 0.01f < Vec3Dot( vCameraPos - pCopyEnt->Position(), pCameraEntity->GetDirection() ) )
		return false;	// light is not in the view frustum of the camera

	STrace tr;
	tr.pvStart = &pCopyEnt->Position();
	tr.pvGoal = &vCameraPos;
	tr.bvType = BVTYPE_DOT;
	tr.pSourceEntity = pCopyEnt;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;

	// check the ray
	this->m_pStage->ClipTrace( tr );

	// check if the ray is obstracted
	if( tr.pTouchedEntity == pCameraEntity )
	{	// ray not blocked - draw glare
		return true;
	}
	else
		return false;
}


#define BE_PLIGHT_FADEOUTTIME	0.1f

void CBE_PointLight::Draw( CCopyEntity* pCopyEnt )
{
	if( !(m_TypeFlag & TYPE_GLARE) )
		return;

	// glare effect has to be rendered for the point light

	float& rfFadeoutTime = FadeoutTime(pCopyEnt);

	if( !CheckRayToCamera(pCopyEnt) )
	{	// ray is blocked
		rfFadeoutTime -= m_pStage->GetFrameTime();
		if( rfFadeoutTime <= 0 )
			return;
	}
	else
		rfFadeoutTime = BE_PLIGHT_FADEOUTTIME;

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	D3DXMATRIX matScale;
	D3DXMATRIX matWorld;

//	D3DXMatrixScaling( &matScale, rfFadeoutTime / BE_PLIGHT_FADEOUTTIME, rfFadeoutTime / BE_PLIGHT_FADEOUTTIME, 1.0f );

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
//	m_pStage->GetBillboardRotationMatrix( matWorld );
//	matWorld._44 = 1;

	Matrix34 billboard_pose;
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	billboard_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );

	int i,j,num_rects[2];
	float span[2] = { 1.0f, 1.0f };	//{ pCopyEnt->f1, pCopyEnt->f2 };
	float fOffset[2];
	const float delta = 0.25f;

	num_rects[0] = (int)(span[0] / delta);
	num_rects[1] = (int)(span[1] / delta);
	Limit( num_rects[0], 1, 4 );
	Limit( num_rects[1], 1, 4 );

	fOffset[0] = -( (num_rects[0]-1) * 0.5f );
	fOffset[1] = -( (num_rects[1]-1) * 0.5f );

	// calc the world position of the particle
//	float fRadius = m_fRadius;
	float r = m_fGlareDiameter / 2.0f;
	D3DXVECTOR3 avBasePos[4];
	avBasePos[0] = D3DXVECTOR3(-r, r, 0 );
	avBasePos[1] = D3DXVECTOR3( r, r, 0 );
	avBasePos[2] = D3DXVECTOR3( r,-r, 0 );
	avBasePos[3] = D3DXVECTOR3(-r,-r, 0 );

	int vert_offset = 0;
	Vector3 vPos;
	Vector3 vEntityPos = pCopyEnt->Position();
	const Matrix33& rWorldOrient = pCopyEnt->GetWorldPose().matOrient;
	for( i=0; i<num_rects[0]; i++ )
	{
		for( j=0; j<num_rects[1]; j++ )
		{
			vPos = vEntityPos
				 + rWorldOrient.GetColumn(0) * ( ( fOffset[0] + (float)i ) * delta )
				 + rWorldOrient.GetColumn(2) * ( ( fOffset[1] + (float)j ) * delta );

			memcpy( &(matWorld._41), &vPos, sizeof(D3DXVECTOR3) );

			// set vertices for billboards
			D3DXVec3TransformCoord( &(m_avBillboardRect[vert_offset+0].vPosition), &avBasePos[0], &matWorld );
			D3DXVec3TransformCoord( &(m_avBillboardRect[vert_offset+1].vPosition), &avBasePos[1], &matWorld );
			D3DXVec3TransformCoord( &(m_avBillboardRect[vert_offset+2].vPosition), &avBasePos[2], &matWorld );
			D3DXVec3TransformCoord( &(m_avBillboardRect[vert_offset+3].vPosition), &avBasePos[3], &matWorld );
			vert_offset += 4;
		}
	}

	DWORD dwZBufMode;
	pd3dDev->GetRenderState( D3DRS_ZENABLE, &dwZBufMode );
	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

	DrawBillboards( num_rects[0] * num_rects[1], 0, 0, m_pStage );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, dwZBufMode );

}


void CBE_PointLight::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
	switch( rGameMessage.iEffect )
	{
	case GM_TERMINATE:
		DeleteDynamicLight( pCopyEnt_Self );
		break;
	}
}


bool CBE_PointLight::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	CBE_Light::LoadSpecificPropertiesFromFile( scanner );

	CBEC_Billboard::LoadSpecificPropertiesFromFile( scanner );


	float r,g,b, a0,a1,a2;

	if( scanner.TryScanLine( "GLARE_DIAMETER", m_fGlareDiameter ) ) return true;

	if( scanner.TryScanLine( "TEX_FILENAME", m_GlareTextureFilepath ) ) return true;

	if( scanner.TryScanLine( "UPPER_COLOR", r, g, b ) )
	{
		m_PointLight.Attribute.UpperDiffuseColor.SetRGB( r,g,b );
		return true;
	}

	if( scanner.TryScanLine( "LOWER_COLOR", r, g, b ) )
	{
		m_PointLight.Attribute.LowerDiffuseColor.SetRGB( r,g,b );
		return true;
	}

	if( scanner.TryScanLine( "ATTENUATION", a0, a1, a2 ) )
	{
		m_PointLight.fAttenuation[0] = a0;
		m_PointLight.fAttenuation[1] = a1;
		m_PointLight.fAttenuation[2] = a2;
		return true;
	}

	if( scanner.TryScanLine( "LIGHT_TIMER_VAL", m_fTimer ) ) return true;

	return false;
}


void CBE_PointLight::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_Light::Serialize( ar, version );
	CBEC_Billboard::SerializeBillboardProperty( ar, version );

	ar & m_fGlareDiameter;
	ar & m_GlareTextureFilepath;
	ar & m_fTimer;
	ar & m_afBaseColor[0] & m_afBaseColor[1] & m_afBaseColor[2];
	ar & m_PointLight;
}
