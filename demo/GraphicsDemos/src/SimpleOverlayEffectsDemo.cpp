#include "SimpleOverlayEffectsDemo.hpp"
#include <boost/foreach.hpp>
#include "amorphous/Graphics/Font/FontBase.hpp"
#include "amorphous/Graphics/HemisphericLight.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
#include "amorphous/Graphics/Shader/ShaderLightManager.hpp"
#include "amorphous/Graphics/Shader/GenericShaderDesc.hpp"
#include "amorphous/Graphics/Shader/GenericShaderGenerator.hpp"
#include "amorphous/Graphics/TextureGenerators/NoiseTextureGenerators.hpp"
#include "amorphous/Graphics/Mesh/BasicMesh.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/ParamLoader.hpp"
#include "amorphous/Support/Macro.h"
#include "amorphous/Support/CameraController.hpp"
#include "amorphous/Input.hpp"

using std::string;
using namespace boost;


SimpleOverlayEffectsDemo::SimpleOverlayEffectsDemo()
:
m_EnableNoiseEffect(true),
m_EnableStripeEffect(true)
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.2f, 0.2f, 0.5f, 1.0f ) );
}


SimpleOverlayEffectsDemo::~SimpleOverlayEffectsDemo()
{
}


void SimpleOverlayEffectsDemo::SetLights()
{	
	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();

	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

//	if( !shader_loaded )
//		return false;

	ShaderLightManager *pShaderLightMgr = shader_mgr.GetShaderLightManager().get();
	if( !pShaderLightMgr )
		return;

	pShaderLightMgr->ClearLights();

	HemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, 0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	pShaderLightMgr->CommitChanges();
}


bool SimpleOverlayEffectsDemo::InitShader()
{
	// initialize shader
//	bool shader_loaded = m_Shader.Load( "./shaders/SimpleOverlayEffectsDemo.fx" );

	ShaderResourceDesc shader_desc;
	GenericShaderDesc gen_shader_desc;
//	gen_shader_desc.Lighting = false;
	gen_shader_desc.LightingTechnique = ShaderLightingTechnique::HEMISPHERIC;
//	gen_shader_desc.Specular = SpecularSource::NONE;
	shader_desc.pShaderGenerator.reset( new GenericShaderGenerator(gen_shader_desc) );
	bool shader_loaded = m_Shader.Load( shader_desc );

	return shader_loaded;
}


int SimpleOverlayEffectsDemo::Init()
{
	if( GetCameraController() )
		GetCameraController()->SetPosition( Vector3( 0, 5, -30 ) );

	m_PseudoNoiseEffect.Init( 0.5f, 2 );

	// stripe texture
	TextureResourceDesc desc;
	desc.Width  = 16;
	desc.Height = 16;
	desc.Format = TextureFormat::A8R8G8B8;
	shared_ptr<StripeTextureGenerator> pGenerator( new StripeTextureGenerator );
	pGenerator->m_StripeWidth = 1;
	int w = 0;
	LoadParamFromFile( "params.txt", "stripe_width", w );
	if( 0 < w )
		pGenerator->m_StripeWidth = w;
	// opacity adjust by vertex diffuse color of the fullscreen rects
//	float alpha = 0.5f;
//	LoadParamFromFile( "params.txt", "stripe_opacity", alpha );
//	pGenerator->m_Color0 = SFloatRGBAColor(0,0,0,alpha);
//	pGenerator->m_Color0 = SFloatRGBAColor(0,0,0,1);
	desc.pLoader = pGenerator;
	bool loaded = m_StripeTexture.Load( desc );
/*
	m_MeshTechnique.SetTechniqueName( "NoLighting" );
	m_DefaultTechnique.SetTechniqueName( "NoShader" );
*/

	string model = "models/fw43.msh";
	LoadParamFromFile( "params.txt", "model", model );
	model = "SimpleOverlayEffectsDemo/" + model;
	m_Meshes.resize( 1 );
	m_Meshes[0].Load( model );

	InitShader();

	return 0;
}


void SimpleOverlayEffectsDemo::Update( float dt )
{
}


void SimpleOverlayEffectsDemo::RenderMeshes()
{
	GraphicsDevice().Enable( RenderStateType::DEPTH_TEST );

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if(!pShaderMgr)
		return;

	ShaderManager& shader_mgr = *pShaderMgr;// pShaderMgr ? *pShaderMgr : FixedFunctionPipelineManager();

	// render the scene

	shader_mgr.SetViewerPosition( GetCurrentCamera().GetPosition() );

//	GetShaderManagerHub().PushViewAndProjectionMatrices( GetCurrentCamera() );

	shader_mgr.SetTechnique( m_MeshTechnique );
	BOOST_FOREACH( MeshHandle& mesh, m_Meshes )
	{
		shader_mgr.SetWorldTransform( Matrix44Identity() );

		BasicMesh *pMesh = mesh.GetMesh().get();

		if( pMesh )
			pMesh->Render( shader_mgr );
	}

//	GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void SimpleOverlayEffectsDemo::Render()
{
	PROFILE_FUNCTION();

	SetLights();

	RenderMeshes();

	m_PseudoNoiseEffect.SetNoiseTexture();

	if( m_EnableNoiseEffect )
		m_PseudoNoiseEffect.RenderNoiseEffect();

	if( m_EnableStripeEffect )
	{
		Viewport vp;
		GraphicsDevice().GetViewport( vp );
		C2DRect rect;
		static SFloatRGBAColor color( SFloatRGBAColor::White() );
		UPDATE_PARAM( "params.txt", "stripe_opacity", color.alpha );
		rect.SetColor( color );
		rect.SetPositionLTRB( 0, 0, (int)vp.Width-1, (int)vp.Height-1 );
//		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2((float)vp.Width/(float)16,(float)vp.Height/(float)16) );
		rect.Draw( m_StripeTexture );
	}
}


void SimpleOverlayEffectsDemo::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case 'N':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_EnableNoiseEffect = !m_EnableNoiseEffect;
		break;

	case 'H':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_EnableStripeEffect = !m_EnableStripeEffect;
		break;

	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;

	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}


void SimpleOverlayEffectsDemo::ReleaseGraphicsResources()
{
//	m_pSampleUI.reset();
}


void SimpleOverlayEffectsDemo::LoadGraphicsResources( const GraphicsParameters& rParam )
{
//	CreateSampleUI();
}
