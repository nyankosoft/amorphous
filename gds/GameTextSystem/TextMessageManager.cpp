#include "TextMessageManager.hpp"
#include "TextMessageRenderer.hpp"

#include "../Graphics/Font/Font.hpp"
#include "../Support/memory_helpers.hpp"
#include "../Support/Log/DefaultLog.hpp"
#include "../Support/StringAux.hpp"


using namespace std;
using namespace boost;


/**
 * text render manager is created in ctor
 */
CTextMessageWindow::CTextMessageWindow( CTextMessageManager *pManager )
:
m_pRenderer(NULL),
m_pManager(pManager)
{
//	m_pRenderer = new CTextMessageRenderManager;

	m_CurrentTextMsgSetIndex = -1;

	m_CurrentMessageUnit = 0;

	m_WaitTimeForCurrent = 0.0f;
}


CTextMessageWindow::~CTextMessageWindow()
{
	SafeDelete( m_pRenderer );
}


void CTextMessageWindow::SetRenderer( CTextMessageRenderer *pRenderer )
{
	SafeDelete(m_pRenderer);
	m_pRenderer = pRenderer;
}


void CTextMessageWindow::UpdateTextMessageSet( int index )
{
	m_CurrentTextMsgSetIndex = index;
	m_CurrentMessageUnit = 0;
	m_WaitTimeForCurrent = 0;

	// set the first message unit
	TextMessageSet& msgset = m_pManager->GetMessageSet( m_CurrentTextMsgSetIndex );
//	TextMessageBase& msgtext = *(msgset.m_vecpMessage[m_CurrentMessageUnit]);
	shared_ptr<TextMessageBase> pTextMsg = msgset.m_vecpMessage[m_CurrentMessageUnit];

	m_pRenderer->UpdateSpeaker( pTextMsg->GetSpeaker() );
	m_pRenderer->UpdateText( pTextMsg->GetText() );

//	TextMessageSet& msgset = m_pManager->GetMessageSet( index )
//	m_pRenderer->UpdateSpeaker( msgset.GetSpeaker() )
//	m_pRenderer->UpdateSpeaker( msgset.GetText() )
}


void CTextMessageWindow::UpdateTextMessages( float dt )
{
	if( m_CurrentTextMsgSetIndex < 0 )
		return;	// no text message is supposed to be displayed right now

	TextMessageSet& msgset = m_pManager->GetMessageSet( m_CurrentTextMsgSetIndex );
	TextMessageBase& msgtext = *(msgset.m_vecpMessage[m_CurrentMessageUnit]);

	m_WaitTimeForCurrent += dt;

	if( msgtext.GetWaitTime() < m_WaitTimeForCurrent )
	{
		// display the next unit of text message
		m_CurrentMessageUnit++;

		if( m_CurrentMessageUnit == msgset.m_vecpMessage.size() )
		{
			// no more text message unit
			m_CurrentTextMsgSetIndex = -1;
			m_CurrentMessageUnit = 0;
			m_pRenderer->UpdateSpeaker( "" );
			m_pRenderer->UpdateText( "" );
			m_pRenderer->OnTextMessageCleared();
			return;
		}

		m_WaitTimeForCurrent -= msgtext.GetWaitTime();

		TextMessageBase& next_msgtext = *(msgset.m_vecpMessage[m_CurrentMessageUnit]);

		m_pRenderer->UpdateSpeaker( next_msgtext.GetSpeaker() );
		m_pRenderer->UpdateText( next_msgtext.GetText() );
	}
}


void CTextMessageWindow::Update( float dt )
{
	UpdateTextMessages( dt );

	m_pRenderer->Update( dt );
}


void CTextMessageWindow::Render()
{
//	m_pRenderer->Render();
}




TextMessageSet &CTextMessageManager::GetMessageSet( int index )
{
	if( index < 0 || (int)m_vecTextMessageSet.size() <= index )
		LOG_PRINT( "An invalid index: " + to_string(index) );

	return m_vecTextMessageSet[index];
}



//========================================================================================
// CTextMessageManager
//========================================================================================

//CTextMessageManager::CTextMessageManager()
CTextMessageManager::CTextMessageManager( const std::string& name )
:
m_Name(name),
m_bLoadingTextMessage(false)
{
	m_pWindow = new CTextMessageWindow( this );

	SetRenderer( new CNullTextMessageRenderer(shared_ptr<CAnimatedGraphicsManager>(),0,0) );

	m_vecTextMessageSet.reserve( NUM_DEFAULT_TEXT_MESSAGE_SETS );


	// register to text message manager hub
	TextMessageManagerHub().Attach( this );
}


CTextMessageManager::~CTextMessageManager()
{
	SafeDelete( m_pWindow );
}


void CTextMessageManager::SetRenderer( CTextMessageRenderer* pRenderer )
{
	m_pWindow->SetRenderer( pRenderer );
}


int CTextMessageManager::StartTextMessage( int index, int mode )
{
	if( index < 0 )
		return TextMessageBase::REQ_REJECTED;	// invalid index

	if( m_bLoadingTextMessage )
		return TextMessageBase::REQ_REJECTED;

	LOG_PRINT( string("text message requested (index: ") + to_string(index) + string(")") );

	switch( mode )
	{
	case TextMessageBase::TYPE_IMMEDIATE:
//		m_bLoadingTextMessage = true;
		if( 0 <= m_pWindow->GetCurrentMessageSetIndex() )
            return TextMessageBase::REQ_REJECTED;
		else
		{
			m_pWindow->UpdateTextMessageSet( index );
            return TextMessageBase::REQ_ACCEPTED;
		}

	default:
		return TextMessageBase::REQ_REJECTED;
	}
}

/*
int CTextMessageManager::StartMessage( int mode )
{
}*/


int CTextMessageManager::StartLoadMessage()
{
	int index = (int)m_vecTextMessageSet.size();

	m_vecTextMessageSet.push_back( TextMessageSet() );

	LOG_PRINT( fmt_string("Starting to load a text message... (index:%d)",index) );

	if( 50 <= index )
	{
		int break_here = 1;
	}

	return index;
}


void CTextMessageManager::AddMessageRef( const char *speaker, const char *message, float waittime )
{
	if( waittime < 0.0f )
		waittime = 3.0f;

	m_vecTextMessageSet.back().m_vecpMessage.push_back( shared_ptr<TextMessageRef>( new TextMessageRef( speaker, message, waittime ) ) );

	LOG_PRINT( string("Added a text message reference: ") + string(message) );
}


/*
void CTextMessageManager::EndMessage()
{
	m_bLoadingTextMessage = false;

	m_pTextMessageWindow->DisplayTextMessage( m_vecTextMessageSet.size() - 1 );
//	m_pTextMessageWindow->DisplayText( m_vecTextMessageSet.back() );
}*/
