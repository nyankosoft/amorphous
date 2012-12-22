#include "BE_TextureAnimation.hpp"
#include "CopyEntity.hpp"
//#include "trace.hpp"
#include "Stage.hpp"
#include "Graphics/GraphicsDevice.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/Shader/ShaderManager.hpp"

using namespace std;


CBE_TextureAnimation::CBE_TextureAnimation()
{
	m_BoundingVolumeType = BVTYPE_AABB;
	m_bNoClip = true;

	m_AnimTypeFlag = TA_BILLBOARD;

	m_fExpansionFactor = 1.0f;

	m_aShaderTechHandle[0].SetTechniqueName( "Billboard" );
}


CBE_TextureAnimation::~CBE_TextureAnimation()
{
}


void CBE_TextureAnimation::Init()
{
	m_AnimTexture.Load( m_AnimTextureFilepath );

	for( int i=0; i<2; i++ )
	{
		CRectSetMesh& rect_mesh = (i==0) ? m_FrontRectMesh : m_RearRectMesh;

		rect_mesh.Init( 1, VFF::POSITION | VFF::DIFFUSE_COLOR | VFF::TEXCOORD2_0 );

		rect_mesh.SetDiffuseColors( SFloatRGBAColor::White() );

		CMeshMaterial& mat = rect_mesh.Material(0);
		mat.TextureDesc.resize( 1 );
		mat.Texture.resize( 1 );
		mat.TextureDesc[0].ResourcePath = m_AnimTextureFilepath;
		mat.Texture[0] = m_AnimTexture;

		Vector3 vRight = Vector3(1,0,0);
		Vector3 vUp    = Vector3(0,1,0);
		float fHalfWidth = this->m_aabb.vMax.x;
//		Vector3 vTest = (i==0) ? Vector3(-1.2f,0,0) : Vector3(1.2f,0,0);
		rect_mesh.SetRectPosition(
			0,
			-vRight * fHalfWidth + vUp * fHalfWidth,// + vTest,
			 vRight * fHalfWidth + vUp * fHalfWidth,// + vTest,
			 vRight * fHalfWidth - vUp * fHalfWidth,// + vTest,
			-vRight * fHalfWidth - vUp * fHalfWidth// + vTest
			);
	}

	// set the local coordinate of the billboard rectangle
	Vector3 vRight = Vector3(1,0,0);
	Vector3 vUp    = Vector3(0,1,0);
	float fHalfWidth = this->m_aabb.vMax.x;

//	int i;
	// secondary billboard - shifted slightly forward to avoid z-fighting
	// --- corrected 050505 - this is not needed if we disable z-writing during rendering
//	for(i=0; i<4; i++)
//		m_avRectangle2[i].vPosition = m_avRectangle1[i].vPosition /* + Vector3(0, 0, 0.02f) */;
}


void CBE_TextureAnimation::InitCopyEntity( CCopyEntity* pCopyEnt )
{
//	float& rfCurrentAnimationTime = pCopyEnt->f1;
//	rfCurrentAnimationTime = 0;
	// animation itme must be set explicitly before this function

//	pCopyEnt->bUseZSort = true;
	pCopyEnt->RaiseEntityFlags( BETYPE_USE_ZSORT );

	float& rfRotAngle = pCopyEnt->f2;
	rfRotAngle = 2.0f * 3.141592f * (float)rand() / (float)RAND_MAX;
}


void CBE_TextureAnimation::Act(CCopyEntity* pCopyEnt)
{
	float& rfCurrentAnimationTime = pCopyEnt->f1;

	if( m_fTotalAnimationTime <= rfCurrentAnimationTime )
///		pCopyEnt-Terminate();
		m_pStage->TerminateEntity( pCopyEnt );
	else
		rfCurrentAnimationTime += m_pStage->GetFrameTime();
}


void CBE_TextureAnimation::Draw(CCopyEntity* pCopyEnt)
{
	float& rfCurrentAnimationTime = pCopyEnt->f1;
	if( rfCurrentAnimationTime < 0 || m_fTotalAnimationTime < rfCurrentAnimationTime )
		return;	// animation hasn't started yet or is already over

	// set the world transformation matrix
	Matrix33 matRotZ;
	Matrix34 world_pose( Matrix34Identity() );

	if( m_AnimTypeFlag & TA_BILLBOARD )
	{
		// set the matrix which rotates a 2D polygon and make it face to the direction of the camera
//		Matrix33 billboard_orient;
		m_pStage->GetBillboardRotationMatrix( world_pose.matOrient );
	}
	else // i.e. animation of arbitrary direction
	{
		world_pose = pCopyEnt->GetWorldPose();
	}


	const Vector3& rvPos =  pCopyEnt->GetWorldPosition();	// current position of this billboard
	world_pose.vPosition = rvPos;
//	matWorld(0,3) = rvPos.x;
//	matWorld(1,3) = rvPos.y;
//	matWorld(2,3) = rvPos.z;
//	matWorld(3,3) = 1;

//	if( /* random rotation is */ true )
	if( false )
	{	// randomly rotates billboards to make them look more diverse
		float& rfRotAngle = pCopyEnt->f2;
		matRotZ = Matrix33RotationZ( rfRotAngle );
		world_pose.matOrient = world_pose.matOrient * matRotZ;
	}

	// set animation
	int iNumTotalFrames = m_iNumTextureSegments * m_iNumTextureSegments;
	int iCurrentFrame = (int)( (float)iNumTotalFrames * rfCurrentAnimationTime / m_fTotalAnimationTime );
	if( iNumTotalFrames <= iCurrentFrame )
		iCurrentFrame = iNumTotalFrames - 1;

	float fTimePerFrame = m_fTotalAnimationTime / (float)iNumTotalFrames;
	float fCurrentFrameTime = ( rfCurrentAnimationTime - fTimePerFrame * (float)iCurrentFrame ) / fTimePerFrame;
//	int i;

	// set alpha values for smooth animation between frames
	float fMargin = 0.001f;
	clamp( fCurrentFrameTime, 0.0f + fMargin, 1.0f + fMargin );
//	fCurrentFrameTime = 0.5f;
	float fFrac = fCurrentFrameTime;
	float fInvFrac = 1.0f - fCurrentFrameTime;
	const SFloatRGBAColor diffuse_color_front( fInvFrac, fInvFrac, fInvFrac, fInvFrac );
	const SFloatRGBAColor diffuse_color_rear(  fFrac,    fFrac,    fFrac,    fFrac );
//	const SFloatRGBAColor diffuse_color_front( 1.0f, 1.0f, 1.0f, 1.0f - fCurrentFrameTime );
//	const SFloatRGBAColor diffuse_color_rear(  1.0f, 1.0f, 1.0f, fCurrentFrameTime );
	m_FrontRectMesh.SetDiffuseColors( diffuse_color_front );
	m_RearRectMesh.SetDiffuseColors( diffuse_color_rear );

	// tested with D3DCOLOR_ARGB() - couldn't solve the flickering.
//	m_FrontRectMesh.SetColorARGB32( D3DCOLOR_ARGB( ((int)(255.0f * (1.0f - fCurrentFrameTime))), 255, 255, 255 ) );
//	m_RearRectMesh.SetColorARGB32( D3DCOLOR_ARGB( ((int)(255.0f * fCurrentFrameTime         )), 255, 255, 255 ) );

//	for(i=0; i<4; i++)
//	{
		// set alpha values for smooth animation between frames
//		m_avRectangle1[i].color = D3DCOLOR_ARGB( ((int)(255.0f * (1.0f - fCurrentFrameTime))), 255, 255, 255 );
//		m_avRectangle2[i].color = D3DCOLOR_ARGB( ((int)(255.0f * fCurrentFrameTime         )), 255, 255, 255 );
//	}

	if( m_fExpansionFactor != 1.0f )
	{
		float r = this->m_aabb.vMax.x * ( 1.0f + fCurrentFrameTime * (m_fExpansionFactor - 1.0f) );
		for( int i=0; i<2; i++ )
		{
			CRectSetMesh& rect_mesh = (i==0) ? m_FrontRectMesh : m_RearRectMesh;
			rect_mesh.SetRectPosition(
				0,
				Vector3(-r, r, 0 ),
				Vector3( r, r, 0 ),
				Vector3( r,-r, 0 ),
				Vector3(-r,-r, 0 )
				);
		}
	}

	SetTextureCoord( m_FrontRectMesh, iCurrentFrame );
	if( iCurrentFrame < iNumTotalFrames - 1 )	// if 'iCurrentFrame' is not the last frame of the texture animation
		SetTextureCoord( m_RearRectMesh, iCurrentFrame + 1 );	// set texture for the secondary billboard

	// debug
//	if( 0.99f < fCurrentFrameTime && 0xF0FFFFFF < m_avRectangle1[0].color )
//		int iUnexpected = 1;


	// use the texture color only
	CShaderManager *pShaderManager = m_MeshProperty.m_ShaderHandle.GetShaderManager();
	CShaderManager& shader_mgr = pShaderManager ? *pShaderManager : FixedFunctionPipelineManager();

	shader_mgr.SetWorldTransform( world_pose );

//	{
/*		if( m_pStage->GetScreenEffectManager()->GetEffectFlag() & ScreenEffect::PseudoNightVision )
		{
            pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD_PNV );
		}
		else
		{
//			pShaderManager->SetTechnique( SHADER_TECH_DEFAULT );
			pShaderManager->SetTechnique( SHADER_TECH_BILLBOARD );
		}*/

		// Supposed to be set by rect mesh.
		// Remove this after comfirming that the rect mesh sets the texture
		shader_mgr.SetTexture( 0, m_AnimTexture );

//		shader_mgr.CommitChanges();
//	}

	// Diable alpha test to render the rect with a premultiplied alpha texture
	GraphicsDevice().Disable( RenderStateType::ALPHA_TEST );

	// disable z-writing
	GraphicsDevice().Disable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );

	bool lighting = false;
	GraphicsDevice().SetRenderState( RenderStateType::LIGHTING, lighting );

	if( iCurrentFrame < iNumTotalFrames - 1 ) // if 'iCurrentFrame' is not the last frame of the texture animation
		m_RearRectMesh.Render();
	m_FrontRectMesh.Render();

	GraphicsDevice().Enable( RenderStateType::WRITING_INTO_DEPTH_BUFFER );
}


// set texture coordinate for billboard
void CBE_TextureAnimation::SetTextureCoord( CRectSetMesh& rect_mesh, int iCurrentFrame )
{
	float fNumTextureSegments = (float)m_iNumTextureSegments;

	// set texture coordinate
	TEXCOORD2 vTexMin, vTexMax;
	vTexMin.u = (float)(iCurrentFrame % m_iNumTextureSegments) / fNumTextureSegments;
	vTexMin.v = (float)(iCurrentFrame / m_iNumTextureSegments) / fNumTextureSegments;

	float fSegmentWidth = 1.0f / fNumTextureSegments;
	vTexMax.u = vTexMin.u + fSegmentWidth - 0.001f;
	vTexMax.v = vTexMin.v + fSegmentWidth - 0.001f;

	// debug
//	rect_mesh.SetTextureCoordMinMax( 0, TEXCOORD2(0,0), TEXCOORD2(1,1) );

	rect_mesh.SetTextureCoordMinMax( 0, vTexMin, vTexMax );
}


bool CBE_TextureAnimation::LoadSpecificPropertiesFromFile( CTextFileScanner& scanner )
{
	string blend_mode, texanim_type;

	if( scanner.TryScanLine( "TEX_FILE",	m_AnimTextureFilepath ) )	return true;
	if( scanner.TryScanLine( "TEX_WIDTH",	m_iTextureWidth ) )			return true;
	if( scanner.TryScanLine( "TEX_SEGS",	m_iNumTextureSegments ) )	return true;
	if( scanner.TryScanLine( "ANIM_TIME",	m_fTotalAnimationTime ) )	return true;
	if( scanner.TryScanLine( "EXPANSION",	m_fExpansionFactor ) )		return true;

	if( scanner.TryScanLine( "TEXANIM_TYPE", texanim_type ) )
	{
		if( texanim_type == "TYPE_NON_BILLBOARD" )
			m_AnimTypeFlag &= ~TA_BILLBOARD;
		return true;
	}

	return false;
}


void CBE_TextureAnimation::Serialize( IArchive& ar, const unsigned int version )
{
	CBaseEntity::Serialize( ar, version );

	ar & m_AnimTextureFilepath;
	ar & m_fTotalAnimationTime;
	ar & m_iNumTextureSegments;
	ar & m_iTextureWidth;
//	ar & m_iDestBlend;
	ar & m_fExpansionFactor;
	ar & m_AnimTypeFlag;
}
