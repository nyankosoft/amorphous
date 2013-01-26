#include "HUD_PlayerAircraft.hpp"
#include "BE_PlayerPseudoAircraft.hpp"
#include "SubDisplay.hpp"
#include "Stage/PlayerInfo.hpp"
#include "3DMath/Matrix22.hpp"
#include "3DMath/AABB2.hpp"

#include "Graphics/2DPrimitive/2DFrameRect.hpp"
#include "Graphics/2DPrimitive/2DTriangle.hpp"
#include "Graphics/GraphicsEffectManager.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "GameTextSystem/TextMessageManager.hpp"
#include "GameTextSystem/TextMessageRenderer.hpp"
#include "Support/StringAux.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Macro.h"


namespace amorphous
{

using std::string;
using std::vector;
using namespace boost;


static const int gs_TimerDisplayBlinkThreasholdMS = 60 * 1000 * 14;


inline Matrix22 Matrix22Scaling( Scalar scaling_factor )
{
	return Matrix22( scaling_factor, 0, 0, scaling_factor );
}



HUD_PlayerAircraft::HUD_PlayerAircraft()
:
m_bDisplayGlobalRadar(false),
//m_pTimeText(NULL),
m_pSubDisplay(NULL),
m_pTextMessageManager(NULL),
m_NumLastRenderedLocalRadarIcons(0),
m_LastRenderedIconsOnGlobalRadar(0)
{
	m_ContainerSize = CONTAINER_SIZE;

	m_aHUDColor[COLOR_WHITEFRAME]   = SFloatRGBAColor( 0.92f, 0.92f, 0.92f, 0.06f );
	m_aHUDColor[COLOR_NORMAL]       = SFloatRGBAColor( 0.19f, 0.69f, 0.19f, 0.44f );	// 0x7030B030;
	m_aHUDColor[COLOR_HIGHLIGHTED]  = m_aHUDColor[COLOR_NORMAL] * 1.25f;
	m_aHUDColor[COLOR_LOCKED_ON]    = SFloatRGBAColor( 0.81f, 0.19f, 0.19f, 0.44f );	// 0x70D03030;
	m_aHUDColor[COLOR_FRAME]        = SFloatRGBAColor( 0.12f, 0.82f, 0.12f, 0.92f );
	m_aHUDColor[COLOR_BG]           = SFloatRGBAColor( 0.00f, 0.00f, 0.00f, 0.38f );
	m_aHUDColor[COLOR_MISSILE_APPROACHING] = SFloatRGBAColor( 0.81f, 0.15f, 0.15f, 0.44f );	// 0x70D03030;

	m_HUD.SetColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );

	m_TimerDisplay.m_Color = m_aHUDColor[COLOR_NORMAL].GetARGB32();
/*
	for( int i=0; i<NUM_MAX_CONTAINER_RECTS; i++ )
		m_apContainer[i] = NULL;

	for( int i=0; i<NUM_MAX_ICONS_ON_LOCAL_RADAR; i++ )
		m_apIconOnLocalRadar[i] = NULL;

	for( int i=0; i<NUM_MAX_ICONS_ON_GLOBAL_RADAR; i++ )
		m_apIconOnGlobalRadar[i] = NULL;*/
}


HUD_PlayerAircraft::~HUD_PlayerAircraft()
{
	Release();
}


void HUD_PlayerAircraft::Release()
{
	SafeDelete( m_pTextMessageManager );
}


void HUD_PlayerAircraft::UpdateScreenSize()
{
}


void HUD_PlayerAircraft::Init()
{
	Release();

	m_HUD.Init();

	const string filename = "./Texture/hud_icon.dds";
	m_TexCache.Load( filename );

	m_pGraphicsEffectManager = shared_ptr<CAnimatedGraphicsManager>( new CAnimatedGraphicsManager() );

	shared_ptr<CGraphicsElementManager> pElementMgr = m_pGraphicsEffectManager->GetGraphicsElementManager();

	pElementMgr->LoadTexture( TEX_RADAR_ICON, "./Texture/hud_icon.dds" );

	m_LocalRadarRect
		= RectAtLeftBottom( LOCAL_RADAR_SIZE,  LOCAL_RADAR_SIZE,  LOCAL_RADAR_LEFT_MARGIN,  LOCAL_RADAR_BOTTOM_MARGIN );

	SRect local_globalradar_rect
		= RectLTWH( 0, 0, GLOBAL_RADAR_SIZE, GLOBAL_RADAR_SIZE );

	m_GlobalRadarRect
		= RectAtLeftBottom( GLOBAL_RADAR_SIZE, GLOBAL_RADAR_SIZE, GLOBAL_RADAR_LEFT_MARGIN, GLOBAL_RADAR_BOTTOM_MARGIN );

	int x,y,r;
	r = LOCAL_RADAR_SIZE / 2;
	x = m_LocalRadarRect.GetCenterX();
	y = m_LocalRadarRect.GetCenterY();
//	AABB2 radar_aabb = AABB2( Vector2(-r,-r), Vector2(r,r) );

//	Vector2 radar_offset = Vector2( x, y );

	r = (int)( r * 0.1f );
	int w = (int)(r * 0.15f);

	int global_radar_layer = 10;
	int local_radar_layer  = 20;
	int base_layer = 30;

	// rect elements for local & global radar
	pElementMgr->CreateFillRect(      m_LocalRadarRect, m_aHUDColor[COLOR_BG],          local_radar_layer + 5 );
	pElementMgr->CreateFrameRect( m_LocalRadarRect, m_aHUDColor[COLOR_FRAME], 4.5f, local_radar_layer );

	m_pGlobalRadarBG = pElementMgr->CreateFillRect(      local_globalradar_rect, m_aHUDColor[COLOR_BG],          global_radar_layer + 5 );
	shared_ptr<CFrameRectElement> pFrame = pElementMgr->CreateFrameRect( local_globalradar_rect, m_aHUDColor[COLOR_FRAME], 4.5f, global_radar_layer );
	m_pGlobalRadarBG->SetTextureCoord( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	// inner frame rect that represents the area of operation
	SRect inner_frame_rect = local_globalradar_rect;
	inner_frame_rect.Inflate( -85, -85 );
	shared_ptr<CFrameRectElement> pInnerFrame = pElementMgr->CreateFrameRect( inner_frame_rect, SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.32f ), 4.0f, global_radar_layer );
	pInnerFrame->SetDestAlphaBlendMode( AlphaBlend::One );

	boost::shared_ptr<CGraphicsElement> apElement[] = { m_pGlobalRadarBG, pFrame, pInnerFrame };
	m_pGlobalRadar = pElementMgr->CreateGroup( apElement, numof(apElement), m_GlobalRadarRect.GetTopLeftCorner() );
	m_pGlobalRadar->SetLocalTopLeftPos( m_GlobalRadarRect.GetTopLeftCorner() );

	// small white cross on the center
	pElementMgr->CreateFillRect( RectLTRB( x-r, y-w, x+r, y+w ), m_aHUDColor[COLOR_WHITEFRAME], base_layer );
	pElementMgr->CreateFillRect( RectLTRB( x-w, y-r, x+w, y+r ), m_aHUDColor[COLOR_WHITEFRAME], base_layer );

	pElementMgr->LoadFont( GLOBAL_FONT_INDEX, "./Texture/BitstreamVeraSansMono_Bold.dds", FontBase::FONTTYPE_TEXTURE, 30, 40 );

	// text for time display
	m_pTimeText = pElementMgr->CreateText( 0, "", RectAtRightTop( 200, 50, 50, 32 ),
		CTextElement::TAL_TOP, CTextElement::TAL_CENTER,
		m_aHUDColor[COLOR_NORMAL], 30, 40, base_layer );

	m_pTimeText->SetDestAlphaBlendMode( AlphaBlend::One );

	for( size_t i=0; i<NUM_MAX_CONTAINER_RECTS; i++ )
	{
		m_apContainer[i] = pElementMgr->CreateFrameRect( RectLTWH( 0, 0, m_ContainerSize, m_ContainerSize ), SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f), 4.0f, base_layer );
		m_apContainer[i]->SetDestAlphaBlendMode( AlphaBlend::One );
	}

	for( size_t i=0; i<NUM_MAX_ICONS_ON_LOCAL_RADAR; i++ )
	{
		m_apIconOnLocalRadar[i] = pElementMgr->CreateFillRect( RectLTWH( -1, -1, 3, 3 ), SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f), local_radar_layer - 2 );
		m_apIconOnLocalRadar[i]->SetTexture( TEX_RADAR_ICON );
	}

	for( size_t i=0; i<NUM_MAX_ICONS_ON_GLOBAL_RADAR; i++ )
	{
		m_apIconOnGlobalRadar[i] = pElementMgr->CreateFillRect( RectLTWH( -1, -1, 3, 3 ), SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f), global_radar_layer - 2 );
		m_apIconOnGlobalRadar[i]->SetTexture( TEX_RADAR_ICON );
	}


	m_pTextMessageManager = new CTextMessageManager( "radio_pac" );
	m_pTextMessageManager->SetRenderer( new CDefaultTextMessageRenderer( m_pGraphicsEffectManager, 0, 10 ) );
}


bool HUD_PlayerAircraft::LoadGlobalMapTexture( const std::string& texture_filename )
{
	LOG_FUNCTION_SCOPE();

//	m_GlobalMap.filename = texture_filename;
//	bool res = m_GlobalMap.Load();
//	if( res )

	if( !m_pGraphicsEffectManager )
	{
		LOG_PRINT_ERROR( " Not initialized yet." );
		return false;
	}

	bool loaded = m_pGraphicsEffectManager->GetGraphicsElementManager()->LoadTexture( TEX_GLOBAL_RADAR, texture_filename );
	if( loaded )
	{
		m_pGlobalRadarBG->SetTexture( TEX_GLOBAL_RADAR );
        return true;
	}
	else
	{
		LOG_PRINT_ERROR( " Unable to load the texture: " +  texture_filename );
//		m_GlobalMap.filename = "";
		return false;
	}
}


void HUD_PlayerAircraft::ReleaseGraphicsResources()
{
}


void HUD_PlayerAircraft::LoadGraphicsResources( const GraphicsParameters& rParam )
{
}


class FlipVariable
{
	float accumulated_time;
	float flip_interval;

public:

	int current;

	FlipVariable( float interval ) : flip_interval(interval), accumulated_time(0), current(0) {}

	void SetFlipInterval( float interval ) { flip_interval = interval; }

	void Update( float dt )
	{
		accumulated_time += dt;
		if( flip_interval < accumulated_time )
		{
			// TODO: accurate flip interval
			accumulated_time = 0;
			current ^= 1;
		}
	}
};

static FlipVariable g_FlipVar = FlipVariable(0.4f);


void HUD_PlayerAircraft::Update( float  dt )
{
	PROFILE_FUNCTION();

	HUD_PlayerBase::Update( dt );

	// update the center position of the HUD
	CBaseEntity *pBaseEntity = SinglePlayerInfo().GetCurrentPlayerBaseEntity();
	if( !pBaseEntity || pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
		return;

	if( m_pTimeText )
	{
		char buffer[32];
		m_TimerDisplay.GetTimeMMSS( buffer );
		m_pTimeText->SetText( buffer );

		if( m_TimerDisplay.m_TimeMS < gs_TimerDisplayBlinkThreasholdMS )
		{
			if( m_TimeTextBlinkEffect == CGraphicsEffectHandle::Null() )
				m_TimeTextBlinkEffect = m_pGraphicsEffectManager->BlinkAlpha( m_pTimeText, 0.5, 0 );
		}
		else
		{
			if( m_TimeTextBlinkEffect != CGraphicsEffectHandle::Null() )
				m_pGraphicsEffectManager->CancelEffect( m_TimeTextBlinkEffect );
		}
	}

//	if( m_pGraphicsEffectManager )
//		m_pGraphicsEffectManager->UpdateEffects( dt );

	CBE_PlayerPseudoAircraft *pPlayerAircraft = (CBE_PlayerPseudoAircraft *)pBaseEntity;

	uint w=0, h=0;
	GraphicsDevice().GetViewportSize( w, h );
	const float width  = (float)w;
	const float height = (float)h;

	m_HUD.SetScreenCenter( width  * ( 0.5f - pPlayerAircraft->GetCameraHeading() * 0.5f ),
			               height * ( 0.5f - pPlayerAircraft->GetCameraPitch()   * 0.5f ) );

/*	m_HUD.ClearTargetInfo();

	const vector<CCopyEntity *>& vecpVisibleEntity = pPlayerAircraft->GetVisibleEntity();
	size_t i, num_visible_entities = vecpVisibleEntity.size();
	for()
*/
	// update flip state for the container on focused target
	g_FlipVar.Update( dt );

	m_pTextMessageManager->Update( dt );
}


/**
 * returns heading angle from the direction in 3D space
 */
inline float GetHeadingAngle( const Vector3& dir )
{
	// vHrzDir == horizontal direction in 2D space where x-axis faces right, and y-axis upward on screen
	Vector2 vHrzDir;
	if( 0.00001f < 1.0f - fabsf(dir.y) )
		vHrzDir = Vec2GetNormalized( Vector2( dir.x, dir.z ) );
	else
		vHrzDir = Vector2(0,1);

	float dp = Vec2Dot( Vector2(0,1), vHrzDir );
	return (float)acos(dp) * ( 0 < vHrzDir.x ? 1.0f : -1.0f );
}


inline void SetTexCoord( int i, TEXCOORD2& vMin, TEXCOORD2& vMax )
{
	float u = 0.5f * (float)i;
	float v = 0.0f;
	vMin = TEXCOORD2( u, v );
	vMax = TEXCOORD2( u + 0.499f, v + 0.499f );
}


void HUD_PlayerAircraft::RenderImpl()
{
	PROFILE_FUNCTION();

	CBaseEntity *pBaseEntity = SinglePlayerInfo().GetCurrentPlayerBaseEntity();

	if( !pBaseEntity || pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
		return;

	if( !m_pGraphicsEffectManager )
		return;

	CBE_PlayerPseudoAircraft *plane = dynamic_cast<CBE_PlayerPseudoAircraft *>(pBaseEntity);

	int state = plane->GetAircraftState();
	SFloatRGBAColor hud_color;
	switch(state)
	{
	case CBE_PlayerPseudoAircraft::STATE_NORMAL:	hud_color = m_aHUDColor[COLOR_NORMAL]; break;
	case CBE_PlayerPseudoAircraft::STATE_LOCKED_ON:	hud_color = m_aHUDColor[COLOR_LOCKED_ON]; break;
	case CBE_PlayerPseudoAircraft::STATE_MISSILE_APPROACHING:	hud_color = m_aHUDColor[COLOR_MISSILE_APPROACHING]; break;
	default: break;
	}

	m_HUD.SetColor( hud_color.GetARGB32() );

	// render general hud for aircraft
//	m_HUD.SetColor( m_State );
	m_HUD.Render( plane->PseudoSimulator() );

	RenderTargetContainerRects( plane );

	//
	// render focused target position indicator
	//

	Camera* pCamera = pBaseEntity->GetCamera();
	if( !pCamera )
		return;

	float scale = GetScreenWidth() / 800.0f;

	shared_ptr<CRadar> pShortRangeRadar = plane->ShortRangeRadar();
	if( !pShortRangeRadar )
		return;

	const CRadarInfo& radar_info = pShortRangeRadar->RadarInfo();

	// a triangle that indicates the position of the currently focused target
	Vector2 focus_indicator[3]
	= { Vector2( 0,0/*arrow_max_len*/)*scale,	// arrow top
		Vector2( 8,0)*scale,					// arrow botton right
		Vector2(-8,0)*scale };					// arrow botton left


	float arrow_base_len = 40.0f*scale;
	Vector2 focus_indicator_offset = Vector2(0,-120*scale);
	Vector2 screen_center = m_HUD.GetScreenCenter();
	Vector3 pos;
	D3DXMATRIX matCamera;
	Matrix44 view;
	pCamera->GetCameraMatrix( view );
//	view.GetRowMajorMatrix44( (float *)&matCamera );

	const HUD_TargetInfo* pTarget = radar_info.GetFocusedTarget();
	if( pTarget )
	{
		float arrow_length 
			= arrow_base_len
			* ( 1.0f - Vec3Dot( Vec3GetNormalized(pTarget->position - pCamera->GetPosition()), pCamera->GetFrontDirection() ) );
//		arrow_length -= arrow_base_len * 0.1f;
		Limit( arrow_length, 0.0f, 1000.0f );

		focus_indicator[0].y = - arrow_length;

//		D3DXVec3TransformCoord( &pos, &pTarget->position, &matCamera );
		pos = view * pTarget->position;
		if( pos.z < 0 ||	// target is behind the player
			15.0f < fabsf(pos.x) && 15.0f < fabsf(pos.y) )	// target is not in the center area of the HUD
		{
			// project 'pos' on the plane along z-axis in camera space
			pos.z = 0;

			Vec3Normalize( pos, pos );
			float angle = acos( Vec2Dot(Vector2(0,1),Vector2(pos.x,pos.y)) );
			if( pos.x < 0 ) angle *= -1;

			Matrix22 matRot = Matrix22Rotation( angle );

			C2DTriangle triangle;

			for( int j=0; j<3; j++ )
                triangle.SetPosition( j, matRot * (focus_indicator[j] + focus_indicator_offset) + screen_center );

//			triangle.SetDestAlphaBlendMode( D3DBLEND_ONE );
			triangle.SetDestAlphaBlendMode( AlphaBlend::One );
			triangle.SetColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );

			triangle.Draw();
		}
	}

	// display targets on the local radar
	RenderLocalRadar( plane );

	// render weapon & ammo status
	RenderPlaneAndWeaponStatus( plane );

	if( m_bDisplayGlobalRadar )
	{
		m_pGlobalRadar->SetAlpha( 1, 1.0f );
		for( int i=0; i<NUM_MAX_ICONS_ON_GLOBAL_RADAR; i++ ) m_apIconOnGlobalRadar[i]->SetAlpha( 1, 1.0f );
	}
	else
	{
		m_pGlobalRadar->SetAlpha( 1, 0.0f );
		for( int i=0; i<NUM_MAX_ICONS_ON_GLOBAL_RADAR; i++ ) m_apIconOnGlobalRadar[i]->SetAlpha( 1, 0.0f );
	}

	if( m_bDisplayGlobalRadar )
		RenderGlobalRadar( plane );

	if( m_pSubDisplay )
	{
		m_pSubDisplay->Render();
	}
}


void HUD_PlayerAircraft::RenderTargetContainerRects( CBE_PlayerPseudoAircraft *plane )
{
	Camera* pCamera = plane->GetCamera();
	if( !pCamera )
		return;

//	const CRadarInfo& radar_info = plane->GetRadarInfo();
	shared_ptr<CRadar> pShortRangeRadar = plane->ShortRangeRadar();
	if( !pShortRangeRadar )
		return;

//	float scale = GetScreenWidth() / 800.0f;

//	D3DXMATRIX matCamera, matProj, matCameraProj;
	Matrix44 view, proj;
	pCamera->GetCameraMatrix( view );
	pCamera->GetProjectionMatrix( proj );
//	view.GetRowMajorMatrix44( (float *)&matCamera );
//	proj.GetRowMajorMatrix44( (float *)&matProj );
	Matrix44 proj_view = proj * view;
//	matCameraProj = matCamera * matProj;

	const CRadarInfo& radar_info = pShortRangeRadar->RadarInfo();

	// render containers on targets
	Vector3 pos;
	float x,y,r = (float)m_ContainerSize * 0.5f - 0.5f;
	SFloatRGBAColor color;
//	rect.SetDestAlphaBlendMode( D3DBLEND_ONE );

	const Vector3 vCamFwdDir = pCamera->GetFrontDirection();
	const Vector3 vCamPos    = pCamera->GetPosition();

//	float screen_width, screen_height;
//	GetViewportSize( screen_width, screen_height );
	int i;
	const int num_targets = (int)radar_info.GetTargetInfo().size();
	const int num_displayable_targets = num_targets < NUM_MAX_CONTAINER_RECTS ? num_targets : NUM_MAX_CONTAINER_RECTS;
	const float container_max_dist = 30000;
	int target_index = 0;
	for( i=0; i<num_displayable_targets; i++ )
	{
		const HUD_TargetInfo& target = radar_info.GetTargetInfo()[i];
//		Vector3 vWorldPos = vecpVisibleEntity[i]->Position();
		Vector3 vWorldPos = target.position;

		// skip if it's not visible
		Vector3 vCamToTargetDir = Vec3GetNormalized( vWorldPos - vCamPos );
		if( deg_to_rad(30.0f) < acos(Vec3Dot(vCamFwdDir,vCamToTargetDir)) )
			continue;

		if( container_max_dist * container_max_dist < Vec3LengthSq( vCamPos - vWorldPos ) )
			continue;

		if( target.type & HUD_TargetInfo::MISSILE
		 || target.type & HUD_TargetInfo::PLAYER )
			continue;

//		D3DXVec3TransformCoord( &pos, &vWorldPos, &matCameraProj );
		pos = proj_view * vWorldPos;

		x = (  pos.x + 1.0f ) * 0.5f * GetReferenceScreenWidth();// screen_width;
		y = ( -pos.y + 1.0f ) * 0.5f * GetReferenceScreenHeight();// screen_height;

		// set color for container
		if( (target.type & HUD_TargetInfo::LOCKED_ON) )
		{
			color = m_aHUDColor[COLOR_LOCKED_ON];	// locked-on - red
		}
		else if( (target.type & HUD_TargetInfo::FOCUSED) && !g_FlipVar.current )
		{
			color = SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.0f );
		}
		else
		{
			color = m_aHUDColor[COLOR_NORMAL];
		}

		m_apContainer[target_index]->SetColor( 0, color );

//		rect.SetPositionLTRB( x-r, y-r, x+r, y+r );
		m_apContainer[target_index]->SetLocalTopLeftPos( Vector2(x-r,y-r) );

		target_index++;
	}

	// set the rest of the rects to transparent
	for( i=target_index; i<NUM_MAX_CONTAINER_RECTS; i++ )
	{
		m_apContainer[i]->SetColor( 0, SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.0f ) );
	}
}


void HUD_PlayerAircraft::DisplayGlobalRadar( bool display )
{
	m_bDisplayGlobalRadar = display;
}


inline U32 HUD_PlayerAircraft::GetIconColor( int target_type )
{
	if( target_type & HUD_TargetInfo::MISSILE )
		return 0xF0F0F0F0;
	else if( target_type & HUD_TargetInfo::ENEMY )
		return 0xF0F00000;
	else if( target_type & HUD_TargetInfo::PLAYER )
		return 0xF0F0F0F0;
	else
		return 0xF0808080;

/*	switch( target_type )
	{
	case HUD_TargetInfo::PLAYER:	return 0xF0F0F0F0;
	case HUD_TargetInfo::ENEMY:		return 0xF0F0F0F0;
	case HUD_TargetInfo::MISSILE:	return 0xF0F00000;
//	case HUD_TargetInfo:::	return ;
//	case HUD_TargetInfo:::	return ;
	default: return 0xF0808080;
	}*/
}


void HUD_PlayerAircraft::RenderLocalRadar( CBE_PlayerPseudoAircraft *plane )
{
	const Vector3 vFwdDir = plane->PseudoSimulator().GetHorizontalForwardDirection();
	float dp = Vec2Dot( Vector2(0,1), Vector2(vFwdDir.x, vFwdDir.z) );
	float heading = (float)acos(dp) * ( 0 < vFwdDir.x ? 1.0f : -1.0f );
//	float heading = (float)acos(dp);
	Matrix22 matInvPlayerOrient = Matrix22Rotation( -heading );
	Matrix22 matPlayerOrient = Matrix22Rotation( heading );

	// display targets on radar
//	const CRadarInfo& radar_info = plane->GetRadarInfo();
	shared_ptr<CRadar> pShortRangeRadar = plane->ShortRangeRadar();
	if( !pShortRangeRadar )
		return;

	const CRadarInfo& radar_info = pShortRangeRadar->RadarInfo();

	const vector<HUD_TargetInfo>& vecTargetInfo = radar_info.GetAllTargetInfo();

	int radar_rect_size = LOCAL_RADAR_SIZE;
	SRect local_radar_rect = RectAtLeftBottom(
		LOCAL_RADAR_SIZE,
		LOCAL_RADAR_SIZE,
		LOCAL_RADAR_LEFT_MARGIN,
		LOCAL_RADAR_BOTTOM_MARGIN );

	float x,y,r;
	x = (float)local_radar_rect.GetCenterX();
	y = (float)local_radar_rect.GetCenterY();
	Vector2 radar_offset = Vector2( x, y );
	r = radar_rect_size / 2.0f;
	AABB2 radar_aabb = AABB2( Vector2(-r,-r) + radar_offset, Vector2(r,r) + radar_offset );

	C2DRect radar_rect;

	//
	// draw icons on local radar
	//
	const Vector2 base_pos[4] = { Vector2(-1,-1), Vector2(1,-1), Vector2(1,1), Vector2(-1,1) };
	Vector3 vPlayerPos = plane->GetPlayerCopyEntity()->GetWorldPosition();
	r = 12;
//	DWORD color;
	Matrix22 matTgtOrient;
	int i, num_tgts = (int)vecTargetInfo.size();
	int num_icons_to_render = 0;
	TEXCOORD2 vMin, vMax;
	for( i=0; i<num_tgts; i++ )
	{
		if( NUM_MAX_ICONS_ON_LOCAL_RADAR <= num_icons_to_render )
			break;

        DWORD color = 0xFFFFFFFF;

		// position measured from player's origin
		Vector3 local_pos = (vecTargetInfo[i].position - vPlayerPos);

		Vector2 offset = Vector2( local_pos.x, local_pos.z * (-1) ) * 0.01f;

		const int tgt_type = vecTargetInfo[i].type;
		if( tgt_type & HUD_TargetInfo::MISSILE )
		{
//			rotation_angle = 0;
			matTgtOrient = Matrix22Identity();
			r = 4;
			SetTexCoord( 1, vMin, vMax );
			color = 0xF0F0F0F0;
		}
		else if( tgt_type & HUD_TargetInfo::TGT_AIR )
		{
//			rotation_angle = GetHeadingAngle(vecTargetInfo[i].direction);
			matTgtOrient = Matrix22Rotation( GetHeadingAngle(vecTargetInfo[i].direction) );
			r = 12;
			SetTexCoord( 0, vMin, vMax );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_SURFACE )
		{
//			rotation_angle = player_heading_angle;
			matTgtOrient = matPlayerOrient;
			r = 8;
			SetTexCoord( 1, vMin, vMax );
		}
		else
			continue;

        color = GetIconColor( tgt_type );

		// flicker if the target is focused
		if( tgt_type & HUD_TargetInfo::FOCUSED )
//			color = g_FlipVar.current ? color : 0x00000000;
			if(!g_FlipVar.current) color = 0x00000000;

		Matrix23 transform
			= Matrix23( radar_offset, matInvPlayerOrient )
			* Matrix23( offset, matTgtOrient * Matrix22Scaling(r) );

		if( !radar_aabb.IsPointInside(transform * Vector2(0,0)) )
			continue; // not on the local radar

		m_apIconOnLocalRadar[num_icons_to_render]->SetLocalTransform( transform );

		SFloatRGBAColor dest_color;
		dest_color.SetARGB32( color );
		m_apIconOnLocalRadar[num_icons_to_render]->SetColor( 0, dest_color );

		m_apIconOnLocalRadar[num_icons_to_render]->SetTextureCoord( vMin, vMax );

		num_icons_to_render++;
	}

	for( i=0; i<num_icons_to_render; i++ )
	{
//		m_apIconOnLocalRadar[i]->Show();
	}

	// hide the redundant icons
	for( i=num_icons_to_render; i<m_NumLastRenderedLocalRadarIcons; i++ )
	{
//		m_apIconOnLocalRadar[i]->Hide();
		m_apIconOnLocalRadar[i]->SetColor( 0, SFloatRGBAColor(0,0,0,0) );
	}

	m_NumLastRenderedLocalRadarIcons = num_icons_to_render;
}


void HUD_PlayerAircraft::RenderGlobalRadar( CBE_PlayerPseudoAircraft *plane )
{
	const Vector3 vFwdDir = plane->PseudoSimulator().GetHorizontalForwardDirection();
	float dp = Vec2Dot( Vector2(0,1), Vector2(vFwdDir.x, vFwdDir.z) );
	float heading = (float)acos(dp) * ( 0 < vFwdDir.x ? 1.0f : -1.0f );

	// display targets on radar
//	const CRadarInfo& radar_info = plane->GetRadarInfo();
	shared_ptr<CRadar> pLongRangeRadar = plane->LongRangeRadar();
	if( !pLongRangeRadar )
		return;

	const CRadarInfo& radar_info = pLongRangeRadar->RadarInfo();

	const vector<HUD_TargetInfo>& vecTargetInfo = radar_info.GetAllTargetInfo();

	int radar_rect_size = GLOBAL_RADAR_SIZE;
	float x = (float)m_GlobalRadarRect.GetCenterX();
	float y = (float)m_GlobalRadarRect.GetCenterY();
	float r = (float)radar_rect_size * 0.5f;

	const SRect global_radar_rect = m_GlobalRadarRect;

	Vector2 radar_offset = Vector2( x, y );	// center pos of the global radar rect
//	radar_rect.SetPositionLTRB( x-r, y-r, x+r, y+r );
//	radar_rect.SetColor( 0xA0000000 );
//	radar_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	if( m_pGraphicsEffectManager->GetGraphicsElementManager()->GetTexture(TEX_GLOBAL_RADAR).GetTexture() )
		m_pGlobalRadarBG->SetColor( 0, SFloatRGBAColor(1.0f,1.0f,1.0f,0.63f) );//0xA0FFFFFF
	else
		m_pGlobalRadarBG->SetColor( 0, SFloatRGBAColor(0.0f,0.0f,0.0f,0.63f) );

	C2DRect radar_rect;
	r *= 0.1f;
	float w = r * 0.15f;
	radar_rect.SetColor( 0x10F0F0F0 );
	radar_rect.SetPositionLTRB( x-r, y-w, x+r, y+w );
	radar_rect.Draw();
	radar_rect.SetPositionLTRB( x-w, y-r, x+w, y+r );
	radar_rect.Draw();

	//
	// draw icons on local radar
	//
	const Vector2 base_pos[4] = {  Vector2(-1,-1), Vector2(1,-1), Vector2(1,1), Vector2(-1,1) };
	DWORD color = 0x00000000;
	Matrix22 matTgtOrient;
	size_t i, num_tgts = vecTargetInfo.size();
	int num_icons_to_render = 0;
	TEXCOORD2 vMin, vMax;
	for( i=0; i<num_tgts; i++ )
	{
		Vector3 pos = vecTargetInfo[i].position;

//		Vector2 offset = Vector2( pos.x, pos.z * (-1) ) * 0.05f;
		Vector2 offset = Vector2( pos.x, pos.z * (-1.0f) ) / 600000.0f * (float)radar_rect_size;

		const int tgt_type = vecTargetInfo[i].type;
		if( tgt_type & HUD_TargetInfo::MISSILE )
		{
			matTgtOrient = Matrix22Identity();
			r = 4;
			SetTexCoord( 1, vMin, vMax );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_AIR )
		{
			matTgtOrient = Matrix22Rotation( GetHeadingAngle(vecTargetInfo[i].direction) );
			r = 12;
			SetTexCoord( 0, vMin, vMax );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_SURFACE )
		{
			matTgtOrient = Matrix22Identity();
			r = 8;
			SetTexCoord( 1, vMin, vMax );
		}
		else
			continue;

        color = GetIconColor( tgt_type );

//		if( tgt_type & HUD_TargetInfo::FOCUSED )
//			color = g_FlipVar.current ? color : 0x00000000;

		SFloatRGBAColor dest_color;
		dest_color.SetARGB32( color );

		m_apIconOnGlobalRadar[num_icons_to_render]->SetColor( 0, dest_color );

		m_apIconOnGlobalRadar[num_icons_to_render]->SetTextureCoord( vMin, vMax );

		Matrix23 transform
			= Matrix23( radar_offset, Matrix22Identity() )
			* Matrix23( offset, matTgtOrient * Matrix22Scaling(r) );

		m_apIconOnGlobalRadar[num_icons_to_render]->SetLocalTransform( transform );

		num_icons_to_render++;

		// set vertex positions
/*		for( int j=0; j<4; j++ )
		{
			Vector2 dest_pos = ( matTgtOrient * (base_pos[j] * r) + offset ) + radar_offset;

			radar_rect.SetPosition( j, dest_pos );
		}
*/
	}

	for( int i=0; i<num_icons_to_render; i++ )
	{
//		m_apIconOnGlobalRadar[i]->Show();
	}

	for( int i=num_icons_to_render; i<m_LastRenderedIconsOnGlobalRadar; i++ )
	{
//		m_apIconOnGlobalRadar[i]->Hide();
		m_apIconOnGlobalRadar[i]->SetColor( 0, SFloatRGBAColor(0,0,0,0) );
	}

	m_LastRenderedIconsOnGlobalRadar = num_icons_to_render;
}


inline const string spaces( int num_spaces )
{
	static const string space_string = "                                                                                ";

	if( num_spaces < 0 )
		num_spaces = 0;
	else if( (int)space_string.length() < num_spaces )
		num_spaces = (int)space_string.length();

	return space_string.substr( 0, num_spaces );
}


void HUD_PlayerAircraft::RenderPlaneAndWeaponStatus( CBE_PlayerPseudoAircraft *plane )
{
	CGI_Aircraft* pAircraft = plane->GetAircraft();

	if( !pAircraft )
		return;

	const float scale = GetScreenWidth() / 1600.0f;

	C2DRect bg_rect = RectAtRightBottom( 320, 120, 40, 80 );
//	C2DRect bg_rect( RectLTWH( 620, 500, 160, 60 ) * scale, 0x50000000 );

	CWeaponSystem& weapon_system = pAircraft->WeaponSystem();

	// borrow the font from graphics element manager
	FontBase *pFont = m_pGraphicsEffectManager->GetGraphicsElementManager()->GetFont( GLOBAL_FONT_INDEX );
	pFont->SetFontColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );
	const int orig_font_w = pFont->GetFontWidth();
	const int orig_font_h = pFont->GetFontHeight();
	pFont->SetFontSize( (int)(20*scale), (int)(32*scale) );

	SetRenderStatesForTextureFont( AlphaBlend::One );
	pFont->SetDestAlphaBlendMode( AlphaBlend::One );

	int current_weapon_index = weapon_system.GetPrimaryWeaponSlotIndex();
	const int weapon_slot_offset = 0;	// 0: gun / 1: missile / 2: special weapon
	DWORD color = 0xFFFFFFFF;
	int i, num = 3;//vec.size();
	for( i=0; i<num; i++ )
	{
		SWeaponSlot& slot = weapon_system.GetWeaponSlot( weapon_slot_offset + i );
		CGI_Ammunition* pAmmo = slot.pChargedAmmo;

		string text = "NONE";
		if( pAmmo )
		{
//			string text = pAmmo->GetName() + string(" : ") + to_string(pAmmo->GetCurrentQuantity());
			text = spaces( 7 - (int)pAmmo->GetName().length() )
				+ pAmmo->GetName()
				+ string(" : ")
				+ to_string(pAmmo->GetCurrentQuantity());

		}
		color = i == current_weapon_index ? m_aHUDColor[COLOR_HIGHLIGHTED].GetARGB32() : m_aHUDColor[COLOR_NORMAL].GetARGB32();
		pFont->DrawText( text.c_str(), Vector2( 1260*scale, (1016 + 32 * i)*scale ), color );
	}

	// display current life by color
	CCopyEntity* pEntity = plane->GetPlayerCopyEntity();
	if( IsValidEntity(pEntity) )
	{
		float fLife = pEntity->fLife;
//		float fOrigLife = fLife;
		float fMaxGreenLife = 100.0f; //PlayerShip.GetMaxLife()
		if( fMaxGreenLife < fLife )
			fLife = fMaxGreenLife;
		else if( fLife < 0.0f )
			fLife = 0.0f;

		DWORD dwColor = D3DCOLOR_XRGB( 250 - (int)(250*fLife/fMaxGreenLife),	// red
									(int)(250*fLife/fMaxGreenLife),			// green
									0 );

		pFont->SetFontSize( (int)(24*scale), (int)(40*scale) );

//		m_pAircraftSetText( 
		pFont->DrawText( to_string(fLife, 0, 3).c_str(), Vector2( 1260*scale, 944*scale ), dwColor );
	}

	// debug - display current position
	Vector3 vPos = pEntity->GetWorldPosition();
	pFont->DrawText( fmt_string("x: %f", vPos.x).c_str(), Vector2(1200*scale, 800*scale) );
	pFont->DrawText( fmt_string("y: %f", vPos.y).c_str(), Vector2(1200*scale, 845*scale) );
	pFont->DrawText( fmt_string("z: %f", vPos.z).c_str(), Vector2(1200*scale, 890*scale) );

	// restore the font size
	pFont->SetFontSize( orig_font_w, orig_font_h );
}


void HUD_PlayerAircraft::CreateRenderTasks()
{
//	if( !m_pSubDisplay )
//		return;

//	SubMonitor *pMonitor = m_pSubDisplay->GetCurrentMonitor();
//	if( !pMonitor )
//		return;

//	pMonitor->m_pStage.lock();
}


/*
	DWORD dwFontColor = 0x7030B030;

	// display current life by color
	float fLife = SinglePlayerInfo().GetCurrentPlayerBaseEntity()->GetCurrentLife();
	float fOrigLife = fLife;
	float fMaxGreenLife = 100.0f; //PlayerShip.GetMaxLife()
	if( fMaxGreenLife < fLife )
		fLife = fMaxGreenLife;
	else if( fLife < 0.0f )
		fLife = 0.0f;

	DWORD dwColor = D3DCOLOR_XRGB( 250 - (int)(250*fLife/fMaxGreenLife),	// red
								  (int)(250*fLife/fMaxGreenLife),			// green
								  0 );										// blue
	m_pAircraftIcon->SetColor(dwColor);


	// draw weapon info - left bottom corner
	C2DRect rect;
	rect.SetPosition( D3DXVECTOR2(  80, 530 ) * fRatio,
		              D3DXVECTOR2( 300, 560 ) * fRatio );
	rect.SetColor( 0x50000000 );
	rect.Draw();
	rect.SetColor( dwFontColor );
	rect.SetDestAlphaBlendMode( D3DBLEND_ONE );
	rect.DrawWireframe();

	const int iSlot = WEAPONSYSTEM.GetCurrentWeaponSlotID();
	char str[2][64];
	sprintf( str[0], "[%d] %s",
		iSlot, WEAPONSYSTEM.GetWeaponSlot(iSlot).pWeapon->GetName().c_str() );
//	sprintf( str[1], %03d / %03d", );

    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	vPos = D3DXVECTOR2( 85 * fRatio, 535 * fRatio);
	m_pFont->SetFontColor( dwFontColor );
	m_pFont->DrawText( str[0], vPos, dwColor );

	font_width = (int)(8.0f * fRatio);
	m_pFont->SetFontSize( font_width, font_width*2 );
*/



bool HUD_PlayerAircraft::OpenTextWindow( CGameTextSet *pTextSet )
{
//	m_pTextWindow->OpenTextWindow( pTextSet );
	return true;
}

/*
CGameTextSet *HUD_PlayerAircraft::GetCurrentTextSetInTextWindow()
{
	return m_pTextWindow->GetCurrentTextSet();
}*/


} // namespace amorphous
