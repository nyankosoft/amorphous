#include "BE_Skybox.h"
#include "BaseEntity_Draw.h"

#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "3DCommon/Shader/Shader.h"
#include "3DCommon/Shader/ShaderManager.h"
#include "3DCommon/D3DXMeshObjectBase.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"

#include "ScreenEffectManager.h"

using namespace std;


CBE_Skybox::CBE_Skybox()
{
	/// must be linked to the root node of the entity tree
	/// TODO: guarantee this
	m_aabb = AABB3( Vector3( -1200000, -1200000, -1200000 ),
		            Vector3(  1200000,  1200000,  1200000 ) );

	m_fRadius = 1800000;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_BoundingVolumeType = BVTYPE_AABB;

	m_MeshProperty.m_ShaderTechnique.resize(1,1);
	m_MeshProperty.m_ShaderTechnique(0,0).SetTechniqueName( "SkySphereFG" );
}


CBE_Skybox::~CBE_Skybox()
{
}


void CBE_Skybox::Init()
{
	Init3DModel();
}


void CBE_Skybox::InitCopyEntity( CCopyEntity* pCopyEnt )
{
//	pCopyEnt->iExtraDataIndex = (int)m_vecpStaticGeometry.size();//GetNewExtraDataID();

//	m_vecpStaticGeometry.push_back( (CStaticGeometryBase *)(pCopyEnt->pUserData) );

	Vector3 vCenterPos = pCopyEnt->Position();

}


void CBE_Skybox::Act(CCopyEntity* pCopyEnt)
{
}


void CBE_Skybox::Draw(CCopyEntity* pCopyEnt)
{
	CD3DXMeshObjectBase* pMeshObject = m_MeshProperty.m_MeshObjectHandle.GetMeshObject();
	if( !pMeshObject )
	{
		ONCE( g_Log.Print( "CBE_Skybox::Draw() - invlid mesh object: base entity '%s'", m_strName.c_str() ) );
		return;
	}

	LPD3DXBASEMESH pMesh = pMeshObject->GetBaseMesh();
	if( !pMesh )
		return;

	HRESULT hr;
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
	CShaderManager *pShaderManager = NULL;
	LPD3DXEFFECT pEffect = NULL;

	// set the world transform
	D3DXMATRIX matWorld;
	D3DXMatrixScaling( &matWorld, 10.0f, 10.0f, 10.0f );

	Vector3 vPos;
	CCamera* pCamera = m_pStage->GetCurrentCamera();
	if( pCamera )
		vPos = pCamera->GetPosition();
	else
		vPos = Vector3(0,0,0);

	matWorld._41 = vPos.x;
	matWorld._42 = vPos.y;
	matWorld._43 = vPos.z;
	matWorld._44 = 1;

	pd3dDev->SetTransform( D3DTS_WORLD, &matWorld );


	pd3dDev->SetVertexDeclaration( pMeshObject->GetVertexDeclaration() );

    int i, dwNumMaterials = pMeshObject->GetNumMaterials();

	bool shift_camera_height = true;

	if( (pShaderManager = CShader::Get()->GetCurrentShaderManager()) &&
		(pEffect = pShaderManager->GetEffect()) )
	{
		if( shift_camera_height )
		{
			CCamera *pCam = m_pStage->GetCurrentCamera();
			float fCamHeight;
			if( pCam )
				fCamHeight = pCam->GetPosition().y;
			else
				fCamHeight = 5.0f;

			pEffect->SetFloat( "g_CameraHeight", fCamHeight );
//			pEffect->SetFloat( "g_TexVShiftFactor", 0.000005f );
		}

		// render the skybox mesh with an HLSL shader

		pShaderManager->SetWorldTransform( matWorld );

		hr = pShaderManager->SetTechnique( m_MeshProperty.m_ShaderTechnique(0,0) );

		// Meshes are divided into subsets by materials. Render each subset in a loop
		for( i=0; i<dwNumMaterials; i++ )
		{
			hr = pShaderManager->SetTexture( 0, m_SkyboxTexture );

			pEffect->CommitChanges();

			UINT p, cPasses;
			pEffect->Begin( &cPasses, 0 );
			for( p = 0; p < cPasses; ++p )
			{
				pEffect->BeginPass( p );

				// Draw the mesh subset
				pMesh->DrawSubset( i );

				pEffect->EndPass();
			}
			pEffect->End();
		}
	}
	else
	{
		pd3dDev->SetVertexShader( NULL );
		pd3dDev->SetRenderState( D3DRS_ZENABLE, FALSE );
		pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

		// Meshes are divided into subsets by materials. Render each subset in a loop
		for( i=0; i<dwNumMaterials; i++ )
		{
			// Set the material and texture for this subset
	//		pd3dDev->SetMaterial( &pMeshObject->GetMaterial(i) );

			pd3dDev->SetTexture( 0, pMeshObject->GetTexture(i,0).GetTexture() );

			// Draw the mesh subset
			pMesh->DrawSubset( i );
		}

		pd3dDev->SetRenderState( D3DRS_ZENABLE, TRUE );
		pd3dDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	}
}

/*
void CBE_Skybox::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}
*/

bool CBE_Skybox::LoadSkyboxTexture( const std::string& texture_filename )
{
	m_SkyboxTexture.filename = texture_filename;
	return m_SkyboxTexture.Load();
}


bool CBE_Skybox::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	return false;
}


void CBE_Skybox::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_SkyboxTexture.filename;
}
