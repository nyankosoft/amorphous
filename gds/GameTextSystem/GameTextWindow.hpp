#ifndef  __GAMETEXTWINDOW_H__
#define  __GAMETEXTWINDOW_H__


#include <d3d9.h>
#include <d3dx9.h>

#include "3DMath/aabb2.hpp"

#include "Graphics/fwd.hpp"
#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/GraphicsComponentCollector.hpp"
#include "Graphics/TextureHandle.hpp"

#include "../Support/FixedVector.hpp"

/*
enum eGameTextWindowState
{
	GTW_CLOSED = 0,
	GTW_OPEN,
	GTW_WAITING_USER_RESPONSE,
};
*/

enum eGameTextWindowInput
{
	GTW_INPUT_NOINPUT = 0,
	GTW_INPUT_CANCEL,
	GTW_INPUT_OK,
};


#define NUM_MAX_ICONTEXTURES	64

#define NUM_MAX_TEXTWINDOW_LINES	16

class CGameTextComponent;
class CGameTextSet;
class CFont;

class CGameTextWindow : public CGraphicsComponent
{
//	bool m_bWaitingUserResponse;
	int m_iCurrentState;

	int m_iNumMaxLines;

	/// how many letters in one line (in bytes)
	int m_iNumMaxCharsPerLine;

	int m_iCurrentLine;

	float m_fNumCharsForCurrentLine;

	CGameTextComponent *m_apLine[NUM_MAX_TEXTWINDOW_LINES];

	/// text set currently being displayed
	CGameTextSet* m_pCurrentTextSet;

	/// a text component which is currently being handled
	int m_iCurrentComponent;

	/// bounding box of the window
	AABB2 m_BoundingRect;

	/// background rectangle of window
	C2DRect m_WindowRect;

	/// texture for window rectangle
	CTextureHandle m_WindowTexture;

	C2DRect m_TexturedIcon;

	TCFixedVector< CTextureHandle, NUM_MAX_ICONTEXTURES > m_IconTexture;

//	int m_iNumIconTextures;
//	char m_acTextureFilename[NUM_MAX_ICONTEXTURES][256];
//	LPDIRECT3DTEXTURE9 m_apTexture[NUM_MAX_ICONTEXTURES];

	CFontBase* m_pFont;
	std::string m_strFontName;
	float m_fFontWidth;
	float m_fFontHeight;


public:
	CGameTextWindow();
	~CGameTextWindow();

	void InitFont( const char *pcFontName, float fFontWidth, float fFontHeight );

	void SetNumMaxCharsPerLine( int num ) { m_iNumMaxCharsPerLine = num; }

	/// set a text set and initiate display
	void OpenTextWindow( CGameTextSet* pGameTextSet );

	void Render(int iInput);

	inline CGameTextSet* GetCurrentTextSet() { return m_pCurrentTextSet; }

	inline int GetCurrentState() { return m_iCurrentState; }

	enum eGameTextWindowState
	{
		STATE_CLOSED = 0,
		STATE_OPEN,
		STATE_WAITING_USER_RESPONSE,
		STATE_1,
		STATE_2
	};

	void UpdateScreenSize();

	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );

};


#endif		/*  __GAMETEXTWINDOW_H__  */