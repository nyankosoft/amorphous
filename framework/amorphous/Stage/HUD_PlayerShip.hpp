#ifndef __HUD_PLAYERSHIP_H__
#define __HUD_PLAYERSHIP_H__


#include "amorphous/Graphics/2DPrimitive/2DRect.hpp"
#include "amorphous/Graphics/Font/TextureFont.hpp"

//#include "QuickMenuManager.hpp"

#include "HUD_PlayerBase.hpp"


namespace amorphous
{


class CGameTextWindow;
class CGameTextSet;

class HUD_PlayerShip : public HUD_PlayerBase
{

	C2DRect m_StatusBackGround;
	C2DRect m_ShipIcon;
	C2DRect m_Crosshair;

	TextureHandle m_ShipIconTexture;
	TextureHandle m_CrosshairTexture;

	TextureFont m_StatusFont;

	CGameTextWindow *m_pTextWindow;

//	CQuickMenuManager m_QuickMenuManager;

	void LoadTextures();

public:

	HUD_PlayerShip();
	~HUD_PlayerShip();

	virtual int GetType() const { return TYPE_GENERAL; }

	void UpdateScreenSize();

	void Init();

	void RenderImpl();

	inline FontBase *GetFont() { return &m_StatusFont; }

	inline virtual void Update( float dt );

	inline virtual bool HandleInput( int iActionCode, int input_type, float fVal );

	bool OpenTextWindow( CGameTextSet *pTextSet );

	CGameTextSet *GetCurrentTextSetInTextWindow();
};

// =============================== inline implementations ===============================


inline void HUD_PlayerShip::Update( float  dt )
{
//	m_QuickMenuManager.Update( dt );
}


inline bool HUD_PlayerShip::HandleInput( int iActionCode, int input_type, float fVal )
{
//	return m_QuickMenuManager.HandleMouseInput( iActionCode, input_type, fVal );
	return false;
}

} // namespace amorphous



#endif  /*  __HUD_PLAYERSHIP_H__  */
