#ifndef __HUD_PlayerAircraft_H__
#define __HUD_PlayerAircraft_H__


#include "HUD_PlayerBase.hpp"
#include "amorphous/Graphics/TextureHandle.hpp"
#include "amorphous/Graphics/FloatRGBAColor.hpp"
#include "amorphous/GameTextSystem/fwd.hpp"
#include "amorphous/GameCommon/PseudoAircraftHUD.hpp"


namespace amorphous
{


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
		NUM_MAX_ICONS_ON_LOCAL_RADAR = 64,
		NUM_MAX_ICONS_ON_GLOBAL_RADAR = 128,

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

	enum TEXTURE_IDS
	{
		TEX_GLOBAL_RADAR = 0,
		TEX_RADAR_ICON,
		NUM_HUD_TEXTURES
	};


	boost::shared_ptr<TextElement> m_pTimeText;

	GraphicsElementAnimationHandle m_TimeTextBlinkEffect;

	boost::shared_ptr<FrameRectElement> m_apContainer[NUM_MAX_CONTAINER_RECTS];

	boost::shared_ptr<GraphicsElementGroup> m_pGlobalRadar;

	boost::shared_ptr<FillRectElement> m_pGlobalRadarBG;

	boost::shared_ptr<FillRectElement> m_apIconOnGlobalRadar[NUM_MAX_ICONS_ON_GLOBAL_RADAR];

	boost::shared_ptr<FillRectElement> m_apIconOnLocalRadar[NUM_MAX_ICONS_ON_LOCAL_RADAR];

	int m_NumLastRenderedLocalRadarIcons;

	int m_LastRenderedIconsOnGlobalRadar;

	SRect m_LocalRadarRect;

	SRect m_GlobalRadarRect;

	TextureHandle m_TexCache;

	TextureHandle m_GlobalMap;

	int m_ContainerSize;

	PseudoAircraftHUD m_HUD;

	/// borrowed reference
	CSubDisplay* m_pSubDisplay;

	SFloatRGBAColor m_aHUDColor[NUM_COLORS];

	bool m_bDisplayGlobalRadar;

//	TCFixedVector<HUD_TargetInfo, NUM_MAX_TGT_INFO> m_vecTargetInfo;

	CTextMessageManager *m_pTextMessageManager;

private:

	void RenderTargetContainerRects( CBE_PlayerPseudoAircraft *plane );

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
	void DisplayGlobalRadar( bool display );

	bool OpenTextWindow( CGameTextSet *pTextSet );

	virtual bool LoadGlobalMapTexture( const std::string& texture_filename );

	virtual void CreateRenderTasks();

	virtual CSubDisplay* GetSubDisplay() { return m_pSubDisplay; }

	void SetSubDisplay( CSubDisplay* pSubDisplay ) { m_pSubDisplay = pSubDisplay; }

	CTextMessageManager *GetTextMessageManager() { return m_pTextMessageManager; }
	

//	CGameTextSet *GetCurrentTextSetInTextWindow();
};

// =============================== inline implementations ===============================


inline bool HUD_PlayerAircraft::HandleInput( int iActionCode, int input_type, float fVal )
{
	return false;
//	return m_QuickMenuManager.HandleMouseInput( iActionCode, input_type, fVal );
}

} // namespace amorphous



#endif  /*  __HUD_PlayerAircraft_H__  */
