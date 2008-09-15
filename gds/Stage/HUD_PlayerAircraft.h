#ifndef __HUD_PlayerAircraft_H__
#define __HUD_PlayerAircraft_H__


#include "HUD_PlayerBase.h"

#include "3DCommon/fwd.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/FloatRGBAColor.h"
#include "3DCommon/TextureFont.h"
#include "GameCommon/PseudoAircraftHUD.h"


class CGameTextWindow;
class CBE_PlayerPseudoAircraft;

class HUD_PlayerAircraft : public HUD_PlayerBase
{
public:

	enum eColor
	{
		COLOR_NORMAL,
		COLOR_HIGHLIGHTED,
		COLOR_LOCKED_ON,
		COLOR_FRAME,
		COLOR_BG, ///< color for background fill rects. COLOR_BACKGROUND is used by some windows header.
		COLOR_WHITEFRAME,
		COLOR_MISSILE_APPROACHING,
		NUM_COLORS
	};

private:

	enum Params
	{
		NUM_MAX_RECTS = 64,
		NUM_MAX_TGT_INFO = 32,

		// properties of graphics elements
		CONTAINER_SIZE = 48,
		LOCAL_RADAR_SIZE = 272,
		LOCAL_RADAR_LEFT_MARGIN = 30,
		LOCAL_RADAR_BOTTOM_MARGIN = 30,
		GLOBAL_RADAR_SIZE = 560,
		GLOBAL_RADAR_LEFT_MARGIN = 20,
		GLOBAL_RADAR_BOTTOM_MARGIN = 20,
		NUM_MAX_CONTAINER_RECTS = 64
	};

	enum FONT_INDEX
	{
		GLOBAL_FONT_INDEX = 0,
		MESSAGE_FONT_INDEX,
		NUM_HUD_FONTS
	};

	enum TEXTURE_INDEX
	{
		GLOBAL_RADAR_TEXTURE_INDEX = 0,
		NUM_HUD_TEXTURES
	};


	CGE_Text *m_pTimeText;

	CGraphicsEffectHandle m_TimeTextBlinkEffect;

	CGE_Rect *m_apContainer[NUM_MAX_CONTAINER_RECTS];

	CGE_Group *m_pGlobalRadar;

	CGE_Rect *m_pGlobalRadarBG;

	SRect m_LocalRadarRect;

	SRect m_GlobalRadarRect;

	CTextureHandle m_TexCache;

	CTextureHandle m_GlobalMap;

	int m_ContainerSize;

	CPseudoAircraftHUD m_HUD;

	HUD_SubDisplay* m_pSubDisplay;

	SFloatRGBAColor m_aHUDColor[NUM_COLORS];

	bool m_bDisplayGlobalRadar;

//	TCFixedVector<HUD_TargetInfo, NUM_MAX_TGT_INFO> m_vecTargetInfo;

//	CGameTextWindow *m_pTextWindow;

	void RenderLocalRadar( CBE_PlayerPseudoAircraft *plane );

	void RenderGlobalRadar( CBE_PlayerPseudoAircraft *plane );

	void RenderPlaneAndWeaponStatus( CBE_PlayerPseudoAircraft *plane );

	void RenderTime();

	inline U32 GetIconColor( int target_type );

public:

	HUD_PlayerAircraft();

	virtual ~HUD_PlayerAircraft();

	void Release();

	virtual int GetType() const { return TYPE_AIRCRAFT; }

	void UpdateScreenSize();

	void Init();

	inline bool HandleInput( int iActionCode, int input_type, float fVal );

	void Update( float dt );

	void RenderImpl();

	/// turns on/off the global radar
	void DisplayGlobalRadar( bool display ) { m_bDisplayGlobalRadar = display; }

	bool OpenTextWindow( CGameTextSet *pTextSet );

	virtual bool LoadGlobalMapTexture( const std::string& texture_filename );

	virtual void LoadGraphicsResources( const CGraphicsParameters& rParam );
	virtual void ReleaseGraphicsResources();

	virtual void CreateRenderTasks();

	virtual HUD_SubDisplay* GetSubDisplay() { return m_pSubDisplay; }

//	CGameTextSet *GetCurrentTextSetInTextWindow();
};

// =============================== inline implementations ===============================


inline bool HUD_PlayerAircraft::HandleInput( int iActionCode, int input_type, float fVal )
{
	return false;
//	return m_QuickMenuManager.HandleMouseInput( iActionCode, input_type, fVal );
}


#endif  /*  __HUD_PlayerAircraft_H__  */
