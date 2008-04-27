#ifndef __HUD_PLAYERBASE_H__
#define __HUD_PLAYERBASE_H__

#include <string>

#include "3DCommon/GraphicsComponentCollector.h"

#include "GameCommon/HUD_TimerDisplay.h"


class CGameTextWindow;
class CGameTextSet;
class CFontBase;
class HUD_SubDisplay;


class HUD_PlayerBase : public CGraphicsComponent
{
protected:

	HUD_TimerDisplay m_TimerDisplay;

public:

	enum eType
	{
		TYPE_GENERAL,
		TYPE_AIRCRAFT,
		NUM_TYPES
	};

	HUD_PlayerBase() {}
	virtual ~HUD_PlayerBase() {}

	virtual int GetType() const = 0;

	virtual void Init() {}

	virtual void Render() = 0;

	virtual CFontBase *GetFont() { return NULL; }

	virtual void Update( float dt ) {}

	virtual bool HandleInput( int iActionCode, int input_type, float fVal ) { return true; }

	virtual HUD_TimerDisplay& TimerDisplay() { return m_TimerDisplay; }

	virtual HUD_SubDisplay *GetSubDisplay() { return NULL; }

	virtual bool LoadGlobalMapTexture( const std::string& texture_filename ) { return false; }

	virtual bool OpenTextWindow( CGameTextSet *pTextSet ) { return false; }
	virtual CGameTextSet *GetCurrentTextSetInTextWindow() { return NULL; }

	virtual void CreateRenderTasks() {}
};






class HUD_PlayerPAC : public HUD_PlayerBase
{

public:

	void Init() {}

	void Render() {}

};





#endif  /*  __HUD_PLAYERBASE_H__  */