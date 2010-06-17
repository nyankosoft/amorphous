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
	const int color_offset = m_ParticleSetMesh.GetVertexElementOffset( VEE::DIFFUSE_COLOR );
	uchar *pVertexBuffer = m_ParticleSetMesh.GetVertexBufferPtr();
	const uint vert_size = m_ParticleSetMesh.GetVertexSize();
	if( 0 <= color_offset )
	{
		if( true )//m_ParticleSetMesh.GetDiffuseColorType() == ARGB32 )
		{
			for(i=0; i<m_MaxNumParticlesPerSet; i++)
			{
				uchar *pColorElement = pVertexBuffer + vert_size * i * 4 + color_offset;
				memcpy( pColorElement,                 &argb_color, sizeof(U32) );
				memcpy( pColorElement + vert_size * 1, &argb_color, sizeof(U32) );
				memcpy( pColorElement + vert_size * 2, &argb_color, sizeof(U32) );
				memcpy( pColorElement + vert_size * 3, &argb_color, sizeof(U32) );
			}
		}
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
//	TEXTUREVERTEX *pParticleVertex = m_avBillboardRect;
	CCustomMesh& mesh = m_ParticleSetMesh;
	uchar *pParticleVertex = mesh.GetVertexBufferPtr();// = m_avBillboardRect_S;
	const int pos_offset = mesh.GetVertexElementOffset( VEE::POSITION );
	const uint vert_size = mesh.GetVertexSize();
	Vector3 avWorldPos[4];
	int j = 0;
	for(i=0; i<num_particles; i++)
	{
		vert_offset = i * 4;

		// calc the world position of the particle

		const Vector3& rvPos   = rParticleSet.pavPosition[i];	// center position of the billboard in world space

		billboard_pose.vPosition = rvPos;

		for(j=0; j<4; j++)
			avWorldPos[j] = billboard_pose * avBasePos[j];

		uchar *pPosElement = pParticleVertex + vert_size * i * 4 + pos_offset;
		memcpy( pPosElement,                 &avWorldPos[0], sizeof(Vector3) );
		memcpy( pPosElement + vert_size * 1, &avWorldPos[1], sizeof(Vector3) );
		memcpy( pPosElement + vert_size * 2, &avWorldPos[2], sizeof(Vector3) );
		memcpy( pPosElement + vert_size * 3, &avWorldPos[3], sizeof(Vector3) );

		// calc color and alpha value of the particle
//		U32 dwColor = 0x00FFFFFF | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );
/*		U32 dwColor = 0xFFFFFFFF;

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

//	DrawBillboards( num_particles, 0, 0, m_pStage );
	m_ParticleSetMesh.Render();
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
