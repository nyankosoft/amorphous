#include "2DPrimitivesTest.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"
#include "amorphous/Graphics/Shader/ShaderManager.hpp"
#include "amorphous/Graphics/Shader/Generic2DShaderGenerator.hpp"
#include "amorphous/Input.hpp"


using namespace std;
using namespace boost;


C2DPrimitivesTest::C2DPrimitivesTest()
:
m_Type(0),
m_UseTexture(true)
{
	SetWindowSize( 1280, 720 );
}


C2DPrimitivesTest::~C2DPrimitivesTest()
{
}


#define NUM_VARIATIONS 5


int C2DPrimitivesTest::InitRects()
{
	m_RectTextures.resize( 2 );
	m_RectTextures[0].Load( "./2DPrimitivesDemo/textures/SidePanel44b_fb.jpg" );
	m_RectTextures[1].Load( "./2DPrimitivesDemo/textures/triangle_frame_blue.png" );

	glViewport( 0, 0, 1280, 720 );

	m_Rects.resize( 4 );
	m_Rects[0].SetPositionLTWH(  32, 50,  64,  64 );
//	m_Rects[0].SetPositionLTWH( -100,-100,  102,  101 );
//	m_Rects[0].SetPositionLTWH( 0, 0, 2, 2 );
	m_Rects[1].SetPositionLTWH( 112, 50, 128, 128 );
//	m_Rects[1].SetPositionLTWH( 5,5,1280-10,720-10);
	m_Rects[2].SetPositionLTWH( 256, 50, 256, 256 );
	m_Rects[3].SetPositionLTWH( 528, 50, 512, 512 );

	for( size_t i=0; i<m_Rects.size(); i++ )
		m_Rects[i].SetColor( SFloatRGBAColor::White() );

	Generic2DShaderDesc shader_desc;
	shader_desc.diffuse_color_and_tex0_blend.rgb.op = '*';
	shader_desc.diffuse_color_and_tex0_blend.alpha.op = '*';
	shader_desc.textures[0].sampler = 0;
	shader_desc.textures[0].coord   = 0;
	shader_desc.tex0_and_tex1_blend.rgb.op = '*';
	shader_desc.tex0_and_tex1_blend.alpha.op = '*';
	shader_desc.textures[1].sampler = 1;
	shader_desc.textures[1].coord   = 0;
	m_Shader = CreateGeneric2DShader( shader_desc );

	return 0;
}


int C2DPrimitivesTest::InitFrameRects()
{
	m_FrameTextures.resize( 3 );
	m_FrameTextures[0].Load( "./2DPrimitivesDemo/textures/rect_frame00.bmp" );
	m_FrameTextures[1].Load( "./2DPrimitivesDemo/textures/rect_frame02.bmp" );
	m_FrameTextures[2].Load( "./2DPrimitivesDemo/textures/blue_grad.png" );

	float left = 20;
	float top  = 120;
	float w = grof(100.0f);
	float h = 100;

	const int num_cols = (int)m_FrameTextures.size();
	const int num_rows = NUM_VARIATIONS;

	m_FrameRects.resize(      num_rows * num_cols );
	m_RoundFrameRects.resize( num_rows * num_cols );

	for( int j=0; j<num_cols; j++ )
	{
		for( int i=0; i<num_rows; i++ )
		{
			int index = j*num_rows + i;
			float x = left + (w+30.0f)*i;
			float y = top  + (h+30.0f)*j;

			m_FrameRects[index].SetPosition( Vector2( x, y ), Vector2( x + w, y + h ) );
			m_FrameRects[index].SetColor( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
			m_FrameRects[index].SetBorderWidth( 4 + i*2 );

			m_RoundFrameRects[index].SetPosition( Vector2( x, y ), Vector2( x + w, y + h ) );
			m_RoundFrameRects[index].SetColor( SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
			m_RoundFrameRects[index].SetBorderWidth( 4 + i*2 );
			m_RoundFrameRects[index].SetCornerRadius( 8 + i*2 );
			m_RoundFrameRects[index].Set2DCircularBorderTextureCoords();
		}
	}

//	m_RoundFrameRects[0].SetColor( 0x603020F0 );

	// set texture coords for frame texture

	return 0;
}


int C2DPrimitivesTest::Init()
{
	InitRects();

	InitFrameRects();

	return 0;
}


void C2DPrimitivesTest::Update( float dt )
{
}


void C2DPrimitivesTest::RenderRects()
{
	if( m_RectTextures.size() < 2 )
		return;

	ShaderManager *pShaderMgr = m_Shader.GetShaderManager();
	if( !pShaderMgr )
		return;

	ShaderManager& shader_mgr = *pShaderMgr;

//	shader_mgr.Begin();



	for( int i=0; i<2; i++ )
	{
		if( m_UseTexture )
			shader_mgr.SetTexture( i, m_RectTextures[i] );
		else
			shader_mgr.SetTexture( i, TextureHandle() );
	}

	for( size_t i=0; i<m_Rects.size(); i++ )
	{
		m_Rects[i].SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );
		Get2DPrimitiveRenderer().RenderRect( shader_mgr, m_Rects[i] );
//		m_Rects[i].Draw();

//		m_Rects[i].draw();
	}
}


void C2DPrimitivesTest::Render()
{
	switch( m_Type )
	{
	case 0:
		RenderRects();
		break;
	case 1:
		for( size_t i=0; i<m_FrameRects.size(); i++ )
			m_FrameRects[i].Draw( m_FrameTextures[ i / NUM_VARIATIONS ] );
		break;
	case 2:
//		for( size_t i=0; i<m_FrameRects.size(); i++ )
//			m_RoundRects[i].Draw( m_FrameTextures[ i / NUM_VARIATIONS ] );
		break;
	case 3:
		for( size_t i=0; i<m_RoundFrameRects.size(); i++ )
			m_RoundFrameRects[i].Draw( m_FrameTextures[ i / NUM_VARIATIONS ] );
		break;
	default:
		break;
	}

}


void C2DPrimitivesTest::HandleInput( const InputData& input )
{
	switch( input.iGICode )
	{
	case GIC_RIGHT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Type = (m_Type+1) % NUM_2D_PRIMITIVE_TYPES_TO_RENDER;
		}
		break;
	case GIC_LEFT:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			m_Type = (m_Type+NUM_2D_PRIMITIVE_TYPES_TO_RENDER-1) % NUM_2D_PRIMITIVE_TYPES_TO_RENDER;
		}
		break;
	case GIC_SPACE:
	case GIC_ENTER:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
//			m_pSampleUI->GetDialog(UIID_DLG_RESOLUTION)->Open();
		}
		break;
	case 'T':
		if( input.iType == ITYPE_KEY_PRESSED )
			m_UseTexture = !m_UseTexture;
		break;
	default:
		CGraphicsTestBase::HandleInput( input );
		break;
	}
}
