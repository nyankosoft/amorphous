
#include "GameMessage.h"
#include "CopyEntity.h"
#include "trace.h"
#include "Stage.h"
#include "Graphics/Direct3D9.h"
#include "Support/memory_helpers.h"
#include "GameCommon/MTRand.h"

#include "BE_Cloud.h"

using namespace std;


CBE_Cloud::CBE_Cloud()
{
}


CBE_Cloud::~CBE_Cloud()
{
}


void CBE_Cloud::Init()
{
	CBE_ParticleSet::Init();

//	const DWORD color = 0xFFFFFFFF;
	const DWORD color = 0xA0FFFFFF;
	int i;
	for(i=0; i<m_MaxNumParticlesPerSet; i++)
	{
		m_avBillboardRect[i*4+0].color = color;
		m_avBillboardRect[i*4+1].color = color;
		m_avBillboardRect[i*4+2].color = color;
		m_avBillboardRect[i*4+3].color = color;
	}
}


void CBE_Cloud::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	Vector3 vCenterPos = pCopyEnt->Position();

	float r = m_fParticleRadius;
	float x,y,z,w;
	int i, num_particls_per_clouds = m_MaxNumParticlesPerSet;
	rParticleSet.iNumParticles = num_particls_per_clouds;
	AABB3 world_aabb;
	world_aabb.Nullify();
	for( i=0; i<num_particls_per_clouds; i++ )
	{
		GaussianRand( x, z );
		GaussianRand( y, w );

		rParticleSet.pavPosition[i] = vCenterPos + Vector3( x * r * 1.2f, y * r * 0.6f, z * r * 1.2f );
//		rParticleSet.pavPosition[i] = vCenterPos + Vector3( x * 20.0f, y * 5.0f, z * 20.0f );
//		rParticleSet.pavPosition[i] = Vector3( -80.0f, 8.0f, 0.0f );

		Sphere sphere( rParticleSet.pavPosition[i], r );
		world_aabb.AddSphere( sphere );
	}

	pCopyEnt->world_aabb = world_aabb;
	pCopyEnt->local_aabb.vMin = world_aabb.vMin - world_aabb.GetCenterPosition();
	pCopyEnt->local_aabb.vMax = world_aabb.vMax - world_aabb.GetCenterPosition();
}


void CBE_Cloud::Act(CCopyEntity* pCopyEnt)
{
}	//behavior in in one frame


void CBE_Cloud::Draw(CCopyEntity* pCopyEnt)
{
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	float fRadius = m_fParticleRadius;
	int i, num_particles = rParticleSet.iNumParticles;
//	short sPatternOffset;

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
	//D3DXMATRIX matWorld;
	//m_pStage->GetBillboardRotationMatrix( matWorld );
	//matWorld._44 = 1;

	Matrix34 billboard_pose;
	D3DXMATRIX matWorld;
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	billboard_pose.GetRowMajorMatrix44( (Scalar *)&matWorld );

	// set up the local coords of the particles
	D3DXVECTOR3 avBasePos[4];
	avBasePos[0] = D3DXVECTOR3(-fRadius, fRadius, 0 );
	avBasePos[1] = D3DXVECTOR3( fRadius, fRadius, 0 );
	avBasePos[2] = D3DXVECTOR3( fRadius,-fRadius, 0 );
	avBasePos[3] = D3DXVECTOR3(-fRadius,-fRadius, 0 );

	// set transformed particle data to dest buffer 'm_avDestParticle'
	int vert_offset;
	TEXTUREVERTEX *pParticleVertex = m_avBillboardRect;
	for(i=0; i<num_particles; i++)
	{
		vert_offset = i * 4;

		// calc the world position of the particle

		D3DXVECTOR3& rvPos   = rParticleSet.pavPosition[i];	// center position of the billboard in world space

//		matWorld._41 =   rvPos.x; matWorld._42 =   rvPos.y;	matWorld._43 =   rvPos.z; matWorld._44 = 1;
		memcpy( &(matWorld._41), &rvPos, sizeof(D3DXVECTOR3) );

		D3DXVec3TransformCoord( &(pParticleVertex[vert_offset+0].vPosition), &avBasePos[0], &matWorld );
		D3DXVec3TransformCoord( &(pParticleVertex[vert_offset+1].vPosition), &avBasePos[1], &matWorld );
		D3DXVec3TransformCoord( &(pParticleVertex[vert_offset+2].vPosition), &avBasePos[2], &matWorld );
		D3DXVec3TransformCoord( &(pParticleVertex[vert_offset+3].vPosition), &avBasePos[3], &matWorld );

		// calc color and alpha value of the particle
//		DWORD dwColor = 0x00FFFFFF | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );
/*		DWORD dwColor = 0xFFFFFFFF;

		pParticleVertex[vert_offset+0].color = dwColor;
		pParticleVertex[vert_offset+1].color = dwColor;
		pParticleVertex[vert_offset+2].color = dwColor;
		pParticleVertex[vert_offset+3].color = dwColor;*/

/*		sPatternOffset = rParticleSet.asPattern[i] * 4;
		m_avDestParticle[0].tex = m_avRandomTexCoordTable[ sPatternOffset ];
		m_avDestParticle[1].tex = m_avRandomTexCoordTable[ sPatternOffset + 1 ];
		m_avDestParticle[2].tex = m_avRandomTexCoordTable[ sPatternOffset + 2 ];
		m_avDestParticle[3].tex = m_avRandomTexCoordTable[ sPatternOffset + 3 ];
*/
	}

/*	char acStr[256];
	sprintf( acStr, "texture: %d", m_BillboardTexture.GetTexture() );
    MessageBox( NULL, acStr, "check", MB_OK|MB_ICONWARNING );*/

	DrawBillboards( num_particles, 0, 0, m_pStage );
}


void CBE_Cloud::Touch(CCopyEntity* pCopyEnt_Self, CCopyEntity* pCopyEnt_Other)
{
}


void CBE_Cloud::ClipTrace( STrace& rLocalTrace, CCopyEntity* pMyself )
{
}


void CBE_Cloud::MessageProcedure(SGameMessage& rGameMessage, CCopyEntity* pCopyEnt_Self)
{
}


bool CBE_Cloud::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	if( CBE_ParticleSet::LoadSpecificPropertiesFromFile( scanner ) )
		return true;

	return false;
}


void CBE_Cloud::Serialize( IArchive& ar, const unsigned int version )
{
	CBE_ParticleSet::Serialize( ar, version );
}
