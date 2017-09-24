#ifndef  __TextMessageManager_H__
#define  __TextMessageManager_H__


#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "amorphous/base.hpp"
#include "fwd.hpp"


namespace amorphous
{


#define GTC_NUM_MAXLETTERS_PER_LINE 64


#define TEXTSET_INVALID_INDEX -1


/**
 * basic unit of text message
 */
class TextMessageBase
{
protected:

	float m_Wait;	///< how long the text will stay on the screen after displaying it [sec]

	int m_Priority;

public:

//	TextMessageBase() {}

	TextMessageBase( float waittime = 3.0f, int priority = 0 )
		:
	m_Wait(waittime),
	m_Priority(priority)
	{}

	virtual ~TextMessageBase() {}

	float GetWaitTime() const { return m_Wait; }

	virtual const char *GetSpeaker() = 0;

	virtual const char *GetText() = 0;

	enum ReqResult
	{
		REQ_ACCEPTED = 0,
		REQ_REJECTED,
		NUM_REQ_RESULTS
	};

	enum Type
	{
		TYPE_IMMEDIATE = 0,
		TYPE_QUEUED,
		NUM_TYPES
	};
};

/**
 reference to a text message unit
 Use this when you already has text data on some memory and don't need extra
 memory to copy the data to but only the reference to the data.
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
 Stores a copy of text message on its own memory
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
 Stores a collection of text message units
 */
class TextMessageSet
{
public:

	int m_Priority;

	/// Owned references
	std::vector< boost::shared_ptr<TextMessageBase> > m_vecpMessage;

public:

	TextMessageSet() : m_Priority(0) {}

	~TextMessageSet() {}
};


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

    CTextMessageRenderer *m_pRenderer;

private:

	void UpdateTextMessages( float dt );

public:

	CTextMessageWindow( CTextMessageManager *pManager );

	~CTextMessageWindow();

	void SetRenderer( CTextMessageRenderer *pRenderer );

//	void SetNumMaxCharsPerLine( int num ) { m_NumMaxCharsPerLine = num; }

//	void GetNumMaxCharsPerLine() const { return m_NumMaxCharsPerLine; }

	inline int GetCurrentState() const { return m_CurrentState; }

	int GetCurrentMessageSetIndex() const { return m_CurrentTextMsgSetIndex; }

    void UpdateTextMessageSet( int index );

	void Update( float dt );

	void Render();
};



class CTextMessageRequest
{
public:
	int m_MessageIndex;
	int m_Priority;
	ulong m_TimeMS;
	ulong m_MaxTimeMS;

	/// display of the text message was attempted or not
	bool m_StartAttempted;

	/// retry when the message was rejected until the max delay time passes
	bool m_Retry;

public:

	CTextMessageRequest()
		:
	m_StartAttempted(false)
	{}
};


/**
 * provides the user with interfaces to load text messages.
 * Stores a collection of text message sets
 */
class CTextMessageManager
{
	std::string m_Name;

	bool m_bLoadingTextMessage;

	std::vector<TextMessageSet> m_vecTextMessageSet;

	CTextMessageWindow *m_pWindow;

	enum eParams
	{
		NUM_DEFAULT_TEXT_MESSAGE_SETS = 64,
	};

	/// Don't use timer
	/// - rationale:
	///   Needs to synchronize with the time in stage
	///   Time in stage is paused, resumed and scaled,
	///   so supply the elased time of the stage directly
	/// - metric: milliseconds
	ulong m_ElapsedTimeMS;

	std::vector<CTextMessageRequest> m_vecTextMessageRequest;

private:

	void ProcessTextMessageRequests();

public:

//	CTextMessageManager();

	CTextMessageManager( const std::string& name = "" );

	~CTextMessageManager();

	void SetRenderer( CTextMessageRenderer* pRenderer );

//	void Release();

	const std::string& GetName() const { return m_Name; }

//	void SetName( const std::string& name ) const { m_Name = name; }

	/// takes request to display registered text message
	/// returns result for the request
	int StartTextMessage( int index, int mode, int priority );

	int ScheduleTextMessage( int index,
		                     double delay,
							 bool retry = false,
							 double max_delay = 0.0,
							 int priority = -0xFFFF );

	/// returns index to the registered text message set
	/// TODO: support MT
	/// - Caller is responsible for saving the returned index to specify the message later.
	int StartLoadMessage( int priority );

	void AddMessageRef( const char *speaker, const char *message, float waittime = -1.0f );

	void EndLoadMessage() {}

	// TODO: provide safe accesss to vector elements
	TextMessageSet &GetMessageSet( int index );

	void Update( float dt );

	void Render() { m_pWindow->Render(); }

	void SetElapsedTimeMS( ulong elapsed_time ) { m_ElapsedTimeMS = elapsed_time; }

	void SetElapsedTime( double elapsed_time ) { SetElapsedTimeMS( (ulong)(elapsed_time * 1000.0) ); }
};


class CTextMessageManagerHub
{
//	std::vector< boost::shared_ptr<TextMessageManager> > m_vecpTextMessageManager;

//	std::map< std::string, boost::shared_ptr<CTextMessageManager> > m_mapNameToTextMessageManager;
	std::map< std::string, CTextMessageManager * > m_mapNameToTextMessageManager;

public:

	CTextMessageManagerHub() {}

	void Attach( CTextMessageManager *pMgr )
	{
		m_mapNameToTextMessageManager[pMgr->GetName()] = pMgr;
	}

	void Detach( CTextMessageManager *pMgr )
	{
//		std::map< std::string, boost::shared_ptr<TextMessageManager> >::iterator itr
		std::map< std::string, CTextMessageManager * >::iterator itr
			= m_mapNameToTextMessageManager.find( pMgr->GetName() );

		if( itr != m_mapNameToTextMessageManager.end() )
			m_mapNameToTextMessageManager.erase( itr );
	}

	CTextMessageManager *GetTextMessageManager( const std::string& name )
	{
		std::map< std::string, CTextMessageManager * >::iterator itr
			= m_mapNameToTextMessageManager.find( name );

		if( itr == m_mapNameToTextMessageManager.end() )
			return NULL; // not found
		else
			return itr->second;
	}
};


inline CTextMessageManagerHub& TextMessageManagerHub()
{
	static CTextMessageManagerHub s_obj;
	return s_obj;
}

} // namespace amorphous



#endif		/*  __TextMessageManager_H__  */
