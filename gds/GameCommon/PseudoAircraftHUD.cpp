#include "PseudoAircraftHUD.hpp"

#include "3DMath/Matrix34.hpp"
#include "3DMath/Matrix44.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "3DMath/Matrix22.hpp"
#include "3DMath/MathMisc.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/D3DMisc.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Support/memory_helpers.hpp"
#include "PseudoAircraftSimulator.hpp"


namespace amorphous
{

using namespace std;


CPseudoAircraftHUD::CPseudoAircraftHUD()
:
m_pFont(NULL)
{
	SetColor( 0x7030B030 );//0xA070D870;

	m_RectSet.SetDestAlphaBlendMode( AlphaBlend::One );
}


CPseudoAircraftHUD::~CPseudoAircraftHUD()
{
	Release();
}


void CPseudoAircraftHUD::Init()
{
	TextureFont *pTexFont = new TextureFont;
	pTexFont->InitFont( "./Texture/MainFont.dds", 8, 12, 16, 8 );
	m_pFont = pTexFont;

	const string tex_filename = "./Texture/AircraftHUD.dds";
	m_Texture.Load( tex_filename );

	m_RectSet.SetNumRects( NUM_MAX_RECTS );
}


void CPseudoAircraftHUD::Release()
{
	SafeDelete( m_pFont );
}


void CPseudoAircraftHUD::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	float scale = GetScreenWidth() / 800.0f;

	m_pFont->Reload();
	// font size is updated every frame in CPseudoAircraftHUD::Render()
//	m_pFont->SetFontSize(  );
}


void CPseudoAircraftHUD::ReleaseGraphicsResources()
{
	m_pFont->Release();
}


static const char s_PitchTable[21][3] = { " 0", " 5", "10", "15", "20", "25", "30", "35", "40", "45",
                                          "50", "55", "60", "65", "70", "75", "80", "85", "90", "XX", "AA" };

static const char s_DirTable[8][3] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };


void CPseudoAircraftHUD::RenderCenterComponents()
{
	C2DRect rect;
	float scale = GetScreenWidth() / 800.0f;
	Vector2 vCenter = m_vScreenCenter;

	// render "w"
	float w = 48.0f * scale;
	float h = 24.0f * scale;
	rect.SetTextureUV( TEXCOORD2(0.0f,0.5f), TEXCOORD2(1.0f,1.0f) );
	rect.SetPosition( vCenter - Vector2(w,h) * 0.5f, vCenter + Vector2(w,h) * 0.5f );
	rect.SetDestAlphaBlendMode( AlphaBlend::One );
	rect.SetColor( m_HUDColor );
	rect.Draw( m_Texture );

	// render "-o-"
}


void CPseudoAircraftHUD::Render( const CPseudoAircraftSimulator& craft )
{
	// enable alpha blending
	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();
    pd3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// blend texture color and diffuse color
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	RenderCenterComponents();

	m_RectSet.SetColor( m_HUDColor );

	float scale = GetScreenWidth() / 800.0f;
	float pitch = craft.GetPitch();
	int rect_index = 0;
	Vector2 vMin, vMax;

	Matrix22 matRotZ = Matrix22Rotation( craft.GetRoll() );

	float width, height, far_clip = 50000.0f;
	GetViewportSize( width, height );
	Matrix44 proj = Matrix44PerspectiveFoV_LH( (float)PI / 4.0f, width / height, 1.0f, far_clip );
	Matrix44 view, proj_view;

//	const Vector2 vScreenCenter = Vector2( width, height ) * 0.5f;
	const Vector2 vScreenCenter = m_vScreenCenter;

	Vector2 vTextPos[2];

	m_pFont->ClearCache();
	m_pFont->SetFontColor( m_HUDColor );

	// calc craft nose direction projected on x-plane
	Vector3 vDirProjX = Matrix33RotationX(pitch) * Vector3(0,0,1);
	Vector3 vRight = Vector3(1,0,0);
	Vector3 vUp = Vec3Cross( vDirProjX, vRight );
	Vector3 vRefPos;

	Matrix34 cam_pose( Matrix34Identity() );
	cam_pose.matOrient.SetColumn( 0, Vector3( vDirProjX.z, 0, -vDirProjX.x ) );
	cam_pose.matOrient.SetColumn( 1, vUp );
	cam_pose.matOrient.SetColumn( 2, vDirProjX );
	view = Matrix44CameraMatrixFromCameraPose( cam_pose );
	proj_view = proj * view;

	Matrix33 matRot;
	Scalar pitch_angle, y, sx, ex, tx, h;

	float font_width  =  8 * scale;
	float font_height = 12 * scale;
	m_pFont->SetFontSize( (int)font_width, (int)font_height );

	// draw pitch ladder every 5 degrees
	int pitch_start = (int)rad_to_deg(pitch) / 5 - 2;
	int pitch_end   = pitch_start + 4;

	sx = 55.0f * scale;	// pitch ladder inner x
	ex = 70.0f * scale;	// pitch ladder outer x
	h =  1.0f * scale;	//  half of the width of each pitch ladder
	Limit( h, 1.0f, 3.0f );
	tx = 80.0f * scale;	// x of the pitch angle text
	for( int i = pitch_start; i<=pitch_end; i++ )
	{
		pitch_angle = deg_to_rad( (float)i * 5.0f );
		matRot = Matrix33RotationAxis( pitch_angle, Vector3(1,0,0) );
		vRefPos = matRot * Vector3(0,0,1) * ( far_clip - 100.0f );

		vRefPos = proj_view * vRefPos;

		// calc y as (0,0) center screen space
		y = ( -vRefPos.y ) * 0.5f * height;

		Vector2 vPos[8];

		// ladder right side
		vPos[0] = Vector2( sx,-h+y);		vPos[1] = Vector2( ex,-h+y);
		vPos[3] = Vector2( sx, h+y);		vPos[2] = Vector2( ex, h+y);
		// ladder left side
		vPos[4] = Vector2(-ex,-h+y);		vPos[5] = Vector2(-sx,-h+y);
		vPos[7] = Vector2(-ex, h+y);		vPos[6] = Vector2(-sx, h+y);

		int vert_index_offset = rect_index * 4;
		for( int j=0; j<8; j++ )
		{
			vPos[j] = matRotZ * vPos[j] + vScreenCenter;
			m_RectSet.SetVertexPosition( vert_index_offset + j, vPos[j].x, vPos[j].y );
		}
		rect_index += 2;

		// render the pitch angle digits every 5 degrees
		vTextPos[0] = matRotZ * Vector2(-tx, y) + Vector2(-font_width, -font_height*0.5f) + vScreenCenter;
		vTextPos[1] = matRotZ * Vector2( tx, y) + Vector2(-font_width, -font_height*0.5f) + vScreenCenter;

		int ui = abs(i);
		if( 18 < ui ) ui = 36 - ui;
		m_pFont->CacheText( s_PitchTable[ui], vTextPos[0] );
		m_pFont->CacheText( s_PitchTable[ui], vTextPos[1] );
	}

	// render heading
	Vector3 vDirH = craft.GetHorizontalForwardDirection();

	cam_pose.matOrient.SetColumn( 0, Vector3( vDirH.z, 0, -vDirH.x ) );
	cam_pose.matOrient.SetColumn( 1, Vector3(0,1,0) );
	cam_pose.matOrient.SetColumn( 2, vDirH );
	view = Matrix44CameraMatrixFromCameraPose( cam_pose );
	proj_view = proj * view;

	Scalar x, heading_angle, yaw = craft.GetYaw();
	int heading_start = (int)rad_to_deg(yaw) / 5 - 4;
	int heading_end   = heading_start + 8;

	y = 150.0f * scale;// y of the direction indicators
	float len_y[2] = { 3.6f*scale, 2.5f*scale };// half of the line length for each direction indicator
	for( int i=heading_start; i<=heading_end; i++ )
	{
		heading_angle = (float)i * deg_to_rad(5.0f);

		matRot = Matrix33RotationY( heading_angle );
		vRefPos = matRot * Vector3(0,0,1) * ( far_clip - 100.0f );

		vRefPos = proj_view * vRefPos;

		// calc y as (0,0) center screen space
		x = ( vRefPos.x * 0.5f * width ) * 0.5f;

		// draw a longer line at N/E/S/W
		float y_hlen = (float)( (i % 9 == 0) ? len_y[0] : len_y[1] );

		vMin = Vector2(x-1,-y_hlen-y) + vScreenCenter;
		vMax = Vector2(x+1, y_hlen-y) + vScreenCenter;
		m_RectSet.SetRectMinMax( rect_index++, vMin.x, vMin.y, vMax.x, vMax.y );

		if( i % 9 == 0 )
		{
			// draw NESW directions
			int heading_index = i/9;
			if( heading_index < 0 ) heading_index += 8;
			float heading_x = (heading_index % 2 == 0) ? -font_width * 0.5f : -font_width;

			vTextPos[0] = Vector2(x+heading_x,-y-font_height*1.2f) + vScreenCenter;
			m_pFont->CacheText( s_DirTable[heading_index], vTextPos[0] );

		}
	}

	// render altimeter every 50 meter
	int altimeter_unit = 50;
	int alt_offset = (int)(craft.GetAltitude()) % altimeter_unit;
	float center_x = vScreenCenter.x;
	sx = center_x - 105 * scale;// left x offset of the altimeter
	ex = center_x -  95 * scale;// right x offset of the altimeter
	for( int i=-3; i<=3; i++ )
	{
		y = vScreenCenter.y + (float)(i * altimeter_unit + alt_offset);

		vMin = Vector2( sx, y-1 );
		vMax = Vector2( ex,  y+1 );
		m_RectSet.SetRectMinMax( rect_index++, vMin.x, vMin.y, vMax.x, vMax.y );
	}

	// text for altimeter & speed
	font_width  = 10.0f * scale;
	font_height = 15.0f * scale;
	tx = 175 * scale;	// x of the text
	m_pFont->SetFontSize( (int)font_width, (int)font_height );

	char str[2][8];
	sprintf( str[0], "%d", (int)craft.GetAltitude() );
	sprintf( str[1], "%d", (int)Vec3Length(craft.GetVelocity()) );
	size_t alt_order = strlen(str[0]);
	vTextPos[0] = Vector2(-tx+(float)(5-alt_order)*font_width,0) + vScreenCenter;
	size_t spd_order = strlen(str[1]);
	vTextPos[1] = Vector2( tx+(float)(4-spd_order)*font_width,0) + vScreenCenter;

	m_pFont->CacheText( str[0], vTextPos[0] );
	m_pFont->CacheText( str[1], vTextPos[1] );


	// render rectangle sets
    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	m_RectSet.Draw( 0, rect_index );

	// render text
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_pFont->SetDestAlphaBlendMode( AlphaBlend::One );
	m_pFont->DrawCachedText();
}


} // namespace amorphous
