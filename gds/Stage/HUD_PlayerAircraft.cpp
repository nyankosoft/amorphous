#include "HUD_PlayerAircraft.h"
#include "BE_PlayerPseudoAircraft.h"
#include "SubDisplay.h"
#include "Stage/PlayerInfo.h"
#include "3DMath/Matrix22.h"
#include "3DMath/AABB2.h"

#include "3DCommon/2DFrameRect.h"
#include "3DCommon/2DTriangle.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DMisc.h"
#include "3DCommon/GraphicsEffectManager.h"
#include "GameTextSystem/TextMessageManager.h"
#include "GameTextSystem/TextMessageRenderer.h"
#include "Support/StringAux.h"
#include "Support/memory_helpers.h"
#include "Support/Profile.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Macro.h"

using namespace std;
using namespace boost;


static const int gs_TimerDisplayBlinkThreasholdMS = 60 * 1000 * 14;


HUD_PlayerAircraft::HUD_PlayerAircraft()
:
m_bDisplayGlobalRadar(false),
m_pTimeText(NULL),
m_pSubDisplay(NULL),
m_pTextMessageManager(NULL)
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

	for( int i=0; i<NUM_MAX_CONTAINER_RECTS; i++ )
		m_apContainer[i] = NULL;
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

	CGraphicsElementManager *pElementMgr = m_pGraphicsEffectManager->GetGraphicsElementManager();

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
	pElementMgr->CreateRect(      m_LocalRadarRect, m_aHUDColor[COLOR_BG],          local_radar_layer + 5 );
	pElementMgr->CreateFrameRect( m_LocalRadarRect, m_aHUDColor[COLOR_FRAME], 4.5f, local_radar_layer );

	m_pGlobalRadarBG = pElementMgr->CreateRect(      local_globalradar_rect, m_aHUDColor[COLOR_BG],          global_radar_layer + 5 );
	CGE_Rect *pFrame = pElementMgr->CreateFrameRect( local_globalradar_rect, m_aHUDColor[COLOR_FRAME], 4.5f, global_radar_layer );
	m_pGlobalRadarBG->SetTextureCoord( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	// inner frame rect that represents the area of operation
	SRect inner_frame_rect = local_globalradar_rect;
	inner_frame_rect.Inflate( -85, -85 );
	CGE_Rect *pInnerFrame = pElementMgr->CreateFrameRect( inner_frame_rect, SFloatRGBAColor( 1.0f, 1.0f, 1.0f, 0.32f ), 4.0f, global_radar_layer );
	pInnerFrame->SetDestAlphaBlendMode( AlphaBlend::One );

	CGraphicsElement *apElement[] = { m_pGlobalRadarBG, pFrame, pInnerFrame };
	m_pGlobalRadar = pElementMgr->CreateGroup( apElement, numof(apElement), m_GlobalRadarRect.GetTopLeftCorner() );
	m_pGlobalRadar->SetTopLeftPos( m_GlobalRadarRect.GetTopLeftCorner() );

	// small white cross on the center
	pElementMgr->CreateRect( RectLTRB( x-r, y-w, x+r, y+w ), m_aHUDColor[COLOR_WHITEFRAME], base_layer );
	pElementMgr->CreateRect( RectLTRB( x-w, y-r, x+w, y+r ), m_aHUDColor[COLOR_WHITEFRAME], base_layer );

	pElementMgr->LoadFont( GLOBAL_FONT_INDEX, "./Texture/BitstreamVeraSansMono_Bold.dds", CFontBase::FONTTYPE_TEXTURE, 30, 40 );

	// text for time display
	m_pTimeText = pElementMgr->CreateTextBox( 0, "", RectAtRightTop( 200, 50, 50, 32 ),
		CGE_Text::TAL_TOP, CGE_Text::TAL_CENTER,
		m_aHUDColor[COLOR_NORMAL], 30, 40, base_layer );

	m_pTimeText->SetDestAlphaBlendMode( AlphaBlend::One );

	for( size_t i=0; i<NUM_MAX_CONTAINER_RECTS; i++ )
		m_apContainer[i] = pElementMgr->CreateFrameRect( RectLTWH( 0, 0, m_ContainerSize, m_ContainerSize ), SFloatRGBAColor(0.0f,0.0f,0.0f,0.0f), 4.0f, base_layer );


	m_pTextMessageManager = new CTextMessageManager( "fg_radio" );
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

	bool loaded = m_pGraphicsEffectManager->GetGraphicsElementManager()->LoadTexture( GLOBAL_RADAR_TEXTURE_INDEX, texture_filename );
	if( loaded )
	{
		m_pGlobalRadarBG->SetTexture( GLOBAL_RADAR_TEXTURE_INDEX );
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


void HUD_PlayerAircraft::LoadGraphicsResources( const CGraphicsParameters& rParam )
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
	CBaseEntity *pBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();
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

	float width, height;
	GetViewportSize( width, height );

	m_HUD.SetScreenCenter( width  * ( 0.5f - pPlayerAircraft->GetCameraHeading() * 0.5f ),
			               height * ( 0.5f - pPlayerAircraft->GetCameraPitch()   * 0.5f ) );

/*	m_HUD.ClearTargetInfo();

	const vector<CCopyEntity *>& vecpVisibleEntity = pPlayerAircraft->GetVisibleEntity();
	size_t i, num_visible_entities = vecpVisibleEntity.size();
	for()
*/
	// update flip state for the container on focused target
	g_FlipVar.Update(dt);

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


inline void SetTexCoord( int i, C2DRect& rect )
{
	float u = 0.5f * (float)i;
	float v = 0.0f;
	rect.SetTextureUV( TEXCOORD2( u, v ), TEXCOORD2( u + 0.499f, v + 0.499f ) );
}


void HUD_PlayerAircraft::RenderImpl()
{
	PROFILE_FUNCTION();

	CBaseEntity *pBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();

	if( !pBaseEntity || pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
		return;

	if( !m_pGraphicsEffectManager )
		return;

	CBE_PlayerPseudoAircraft *plane = (CBE_PlayerPseudoAircraft *)pBaseEntity;

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

	CCamera* pCamera = pBaseEntity->GetCamera();
	if( !pCamera )
		return;

	float scale = GetScreenWidth() / 800.0f;

	D3DXMATRIX matCamera, matProj, matCameraProj;
	pCamera->GetCameraMatrix( matCamera );
	pCamera->GetProjectionMatrix( matProj );
	matCameraProj = matCamera * matProj;
	
//	vector<CCopyEntity *> vecpVisibleEntity = plane->GetVisibleEntity();
	const RadarInfo& radar_info = plane->GetRadarInfo();

	// render containers on targets
	D3DXVECTOR3 pos;
	float x,y,r = (float)m_ContainerSize * 0.5f - 0.5f;
	SFloatRGBAColor color;
//	rect.SetDestAlphaBlendMode( D3DBLEND_ONE );

//	float screen_width, screen_height;
//	GetViewportSize( screen_width, screen_height );
	size_t i, num_container_rects = radar_info.GetNumVisibleTargets() < NUM_MAX_CONTAINER_RECTS ? radar_info.GetNumVisibleTargets() : NUM_MAX_CONTAINER_RECTS;
	for( i=0; i<num_container_rects; i++ )
	{
		const HUD_TargetInfo& target = radar_info.GetVisibleTarget((int)i);
//		D3DXVECTOR3 vWorldPos = vecpVisibleEntity[i]->Position();
		D3DXVECTOR3 vWorldPos = target.position;

		D3DXVec3TransformCoord( &pos, &vWorldPos, &matCameraProj );

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

		m_apContainer[i]->SetColor( 0, color );

//		rect.SetPositionLTRB( x-r, y-r, x+r, y+r );
		m_apContainer[i]->SetLocalTopLeftPos( Vector2(x-r,y-r) );
	}

	// set the rest of the rects to transparent
	for( ; i<NUM_MAX_CONTAINER_RECTS; i++ )
	{
		m_apContainer[i]->SetColor( 0, SFloatRGBAColor( 0.0f, 0.0f, 0.0f, 0.0f ) );
	}

	//
	// render focused target position indicator
	//

	// a triangle that indicates the position of the currently focused target
	Vector2 focus_indicator[3]
	= { Vector2( 0,0/*arrow_max_len*/)*scale,	// arrow top
		Vector2( 8,0)*scale,					// arrow botton right
		Vector2(-8,0)*scale };					// arrow botton left


	float arrow_base_len = 40.0f*scale;
	Vector2 focus_indicator_offset = Vector2(0,-120*scale);
	Vector2 screen_center = m_HUD.GetScreenCenter();

	const HUD_TargetInfo* pTarget = radar_info.GetFocusedTarget();
	if( pTarget )
	{
		float arrow_length 
			= arrow_base_len
			* ( 1.0f - Vec3Dot( Vec3GetNormalized(pTarget->position - pCamera->GetPosition()), pCamera->GetFrontDirection() ) );
//		arrow_length -= arrow_base_len * 0.1f;
		Limit( arrow_length, 0.0f, 1000.0f );

		focus_indicator[0].y = - arrow_length;

		D3DXVec3TransformCoord( &pos, &pTarget->position, &matCamera );
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
		m_pGlobalRadar->SetAlpha( 1, 1.0f );
	else
		m_pGlobalRadar->SetAlpha( 1, 0.0f );

	if( m_bDisplayGlobalRadar )
		RenderGlobalRadar( plane );

	if( m_pSubDisplay )
	{
		m_pSubDisplay->Render();
	}
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

	float scale = GetScreenWidth() / 800.0f;

	// display targets on radar
	const RadarInfo& radar_info = plane->GetRadarInfo();

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
	r = radar_rect_size / 2 * scale;
	AABB2 radar_aabb = AABB2( Vector2(-r,-r), Vector2(r,r) );

	Vector2 radar_offset = Vector2( x, y );

	C2DRect radar_rect;

	//
	// draw icons on local radar
	//
	const Vector2 base_pos[4] = { Vector2(-1,-1), Vector2(1,-1), Vector2(1,1), Vector2(-1,1) };
	Vector3 vPlayerPos = plane->GetPlayerCopyEntity()->Position();
	r = 6*scale;
//	DWORD color;
	Matrix22 matTgtOrient;
	size_t i, num_tgts = vecTargetInfo.size();
	for( i=0; i<num_tgts; i++ )
	{
        DWORD color = 0xFFFFFFFF;

		// position measured from player's origin
		Vector3 local_pos = (vecTargetInfo[i].position - vPlayerPos);

		Vector2 offset = Vector2( local_pos.x, local_pos.z * (-1) ) * 0.01f * scale;

		if( !radar_aabb.IsPointInside(offset) )
			continue;

		const int tgt_type = vecTargetInfo[i].type;
		if( tgt_type & HUD_TargetInfo::MISSILE )
		{
			matTgtOrient = Matrix22Identity();
			r = 2*scale;
			SetTexCoord( 1, radar_rect );
			color = 0xF0F0F0F0;
		}
		else if( tgt_type & HUD_TargetInfo::TGT_AIR )
		{
			matTgtOrient = Matrix22Rotation( GetHeadingAngle(vecTargetInfo[i].direction) );
			r = 6*scale;
			SetTexCoord( 0, radar_rect );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_SURFACE )
		{
			matTgtOrient = matPlayerOrient;
			r = 4*scale;
			SetTexCoord( 1, radar_rect );
		}

        color = GetIconColor( tgt_type );

		// flicker if the target is focused
		if( tgt_type & HUD_TargetInfo::FOCUSED )
//			color = g_FlipVar.current ? color : 0x00000000;
			if(!g_FlipVar.current) color = 0x00000000;

		// set vertex positions
		for( int j=0; j<4; j++ )
		{
			Vector2 dest_pos = matInvPlayerOrient
				             * ( matTgtOrient * (base_pos[j] * r) + offset )
							 + radar_offset;

			radar_rect.SetPosition( j, dest_pos );
		}

		radar_rect.SetColor( color );

		radar_rect.Draw( m_TexCache.GetTexture() );
	}
}


void HUD_PlayerAircraft::RenderGlobalRadar( CBE_PlayerPseudoAircraft *plane )
{
	const Vector3 vFwdDir = plane->PseudoSimulator().GetHorizontalForwardDirection();
	float dp = Vec2Dot( Vector2(0,1), Vector2(vFwdDir.x, vFwdDir.z) );
	float heading = (float)acos(dp) * ( 0 < vFwdDir.x ? 1.0f : -1.0f );

	float scale = GetScreenWidth() / 800.0f;

	// display targets on radar
	const RadarInfo& radar_info = plane->GetRadarInfo();

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

	if( m_pGraphicsEffectManager->GetGraphicsElementManager()->GetTexture(GLOBAL_RADAR_TEXTURE_INDEX).GetTexture() )
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
	for( i=0; i<num_tgts; i++ )
	{
		Vector3 pos = vecTargetInfo[i].position;

//		Vector2 offset = Vector2( pos.x, pos.z * (-1) ) * 0.05f;
		Vector2 offset = Vector2( pos.x, pos.z * (-1.0f) ) / 600000.0f * (float)radar_rect_size;

		const int tgt_type = vecTargetInfo[i].type;
		if( tgt_type & HUD_TargetInfo::MISSILE )
		{
			matTgtOrient = Matrix22Identity();
			r = 2;
			SetTexCoord( 1, radar_rect );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_AIR )
		{
			matTgtOrient = Matrix22Rotation( GetHeadingAngle(vecTargetInfo[i].direction) );
			r = 6*scale;
			SetTexCoord( 0, radar_rect );
		}
		else if( tgt_type & HUD_TargetInfo::TGT_SURFACE )
		{
			matTgtOrient = Matrix22Identity();
			r = 4*scale;
			SetTexCoord( 1, radar_rect );
		}

        color = GetIconColor( tgt_type );

//		if( tgt_type & HUD_TargetInfo::FOCUSED )
//			color = g_FlipVar.current ? color : 0x00000000;

		// set vertex positions
		for( int j=0; j<4; j++ )
		{
			Vector2 dest_pos = ( matTgtOrient * (base_pos[j] * r) + offset ) + radar_offset;

			radar_rect.SetPosition( j, dest_pos );
		}

		radar_rect.SetColor( color );

		radar_rect.Draw( m_TexCache.GetTexture() );
	}
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

	const float scale = GetScreenWidth() / 800.0f;

	C2DRect bg_rect( RectLTWH( 620, 500, 160, 60 ) * scale, 0x50000000 );
	bg_rect.Draw();

	CWeaponSystem& weapon_system = pAircraft->WeaponSystem();

	// borrow the font from graphics element manager
	CFontBase *pFont = m_pGraphicsEffectManager->GetGraphicsElementManager()->GetFont( GLOBAL_FONT_INDEX );
	pFont->SetFontColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );
	const int orig_font_w = pFont->GetFontWidth();
	const int orig_font_h = pFont->GetFontHeight();
	pFont->SetFontSize( (int)(10*scale), (int)(16*scale) );

	SetRenderStatesForTextureFont( AlphaBlend::One );

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
		pFont->DrawText( text.c_str(), D3DXVECTOR2( 630*scale, (508 + 16 * i)*scale ), color );
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

		pFont->SetFontSize( (int)(12*scale), (int)(20*scale) );

		pFont->DrawText( to_string(fLife, 0, 3).c_str(), D3DXVECTOR2( 630*scale, 472*scale ), dwColor );
	}

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

	DWORD dwFontColor = 0x7030B030;

	// background for status display
	m_StatusBackGround.Draw();

	// display current life by color
	float fLife = PLAYERINFO.GetCurrentPlayerBaseEntity()->GetCurrentLife();
	float fOrigLife = fLife;
	float fMaxGreenLife = 100.0f; //PlayerShip.GetMaxLife()
	if( fMaxGreenLife < fLife )
		fLife = fMaxGreenLife;
	else if( fLife < 0.0f )
		fLife = 0.0f;

	DWORD dwColor = D3DCOLOR_XRGB( 250 - (int)(250*fLife/fMaxGreenLife),	// red
								  (int)(250*fLife/fMaxGreenLife),			// green
								  0 );										// blue
	m_ShipIcon.SetColor(dwColor);
	m_ShipIcon.Draw();

	m_Crosshair.Draw();

	// display current life by digit
	char acLife[16];
	sprintf( acLife, "%03d", (int)fOrigLife );

    pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    pd3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	float fScreenWidth = (int)GAMEWINDOWMANAGER.GetScreenWidth();
	float fRatio = fScreenWidth / 800.0f;

	D3DXVECTOR2 vPos = D3DXVECTOR2( 400 - 15, 547.5f ) * fRatio;
	int font_width = (int)(10.0f * fRatio);	// font size 10*20 (when screen width = 800)
	m_pFont->SetFontSize( font_width, font_width*2 );
	m_pFont->SetFontColor( dwColor );
	m_pFont->DrawText( acLife, vPos, dwColor );


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

	m_QuickMenuManager.Render( &m_StatusFont );*/



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
