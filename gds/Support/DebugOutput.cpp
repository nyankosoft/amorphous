#include "DebugOutput.h"

#include "3DCommon/2DRect.h"
#include "3DCommon/Font.h"
#include "3DCommon/TextureFont.h"
#include "3DCommon/LogOutput_OnScreen.h"
#include "3DCommon/GraphicsResourceManager.h"
#include "GameCommon/Timer.h"
#include "Sound/SoundManager.h"

#include "Support/memory_helpers.h"
#include "Support/StringAux.h"
#include "Support/Profile.h"
#include "Support/Log/StateLog.h"

using namespace std;


class CFontFactory
{
public:

	/// returns a owned ref
	CFontBase* CreateFont( const std::string& font_name, int font_width, int font_height );
};


CFontBase* CFontFactory::CreateFont( const string& font_name, int font_width, int font_height )
{
	if( font_name.find(".dds") != string::npos
	 || font_name.find(".bmp") != string::npos
	 || font_name.find(".tga") != string::npos
	 || font_name.find(".jpg") != string::npos )
	{
		return new CTextureFont( font_name, font_width, font_height );
	}
	else
	{
		return new CFont( font_name, font_width, font_height );
	}
}



CDebugItem_Log::CDebugItem_Log( CLogOutput_OnScreen* pLogOutput )
{
//	m_pLogOutput = new CLogOutput_OnScreen()// "Texture/...", w, h, rows, chars );
	m_pLogOutput = pLogOutput;
}


void CDebugItem_Log::SetFont( CFontBase* pFont )
{
	CDebugItemBase::SetFont( pFont );
	if( m_pLogOutput )
        m_pLogOutput->SetBorrowedFont( m_pFont );
}


void CDebugItem_Log::Render()
{
	m_pLogOutput->Render();
}


void CDebugItem_Profile::Render()
{
	const vector<string>& rvecProfileText = GetProfileText();

	int num_text_rows = GetNumProfileTextRows();

	// get num text rows and columns to determine background rect size
	size_t num_max_chars = 0;
	size_t i, num_lines = num_text_rows;//rvecProfileText.size();
	for( i=0; i<num_lines; i++ )
	{
		if( num_max_chars < rvecProfileText[i].length() )
			num_max_chars = rvecProfileText[i].length();
	}

	// draw a background rect
	int font_width, font_height;
	m_pFont->GetFontSize( font_width, font_height );

	const D3DXVECTOR2 vLeftTop = m_vTopLeftPos;
	C2DRect rect( 
		vLeftTop,
		vLeftTop + D3DXVECTOR2( (float)(font_width * (num_max_chars + 2)), (float)(font_height * num_lines) ),
		0x80000000 );
	rect.Draw();

	// draw text
	D3DXVECTOR2 vPos = m_vTopLeftPos;
	CFontBase *pFont = m_pFont;
	for( i=0; i<num_lines; i++ )
	{
		pFont->DrawText( rvecProfileText[i].c_str(), vPos, 0xFFFFFFFF );
		vPos.y += font_height;
	}
}


void CDebugItem_StateLog::Render()
{
	CFontBase* pFont = m_pFont;

	float fLetterHeight = (float)pFont->GetFontHeight();
	Vector2 v2d = m_vTopLeftPos;

	int row, num_rows = StateLog.GetNumLogs();
	for( row=2; row<num_rows; row++ )
	{
		pFont->DrawText( StateLog.GetLog(row).c_str(), v2d );
		v2d.y += fLetterHeight;
	}
}


CDebugItem_ResourceManager::CDebugItem_ResourceManager()
{
	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
}


void CDebugItem_ResourceManager::Render()
{
	// set text info to the buffer
	GetTextInfo();

//	C2DRect rect( 745, 45, 1400, 700, 0x60000000 );
	C2DRect rect( RectLTWH(
		(int)m_vTopLeftPos.x,
		(int)m_vTopLeftPos.y,
		640,
		640 ),
		0x60000000 );

	rect.Draw();

	Vector2 vTextTopLeftPos = m_vTopLeftPos + Vector2(5,5);
	m_pFont->DrawText( m_TextBuffer, vTextTopLeftPos, 0xFFFFFFFF );
}


void CDebugItem_GraphicsResourceManager::GetTextInfo()
{
	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
	SoundManager().GetTextInfo( m_TextBuffer );
}


void CDebugItem_SoundManager::GetTextInfo()
{
	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );
}



//======================================================================================
// CDebugOutput
//======================================================================================


CSingleton<CGlobalDebugOutput> CGlobalDebugOutput::m_obj;


CDebugOutput::CDebugOutput( const std::string& font_name, int w, int h, U32 color )
: m_ItemIndex(0)
{
//	m_pFont = new CTextureFont( font_name, w, h );
//	m_pFont->SetColor( color );

	CFontFactory factory;
	m_pFont = factory.CreateFont( font_name, w, h );
	m_pFont->SetFontColor( color );
}


CDebugOutput::~CDebugOutput()
{
	Release();
}


void CDebugOutput::Release()
{
	SafeDelete( m_pFont );
	SafeDeleteVector( m_vecpDebugItem );
}


void CDebugOutput::RenderFPS()
{
	if( !m_pFont )
		return;

///	StateLog.Update( 0, "FPS: " + to_string(TIMER.GetFPS()) );
//	StateLog.Update( 1, "AVE. FPS: " + to_string(FPS.GetAverageFPS()) );
	string fps_text = "FPS: " + to_string(TIMER.GetFPS());

	U32 color = ( TIMER.GetFPS() < 40.0f ) ? 0xFFFF0000 /* fps low - red */ : 0xFF00FF00; /* green */

	m_pFont->DrawText( fps_text.c_str(), m_vTopLeftPos, color );
}


void CDebugOutput::Render()
{
	if( !m_bDisplay )
		return;

	const int index = m_ItemIndex;

	if( index < 0 || (int)m_vecpDebugItem.size() <= index )
		return;

	// 23:38 2007/10/07 commented out - background rect is rendered by each debug item object
//	m_BackgroundRect.Draw();

	RenderFPS();

	m_vecpDebugItem[index]->Render();
}


void CDebugOutput::AddDebugItem( const std::string& item_name, CDebugItemBase *pDebugItem )
{
	m_vecpDebugItem.push_back( pDebugItem );
	m_vecpDebugItem.back()->SetFont( m_pFont );
	m_vecpDebugItem.back()->SetName( item_name );
	m_vecpDebugItem.back()->SetTopLeftPos( m_vTopLeftPos );
}


void CDebugOutput::SetTopLeftPos( Vector2& vTopLeftPos )
{
	m_vTopLeftPos = vTopLeftPos;

	const int font_height = m_pFont ? m_pFont->GetFontHeight() : 0;

	// top left pos for each debug item (offset for FPS text row)
	Vector2 vItemTopLeftPos = m_vTopLeftPos;
	vItemTopLeftPos.y += font_height;

	size_t i, num_items = m_vecpDebugItem.size();
	for( i=0; i<num_items; i++ )
		m_vecpDebugItem[i]->SetTopLeftPos( vItemTopLeftPos );
}


bool CDebugOutput::ReleaseDebugItem( const std::string& item_name )
{
	size_t i, num_items = m_vecpDebugItem.size();
	for( i=0; i<num_items; i++ )
	{
		if( m_vecpDebugItem[i]->GetName() == item_name )
		{
			SafeDelete( m_vecpDebugItem[i] );
			m_vecpDebugItem.erase( m_vecpDebugItem.begin() + i );
			ClampIndex( m_ItemIndex, m_vecpDebugItem );
			return true;
		}
	}

	return false;
}


void CDebugOutput::SetDebugItemIndex( int index )
{
	m_ItemIndex = index;
}


void CDebugOutput::ReleaseGraphicsResources()
{
	m_pFont->Release();
}


void CDebugOutput::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_pFont->Reload();
}
