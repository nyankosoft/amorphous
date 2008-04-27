
#include "HUD_PlayerAircraft.h"
#include "HUD_SubDisplay.h"
#include "BE_PlayerPseudoAircraft.h"
#include "Stage/PlayerInfo.h"
#include "3DMath/Matrix22.h"
#include "3DMath/AABB2.h"

#include "3DCommon/2DFrameRect.h"
#include "3DCommon/2DTriangle.h"
#include "3DCommon/Direct3D9.h"
#include "3DCommon/D3DMisc.h"
//#include "Item/WeaponSystem.h"
//#include "GameInput/3DActionCode.h"
#include "GameTextSystem/GameTextWindow.h"
#include "Support/StringAux.h"
#include "Support/memory_helpers.h"
#include "Support/Log/DefaultLog.h"


HUD_PlayerAircraft::HUD_PlayerAircraft()
:
m_bDisplayGlobalRadar(false),
m_pFont(NULL),
m_pSubDisplay( new HUD_SubDisplay() )
//m_pTextWindow(NULL)
{
	m_TexCache.filename = "Texture\\hud_icon.dds";

	m_aHUDColor[COLOR_NORMAL]		= SFloatRGBAColor( 0.19f, 0.69f, 0.19f, 0.44f );	// 0x7030B030;
	m_aHUDColor[COLOR_HIGHLIGHTED]	= m_aHUDColor[COLOR_NORMAL] * 1.25f;
	m_aHUDColor[COLOR_LOCKED_ON]	= SFloatRGBAColor( 0.81f, 0.19f, 0.19f, 0.44f );	// 0x70D03030;
	m_aHUDColor[COLOR_MISSILE_APPROACHING]	= SFloatRGBAColor( 0.81f, 0.15f, 0.15f, 0.44f );	// 0x70D03030;

	m_HUD.SetColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );

	m_TimerDisplay.m_Color = m_aHUDColor[COLOR_NORMAL].GetARGB32();

	// set up the sub-display

	Matrix34 local_pose = Matrix34( Vector3( 0.0f, 0.0f, -20.0f ), Matrix33Identity() );	// front view
//	Matrix34 local_pose = Matrix34( Vector3( 0.0f, 0.0f, -20.0f ), Matrix33RotationY( 3.141592f ) );	// rear view
	m_pSubDisplay->Monitor().push_back( new SubMonitor_FixedView( PLAYERINFO.GetCurrentPlayerEntity(), local_pose ) );
	m_pSubDisplay->Monitor().push_back( new SubMonitor_EntityTracker( PLAYERINFO.GetCurrentPlayerEntity() ) );
	m_pSubDisplay->SetMonitorIndex( 1 );
}


HUD_PlayerAircraft::~HUD_PlayerAircraft()
{
	SafeDelete( m_pFont );

	SafeDelete( m_pSubDisplay );
}


void HUD_PlayerAircraft::UpdateScreenSize()
{
}


void HUD_PlayerAircraft::Init()
{
	m_pFont = new CTextureFont;
	m_pFont->InitFont( "Texture\\HGGE_16x8_256.dds", 256, 256, 16, 8 );

	m_HUD.Init();

	LoadGraphicsResources( CGraphicsParameters() );
/*
//	m_pFont->InitFont( "Texture\\MainFont.dds", 256, 256, 16, 8 );

	m_pTextWindow = new CGameTextWindow;
	m_pTextWindow->InitFont( "‚l‚r ƒSƒVƒbƒN", 0.018f, 0.036f );

	UpdateScreenSize();*/
}


bool HUD_PlayerAircraft::LoadGlobalMapTexture( const std::string& texture_filename )
{
	g_Log.Print( "HUD_PlayerAircraft::LoadGlobalMapTexture() called" );

	m_GlobalMap.filename = texture_filename;
	bool res = m_GlobalMap.Load();
	if( !res )
	{
		g_Log.Print( "HUD_PlayerAircraft::LoadGlobalMapTexture() - unable load texture: %s", texture_filename.c_str() );
		m_GlobalMap.filename = "";
		return false;
	}
	else
        return true;
}


void HUD_PlayerAircraft::ReleaseGraphicsResources()
{
	m_pFont->Release();

	m_TexCache.Release();

	m_GlobalMap.Release();

//	m_pSubDisplay->ReleaseGraphicsResources();
}


void HUD_PlayerAircraft::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	float scale = GetScreenWidth() / 800.0f;

	m_ContainerSize = (int)( 24.0f * scale );

	m_pFont->Reload();

	m_TexCache.Load();

	m_GlobalMap.Load();

//	m_pSubDisplay->LoadGraphicsResources();
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


void HUD_PlayerAircraft::RenderTime()
{
	SetRenderStatesForTextureFont( AlphaBlend::One );

	float scale = GetScreenWidth() / 800.0f;
	float sx = 600 * scale;
	float sy = 16  * scale;
	m_pFont->SetFontSize( (int)(15*scale), (int)(20*scale) );
	m_pFont->SetFontColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );

	m_TimerDisplay.Render( m_pFont, (int)sx, (int)sy );
}


void HUD_PlayerAircraft::Update( float  dt )
{
	// update the center position of the HUD
	CBaseEntity *pBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();
	if( !pBaseEntity || pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
		return;

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

	m_pSubDisplay->Update( dt );
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


void HUD_PlayerAircraft::Render()
{
	CBaseEntity *pBaseEntity = PLAYERINFO.GetCurrentPlayerBaseEntity();

	if( !pBaseEntity || pBaseEntity->GetArchiveObjectID() != CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT )
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
	DWORD color;
	C2DFrameRect rect;
	rect.SetBorderWidth( (int)( 2.0f * scale ) );
//	rect.SetDestAlphaBlendMode( D3DBLEND_ONE );

	float screen_width, screen_height;
	GetViewportSize( screen_width, screen_height );
//	size_t i, num_container_rects = vecpVisibleEntity.size();
	size_t i, num_container_rects = radar_info.GetNumVisibleTargets();
	for( i=0; i<num_container_rects; i++ )
	{
		const HUD_TargetInfo& target = radar_info.GetVisibleTarget((int)i);
//		D3DXVECTOR3 vWorldPos = vecpVisibleEntity[i]->Position();
		D3DXVECTOR3 vWorldPos = target.position;

		D3DXVec3TransformCoord( &pos, &vWorldPos, &matCameraProj );

		x = (  pos.x + 1.0f ) * 0.5f * screen_width;
		y = ( -pos.y + 1.0f ) * 0.5f * screen_height;

		// set color for container
		if( (target.type & HUD_TargetInfo::LOCKED_ON) )
		{
			color = m_aHUDColor[COLOR_LOCKED_ON].GetARGB32();	// locked-on - red
		}
		else if( (target.type & HUD_TargetInfo::FOCUSED) && !g_FlipVar.current )
		{
			color = 0x00000000;
		}
		else
		{
			color = m_aHUDColor[COLOR_NORMAL].GetARGB32();
		}

		rect.SetColor( color );

		rect.SetPositionLTRB( x-r, y-r, x+r, y+r );
		rect.Draw();
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
			15.0f < fabsf(pos.x) && 15.0f < fabsf(pos.y) )	// targe is not in the center area of the HUD
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

	// display time
	RenderTime();

	// render weapon & ammo status
	RenderPlaneAndWeaponStatus( plane );

	if( m_bDisplayGlobalRadar )
		RenderGlobalRadar( plane );

	if( m_pSubDisplay )
	{
		vector<SubMonitor *>& vecpMonitor = m_pSubDisplay->Monitor();
		size_t num_monitors = vecpMonitor.size();

		SubMonitor_EntityTracker *pTrackMonitor = NULL;
		for( size_t k=0; k<num_monitors; k++ )
		{
			if( vecpMonitor[k]->GetType() == SubMonitor::ENTITY_TRACKER )
			{
				pTrackMonitor = (SubMonitor_EntityTracker *)(vecpMonitor[k]);
				break;
			}
		}

		if( pTrackMonitor )
		{
			// direct the camera toward the currently focused target
			CCopyEntity* pPlayerEntity = plane->GetPlayerCopyEntity();
			Vector3 vPos = pPlayerEntity->Position() + pPlayerEntity->GetWorldPose().matOrient.GetColumn(2) * 1000.0f;
			float radius = 45.0f;

			const std::vector<HUD_TargetInfo>& vecTarget = radar_info.GetAllTargetInfo();
			size_t num_tgts = vecTarget.size();
			for( size_t k=0; k<num_tgts; k++ )
			{
				const HUD_TargetInfo& target = vecTarget[k];
				if( target.type & HUD_TargetInfo::FOCUSED )
				{
					vPos = target.position;
					radius = target.radius;
					break;
				}
			}

			pTrackMonitor->UpdateTargetPosition(vPos);
			pTrackMonitor->UpdateTargetRadius(radius);
		}

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

	int radar_rect_size = 136;
	float x,y,r;
	x = 80*scale;
	y = (600 - 80)*scale;
	r = radar_rect_size / 2 * scale;
	AABB2 radar_aabb = AABB2( Vector2(-r,-r), Vector2(r,r) );

	Vector2 radar_offset = Vector2( x, y );
	C2DRect radar_rect( x-r, y-r, x+r, y+r, 0x60000000 );
	C2DFrameRect radar_frame( x-r, y-r, x+r, y+r, 0xF020D020, 2.2f*scale );

	radar_rect.Draw();
	radar_frame.Draw();

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

	int radar_rect_size = 280;
	float x,y,r;
	x = 150 * scale;
	y = ( 600 - 150 ) * scale;
	r = radar_rect_size / 2 * scale;

	Vector2 radar_offset = Vector2( x, y );	// center pos of the global radar rect
	C2DRect radar_rect;
	radar_rect.SetPositionLTRB( x-r, y-r, x+r, y+r );
	radar_rect.SetColor( 0xA0000000 );
	radar_rect.SetTextureUV( TEXCOORD2(0,0), TEXCOORD2(1,1) );

	if( m_GlobalMap.GetTexture() )
	{
		radar_rect.SetColor( 0xA0FFFFFF );
		radar_rect.Draw( m_GlobalMap.GetTexture() );
	}
	else
	{
		radar_rect.SetColor( 0xA0000000 );
		radar_rect.Draw();
	}

	// mission area
	int br = (int)(r * 0.85);
	C2DFrameRect boundary_rect( x-br, y-br, x+br, y+br, 0x50FFFFFF, 2.0f * scale );
	boundary_rect.SetDestAlphaBlendMode( AlphaBlend::One );
	boundary_rect.Draw();

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

	m_pFont->SetFontColor( m_aHUDColor[COLOR_NORMAL].GetARGB32() );
	m_pFont->SetFontSize( (int)(10*scale), (int)(16*scale) );

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
		m_pFont->DrawText( text.c_str(), D3DXVECTOR2( 630*scale, (508 + 16 * i)*scale ), color );
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

		m_pFont->SetFontSize( (int)(12*scale), (int)(20*scale) );

		m_pFont->DrawText( to_string(fLife, 0, 3).c_str(), D3DXVECTOR2( 630*scale, 472*scale ), dwColor );
	}
}


void HUD_PlayerAircraft::CreateRenderTasks()
{
	if( m_pSubDisplay )
	{
	}
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
