#ifndef  __TextMessageRenderer_H__
#define  __TextMessageRenderer_H__


#include <vector>
#include <string>

#include "3DCommon/fwd.h"
#include "3DCommon/2DRect.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "Support/memory_helpers.h"
#include "Graphics/Rect.h"
using namespace Graphics;

#define GTC_NUM_MAXLETTERS_PER_LINE 64


#define TEXTSET_INVALID_INDEX -1


/**
 Defines the interfaces to render messages.
 */
class CTextMessageRenderer : public CGraphicsComponent
{
//	CTextMessageWindow *m_pWindow;

protected:

	boost::shared_ptr<CAnimatedGraphicsManager> m_pEffectManager;

public:

	CTextMessageRenderer( boost::shared_ptr<CAnimatedGraphicsManager> pEffectMgr = boost::shared_ptr<CAnimatedGraphicsManager>() );

	virtual ~CTextMessageRenderer();

//	void Release();

	virtual void UpdateSpeaker( const char *pSpeaker ) {}

	virtual void UpdateText( const char *pText ) {}

	virtual void Update( float dt );

	virtual void Render() = 0;
};


/**
 Renders text messages and related graphical components.
 This is a sample implementation of CTextMessageRenderer
 */
class CDefaultTextMessageRenderer : public CTextMessageRenderer
{
	enum PrivateParams
	{
		MAX_TEXT_LENGTH = 256,
		NUM_MAX_ICONTEXTURES = 64,
	};

	enum FontTypes
	{
		FONT_SPEAKER,
		FONT_TEXT,
		NUM_FONTS
	};

	int m_aFontID[NUM_FONTS];

	/// holds window size in 800x600 resolution
	SRect m_BaseWindowRect;

	/// background rectangle of window
	C2DRect m_WindowRect;

	CGE_Rect *m_pWindowBGRect;

	CGE_Text *m_pText;

	CGE_Text *m_pSpeaker;

	/// texture for window rectangle
	CTextureHandle m_WindowTexture;

	C2DRect m_TexturedIcon;


//	TCFixedVector< CTextureHandle, NUM_MAX_ICONTEXTURES > m_IconTexture;

//	std::string m_strFontName[NUM_FONTS];

	SPoint m_BaseFontSize;

	/// buffer to hold speaker name
	char m_acSpeaker[MAX_TEXT_LENGTH-1];

	/// buffer to hold message text
    char m_acText[MAX_TEXT_LENGTH-1];

	SPoint m_vTextPos;
	SPoint m_vSpeakerPos;

public:

	CDefaultTextMessageRenderer( boost::shared_ptr<CAnimatedGraphicsManager> pEffectMgr,
		int top_layer,
		int bottom_layer );

	~CDefaultTextMessageRenderer();

	inline bool NoMessage();

	void Update( float dt ) {}

	void Render();

	void UpdateSpeaker( const char *pSpeaker );

	void UpdateText( const char *pText );

//	void UpdateScreenSize();
	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


inline bool CDefaultTextMessageRenderer::NoMessage()
{
	if( strlen(m_acSpeaker) == 0 && strlen(m_acText) == 0 )
		return true;
	else
		return false;
}


class CNullTextMessageRenderer : public CTextMessageRenderer
{

public:

	CNullTextMessageRenderer( boost::shared_ptr<CAnimatedGraphicsManager> pEffectMgr,
		int top_layer,
		int bottom_layer ) {}

	~CNullTextMessageRenderer() {}

	void Update( float dt ) {}

	void Render() {}

	void UpdateSpeaker( const char *pSpeaker ) {}

	void UpdateText( const char *pText ) {}

	void ReleaseGraphicsResources() {}
	void LoadGraphicsResources( const CGraphicsParameters& rParam ) {}
};


#endif		/*  __TextMessageRenderer_H__  */
