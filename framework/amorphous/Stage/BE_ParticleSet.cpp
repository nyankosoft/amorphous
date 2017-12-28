#include "BE_ParticleSet.hpp"
#include "GameMessage.hpp"
#include "Stage.hpp"
//#include "trace.hpp"
//#include "ScreenEffectManager.hpp"
#include "EntityRenderManager.hpp"

#include "amorphous/3DMath/MathMisc.hpp"
#include "amorphous/3DMath/MatrixConversions.hpp"
//#include "amorphous/Graphics/Shader/2DPrimitiveCommonShaders.hpp"
#include "amorphous/Graphics/Shader/CommonShaders.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/RectTriListIndex.hpp"
#include "amorphous/Graphics/MeshGenerators/MeshGenerators.hpp" // box mesh for debugging
#include "amorphous/Graphics/TextureGenerators/SingleColorTextureGenerator.hpp"
#include "amorphous/Graphics/TextureGenerators/ParticleTextureGenerator.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/MTRand.hpp"


namespace amorphous
{

using std::string;
using std::shared_ptr;


class CParticleThreadStarter
{
	CBE_ParticleSet *m_ParticleSetBaseEntity;

public:

	CParticleThreadStarter( CBE_ParticleSet *pBaseEntity ) : m_ParticleSetBaseEntity(pBaseEntity) {}

	void operator()()
	{
		// start particle thread
		m_ParticleSetBaseEntity->ParticleThreadMain();
	}
};


void CBE_ParticleSet::ParticleThreadMain()
{
//	const float dt = m_fParticleUpdateTimestep;
	const float dt = 0.03f;
	const int num_particle_sets = m_MaxNumParticleSets;
	while( !m_TerminateParticleThread )
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if( m_fFrameTimeLeft <= 0 )
			continue;

		m_fFrameTimeLeft -= dt;

		for( int i=0; i<num_particle_sets; i++ )
		{
			auto& particle_set = m_paParticleSet[i];
			AABB3 aabb;

			if( 0 < particle_set.iNumParticles )
//				UpdateParticles( particle_set, dt, aabb );
				UpdateParticlePositions( particle_set, dt, aabb );
		}

	}
}


CBE_ParticleSet::CBE_ParticleSet()
:
m_fParticleImageStandardDeviation(0.4f)
{
	m_iParticleSetCurrentIndex = 0;

	m_bNoClip = true;
	m_bNoClipAgainstMap = true;

	m_iNumTextureSegments = 1;

//	m_iNumParticles				= 8;
	m_fDuration					= 1.0f;
	m_fAnimTimeOffsetMin        = 0.0f;
	m_fAnimTimeOffsetMax        = 0.0f;
	m_fRandomVelocity_XZ		= 0.0f;
	m_fRandomVelocity_Y			= 0.0f;
	m_fExpansionFactor			= 1.0f;
	m_fGravityInfluenceFactor	= 1.0f;

	m_ParticleType = TYPE_SMOKE_NORMAL;

	m_bWorldOffset = false;

	m_bLocalPositionsForVertexElement = false;

	m_VertexColor.SetRGB( 1.0f, 1.0f, 1.0f );

	m_bMinimumParticleUpdates = false;

	m_ParticleColor = SFloatRGBAColor::White();

	// particle threads (experimental)

	m_bCreateParticleThread = false;

	m_fFrameTimeLeft = 0;

	m_TerminateParticleThread = false;

	m_MaxNumParticleSets = NUM_DEFAULT_PARTICLESETS;

	m_paParticleSet = NULL;

    m_MaxNumParticlesPerSet = NUM_DEFAULT_PARTICLES_PER_SET;

	m_VertexBufferType = VBT_SHARED_VERTEX_BUFFER;
}


CBE_ParticleSet::~CBE_ParticleSet()
{
	ReleaseGraphicsResources();

//	if( m_bCreateParticleThread )
//	{
//		m_TerminateParticleThread = true;
//
//		// wait until all the particle threads are released
//		m_ParticleThreadGroup.join_all();
//	}

	SafeDeleteArray( m_paParticleSet );
}


void CBE_ParticleSet::InitParticleSetMesh()
{
	// add a mesh material
	C3DMeshModelArchive ar;
	ar.GetMaterial().resize( 1 );

	CMMA_VertexSet& vert_set = ar.GetVertexSet();
	vert_set.m_VertexFormatFlag
		= CMMA_VertexSet::VF_POSITION
		| CMMA_VertexSet::VF_DIFFUSE_COLOR
		| CMMA_VertexSet::VF_2D_TEXCOORD0
		| CMMA_VertexSet::VF_2D_TEXCOORD1;

	const int num_verts = m_MaxNumParticlesPerSet * 4;
	vert_set.vecPosition.resize( num_verts );
	vert_set.vecDiffuseColor.resize( num_verts );
	vert_set.vecTex.resize( 1 );
	vert_set.vecTex[0].resize( num_verts );

	ar.GetTriangleSet().resize( 1 );
	ar.GetTriangleSet()[0].m_iNumTriangles = m_MaxNumParticlesPerSet * 2;
	ar.GetTriangleSet()[0].m_iNumVertexBlocksToCover = num_verts;

	const int num_indices = m_MaxNumParticlesPerSet * 6;
	ar.GetVertexIndex().resize( num_indices );

	ar.GetMaterial().resize( 1 );
	ar.GetMaterial()[0].vecTexture.resize( 1 );

	if( 0 < m_BillboardTextureFilepath.length() )
	{
		// Load a particle texture from an image file
		ar.GetMaterial()[0].vecTexture[0].ResourcePath = m_BillboardTextureFilepath;
	}
	else
	{
		// Create a particle texture (draw a blurred circle via Gaussian blur)
		ar.GetMaterial()[0].vecTexture[0].Width  = 64;
		ar.GetMaterial()[0].vecTexture[0].Height = 64;
		ar.GetMaterial()[0].vecTexture[0].Format = TextureFormat::A8R8G8B8;
		shared_ptr<ParticleTextureGenerator> pGenerator;
		pGenerator.reset( new ParticleTextureGenerator );
		pGenerator->m_Color = m_ParticleColor;
		pGenerator->m_fStandardDeviation = m_fParticleImageStandardDeviation;
//		shared_ptr<SingleColorTextureGenerator> pGenerator( new SingleColorTextureGenerator( SFloatRGBAColor::Green() ) );
		ar.GetMaterial()[0].vecTexture[0].pLoader = pGenerator;
	}


	m_ParticleSetMesh.LoadFromArchive( ar );
}


void CBE_ParticleSet::InitParticleSetEntity( CCopyEntity& entity )
{
	CurrentTime(entity) = 0.0f;

	entity.RaiseEntityFlags( BETYPE_USE_ZSORT );

	entity.iExtraDataIndex = GetNewExtraDataID();
}


void CBE_ParticleSet::Init()
{
	// allocate buffer for particle sets
	SafeDeleteArray( m_paParticleSet );
	m_paParticleSet = new ParticleSetExtraData [m_MaxNumParticleSets];

	int i;
	const int max_num_particle_sets = m_MaxNumParticleSets;
	for( i=0; i<max_num_particle_sets; i++ )
	{
		m_paParticleSet[i].Init( ParticleSetFlag::ALL, m_MaxNumParticlesPerSet );
	}

	// Create an empty mesh with a billboard texture
	InitParticleSetMesh();

	// Set vertices and indices to m_ParticleSetMesh
	InitBillboardRects();

	InitParticles();

//	m_Type = TYPE_RECT_ARRAY_AND_INDICES;

//	HRESULT hr;
/*	switch( m_Type )
	{
	case TYPE_BILLBOARDARRAYMESH:
	case TYPE_BILLBOARDARRAYMESH_SHARED:
		{
			LoadBillboardArrayMesh( m_fParticleRadius, m_MaxNumParticleSets, m_MaxNumParticlesPerSet, m_iNumTextureSegments );
		}
		break;

	case TYPE_RECT_ARRAY_AND_INDICES:
		{
			InitBillboardRects();
		}
		break;

	default:
		break;
	}*/

	// start particle thread
	if( false )//m_bCreateParticleThread )
	{
/*		m_pThreadPooler = new CThreadPooler();

		m_pThreadPooler->StartThread( 1 );

		smart_ptr< function_callback > fn( new function_callback_vm0<CBE_ParticleSet>(ParticleThreadMain,this) );
//		m_fn = smart_ptr< function_callback >( new function_callback_vm0<CBE_ParticleSet>(ParticleThreadMain,this), false );

		// let the particle thread do its job
		m_pThreadPooler->Invoke( fn );*/

//		for( i=0; i<1; i++ )
//			m_ParticleThreadGroup.add_thread( new boost::thread(CParticleThreadStarter(this)) );
	}

//	m_MeshProperty.m_ShaderHandle = Get2DPrimitiveCommonShader( C2DPrimitiveCommonShaders::ST_DIFFUSE_COLOR_AND_TEXTURE );
	m_MeshProperty.m_ShaderHandle = GetNoLightingShader();

	MeshResourceDesc mesh_desc;
	shared_ptr<BoxMeshGenerator> pBoxMeshGenerator( new BoxMeshGenerator );
	pBoxMeshGenerator->SetEdgeLengths( Vector3(1,1,1) * 0.1f );
	mesh_desc.pMeshGenerator = pBoxMeshGenerator;
	m_ParticleDebugBox.Load( mesh_desc );
}


void CBE_ParticleSet::InitParticles()
{
	int i, j;
	for(i=0; i<m_MaxNumParticleSets; i++)
	{
		for(j=0; j<m_MaxNumParticlesPerSet; j++)
		{
			m_paParticleSet[i].pafAnimDuration[j] = m_fDuration + RangedRand( -0.1f, 0.2f );
		}
	}
}


void CBE_ParticleSet::InitBillboardRects()
{
	CustomMesh& mesh = m_ParticleSetMesh;

	// vert buffer initialization - done in InitParticleSetMesh()
/*	U32 vert_flags
		= VFF::POSITION
		| VFF::DIFFUSE_COLOR
		| VFF::TEXCOORD2_0;
		| VFF::TEXCOORD2_1;

	mesh.InitVertexBuffer( m_MaxNumParticlesPerSet * 4, vert_flags );*/
	uchar *pVertexBuffer = mesh.GetVertexBufferPtr();

	int tex_offset = mesh.GetVertexElementOffset( VEE::TEXCOORD2_0 );
//	if( tex_offset < 0 )
//		return;

	// set particle properties
	float fPRadius = m_fParticleRadius;

	int i;
	int num_tex_segs = m_iNumTextureSegments;
	int num_tex_patterns = num_tex_segs * num_tex_segs;
	float fTex = 1.0f / (float)m_iNumTextureSegments;	// TODO: random texture coord for particles
	int offset;
	TEXCOORD2 tex0, tex1, tex2, tex3;
	const int max_num_particles_per_set = m_MaxNumParticlesPerSet;
	uint vert_size = mesh.GetVertexSize();
	for(i=0; i<max_num_particles_per_set; i++)
	{
		// set random texture patterns for each particle in advance
		offset = RangedRand( 0, num_tex_patterns - 1 );
		float u = (float)(offset % num_tex_segs) * fTex;
		float v = (float)(offset / num_tex_segs) * fTex;

//		SetTexCoord( i, 0, TEXCOORD2(u,        v       ) );
//		SetTexCoord( i, 1, TEXCOORD2(u + fTex, v       ) );
//		SetTexCoord( i, 2, TEXCOORD2(u + fTex, v + fTex) );
//		SetTexCoord( i, 3, TEXCOORD2(u,        v + fTex) );

		tex0 = TEXCOORD2(u + fTex, v + fTex);
		tex1 = TEXCOORD2(u + fTex, v       );
		tex2 = TEXCOORD2(u,        v       );
		tex3 = TEXCOORD2(u,        v + fTex);

		uchar *pVertElement = pVertexBuffer + i * 4 * vert_size + tex_offset;
		memcpy( pVertElement,                 &tex0, sizeof(TEXCOORD2) );
		memcpy( pVertElement + vert_size * 1, &tex1, sizeof(TEXCOORD2) );
		memcpy( pVertElement + vert_size * 2, &tex2, sizeof(TEXCOORD2) );
		memcpy( pVertElement + vert_size * 3, &tex3, sizeof(TEXCOORD2) );
	}

	// set rect vertex positions in local space
	// these values are not changed during particle rendering
	const float r = m_fParticleRadius;
	int lo_offset = mesh.GetVertexElementOffset( VEE::TEXCOORD2_1 );
	bool use_local_offset = (0 <= lo_offset) && m_bLocalPositionsForVertexElement;
//	bool use_local_offset = false;
	Vector2 lo0, lo1, lo2, lo3;
	if( use_local_offset )
	{
		lo0 = Vector2(-r, r);
		lo1 = Vector2( r, r);
		lo2 = Vector2( r,-r);
		lo3 = Vector2(-r,-r);
		for(i=0; i<max_num_particles_per_set; i++)
		{
			uchar *pVertElement = pVertexBuffer + i * 4 * vert_size + lo_offset;
			memcpy( pVertElement,                 &lo0, sizeof(Vector2) );
			memcpy( pVertElement + vert_size * 1, &lo1, sizeof(Vector2) );
			memcpy( pVertElement + vert_size * 2, &lo2, sizeof(Vector2) );
			memcpy( pVertElement + vert_size * 3, &lo3, sizeof(Vector2) );
		}
	}

	// set indices
	uchar *pIndexBuffer = mesh.GetIndexBufferPtr();
	const int num_indices = max_num_particles_per_set * 6;
	mesh.InitIndexBuffer( num_indices, sizeof(U16) ); // Actually this has already been done in InitParticleSetMesh()
	CRectTriListIndexBuffer::SetNumMaxRects( max_num_particles_per_set );
	memcpy( pIndexBuffer, &(CRectTriListIndexBuffer::GetIndexBuffer()[0]), sizeof(U16) * num_indices );


/*
#ifdef BE_PARTICLESET_USE_VB_AND_IB

	InitVertexBufferAndIndexBuffer();

#endif
*/

	m_FadeTable.SetQuadraticFadeout( 1.0f, 0.0f, 1.2f, 1.0f, 0.0f );

}


void CBE_ParticleSet::InitCopyEntity(CCopyEntity* pCopyEnt)
{
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime = 0.0f;

	pCopyEnt->iExtraDataIndex = GetNewExtraDataID();
	auto& particle_set = GetExtraData( pCopyEnt->iExtraDataIndex );

	float fRandX, fRandY, fRandZ;
	float fRandVelocity_XZ = m_fRandomVelocity_XZ;
	float fRandVelocity_Y  = m_fRandomVelocity_Y;
	int i, num_particles = m_MaxNumParticlesPerSet;
	for(i=0; i<num_particles; i++)
	{
		particle_set.pavPosition[i] = pCopyEnt->GetWorldPosition();
		fRandX = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_XZ;
		fRandZ = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_XZ;
		fRandY = ( 0.5f - RangedRand(0.0f, 1.0f) ) * fRandVelocity_Y;
		particle_set.pavVelocity[i] = pCopyEnt->Velocity() + Vector3(fRandX, fRandY, fRandZ);

		particle_set.pafAnimationTime[i] = 0.0f;
		particle_set.pasPattern[i] = 0;
	}
	particle_set.iNumParticles = num_particles;

	pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );
//	pCopyEnt->bUseZSort = true;
}


void CBE_ParticleSet::Act(CCopyEntity* pCopyEnt)
{
	float fFrameTime = m_pStage->GetFrameTime();
	float& rfCurrentTime = pCopyEnt->f2;
	rfCurrentTime += fFrameTime;
	if( m_fDuration <= rfCurrentTime )
	{
		m_pStage->TerminateEntity( pCopyEnt );
		return;
	}

	// update the positions of particles

	Vector3 vGravityAccel = m_pStage->GetGravityAccel();
	float fGravityInfluenceFactor = m_fGravityInfluenceFactor;

//	U32 dwColor = D3DCOLOR_ARGB( ((int)((m_fDuration - rfCurrentTime) / m_fDuration * 255.0f)), 255, 255, 255 );

	ParticleSetExtraData& particle_set = GetExtraData( pCopyEnt->iExtraDataIndex );
	int i, num_particles = m_MaxNumParticlesPerSet;

	if( m_ParticleType == TYPE_SMOKE_SHOOTING )
	{
		for(i=0; i<num_particles; i++)
		{
		  if( 0.01f < Vec3Dot( particle_set.pavVelocity[i], particle_set.pavOrigDirection[i] ) )
		  {

			// update velocity
			particle_set.pavVelocity[i] -= particle_set.pavOrigDirection[i] * 28.0f * fFrameTime * RangedRand( 0.9f, 1.1f );
//			particle_set.pavVelocity[i]
//				-= particle_set.pavOrigDirection[i]
//				* Vec3Dot( particle_set.pavVelocity[i], particle_set.pavOrigDirection[i] ) * 1.0f * fFrameTime;

			if( Vec3Dot( particle_set.pavVelocity[i], particle_set.pavOrigDirection[i] ) < 0 )
				particle_set.pavVelocity[i] = Vector3(0,0,0);
		  }


			particle_set.pavVelocity[i] += (vGravityAccel * fGravityInfluenceFactor) * fFrameTime;

			// update position
			particle_set.pavPosition[i] += particle_set.pavVelocity[i] * fFrameTime;

			// update animation time
			particle_set.pafAnimationTime[i] += fFrameTime;
		}
	}
	else
	{
		AABB3 aabb;
		aabb.Nullify();
		UpdateParticles( particle_set, fFrameTime, aabb );

		pCopyEnt->world_aabb.MergeAABB( aabb );
	}

/*	if( m_Type == CBEC_Billboard::TYPE_BILLBOARDARRAYMESH )
	{
		// each copy entity (i.e. particle group) has its own paticles vertices
		UpdateVertexBuffer( pCopyEnt );
	}*/
}


void CBE_ParticleSet::Draw(CCopyEntity* pCopyEnt)
{
	DrawParticles(pCopyEnt);
}


static void SetRectDiffuseColorAndAlpha_FRGBA( uchar *pDiffuseColorElement, int vert_size, const float *rgba )
{
	memcpy( pDiffuseColorElement,                 rgba, sizeof(float) * 4 );
	memcpy( pDiffuseColorElement + vert_size * 1, rgba, sizeof(float) * 4 );
	memcpy( pDiffuseColorElement + vert_size * 2, rgba, sizeof(float) * 4 );
	memcpy( pDiffuseColorElement + vert_size * 3, rgba, sizeof(float) * 4 );
}

static void SetRectDiffuseColorAndAlpha_ARGB32( uchar *pDiffuseColorElement, int vert_size, const float *rgba )
{
	U32 color
		= ((int)(rgba[3] * 255.0f)) << 24
		| ((int)(rgba[0] * 255.0f)) << 16
		| ((int)(rgba[1] * 255.0f)) << 8
		| ((int)(rgba[2] * 255.0f));
	memcpy( pDiffuseColorElement,                 &color, sizeof(U32) );
	memcpy( pDiffuseColorElement + vert_size * 1, &color, sizeof(U32) );
	memcpy( pDiffuseColorElement + vert_size * 2, &color, sizeof(U32) );
	memcpy( pDiffuseColorElement + vert_size * 3, &color, sizeof(U32) );
}


static void SetRectDiffuseColor_FRGBA( uchar *pDiffuseColorElement, int vert_size, const float *rgb )
{
	memcpy( pDiffuseColorElement,                 rgb, sizeof(float) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 1, rgb, sizeof(float) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 2, rgb, sizeof(float) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 3, rgb, sizeof(float) * 3 );
}

static void SetRectDiffuseColor_ARGB32( uchar *pDiffuseColorElement, int vert_size, const float *rgb )
{
	U32 color
		= ((int)(rgb[0] * 255.0f)) << 16
		| ((int)(rgb[1] * 255.0f)) << 8
		| ((int)(rgb[2] * 255.0f));
	memcpy( pDiffuseColorElement,                 &color, sizeof(U8) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 1, &color, sizeof(U8) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 2, &color, sizeof(U8) * 3 );
	memcpy( pDiffuseColorElement + vert_size * 3, &color, sizeof(U8) * 3 );
}


/*
static void SetRectDiffuseAlpha_FRGBA( uchar *pVertElement, int vert_size, float fAlpha )
{
	uchar *pAlphaOffset = pVertElement + sizeof(float) * 3;
	memcpy( pAlphaOffset,                 &fAlpha, sizeof(float) );
	memcpy( pAlphaOffset + vert_size * 1, &fAlpha, sizeof(float) );
	memcpy( pAlphaOffset + vert_size * 2, &fAlpha, sizeof(float) );
	memcpy( pAlphaOffset + vert_size * 3, &fAlpha, sizeof(float) );
}
*/

static U32 sg_VertColor = 0xFFFFFFFF;

/*
static void SetRectDiffuseAlpha_ARGB32( uchar *pVertElement, int vert_size, float fAlpha )
{
	U32 color = sg_VertColor | ( ((int)(fAlpha * 255.0f)) << 24 );
	memcpy( pVertElement,                 &color, sizeof(U32) );
	memcpy( pVertElement + vert_size * 1, &color, sizeof(U32) );
	memcpy( pVertElement + vert_size * 2, &color, sizeof(U32) );
	memcpy( pVertElement + vert_size * 3, &color, sizeof(U32) );

//	U8 a = (int)(fAlpha * 255.0f);
//	memcpy( pVertElement,                 &color, sizeof(U8) );
//	memcpy( pVertElement + vert_size * 1, &color, sizeof(U8) );
//	memcpy( pVertElement + vert_size * 2, &color, sizeof(U8) );
//	memcpy( pVertElement + vert_size * 3, &color, sizeof(U8) );
}
*/

void CBE_ParticleSet::UpdateVertices( CCopyEntity& entity )
{
	PROFILE_FUNCTION();

	CustomMesh& mesh = m_ParticleSetMesh;

	ParticleSetExtraData& particle_set = GetExtraData( entity.iExtraDataIndex );

///	float fRadius, fBaseRadius = m_fParticleRadius;
	float fFraction, fTotalAnimationTime = m_fDuration;
	float fExpansionFactor = m_fExpansionFactor;
	int i, num_particles = particle_set.iNumParticles;
//	short sPatternOffset;

	// vertex color - alpha is set later for each particle to make fading effects
	const SFloatRGBColor vert_color = m_VertexColor;
	sg_VertColor = 0x00FFFFFF & m_VertexColor.GetARGB32();

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
	Matrix44 matWorld;
	Matrix34 billboard_pose;
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	ToMatrix44( billboard_pose, matWorld );

	// set transformed particle data to dest buffer 'm_avDestParticle'
	int vert_offset;
	float factor;

	float particle_set_anim_time;
	if( m_bMinimumParticleUpdates )
		particle_set_anim_time = CurrentTime(entity);
	else
		particle_set_anim_time = 0;

	void (*SetRectDiffuseColor)( uchar *, int , const float * ) = NULL;

//	if( mesh.GetDiffuseColorElementSize() == sizeof(U32) )
	if( true )
		SetRectDiffuseColor = SetRectDiffuseColor_ARGB32; // mainly used by Direct3D
	else
		SetRectDiffuseColor = SetRectDiffuseColor_FRGBA;  // mainly used by OpenGL

//	ProfileBegin( "CBE_ParticleSet::UpdateVB() - lock VB" );

	uchar *pParticleVertex = mesh.GetVertexBufferPtr();// = m_avBillboardRect_S;

/*  // Cache fractions?
	float *pFractions = m_afTempFractionBuffer;
	bool needs_fraction = true;
	if( needs_fraction )
	{
		for(i=0; i<num_particles; i++)
		{
			float fCurrentTime = particle_set_anim_time + particle_set.pafAnimationTime[i];
			pFractions[i] = fCurrentTime / fTotalAnimationTime;
		}
	}*/

	const int vert_size     = mesh.GetVertexSize();
	const int color_offset  = mesh.GetVertexElementOffset( VEE::DIFFUSE_COLOR );
	const int pos_offset    = mesh.GetVertexElementOffset( VEE::POSITION );
	const int factor_offset = -1;//mesh.GetVertexElementOffset( VEE::TEXCOORD1_0 );

	// Use this when update of an element is outside the main loop
//	int 2vert_size = vert_size * 2;
//	int 3vert_size = vert_size * 3;

	float rgb[3] = {0,0,0};
	const float zero_rgb[3] = {0,0,0};

	// for visual debugging
	shared_ptr<BasicMesh> pBoxMesh = m_ParticleDebugBox.GetMesh();

	for(i=0; i<num_particles; i++)
	{
		// change anim duration for each particle
		fTotalAnimationTime = particle_set.pafAnimDuration[i];

		vert_offset = i * 4 * vert_size;
		uchar *pVert0 = pParticleVertex + i * 4 * vert_size;
		uchar *pVert1 = pVert0 + vert_size;
		uchar *pVert2 = pVert1 + vert_size;
		uchar *pVert3 = pVert2 + vert_size;

		// set alpha value - smoke particles fade away as time passes
		float fCurrentTime = particle_set_anim_time + particle_set.pafAnimationTime[i];

		if( fCurrentTime < 0 || fTotalAnimationTime <= fCurrentTime )
		{
			(*SetRectDiffuseColor)( pVert0 + color_offset, vert_size, zero_rgb );
			continue;
		}

		const Vector3 vBasePos = particle_set.pavPosition[i];
		memcpy( pVert0 + pos_offset, &vBasePos, sizeof(Vector3) );
		memcpy( pVert1 + pos_offset, &vBasePos, sizeof(Vector3) );
		memcpy( pVert2 + pos_offset, &vBasePos, sizeof(Vector3) );
		memcpy( pVert3 + pos_offset, &vBasePos, sizeof(Vector3) );

//		if( pBoxMesh )
//		{
//			FixedFunctionPipelineManager().SetWorldTransform( Matrix34( vBasePos, Matrix33Identity() ) );
//			pBoxMesh->Render();
//		}

		fFraction = fCurrentTime / fTotalAnimationTime;

//		bool expand = true;
//		if( expand )
		if( 0 <= factor_offset )
		{
			// Particles grow
//			fRadius = fBaseRadius * (1.0f + fFraction * 1.5f);	// expand particle up to 2.5 times
///			fRadius = fBaseRadius * (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
			factor = (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
			memcpy( pVert0 + factor_offset, &factor, sizeof(float) );
			memcpy( pVert1 + factor_offset, &factor, sizeof(float) );
			memcpy( pVert2 + factor_offset, &factor, sizeof(float) );
			memcpy( pVert3 + factor_offset, &factor, sizeof(float) );
		}


		// calc color and alpha value of the particle
//		U32 color = 0x00FFFFFF | ( ((int)((m_FadeTable.GetValue(fFraction)) * 255.0f)) << 24 );
//		U32 color = 0x00FFFFFF | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );
		float fAlpha = 1.0f - fFraction;
//		U32 color = vert_color | ( ((int)(fAlpha * 255.0f)) << 24 );

//		float& rfFadeVel = particle_set.pafFadeVel[i];
//		U32 color = 0x00FFFFFF | (int)( SmoothCD( fFraction, 0.0f, rfFadeVel, fTotalAnimationTime, dt ) );
//		color = 0xFFFFFFFF;

		rgb[0] = vert_color.red   * fAlpha;
		rgb[1] = vert_color.green * fAlpha;
		rgb[2] = vert_color.blue  * fAlpha;
		(*SetRectDiffuseColor)( pVert0 + color_offset, vert_size, rgb );
	}

/*	bool expand = true;
	if( expand )
	{
		uchar *pVertElement = pParticleVertex + factor_offset;
		for(i=0; i<num_particles; i++)
		{
			factor = (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
			memcpy( pVertElement,                 &factor, sizeof(float) );
			memcpy( pVertElement + vert_size * 1, &factor, sizeof(float) );
			memcpy( pVertElement + vert_size * 2, &factor, sizeof(float) );
			memcpy( pVertElement + vert_size * 3, &factor, sizeof(float) );
		}
	}*/

/*	if( m_bWorldOffset )
	{
		Matrix34 world_pose = pCopyEnt->GetWorldPose();
		for( i=0; i<num_particles * 4; i++ )
		{
			world_pose.Transform( pParticleVertex[i].vPosition, pParticleVertex[i].vPosition );
		}
	}*/

//	ProfileEnd( "CBE_ParticleSet::UpdateVB() - VB setup" );
}


void CBE_ParticleSet::UpdateVerticesFFP( CCopyEntity& entity )
{
	PROFILE_FUNCTION();

	CustomMesh& mesh = m_ParticleSetMesh;

	ParticleSetExtraData& particle_set = GetExtraData( entity.iExtraDataIndex );

///	float fRadius, fBaseRadius = m_fParticleRadius;
	float fFraction, fTotalAnimationTime = m_fDuration;
	float fExpansionFactor = m_fExpansionFactor;
	int i, num_particles = particle_set.iNumParticles;
//	short sPatternOffset;

	// vertex color - alpha is set later for each particle to make fading effects
	const SFloatRGBColor vert_color = m_VertexColor;
	sg_VertColor = 0x00FFFFFF & m_VertexColor.GetARGB32();

	// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
	Matrix44 matWorld;
	Matrix34 billboard_pose( Matrix34Identity() );
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	ToMatrix44( billboard_pose, matWorld );

	Vector3 avWorldPos[4];

	// set transformed particle data to dest buffer 'm_avDestParticle'
	int vert_offset;
	float factor;

	float particle_set_anim_time;
	if( m_bMinimumParticleUpdates )
		particle_set_anim_time = CurrentTime(entity);
	else
		particle_set_anim_time = 0;

	void (*SetRectDiffuseColor)( uchar *, int , const float * ) = NULL;

//	if( mesh.GetDiffuseColorElementSize() == sizeof(U32) )
	if( true )
		SetRectDiffuseColor = SetRectDiffuseColorAndAlpha_ARGB32; // mainly used by Direct3D
	else
		SetRectDiffuseColor = SetRectDiffuseColorAndAlpha_FRGBA;  // mainly used by OpenGL

//	ProfileBegin( "CBE_ParticleSet::UpdateVB() - lock VB" );

	uchar *pParticleVertex = mesh.GetVertexBufferPtr();// = m_avBillboardRect_S;

	const int vert_size     = mesh.GetVertexSize();
	const int color_offset  = mesh.GetVertexElementOffset( VEE::DIFFUSE_COLOR );
	const int pos_offset    = mesh.GetVertexElementOffset( VEE::POSITION );

	// Use this when update of an element is outside the main loop
//	int 2vert_size = vert_size * 2;
//	int 3vert_size = vert_size * 3;

//	float rgb[3] = {0,0,0};
//	const float zero_rgb[3] = {0,0,0};
	float rgba[4] = {0,0,0,0};
	const float zero_rgba[4] = {0,0,0,0};

	// for visual debugging
//	shared_ptr<BasicMesh> pBoxMesh = m_ParticleDebugBox.GetMesh();

	for(i=0; i<num_particles; i++)
	{
		// change anim duration for each particle
		fTotalAnimationTime = particle_set.pafAnimDuration[i];

		vert_offset = i * 4 * vert_size;
		uchar *pVert0 = pParticleVertex + i * 4 * vert_size;
		uchar *pVert1 = pVert0 + vert_size;
		uchar *pVert2 = pVert1 + vert_size;
		uchar *pVert3 = pVert2 + vert_size;

		// set alpha value - smoke particles fade away as time passes
		float fCurrentTime = particle_set_anim_time + particle_set.pafAnimationTime[i];

		if( fCurrentTime < 0 || fTotalAnimationTime <= fCurrentTime )
		{
			(*SetRectDiffuseColor)( pVert0 + color_offset, vert_size, zero_rgba );
			continue;
		}
	
//		factor = (1.0f + fFraction * ( fExpansionFactor - 1.0f ));
		factor = 1.0f;
		float r = m_fParticleRadius * factor;
		const Vector3 vBasePos = particle_set.pavPosition[i];
		billboard_pose.vPosition = vBasePos;
		avWorldPos[0] = billboard_pose * Vector3( r, r, 0);
		avWorldPos[1] = billboard_pose * Vector3( r,-r, 0);
		avWorldPos[2] = billboard_pose * Vector3(-r,-r, 0);
		avWorldPos[3] = billboard_pose * Vector3(-r, r, 0);
		memcpy( pVert0 + pos_offset, &avWorldPos[0], sizeof(Vector3) );
		memcpy( pVert1 + pos_offset, &avWorldPos[1], sizeof(Vector3) );
		memcpy( pVert2 + pos_offset, &avWorldPos[2], sizeof(Vector3) );
		memcpy( pVert3 + pos_offset, &avWorldPos[3], sizeof(Vector3) );

//		if( pBoxMesh )
//		{
//			FixedFunctionPipelineManager().SetWorldTransform( Matrix34( vBasePos, Matrix33Identity() ) );
//			pBoxMesh->Render();
//		}

		fFraction = fCurrentTime / fTotalAnimationTime;

		// calc color and alpha value of the particle
//		U32 color = 0x00FFFFFF | ( ((int)((m_FadeTable.GetValue(fFraction)) * 255.0f)) << 24 );
//		U32 color = 0x00FFFFFF | ( ((int)((1.0f - fFraction) * 255.0f)) << 24 );
		float fAlpha = 1.0f - fFraction;
//		U32 color = vert_color | ( ((int)(fAlpha * 255.0f)) << 24 );

		// 2015-11-29 Commented out; Direct3D and OpenGL use different diffuse color formats (U32 and float[4] respectively),
		// so performance concerns aside we should let the CustomMesh API handle the differences.
//		rgba[0] = vert_color.red   * fAlpha;
//		rgba[1] = vert_color.green * fAlpha;
//		rgba[2] = vert_color.blue  * fAlpha;
//		rgba[3] = fAlpha;
//		(*SetRectDiffuseColor)( pVert0 + color_offset, vert_size, rgba );

		SFloatRGBAColor vertex_color;
		vertex_color.red   = vert_color.red   * fAlpha;
		vertex_color.green = vert_color.green * fAlpha;
		vertex_color.blue  = vert_color.blue  * fAlpha;
		vertex_color.alpha = 0;
		SFloatRGBAColor( rgba[0], rgba[1], rgba[2], rgba[3] );
		for( int j=0; j<4; j++ )
		{
			mesh.SetDiffuseColor( i*4+j, vertex_color );
		}
	}

/*	if( m_bWorldOffset )
	{
		Matrix34 world_pose = pCopyEnt->GetWorldPose();
		for( i=0; i<num_particles * 4; i++ )
		{
			world_pose.Transform( pParticleVertex[i].vPosition, pParticleVertex[i].vPosition );
		}
	}*/

//	ProfileEnd( "CBE_ParticleSet::UpdateVB() - VB setup" );
}


void CBE_ParticleSet::UpdateMesh( CCopyEntity* pCopyEnt )
{
	bool lockless_mesh = true;
	if( lockless_mesh )
	{
		UpdateVertices( *pCopyEnt );
	}
/*	else
	{
		CLockedVertexBuffer *pVB = mesh.LockVertexBuffer();
		if( pVB )
		{
			UpdateVertices( pCopyEnt );
			mesh.UnlockVertexBuffer();
		}
	}*/

//	UpdateVertices();
}


/// draw particles through billboard array mesh or rect array
void CBE_ParticleSet::DrawParticles( CCopyEntity* pCopyEnt )
{
	CCopyEntity& entity = *pCopyEnt;

	PROFILE_FUNCTION();

	ShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	if( pShaderManager )
//		UpdateVertices( pCopyEnt );
		UpdateVerticesFFP( entity );
	else
		UpdateVerticesFFP( entity );

/*	if( m_Type != TYPE_BILLBOARDARRAYMESH )
	{
		// particle vertices are shared by copy entities of this base entity
		// to save memory - need to update VB in rendering routine
		UpdateVertexBuffer( pCopyEnt );
	}*/

//	ParticleSetExtraData& particle_set = GetExtraData( pCopyEnt->iExtraDataIndex );
//	const int num_particles = particle_set.iNumParticles;

//	ProfileBegin( "DrawParticles(): pEffect->SetMatrix(), etc." );

	GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
	GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );

	GraphicsDevice().Disable( RenderStateType::LIGHTING );

	Matrix34 billboard_pose( Matrix34Identity() );
	m_pStage->GetBillboardRotationMatrix( billboard_pose.matOrient );
	const Matrix44 billboard_matrix = ToMatrix44( billboard_pose );
	if( pShaderManager )
		pShaderManager->SetParam( "ParticleWorldRot", billboard_matrix );

//	ProfileEnd( "DrawParticles(): pEffect->SetMatrix(), etc." );

	GraphicsDevice().Enable( RenderStateType::ALPHA_BLEND );
	GraphicsDevice().SetSourceBlendMode( AlphaBlend::One );
	GraphicsDevice().SetDestBlendMode( AlphaBlend::InvSrcAlpha );

	// draw particles
	if( pShaderManager )
	{
		auto& shader_mgr = *pShaderManager;
		// render particle via programmable shader
		ShaderTechniqueHandle tech;
		tech.SetTechniqueName( "Default" );
		shader_mgr.SetTechnique( tech );
		shader_mgr.SetWorldTransform( Matrix44Identity() );
		m_ParticleSetMesh.Render( shader_mgr );

//		if( m_ParticleSetMesh.GetMaterial(0).TextureDesc[0].ResourcePath.find("<Texture>") == 0 )
//		{
//			static int s_saved = 0;
//			if( s_saved == 0 )
//			{
//				m_ParticleSetMesh.Material(0).Texture[0].SaveTextureToImageFile( "particle_texture.png" );
//				s_saved = 1;
//			}
//		}
	}
	else
	{
		// render particle via fixed function pipeline
//		FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );
//		m_ParticleSetMesh.Render();
	}

/*	if( m_Type == TYPE_BILLBOARDARRAYMESH )
		DrawBillboards( num_particles, pCopyEnt->iExtraDataIndex, m_MaxNumParticlesPerSet, m_pStage );
	else
		DrawBillboards( num_particles, 0, 0, m_pStage );
*/
	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}


bool CBE_ParticleSet::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	BaseEntity::LoadSpecificPropertiesFromFile( scanner );

	string smoke_type;

	if( scanner.TryScanLine( "TEXTURE", m_BillboardTextureFilepath ) ) return true;

	if( scanner.TryScanLine( "PTCL_RADIUS",     m_fParticleRadius ) ) return true;
	if( scanner.TryScanLine( "DURATION",        m_fDuration ) ) return true;
	if( scanner.TryScanLine( "RANDVEL_XZ",      m_fRandomVelocity_XZ ) ) return true;
	if( scanner.TryScanLine( "RANDVEL_Y",       m_fRandomVelocity_Y ) ) return true;

	if( scanner.TryScanLine( "ANIMTIME_OFFSET", m_fAnimTimeOffsetMin, m_fAnimTimeOffsetMax ) ) return true;

	if( scanner.TryScanLine( "MAX_NUM_PARTICLE_GROUPS",     m_MaxNumParticleSets ) ) return true;
	if( scanner.TryScanLine( "MAX_NUM_PARTICLES_PER_GROUP", m_MaxNumParticlesPerSet ) ) return true;
//	if( scanner.TryScanLine( "NUM_PTCLS", m_iNumParticles ) )
//	{
//		Limit( m_iNumParticles, 1, m_MaxNumParticlesPerSet );
//		return true;
//	}

	float& rho = m_fParticleImageStandardDeviation;
	SFloatRGBAColor& pc = m_ParticleColor;
	if( scanner.TryScanLine( "PARTICLE_TEXTURE", pc.red, pc.green, pc.blue, rho ) ) return true;

	SFloatRGBColor& color = m_VertexColor;
	if( scanner.TryScanLine( "VERT_COLOR",	color.red, color.green, color.blue ) ) return true;

	if( scanner.TryScanLine( "NUM_TEXSEGS", m_iNumTextureSegments ) )
	{
		if( PTCL_NUM_MAX_TEXTURE_SEGMENTS < m_iNumTextureSegments )
			m_iNumTextureSegments = PTCL_NUM_MAX_TEXTURE_SEGMENTS;
		return true;
	}

	if( scanner.TryScanLine( "EXPANSION", m_fExpansionFactor ) ) return true;
	if( scanner.TryScanLine( "GRAVITY_INFLUENCE", m_fGravityInfluenceFactor ) ) return true;


	if( scanner.TryScanLine( "SMOKE_TYPE", smoke_type ) ) 
	{
		if( smoke_type == "TYPE_NORMAL" )			m_ParticleType = TYPE_SMOKE_NORMAL;
		else if( smoke_type == "TYPE_DIRECTIONAL" )	m_ParticleType = TYPE_SMOKE_DIRECTIONAL;
		else if( smoke_type == "TYPE_SHOOTING" )	m_ParticleType = TYPE_SMOKE_SHOOTING;
		else return false;

		return true;
	}

	return false;
}


void CBE_ParticleSet::Serialize( serialization::IArchive& ar, const unsigned int version )
{
	BaseEntity::Serialize( ar, version );

	ar & m_ParticleType;
	ar & m_fParticleRadius;
//	ar & m_iNumParticles;
	ar & m_iNumTextureSegments;
	ar & m_fDuration;
	ar & m_fAnimTimeOffsetMin & m_fAnimTimeOffsetMax;
	ar & m_fRandomVelocity_XZ & m_fRandomVelocity_Y;
	ar & m_fExpansionFactor;
	ar & m_fGravityInfluenceFactor;
	ar & m_VertexColor;
	ar & m_bWorldOffset;
	ar & m_bLocalPositionsForVertexElement;
	ar & m_bCreateParticleThread;
	ar & m_bMinimumParticleUpdates;
	ar & m_MaxNumParticleSets;
    ar & m_MaxNumParticlesPerSet;
	ar & m_VertexBufferType;
	ar & m_BillboardTextureFilepath;
	ar & m_ParticleColor;
	ar & m_fParticleImageStandardDeviation;
}


} // namespace amorphous
