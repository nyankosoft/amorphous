#ifndef __HUD_PLAYERBASE_H__
#define __HUD_PLAYERBASE_H__

#include <string>

#include "gds/Graphics/GraphicsComponentCollector.hpp"
#include "gds/Graphics/GraphicsEffectManager.hpp"
#include "gds/GameCommon/HUD_TimerDisplay.hpp"


namespace amorphous
{


class CGameTextWindow;
class CGameTextSet;
class CSubDisplay;


class HUD_PlayerBase : public CGraphicsComponent
{
protected:

	boost::shared_ptr<CAnimatedGraphicsManager> m_pGraphicsEffectManager;

	/// show / hide the HUD
	bool m_bShow;

	/// deprecated
	/// - Use text elements (CTextElement) instead.
	HUD_TimerDisplay m_TimerDisplay;

public:

	enum Type
	{
		TYPE_GENERAL,
		TYPE_AIRCRAFT,
		NUM_TYPES
	};

	HUD_PlayerBase() : m_bShow(true) {}

	virtual ~HUD_PlayerBase() {}

	virtual int GetType() const = 0;

	virtual void Init() {}

	inline void Render();

	/// Derived classes implement this to render HUD components
	/// - Deprecated. All the components should be rendered as graphics elements with CGraphicsElementManager
	/// - Called after all the graphics elements are rendered
	virtual void RenderImpl() {};

	virtual CFontBase *GetFont() { return NULL; }

	/// Must be called by derived classes if overridden
	inline virtual void Update( float dt );

	void Show() { m_bShow = true; }
	void Hide() { m_bShow = false; }
	void ToggleShowHide() { m_bShow = (!m_bShow); }

	virtual bool HandleInput( int iActionCode, int input_type, float fVal ) { return true; }

	virtual HUD_TimerDisplay& TimerDisplay() { return m_TimerDisplay; }

	virtual CSubDisplay *GetSubDisplay() { return NULL; }

	virtual bool LoadGlobalMapTexture( const std::string& texture_filename ) { return false; }

	virtual bool OpenTextWindow( CGameTextSet *pTextSet ) { return false; }
	virtual CGameTextSet *GetCurrentTextSetInTextWindow() { return NULL; }

	void SetGraphicsEffectManager( boost::shared_ptr<CAnimatedGraphicsManager> pEffectMgr ) { m_pGraphicsEffectManager = pEffectMgr; }

	virtual void CreateRenderTasks() {}
};


//----------------------------------- inline implementations -----------------------------------

inline void HUD_PlayerBase::Render()
{
	if( !m_bShow )
		return;

	m_pGraphicsEffectManager->GetGraphicsElementManager()->Render();

	RenderImpl();
}


inline void HUD_PlayerBase::Update( float dt )
{
	if( m_pGraphicsEffectManager )
		m_pGraphicsEffectManager->UpdateEffects( dt );
}

} // namespace amorphous



#endif  /*  __HUD_PLAYERBASE_H__  */
