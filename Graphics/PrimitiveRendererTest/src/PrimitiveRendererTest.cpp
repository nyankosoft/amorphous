#include "PrimitiveRendererTest.hpp"
#include "gds/Graphics/PrimitiveRenderer.hpp"
#include "gds/Graphics/Camera.hpp"
#include "gds/Graphics/2DPrimitive/2DRect.hpp"
#include "gds/Graphics/Shader/ShaderManager.hpp"
#include "gds/Graphics/Shader/FixedFunctionPipelineManager.hpp"
//#include "gds/Graphics/Shader/ShaderLightManager.hpp"
#include "gds/Graphics/Font/BuiltinFonts.hpp"
#include "gds/Support/Profile.hpp"
#include "gds/Support/ParamLoader.hpp"
#include "gds/Support/Macro.h"

using std::string;
using std::vector;
using namespace boost;


extern CGraphicsTestBase *CreateTestInstance()
{
	return new CPrimitiveRendererTest();
}


extern const std::string GetAppTitle()
{
	return string("PrimitiveRendererTest");
}


CPrimitiveRendererTest::CPrimitiveRendererTest()
{
	m_MeshTechnique.SetTechniqueName( "NoLighting" );

	SetBackgroundColor( SFloatRGBAColor( 0.1f, 0.1f, 0.1f, 1.0f ) );

//	LoadParamFromFile( "params.txt", "num_primitives_to_draw", m_NumPrimitivesToDraw );

	g_Camera.SetPosition( Vector3( 0, 1, -120 ) );
//	g_Camera.SetPosition( Vector3( 0, 520, 120 ) );
}


CPrimitiveRendererTest::~CPrimitiveRendererTest()
{
}


/*
bool CPrimitiveRendererTest::InitShader()
{
	// initialize shader
	bool shader_loaded = m_Shader.Load( "./shaders/PrimitiveRendererTest.fx" );
	
	if( !shader_loaded )
		return false;

	CShaderLightManager *pShaderLightMgr = m_Shader.GetShaderManager()->GetShaderLightManager().get();

	CHemisphericDirectionalLight light;
	light.Attribute.UpperDiffuseColor.SetRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	light.Attribute.LowerDiffuseColor.SetRGBA( 0.1f, 0.1f, 0.1f, 1.0f );
	light.vDirection = Vec3GetNormalized( Vector3( -1.0f, -1.8f, -0.9f ) );

//	pShaderLightMgr->SetLight( 0, light );
//	pShaderLightMgr->SetDirectionalLightOffset( 0 );
//	pShaderLightMgr->SetNumDirectionalLights( 1 );
	pShaderLightMgr->SetHemisphericDirectionalLight( light );

	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4, 640.0f / 480.0f, 0.1f, 500.0f );
	m_Shader.GetShaderManager()->SetProjectionTransform( proj );

	return true;
}*/


int CPrimitiveRendererTest::Init()
{
	m_pFont = CreateDefaultBuiltinFont();
	if( m_pFont )
		m_pFont->SetFontSize( 6, 12 );

//	InitShader();

	return 0;
}


void CPrimitiveRendererTest::Update( float dt )
{
}


void CPrimitiveRendererTest::Render()
{
	PROFILE_FUNCTION();

//	m_TextBuffer.resize( 0 );
//	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );

	Vector3 shift = Vector3(2,0,0);
	vector<Vector3> points;
	points.resize( 4 * 20 );
	vector<SFloatRGBAColor> colors;
	colors.resize( points.size() );
	for( int i=0; i<(int)points.size() / 4; i++ )
	{
		shift.y = (float)i * 0.3f;
		points[i*4  ] = Vector3( 1, 0, 1) + shift;
		points[i*4+1] = Vector3( 1, 0,-1) + shift;
		points[i*4+2] = Vector3(-1, 0,-1) + shift;
		points[i*4+3] = Vector3(-1, 0, 1) + shift;

		float b = get_clamped( (float)(20 - i) / 20.0f, 0.0f, 1.0f );

		const SFloatRGBAColor color = SFloatRGBAColor(b,b,b,1);
		colors[i*4  ] = color;
		colors[i*4+1] = color;
		colors[i*4+2] = color;
		colors[i*4+3] = color;
	}

	GetPrimitiveRenderer().DrawConnectedLines( points, colors );

	for( int i=0; i<(int)points.size(); i++ )
		points[i] += Vector3(3,0,0);

	GetPrimitiveRenderer().DrawPoints( points, SFloatRGBAColor::White() );

	points.resize( 20 * 2 );
	for( int i=0; i<(int)points.size() / 2; i++ )
	{
		float y = (float)i * 0.2f;
		points[i*2  ] = Vector3(-2,y,5);
		points[i*2+1] = Vector3(-1,y,5);
	}

//	GetPrimitiveRenderer().DrawConnectedLines( points, SFloatRGBAColor::White() );

//	Vector2 vTopLeft(     GetWindowWidth() / 4,  16 );
//	Vector2 vBottomRight( GetWindowWidth() - 16, GetWindowHeight() * 3 / 2 );
//	C2DRect rect( vTopLeft, vBottomRight, 0x50000000 );
//	rect.Draw();

//	m_pFont->DrawText( m_TextBuffer, vTopLeft );
}


void CPrimitiveRendererTest::HandleInput( const SInputData& input )
{
/*
	switch( input.iGICode )
	{
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
		}
		break;
	default:
		break;
	}*/
}
