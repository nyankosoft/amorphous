#include "2DPrimitivesTest.hpp"
#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"
#include "amorphous/Input.hpp"


using namespace std;
using namespace boost;


C2DPrimitivesTest::C2DPrimitivesTest()
:
m_Type(0)
{
	SetWindowSize( 1280, 720 );
}


C2DPrimitivesTest::~C2DPrimitivesTest()
{
}


#define NUM_VARIATIONS 5


int C2DPrimitivesTest::Init()
{
	m_vecTexture.resize( 3 );

	m_vecTexture[0].Load( "./2DPrimitivesDemo/textures/rect_frame00.bmp" );
	m_vecTexture[1].Load( "./2DPrimitivesDemo/textures/rect_frame02.bmp" );
	m_vecTexture[2].Load( "./2DPrimitivesDemo/textures/blue_grad.png" );

	float left = 20;
	float top  = 120;
	float w = grof(100.0f);
	float h = 100;

	const int num_cols = (int)m_vecTexture.size();
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


void C2DPrimitivesTest::Update( float dt )
{
}


void C2DPrimitivesTest::Render()
{
//	C2DRect rect( Vector2( 80, 80 ), Vector2( 100, 100 ), 0xFFFF0000 );

//	C2DRect rect;
//	rect.SetDestAlphaBlendMode( AlphaBlend::InvSrcAlpha );
//	rect.SetColor( 0x603020F0 );

	switch( m_Type )
	{
	case 0:
		for( size_t i=0; i<m_FrameRects.size(); i++ )
			m_FrameRects[i].Draw( m_vecTexture[ i / NUM_VARIATIONS ] );
		break;
	case 1:
		for( size_t i=0; i<m_RoundFrameRects.size(); i++ )
			m_RoundFrameRects[i].Draw( m_vecTexture[ i / NUM_VARIATIONS ] );
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
	default:
		break;
	}
}
