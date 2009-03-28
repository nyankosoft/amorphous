
#include "BE_Bullet.hpp"

#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "CopyEntityDesc.hpp"
#include "trace.hpp"
#include "Stage.hpp"
#include "SurfaceMaterial.hpp"
#include "Serialization_BaseEntityHandle.hpp"

#include "Graphics/Direct3D9.hpp"
#include "Graphics/D3DXMeshObject.hpp"
#include "Graphics/3DGameMath.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

#include "Sound/SoundManager.hpp"


inline float& TraveledDist(CCopyEntity* pCopyEnt) { return pCopyEnt->f2; }

CBE_Bullet::CBE_Bullet()
{
	m_BoundingVolumeType = BVTYPE_DOT;
	m_fRadius = 0;
	m_aabb.SetMaxAndMin( Vector3(0,0,0), Vector3(0,0,0) );

//	RaiseEnttiyFlag(BETYPE_GLARESOURCE);

	m_fMaxRange		= 1000.0f;
	m_fPenetration = 0.0f;
	m_fBulletPower = 0.001f;
	m_fSpeedAttenuAtReflection = 0.0f;
	m_fNumMaxReflections = 0;

	m_fBillboardRadius = 0.1f;

	m_bLighting = false;
}


void CBE_Bullet::Init()
{
	if( 3 < m_MeshProperty.m_MeshDesc.ResourcePath.length() )
		Init3DModel();

	// load the texture
	m_BillboardTexture.Load( m_BillboardTextureFilepath );

	float r = m_fBillboardRadius;
	m_avBillboardRect[0].vPosition = Vector3(-r, r, 0 );
	m_avBillboardRect[1].vPosition = Vector3( r, r, 0 );
	m_avBillboardRect[2].vPosition = Vector3( r,-r, 0 );
	m_avBillboardRect[3].vPosition = Vector3(-r,-r, 0 );

	// these values are set during the drawing proccess if random texture coordinate table is used
	m_avBillboardRect[0].tex = TEXCOORD2(0.0f, 0.0f);
	m_avBillboardRect[1].tex = TEXCOORD2(1.0f, 0.0f);
	m_avBillboardRect[2].tex = TEXCOORD2(1.0f, 1.0f);
	m_avBillboardRect[3].tex = TEXCOORD2(0.0f, 1.0f);

	m_Spark.SetBaseEntityName( "sprk" );

	LoadBaseEntity( m_Spark );
	LoadBaseEntity( m_Light );
}


// Don't forget to set 'vDirection' for copy entities of 'CBE_Bullet'
void CBE_Bullet::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	Vector3& rvPrevPosition = pCopyEnt->v1;
	rvPrevPosition = pCopyEnt->Position();

	pCopyEnt->fSpeed = Vec3Length( pCopyEnt->Velocity() );

	TraveledDist(pCopyEnt) = 0.0f;

	// init reflection count
	float& rfNumCurrentReflections = pCopyEnt->f3;
	rfNumCurrentReflections = 0;

	// test using bullet of 20[g]
//	pCopyEnt->fMass_inv = 50.0f;

	// bullet is approximated by point - but is it okay to set the inverse of inertia tensor to zero?
//	memset( &pCopyEnt->GetWorldInvInertia(), 0, sizeof(float) * 16 );
//	pCopyEnt->GetWorldInvInertia()._44 = 1.0f;

	// move the bullet slightly to avoid hitting the shooter
	Move( pCopyEnt );


	if( 0 < strlen(m_Light.GetBaseEntityName()) )
	{	// create dynamic light entity that moves with this bullet
		CCopyEntityDesc light;
		light.pBaseEntityHandle = &m_Light;
		light.SetWorldPose( pCopyEnt->GetWorldPose() );
		light.vVelocity = pCopyEnt->Velocity();
		light.pParent = pCopyEnt;
//		light.pBaseEntity = m_pStage->FindBaseEntity( "DynamicLightX" );

		m_pStage->CreateEntity( light );
	}

}


void CBE_Bullet::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
/*
	if( pCopyEnt_Self->sState & CESTATE_ATREST )
	{	// lost all the momentum
		pCopyEnt_Self->Velocity() = Vector3(0, 0, 0);
		m_pStage->TerminateEntity( pCopyEnt_Self );
	}*/
}


void CBE_Bullet::BulletMove(CCopyEntity* pCopyEnt)
{
	if( pCopyEnt->sState & CESTATE_ATREST )
		return;

	STrace tr;
	float fFrametime = m_pStage->GetFrameTime();
	//Vector3 vVel = pCopyEnt->GetDirection() * pCopyEnt->fSpeed;

	tr.pvStart = &pCopyEnt->Position();
	Vector3 vGoal = pCopyEnt->Position() + pCopyEnt->Velocity() * fFrametime;
	tr.pvGoal = &vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;
	tr.fFraction = 1.0f;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType |= TRACETYPE_GET_MATERIAL_INFO;


	this->m_pStage->ClipTrace( tr );

	pCopyEnt->Position() = tr.vEnd;
	pCopyEnt->touch_plane = tr.plane;

	if( tr.in_solid )
		NudgePosition( pCopyEnt );

	if( tr.fFraction < 1.0f )
	{
		// hit something
		// - see if there is anything that has to be done upon hitting the surface
		//   This is determined by the type of surface.
		//   e.g.) 
		OnBulletHit( pCopyEnt, tr );

		pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );
		pCopyEnt->fSpeed = 0;
//		pCopyEnt->SetDirection( Vector3(0,0,0) );

		// set velocity to zero so that the bullet will be terminated during the next turn
		pCopyEnt->SetVelocity( Vector3(0,0,0) );

//		pCopyEnt->sState |= CESTATE_ATREST;
		return;
	}

	//pCopyEnt->fSpeed = D3DXVec3Length(&vVel);
	//pCopyEnt->Direction( vVel / pCopyEnt->fSpeed );
}


void CBE_Bullet::PenetrationMove(CCopyEntity* pCopyEnt)
{
	if( pCopyEnt->sState & CESTATE_ATREST )
		return;

	float fFrametime = m_pStage->GetFrameTime();

	if( pCopyEnt->bInSolid )
	{	// continue penetration iteration
		PenetrationIteration( pCopyEnt, fFrametime );
		return;
	}

	// perform line segment check first

	STrace tr;
	tr.pvStart = &pCopyEnt->Position();
	Vector3 vGoal = pCopyEnt->Position() + pCopyEnt->Velocity() * fFrametime;
	tr.pvGoal = &vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;
	tr.fFraction = 1.0f;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType |= TRACETYPE_GET_MATERIAL_INFO;


	this->m_pStage->ClipTrace( tr );

	pCopyEnt->Position() = tr.vEnd;
	pCopyEnt->touch_plane = tr.plane;	// save contacted surface

	if( tr.in_solid && tr.plane.normal == Vector3(0,0,0) )
	{
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	if( tr.fFraction < 1.0f )	// hit something
	{
		pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );

		OnBulletHit( pCopyEnt, tr );

		float fFrameTime_Left = fFrametime * (1.0f - tr.fFraction);

		// start pseudo penetration simulation
		PenetrationIteration( pCopyEnt, fFrameTime_Left );

		return;
	}

}


#define CBE_BULLET_PENETRATION_CALC_STEP	0.025f

void CBE_Bullet::PenetrationIteration( CCopyEntity* pCopyEnt, float fFrameTime_Left )
{
	if( pCopyEnt->touch_plane.normal == Vector3(0,0,0) )
		pCopyEnt->touch_plane.normal = Vector3(0,1,0);

	float fDeltaTime = - CBE_BULLET_PENETRATION_CALC_STEP / Vec3Dot( pCopyEnt->Velocity(), pCopyEnt->touch_plane.normal );
	pCopyEnt->fSpeed = Vec3Length( pCopyEnt->Velocity() );			// update current speed
	pCopyEnt->SetDirection( pCopyEnt->Velocity() / pCopyEnt->fSpeed );	// update current direction

	STrace tr;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;
	tr.fFraction = 1.0f;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	Vector3 vPrevPos = pCopyEnt->Position();

	while( 0 < fFrameTime_Left )
	{
		tr.vEnd = vPrevPos + pCopyEnt->Velocity() * fDeltaTime;
		tr.fFraction = 1.0f;
		tr.in_solid = false;
		m_pStage->CheckPosition( tr );

		pCopyEnt->fSpeed -= 32.0f / m_fPenetration /*  * tr.pSurfaceMaterial->GetBulletResitance() */;
		if( pCopyEnt->fSpeed < 0.0f )
			pCopyEnt->fSpeed = 0.0f;

		// update the bullet speed
		pCopyEnt->Velocity() = pCopyEnt->GetDirection() * pCopyEnt->fSpeed;

		// update the bullet position
		pCopyEnt->Position() = tr.vEnd;

		pCopyEnt->bInSolid = tr.in_solid;

		if( tr.in_solid )
		{	// proceeding inside the material
			if( pCopyEnt->fSpeed <= 0.0f )
			{	// lost all the momentum and ended up in a wall
//				pCopyEnt->sState |= CESTATE_ATREST;
				// set velocity to zero so that the bullet will be terminated during the next turn
				pCopyEnt->SetVelocity( Vector3(0,0,0) );
//				pCopyEnt->SetDirection( Vector3(0,0,0) );
				break;
			}
		}
		else
		{	// penetrated the material
			pCopyEnt->f1 = Vec3LengthSq( pCopyEnt->vVelocity );

			// calculate the exit point by going back the trace
			Vector3 vCurrentPos = tr.vEnd;
			tr.pvStart = &vCurrentPos;
			tr.pvGoal  = &vPrevPos;
			tr.fFraction = 1.0f;
			m_pStage->ClipTrace( tr );

			// make a hole at the other side of the wall if the bullet has hit the static geometry
///			if( tr.fFraction < 1.0f && !tr.pTouchedEntity )
///				CreateBulletHoleDecal( tr.vEnd, tr.plane.normal );

//			pCopyEnt->touch_plane.Flip();
//			CreateBulletHoleDecal( tr.vEnd, tr.plane.normal );
//			pCopyEnt->touch_plane.Flip();

			// move a little forward so that bullet can really get out of the wall
			pCopyEnt->Position() += pCopyEnt->GetDirection() * CBE_BULLET_PENETRATION_CALC_STEP;
			break;
		}

		vPrevPos = tr.vEnd;
		fFrameTime_Left -= fDeltaTime;
	}
}


#define CBE_BULLET_REFLECTIVE_MIN_SPEED  25.0f

/**
 * simulates behavior of a projectile that has reflective property
 */
void CBE_Bullet::ReflectiveMove(CCopyEntity* pCopyEnt)
{
	if( pCopyEnt->sState & CESTATE_ATREST )
		return;

	STrace tr;
	float fFrameTime_Left = m_pStage->GetFrameTime();

	Vector3 vGoal;
	tr.aabb = this->m_aabb;
	tr.bvType = this->m_BoundingVolumeType;
	tr.fRadius = this->m_fRadius;
	tr.pSourceEntity = pCopyEnt;
	tr.fFraction = 1.0f;
	tr.sTraceType = TRACETYPE_IGNORE_NOCLIP_ENTITIES;
	tr.sTraceType |= TRACETYPE_GET_MATERIAL_INFO;

	float& rfNumCurrentReflections = pCopyEnt->f3;

	while( 0 < fFrameTime_Left && rfNumCurrentReflections <= m_fNumMaxReflections )
	{

		// check the next position
		tr.pvStart = &pCopyEnt->Position();
		vGoal = pCopyEnt->Position() + pCopyEnt->vVelocity * fFrameTime_Left;
		tr.pvGoal = &vGoal;
		tr.fFraction = 1.0f;
		tr.pTouchedEntity = NULL;
		this->m_pStage->ClipTrace( tr );

		// update position
		pCopyEnt->Position() = tr.vEnd;
		pCopyEnt->touch_plane = tr.plane;

//		if( tr.in_solid )
//			NudgePosition( pCopyEnt );

		if( tr.fFraction < 1.0f )
		{	// calculate pseudo-penetration

			pCopyEnt->AddTouchedEntity( tr.pTouchedEntity );

			rfNumCurrentReflections++;
			if( m_fNumMaxReflections < rfNumCurrentReflections )
			{	// exceeded the maximum reflection counts
//				pCopyEnt->sState |= CESTATE_ATREST;
				return;
			}

			// create decal/spark entities and make a hit sound
			// according to the surface material
			OnBulletHit( pCopyEnt, tr );

			// update speed
			pCopyEnt->fSpeed *= m_fSpeedAttenuAtReflection /* * surface_material.GetSpeedAttenuAtReflection() */;
			if( pCopyEnt->fSpeed < CBE_BULLET_REFLECTIVE_MIN_SPEED )
			{
				pCopyEnt->fSpeed = 0;
				pCopyEnt->Velocity() = Vector3(0,0,0);
//				pCopyEnt->sState |= CESTATE_ATREST;
				break;
			}

			// update velocity & direction
			Vector3 vPrevDir = pCopyEnt->GetDirection();
			pCopyEnt->SetDirection( vPrevDir - 2 * Vec3Dot( tr.plane.normal, vPrevDir ) * tr.plane.normal );
			pCopyEnt->Velocity() = pCopyEnt->GetDirection() * pCopyEnt->fSpeed;

			// update frametime left
			fFrameTime_Left *= ( 1.0f - tr.fFraction );

		}
		else
		{
			break;
		}
	}
}


void CBE_Bullet::Move( CCopyEntity* pCopyEnt )
{
	// measure the covered distance in this frame
	Vector3 vStartPos = pCopyEnt->Position();

	// simulate according to the property of this bullet
	if( 0 < m_fPenetration )
	{
		PenetrationMove( pCopyEnt );	// bullets that have penetration
	}
	else if( 0 < m_fSpeedAttenuAtReflection )
	{
		ReflectiveMove( pCopyEnt );		// bullets which reflect at the surface
	}
	else
	{
		BulletMove( pCopyEnt );
	}

	TraveledDist(pCopyEnt) += Vec3Length( pCopyEnt->Position() - vStartPos );
}


void CBE_Bullet::Act(CCopyEntity* pCopyEnt)
{
	pCopyEnt->f1 = Vec3LengthSq( pCopyEnt->Velocity() );

	if( pCopyEnt->f1 < 1.0f )
		pCopyEnt->sState |= CESTATE_ATREST;

	if( pCopyEnt->sState & CESTATE_ATREST || m_fMaxRange < TraveledDist(pCopyEnt) )
	{
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	// velocity update - make it less affected by gravity
	if( !m_MeshProperty.m_MeshObjectHandle.IsLoaded() )
	{	// bullet which is represented by 3D mesh object is not affected gravity
		Vector3 vGravityAccel = this->m_pStage->GetGravityAccel();
		pCopyEnt->vVelocity += vGravityAccel * m_pStage->GetFrameTime() * 0.5f;
	}

	// move the bullet
	Move( pCopyEnt );

	// update child entity (dynamic light that moves with the bullet)
	CCopyEntity *pChild = pCopyEnt->GetChild(0);
	if( pChild )
		pChild->Act();


}


/**
 When a bullet hit a polygon of the map, the decal entity is placed to make a bullet hole.
 Bullet holes decals are placed only on the surface of the static geometry.
 They are not placed on entities that move or disappear.
 */
void CBE_Bullet::OnBulletHit( CCopyEntity* pCopyEnt, STrace& tr )
{
	CCopyEntity* pCopyEnt_Other = tr.pTouchedEntity;

	if( pCopyEnt_Other )
	{
		// send message to the entity hit by the bullet
		SGameMessage msg;

		msg.iEffect = GM_DAMAGE;
		msg.s1 = DMG_BULLET;
//		msg.pSenderEntity = pCopyEnt_Self;
		msg.pSenderEntity = pCopyEnt;

		// amount of damage given by this bullet
		//msg.fParam1 = Vec3LengthSq( pCopyEnt_Self->vVelocity ) * this->m_fBulletPower;
		msg.fParam1 = pCopyEnt->f1 * this->m_fBulletPower;

		SendGameMessageTo( msg, pCopyEnt_Other );

		if( pCopyEnt_Other->GetEntityFlags() & BETYPE_RIGIDBODY )
		{	// apply impulse to the entity hit by this bullet
			pCopyEnt_Other->ApplyWorldImpulse( pCopyEnt->Velocity() / 50.0f, pCopyEnt->Position() );
		}
		else if( pCopyEnt_Other->GetEntityFlags() & BETYPE_PLAYER )
		{
			pCopyEnt_Other->ApplyWorldImpulse( pCopyEnt->Velocity() * m_pStage->GetFrameTime(), pCopyEnt->Position() );
		}

		this->m_pStage->CreateEntity( m_Spark, pCopyEnt->Position(),
			pCopyEnt->touch_plane.normal * 1.8f, Vector3(0,0,0) );
	}


	if( 0 <= tr.iMaterialIndex )
	{
		CSurfaceMaterial& mat = m_pStage->GetMaterial( tr.iMaterialIndex );

		if( mat.IsValid() )
		{
			int i = 0;
			int iNum = mat.GetNumReactionEntities( CSurfaceMaterial::REACTION_TO_BULLET_HIT );

			if( 0 < iNum )
			{
				CCopyEntityDesc entity_desc;

//				entity_desc.vPosition = pCopyEnt->Position() + tr.plane.normal * 0.01f;
				entity_desc.SetWorldPosition( tr.vEnd + tr.plane.normal * 0.01f );

				// decal/particle entities need the plane normal to decide the direction
//				entity_desc.vDirection = tr.plane.normal;
				entity_desc.SetWorldOrient( CreateOrientFromFwdDir( tr.plane.normal ) );

				for( i=0; i<iNum; i++ )
				{
					entity_desc.pBaseEntityHandle = &mat.GetReactionEntity( CSurfaceMaterial::REACTION_TO_BULLET_HIT, i );
					m_pStage->CreateEntity( entity_desc );
				}
			}

			if( mat.HasSoundOf( CSurfaceMaterial::REACTION_TO_HIT ) )
			{
				// play hit sound
				SoundManager().PlayAt( mat.GetSound_Random( CSurfaceMaterial::REACTION_TO_HIT ), pCopyEnt->Position() );
			}
		}
	}
}


void CBE_Bullet::Draw3DObjectBullet(CCopyEntity* pCopyEnt)
{
	Draw3DModel( pCopyEnt );
}


void CBE_Bullet::Draw(CCopyEntity* pCopyEnt)
{
	if( m_MeshProperty.m_MeshObjectHandle.GetMesh() )
	{
		Draw3DObjectBullet(pCopyEnt);
	}

	if( m_BillboardTexture.GetTexture() )
	{
		DrawBillboradTexture(pCopyEnt);
	}
}


void CBE_Bullet::DrawBillboradTexture( CCopyEntity* pCopyEnt )
{
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

	// set the world transformation matrix
	D3DXMATRIX matWorld, matRotZ;

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
//	m_pStage->GetBillboardRotationMatrix( matWorld );
	Matrix34 billboard_pose;
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	billboard_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );

	Vector3& rvPos =  pCopyEnt->Position();	// current position of this billboard
	matWorld._41 =   rvPos.x; matWorld._42 =   rvPos.y;	matWorld._43 =   rvPos.z; matWorld._44 = 1;

//	if( /* random rotation is */ true )
/*	{	// randomly rotates billboards to make them look more diverse
		float& rfRotAngle = pCopyEnt->f4;
		D3DXMatrixRotationZ( &matRotZ, rfRotAngle );
		D3DXMatrixMultiply( &matWorld, &matRotZ, &matWorld );
	}*/

	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	// use the texture color only
	pd3dDev->SetTexture( 0, m_BillboardTexture.GetTexture() );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//	pd3dDev->SetRenderState( D3DRS_DESTBLEND, m_iDestBlend );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	pd3dDev->SetVertexShader( NULL );
	pd3dDev->SetFVF( D3DFVF_TEXTUREVERTEX );

	// disable z-writing
	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	pd3dDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_avBillboardRect, sizeof(TEXTUREVERTEX) );

	pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}


bool CBE_Bullet::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string light_name;

	if( scanner.TryScanLine( "TEX_FILE",         m_BillboardTextureFilepath ) ) return true;

	if( scanner.TryScanLine( "BILLBOARD_RADIUS", m_fBillboardRadius ) ) return true;

	if( scanner.TryScanLine( "BULLET_POWER",     m_fBulletPower ) ) return true;

	if( scanner.TryScanLine( "PENETRATION",      m_fPenetration ) ) return true;

	if( scanner.TryScanLine( "REFLECTION",       m_fSpeedAttenuAtReflection ) ) return true;

	if( scanner.TryScanLine( "MAX_REFLECTS",     m_fNumMaxReflections ) ) return true;

	if( scanner.TryScanLine( "LIGHT", light_name ) )
	{
		m_Light.SetBaseEntityName(light_name.c_str());
		return true;
	}

	return false;
}


void CBE_Bullet::ReleaseGraphicsResources()
{
	CBaseEntity::ReleaseGraphicsResources();
}


void CBE_Bullet::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	CBaseEntity::LoadGraphicsResources( rParam );
}


void CBE_Bullet::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_BillboardTextureFilepath;

	ar & m_fBillboardRadius;

	ar & m_fBulletPower;
	ar & m_fPenetration;
	ar & m_fSpeedAttenuAtReflection;
	ar & m_fNumMaxReflections;

	ar & m_Spark;
	ar & m_Light;
}



/*
void CBE_Bullet::Draw(CCopyEntity* pCopyEnt)
{

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	short& rsRenderBullet = pCopyEnt->s1;
//	if( rsRenderBullet == 0 )
//		return;

	if(m_p3DModel)
	{
		Draw3DModel(pCopyEnt);
		return;
	}

	COLORVERTEX avBulletTrace[2];

	avBulletTrace[0].color = 0xCCFFEE50;
	avBulletTrace[1].color = 0xCCFFEE50;

	D3DXVECTOR3& rvPrevPosition = pCopyEnt->v1;
	avBulletTrace[0].vPosition = rvPrevPosition;
	avBulletTrace[1].vPosition = pCopyEnt->Position();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );

	pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	//use only the vertex colors to draw the bullet trace
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	pd3dDev->SetFVF( D3DFVF_COLORVERTEX );

	pd3dDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, avBulletTrace, sizeof(COLORVERTEX));

	rvPrevPosition = pCopyEnt->Position();
}*/
