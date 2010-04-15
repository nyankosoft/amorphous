#include "BE_Cloud.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "GameMessage.hpp"
#include "CopyEntity.hpp"
#include "Stage.hpp"
#include "Support/MTRand.hpp"


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

//	const U32 color = 0xFFFFFFFF;
	const U32 argb_color = 0xA0FFFFFF;
	int i;
	for(i=0; i<m_MaxNumParticlesPerSet; i++)
	{
		m_avBillboardRect[i*4+0].color = argb_color;
		m_avBillboardRect[i*4+1].color = argb_color;
		m_avBillboardRect[i*4+2].color = argb_color;
		m_avBillboardRect[i*4+3].color = argb_color;
	}
}


void CBE_Cloud::InitCopyEntity( CCopyEntity* pCopyEnt )
{
	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	SBE_ParticleSetExtraData& rParticleSet = GetExtraData( pCopyEnt->iExtraDataIndex );

	Vector3 vCenterPos = pCopyEnt->GetWorldPosition();

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

	Matrix34 billboard_pose( Matrix34Identity() );
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	Matrix44 matWorld = ToMatrix44( billboard_pose );

	// set up the local coords of the particles
	Vector3 avBasePos[4];
	avBasePos[0] = Vector3(-fRadius, fRadius, 0 );
	avBasePos[1] = Vector3( fRadius, fRadius, 0 );
	avBasePos[2] = Vector3( fRadius,-fRadius, 0 );
	avBasePos[3] = Vector3(-fRadius,-fRadius, 0 );

	// set transformed particle data to dest buffer 'm_avDestParticle'
	int vert_offset;
	TEXTUREVERTEX *pParticleVertex = m_avBillboardRect;
	for(i=0; i<num_particles; i++)
	{
		vert_offset = i * 4;

		// calc the world position of the particle

		Vector3& rvPos   = rParticleSet.pavPosition[i];	// center position of the billboard in world space

		matWorld(0,3) = rvPos.x;
		matWorld(1,3) = rvPos.y;
		matWorld(2,3) = rvPos.z;

		pParticleVertex[vert_offset+0].vPosition = matWorld * avBasePos[0];
		pParticleVertex[vert_offset+1].vPosition = matWorld * avBasePos[1];
		pParticleVertex[vert_offset+2].vPosition = matWorld * avBasePos[2];
		pParticleVertex[vert_offset+3].vPosition = matWorld * avBasePos[3];

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
