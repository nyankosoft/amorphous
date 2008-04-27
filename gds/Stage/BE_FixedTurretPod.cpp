#include "BE_FixedTurretPod.h"
#include "CopyEntity.h"
#include "CopyEntityDesc.h"
#include "Stage.h"
#include "ScreenEffectManager.h"

#include "Serialization_BaseEntityHandle.h"

#include  "3DCommon/Shader/Shader.h"
#include  "3DCommon/Shader/ShaderManager.h"
#include  "3DCommon/D3DXMeshObject.h"

#include  "Support/memory_helpers.h"
#include  "Support/Log/DefaultLog.h"


CBE_FixedTurretPod::CBE_FixedTurretPod()
{
	m_bNoClip = true;
	m_bLighting = true;

	m_vTurretGunLocalOffset = Vector3(0,0,0);

//	int i;
//	for( i=0; i<NUM_TURRET_MESHES; i++ )
//		m_apTurretMesh[i] = NULL;
}


CBE_FixedTurretPod::~CBE_FixedTurretPod()
{
	ReleaseGraphicsResources();
}


void CBE_FixedTurretPod::Init()
{
	LoadGraphicsResources( CGraphicsParameters() );

	PrintLog( " CBE_FixedTurretPod::Init() - loaded the graphics resources" );

	// turret pod does not use 'm_pMeshObject', which is the standard
	// mesh object for base entity
	// Therefore, the specular texture needs to be loaded separately
	if( 0 < m_MeshProperty.m_SpecTex.filename.length() )
	{
		m_MeshProperty.m_SpecTex.Load();
	}

	PrintLog( " CBE_FixedTurretPod::Init() - loaded a texture" );

	LoadBaseEntity( m_TurretGun );
}


void CBE_FixedTurretPod::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	// save the original pose of the turret
/*	Vector3& vOrigRightDir = pCopyEnt->v1;
	Vector3& vOrigFwdDir   = pCopyEnt->v2;
	vOrigRightDir = pCopyEnt->GetRightDirection();
	vOrigFwdDir   = pCopyEnt->GetDirection();
*/
	// create a turret gun that is mounted on the pod
	CCopyEntityDesc turret;
	turret.pBaseEntityHandle = &m_TurretGun;
	turret.SetWorldPosition( pCopyEnt->Position() + m_vTurretGunLocalOffset );
	turret.SetWorldOrient( pCopyEnt->GetWorldPose().matOrient );
	turret.vVelocity  = Vector3(0,0,0);
	turret.pParent = pCopyEnt;
	m_pStage->CreateEntity( turret );

}


void CBE_FixedTurretPod::Act(CCopyEntity* pCopyEnt)
{
	CCopyEntity *pTurret = pCopyEnt->GetChild(0);
	if( pTurret )
		pTurret->Act();
	else	// the child turret has been destroyed
		m_pStage->TerminateEntity( pCopyEnt );
}


void CBE_FixedTurretPod::Draw(CCopyEntity* pCopyEnt)
{
	if( !(pCopyEnt->GetChild(0)) )
	{
//		assert( pCopyEnt->GetChild(0) );
		return;
	}

	D3DXMATRIX matWorld;

//	int shader_tech_id = SHADER_TECH_INVALID;
/*	if( m_AttribFlag & ATTRIB_OPTCAM )
		shader_tech_id = SHADER_TECH_DEFAULT;	// SHADER_TECH_ENTITY_OPTCAM;
	else
		shader_tech_id = SHADER_TECH_DEFAULT;	// SHADER_TECH_ENTITY_DEFAULT;
*/
//	shader_tech_id = m_MeshProperty.m_ShaderTechniqueID;

	CShaderManager *pShaderManager = NULL;
	LPD3DXEFFECT pEffect = NULL;
	if( pShaderManager = CShader::Get()->GetCurrentShaderManager() )
	{
		if( m_MeshProperty.m_SpecTex.GetTexture() )
		{
			pShaderManager->SetTexture( 2, m_MeshProperty.m_SpecTex.GetTexture() );
//			shader_tech_id = SHADER_TECH_MESH_NOBUMP_TEXSPEC;
		}
//		else
//			shader_tech_id = SHADER_TECH_MESH_NOBUMP;
//		}
	}
//	shader_tech_id = SHADER_TECH_DEFAULT;

	const Matrix34& rWorldPose = pCopyEnt->GetWorldPose();

	// set the pose for the turret stand (constant)
	// get the up direction which is constant for turret pod
//	Vector3 vUp = rWorldPose.matOrient.GetColumn(1);

	DrawMeshObject( rWorldPose, m_aMesh[MESH_POD].m_MeshObjectHandle.GetMeshObject(), m_MeshProperty.m_ShaderTechnique );


	// set the horizontal orientation for the turntable of the turret
	// the up direction for the turn table is the same with that of the pod
	Vector3& rvCurrentDirection_H = pCopyEnt->GetChild(0)->v3;	// current horizontal direction
	Vector3 vRight, vUp, vDir;
	vUp  = rWorldPose.matOrient.GetColumn(1);
	vDir = rvCurrentDirection_H;
	Vec3Cross( vRight, vUp, vDir );

	Matrix34 world_pose = rWorldPose;
	world_pose.matOrient.SetColumn( 0, vRight );
	world_pose.matOrient.SetColumn( 2, vDir );
//	matWorld._11 = vRight.x; matWorld._12 = vRight.y; matWorld._13 = vRight.z; matWorld._14 = 0;
//	matWorld._31 =   vDir.x; matWorld._32 =   vDir.y; matWorld._33 =   vDir.z; matWorld._34 = 0;

	DrawMeshObject( world_pose, m_aMesh[MESH_TURNTABLE].m_MeshObjectHandle.GetMeshObject(), m_MeshProperty.m_ShaderTechnique );

	return;
}


void CBE_FixedTurretPod::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	int i;
	for( i=0; i<NUM_TURRET_MESHES; i++ )
	{
		m_aMesh[i].LoadMeshObject();

/*		if( 0 < m_strTurretMeshName[i].size() )
		{
			m_apTurretMesh[i] = new CD3DXMeshObject( m_strTurretMeshName[i].c_str() );

			if( !(m_apTurretMesh[i]->GetBaseMesh()) )
				MessageBox( NULL, "failed to load a mesh for turret entity", "error", MB_OK|MB_ICONWARNING );

		}*/
/*
		if( !m_apTurretMesh[i] || !(m_apTurretMesh[i]->GetBaseMesh()) )
		{
			MessageBox( NULL, "unable to load a mesh", m_strTurretMeshName[i].c_str(), MB_OK|MB_ICONWARNING );
		}*/
	}
}


void CBE_FixedTurretPod::ReleaseGraphicsResources()
{
	int i;
	for( i=0; i<NUM_TURRET_MESHES; i++ )
	{
		m_aMesh[i].Release();
//		SafeDelete( m_apTurretMesh[i] );
	}
}


bool CBE_FixedTurretPod::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string turret_gun_name;

	if( scanner.TryScanLine( "TURRETGUN_NAME", turret_gun_name ) )
	{
		m_TurretGun.SetBaseEntityName( turret_gun_name.c_str() );
		return true;
	}

	if( scanner.TryScanLine( "GUN_OFFSET",	m_vTurretGunLocalOffset ) ) return true;

	if( scanner.TryScanLine( "STAND_MESH",	m_aMesh[MESH_POD].m_MeshObjectHandle.filename ) ) return true;

	if( scanner.TryScanLine( "TTBL_MESH",	m_aMesh[MESH_TURNTABLE].m_MeshObjectHandle.filename ) ) return true;

	return false;
}


void CBE_FixedTurretPod::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_TurretGun;
	ar & m_vTurretGunLocalOffset;
	ar & m_aMesh[MESH_POD];
	ar & m_aMesh[MESH_TURNTABLE];

//	PrintLog( string(m_TurretGun.GetBaseEntityName()) + " / "
//		+ m_strTurretMeshName[MESH_POD] + " / " + m_strTurretMeshName[MESH_TURNTABLE] );
}

