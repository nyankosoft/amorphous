#include "TextMessageManager.hpp"
#include "TextMessageRenderer.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/StringAux.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using boost::shared_ptr;


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





//========================================================================================
// CTextMessageManager
//========================================================================================

TextMessageSet &CTextMessageManager::GetMessageSet( int index )
{
	if( index < 0 || (int)m_vecTextMessageSet.size() <= index )
		LOG_PRINT( "An invalid index: " + to_string(index) );

	return m_vecTextMessageSet[index];
}


void CTextMessageManager::ProcessTextMessageRequests()
{
	vector<CTextMessageRequest>::iterator itr = m_vecTextMessageRequest.begin();
	while( itr != m_vecTextMessageRequest.end() )
	{
		if( m_ElapsedTimeMS <= itr->m_TimeMS
		 && ( m_ElapsedTimeMS <= itr->m_MaxTimeMS || !itr->m_StartAttempted ) )
		{
			// the scheduled time has passed
			// && ( max delay time has not passed yet || the message has never been displayed before )
			int mode = TextMessageBase::TYPE_IMMEDIATE;
			int res = StartTextMessage( itr->m_MessageIndex, mode, itr->m_Priority );

			itr->m_StartAttempted = true;

			if( res == TextMessageBase::REQ_ACCEPTED || !itr->m_Retry )
			{
				// discard the message
				// - Don't touch the iterator after this.
				itr = m_vecTextMessageRequest.erase( itr );
			}
			else
			{
				// keep the message
				itr++;
			}
		}
		else
			itr++;
	}
}


void CTextMessageManager::Update( float dt )
{
	// scoped mutex

	if( !m_vecTextMessageRequest.empty() )
		ProcessTextMessageRequests();

	m_pWindow->Update(dt);
}


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


int CTextMessageManager::StartTextMessage( int index, int mode, int priority )
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
		{
			TextMessageSet& current_msg_set   = GetMessageSet( m_pWindow->GetCurrentMessageSetIndex() );
			TextMessageSet& requested_msg_set = GetMessageSet( index );
			if( current_msg_set.m_Priority < requested_msg_set.m_Priority )
			{
				m_pWindow->UpdateTextMessageSet( index );
				return TextMessageBase::REQ_ACCEPTED;
			}
			else
				return TextMessageBase::REQ_REJECTED;
		}
		else
		{
			m_pWindow->UpdateTextMessageSet( index );
            return TextMessageBase::REQ_ACCEPTED;
		}

	default:
		return TextMessageBase::REQ_REJECTED;
	}
}


int CTextMessageManager::ScheduleTextMessage( int index, double delay, bool retry, double max_delay, int priority )
{
	// scoped mutex

	// override the text message priority if a valid priority is specified as the argument
	int real_priority
		= ( priority == -0xFFFF ) ? GetMessageSet( index ).m_Priority : priority;

	CTextMessageRequest req;
	req.m_MessageIndex = index;
	req.m_Priority     = real_priority;
	req.m_TimeMS       = m_ElapsedTimeMS + (U32)(delay * 1000.0);
	req.m_MaxTimeMS    = m_ElapsedTimeMS + (U32)(max_delay * 1000.0);
	req.m_Retry        = retry;

	m_vecTextMessageRequest.push_back( req );

	return 0;
}


/*
int CTextMessageManager::StartMessage( int mode )
{
}*/


int CTextMessageManager::StartLoadMessage( int priority )
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


} // namespace amorphous
