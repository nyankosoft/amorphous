#include "TextMessageManager.h"

#include <stdlib.h>
#include "../3DCommon/font.h"
#include "../Support/memory_helpers.h"
#include "../Support/Log/DefaultLog.h"
#include "../Support/StringAux.h"
#include "../Support/MsgBox.h"

using namespace std;


CTextMessageRenderManager::CTextMessageRenderManager()
{
	// set graphics components in 800x600 resolution
	m_BaseFontSize = SPoint( 12, 24 );

	m_strFontName[TEXT] = "‚l‚r ‚oƒSƒVƒbƒN";
	m_apFont[TEXT] = new CFont;

	m_strFontName[SPEAKER] = "Arial";
	m_apFont[SPEAKER] = new CFont;

	m_BaseWindowRect.SetPositionLTWH( 120, 32, 560, 70 );

	LoadGraphicsResources( GetCurrentGraphicsParams() );

	memset( m_acText,    0, sizeof(m_acText) );
	memset( m_acSpeaker, 0, sizeof(m_acSpeaker) );
}


CTextMessageRenderManager::~CTextMessageRenderManager()
{
	ReleaseGraphicsResources();

	for( int i=0; i<NUM_FONTS; i++ )
        SafeDelete( m_apFont[i] );
}


void CTextMessageRenderManager::Render()
{
	if( NoMessage() )
		return;

	m_WindowRect.Draw();

	m_apFont[TEXT]->DrawText( m_acText, m_vTextPos.x, m_vTextPos.y, 0xFFC0C0C0 );

	m_apFont[SPEAKER]->DrawText( m_acSpeaker, m_vSpeakerPos.x, m_vSpeakerPos.y, 0xFF60C0FF );
}


void CTextMessageRenderManager::UpdateSpeaker( const char *pSpeaker )
{
	if( strlen(pSpeaker) <= MAX_TEXT_LENGTH )
		strcpy( m_acSpeaker, pSpeaker );
	else
	{
        strncpy( m_acSpeaker, pSpeaker, MAX_TEXT_LENGTH );
		m_acSpeaker[MAX_TEXT_LENGTH-1] = '\0';
	}
}


void CTextMessageRenderManager::UpdateText( const char *pText )
{
	if( strlen(pText) <= MAX_TEXT_LENGTH )
		strcpy( m_acText, pText );
	else
	{
        strncpy( m_acText, pText, MAX_TEXT_LENGTH );
		m_acText[MAX_TEXT_LENGTH-1] = '\0';
	}
}


void CTextMessageRenderManager::UpdateScreenSize()
{
}


void CTextMessageRenderManager::ReleaseGraphicsResources()
{
//	m_WindowTexture.Release();

	for( int i=0; i<NUM_FONTS; i++ )
		m_apFont[i]->Release();
}


void CTextMessageRenderManager::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	float screen_width = (float)GetScreenWidth();
	float factor = (float)(screen_width / 800.0f);

//	m_WindowTexture.Load();

	SPoint font_size_spkr = m_BaseFontSize * ( factor * 0.5f );
	SPoint font_size_text = m_BaseFontSize * factor;
	
	PrintLog( string("TextMessafeManager - font size") +  to_string(font_size_text.x) + to_string(font_size_text.y) );

/*	char buffer[256];
	sprintf( buffer, "TextMessafeManager - font size: %d x %d, factor: %f, screen_width: %d",
		font_size_text.x, font_size_text.y, factor, screen_width );

	MessageBox( NULL, buffer, "msg", MB_OK|MB_ICONWARNING );
*/
	// TODO: support texture font
	((CFont *)m_apFont[SPEAKER])->InitFont( m_strFontName[SPEAKER].c_str(), font_size_spkr.x, font_size_spkr.y );
	((CFont *)m_apFont[TEXT])->InitFont( m_strFontName[TEXT].c_str(), font_size_text.x, font_size_text.y );

    SRect rect = m_BaseWindowRect * factor;

	m_WindowRect.SetPositionLTRB( rect.left, rect.top, rect.right, rect.bottom );

	m_WindowRect.SetColor( 0x50000000 );	// a translucent, black rectangle

	m_vTextPos		= SPoint( 130, 48 ) * factor;
	m_vSpeakerPos	= SPoint( 130, 34 ) * factor;
}


/**
 * text render manager is created in ctor
 */
CTextMessageWindow::CTextMessageWindow( CTextMessageManager *pManager )
: m_pManager(pManager)
{
	m_pRenderManager = new CTextMessageRenderManager;

	m_CurrentTextMsgSetIndex = -1;

	m_CurrentMessageUnit = 0;

	m_WaitTimeForCurrent = 0.0f;
}


CTextMessageWindow::~CTextMessageWindow()
{
	SafeDelete( m_pRenderManager );
}


void CTextMessageWindow::UpdateTextMessageSet( int index )
{
	m_CurrentTextMsgSetIndex = index;
	m_CurrentMessageUnit = 0;
	m_WaitTimeForCurrent = 0;

	// set the first message unit
	TextMessageSet& msgset = m_pManager->GetMessageSet( m_CurrentTextMsgSetIndex );
//	TextMessageBase& msgtext = *(msgset.m_vecpMessage[m_CurrentMessageUnit]);
	TextMessageBase* pTextMsg = msgset.m_vecpMessage[m_CurrentMessageUnit];

	m_pRenderManager->UpdateSpeaker( pTextMsg->GetSpeaker() );
	m_pRenderManager->UpdateText( pTextMsg->GetText() );

//	TextMessageSet& msgset = m_pManager->GetMessageSet( index )
//	m_pRenderManager->UpdateSpeaker( msgset.GetSpeaker() )
//	m_pRenderManager->UpdateSpeaker( msgset.GetText() )
}


void CTextMessageWindow::Update( float dt )
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
			m_pRenderManager->UpdateSpeaker( "" );
			m_pRenderManager->UpdateText( "" );
			return;
		}

		m_WaitTimeForCurrent -= msgtext.GetWaitTime();

		TextMessageBase& next_msgtext = *(msgset.m_vecpMessage[m_CurrentMessageUnit]);

		m_pRenderManager->UpdateSpeaker( next_msgtext.GetSpeaker() );
		m_pRenderManager->UpdateText( next_msgtext.GetText() );
	}
}





TextMessageSet &CTextMessageManager::GetMessageSet( int index )
{
	if( index < 0 || m_vecTextMessageSet.size() <= index )
		PrintLog( "CTextMessageManager::GetMessageSet() - invalid index" );

	return m_vecTextMessageSet[index];
}


CTextMessageManager::CTextMessageManager()
:m_bLoadingTextMessage(false)
{
	m_pWindow = new CTextMessageWindow( this );

	m_vecTextMessageSet.reserve( NUM_DEFAULT_TEXT_MESSAGE_SETS );
}


CTextMessageManager::~CTextMessageManager()
{
	SafeDelete( m_pWindow );
}


int CTextMessageManager::StartTextMessage( int index, int mode )
{
	if( index < 0 )
		return TextMessageBase::REQ_REJECTED;	// invalid index

	if( m_bLoadingTextMessage )
		return TextMessageBase::REQ_REJECTED;

	PrintLog( string("text message requested (index: ") + to_string(index) + string(")") );

	switch( mode )
	{
	case TextMessageBase::MODE_IMMEDIATE:
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

	string str = string("CTextMessageManager::StartLoadMessage() -  text message loading start (index: ")
		+ to_string(index) + string(").");

	PrintLog( str );
//	MsgBox( str.c_str() );

	return index;
}


void CTextMessageManager::AddMessageRef( const char *speaker, const char *message, float waittime )
{
	if( waittime < 0.0f )
		waittime = 3.0f;

	m_vecTextMessageSet.back().m_vecpMessage.push_back( new TextMessageRef( speaker, message, waittime ) );

	PrintLog( string("added text message reference: ") + string(message) );
}


/*
void CTextMessageManager::EndMessage()
{
	m_bLoadingTextMessage = false;

	m_pTextMessageWindow->DisplayTextMessage( m_vecTextMessageSet.size() - 1 );
//	m_pTextMessageWindow->DisplayText( m_vecTextMessageSet.back() );
}*/