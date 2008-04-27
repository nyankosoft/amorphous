#ifndef  __TextMessageManager_H__
#define  __TextMessageManager_H__


#include <vector>
#include <string>

#include "3DCommon/2DRect.h"
#include "3DCommon/TextureHandle.h"
#include "3DCommon/GraphicsComponentCollector.h"
#include "Support/memory_helpers.h"


#include "Graphics/Rect.h"
using namespace Graphics;

#define GTC_NUM_MAXLETTERS_PER_LINE 64


#define TEXTSET_INVALID_INDEX -1

class CFontBase;


/**
 * basic unit of text message
 */
class TextMessageBase
{
protected:

	float m_Wait;	///< how long the text will stay on the screen after displaying it [sec]

public:

//	TextMessageBase() {}
	TextMessageBase( float waittime ) : m_Wait(waittime) {}
	virtual ~TextMessageBase() {}

	float GetWaitTime() const { return m_Wait; }
	virtual const char *GetSpeaker() = 0;
	virtual const char *GetText() = 0;

	enum eReqResult
	{
		REQ_ACCEPTED = 0,
		REQ_REJECTED
	};

	enum eMode
	{
		MODE_IMMEDIATE = 0,
	};
};

/**
 * reference to a text message unit
 * meant to be used when the user has already loaded text message on memory
 * and doesn't want copy operation
 */
class TextMessageRef : public TextMessageBase
{
	const char *m_pSpeaker;
	const char *m_pText;

public:

	TextMessageRef( const char *speaker, const char *text, float waittime )
		: m_pSpeaker(speaker), m_pText(text), TextMessageBase(waittime) {}

	virtual const char *GetSpeaker() { return m_pSpeaker; }
	virtual const char *GetText() { return m_pText; }
};

/**
 * stores a copy of text message in its own memory
 */
class TextMessage : public TextMessageBase
{
	std::string m_strSpeaker;
	std::string m_strText;

public:

	// error - base class member 'm_Wait' cannot be initialized here
//	TextMessage( const std::string& speaker, const std::string& text, float waittime )
//		: m_strSpeaker(speaker), m_strText(text), m_Wait(waittime) {}

	TextMessage( const std::string& speaker, const std::string& text, float waittime )
		: m_strSpeaker(speaker), m_strText(text), TextMessageBase(waittime) {}

	virtual const char *GetSpeaker() { return m_strSpeaker.c_str(); }
	virtual const char *GetText() { return m_strText.c_str(); }
};


/**
 * stores a collection of text message units
 */
class TextMessageSet
{
public:
	std::vector<TextMessageBase *> m_vecpMessage;

	TextMessageSet() {}
	~TextMessageSet() { SafeDeleteVector( m_vecpMessage ); }
};


/**
 * renders text messages and related graphical components
 */
class CTextMessageRenderManager : public CGraphicsComponent
{
	enum PrivateParams
	{
		MAX_TEXT_LENGTH = 256,
		NUM_MAX_ICONTEXTURES = 64,
	};

	enum FontTypes
	{
		SPEAKER,
		TEXT,
		NUM_FONTS
	};

	/// holds window size in 800x600 resolution
	SRect m_BaseWindowRect;

	/// background rectangle of window
	C2DRect m_WindowRect;

	/// texture for window rectangle
	CTextureHandle m_WindowTexture;

	C2DRect m_TexturedIcon;


//	TCFixedVector< CTextureHandle, NUM_MAX_ICONTEXTURES > m_IconTexture;

	CFontBase* m_apFont[NUM_FONTS];
	std::string m_strFontName[NUM_FONTS];

	SPoint m_BaseFontSize;

	/// buffer to hold speaker name
	char m_acSpeaker[MAX_TEXT_LENGTH-1];

	/// buffer to hold message text
    char m_acText[MAX_TEXT_LENGTH-1];

	SPoint m_vTextPos;
	SPoint m_vSpeakerPos;

public:

	CTextMessageRenderManager();
	~CTextMessageRenderManager();

	inline bool NoMessage();

	void Render();

	void UpdateSpeaker( const char *pSpeaker );

	void UpdateText( const char *pText );

	void UpdateScreenSize();
	void ReleaseGraphicsResources();
	void LoadGraphicsResources( const CGraphicsParameters& rParam );
};


inline bool CTextMessageRenderManager::NoMessage()
{
	if( strlen(m_acSpeaker) == 0 && strlen(m_acText) == 0 )
		return true;
	else
		return false;
}



class CTextMessageManager;


class CTextMessageWindow
{
//	bool m_bWaitingUserResponse;
	int m_CurrentState;

	/// how many letters in one line (in bytes)
//	int m_NumMaxCharsPerLine;

	/// text set currently being displayed
	int m_CurrentTextMsgSetIndex;

	int m_CurrentMessageUnit;

	float m_WaitTimeForCurrent;

	/// bounding box of the window
//	SRect m_BoundingRect;

	CTextMessageManager *m_pManager;

    CTextMessageRenderManager *m_pRenderManager;

public:

	CTextMessageWindow( CTextMessageManager *pManager );
	~CTextMessageWindow();

//	void SetNumMaxCharsPerLine( int num ) { m_NumMaxCharsPerLine = num; }

//	void GetNumMaxCharsPerLine() const { return m_NumMaxCharsPerLine; }

	inline int GetCurrentState() const { return m_CurrentState; }

	int GetCurrentMessageSetIndex() const { return m_CurrentTextMsgSetIndex; }

    void UpdateTextMessageSet( int index );

	void Update( float dt );

	void Render() { m_pRenderManager->Render(); }
};


/**
 * provides the user with interfaces to load text messages
 * stores a collection of text message sets
 */
class CTextMessageManager
{
	bool m_bLoadingTextMessage;

	std::vector<TextMessageSet> m_vecTextMessageSet;

	CTextMessageWindow *m_pWindow;

	enum eParams
	{
		NUM_DEFAULT_TEXT_MESSAGE_SETS = 64,
	};

public:

	CTextMessageManager();
	~CTextMessageManager();

//	void Release();

	/// takes request to display registered text message
	/// returns result for the request
	int StartTextMessage( int index, int mode );

	/// returns index to the registered text message set
	/// TODO: support MT
	int StartLoadMessage();

	void AddMessageRef( const char *speaker, const char *message, float waittime = -1.0f );

	void EndLoadMessage() {}

	// TODO: provide safe accesss to vector elements
	TextMessageSet &GetMessageSet( int index );

	void Update( float dt ) { m_pWindow->Update(dt); }

	void Render() { m_pWindow->Render(); }
};

/*
inline CTextMessageManager& GetTextMessageManager()
{
	// Using an accessor function gives control of the construction order
	static CTextMessageManager obj;
	return obj;
}
*/


#endif		/*  __TextMessageManager_H__  */
