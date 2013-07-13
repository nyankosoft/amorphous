#include "DebugOutput.hpp"

#include "Graphics/2DPrimitive/2DRect.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Graphics/GraphicsResourceManager.hpp"
#include "Sound/SoundManager.hpp"
#include "Input/InputDevice.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/StringAux.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/StateLog.hpp"


namespace amorphous
{

using namespace std;


DebugInfo_Log::DebugInfo_Log( LogOutput_ScrolledTextBuffer* pLogOutput )
:
m_pLogOutput(pLogOutput)
{
//	m_pLogOutput = new LogOutput_ScrolledTextBuffer()// "Texture/...", w, h, rows, chars );
}


void DebugInfo_Log::UpdateDebugInfoText()
{
/*	FontBase *pFont = m_pFont;

	const int font_width  = pFont->GetFontWidth();
	const int font_height = pFont->GetFontHeight();*/
//	const int num_rows = ???;
	const int num_current_rows = m_pLogOutput->GetNumCurrentRows();

	// draw rect for background
//	const int sx = (int)m_vTopLeftPos.x;
//	const int sy = (int)m_vTopLeftPos.y;
//	C2DRect bg_rect( sx, sy, sx + font_width * 80/*m_NumChars*/, sy + font_height * num_current_rows, 0x80000000 );
//	bg_rect.Draw();


//	Vector2 vTextPos = m_vTopLeftPos;
	SFloatRGBAColor color;
	for( int i=0; i<num_current_rows; i++ )//, vTextPos.y += font_height )
	{
		const char *pText = m_pLogOutput->GetText( i );
		const U32 argb_color   = m_pLogOutput->GetTextColor( i );

//		pFont->DrawText( pText, vTextPos, color );

		color.SetARGB32( argb_color );
		m_MultiLineTextBuffer.push_back( pText );
		m_LineColors.push_back( color );
	}
}


void DebugInfo_Profile::UpdateDebugInfoText()
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
//	int font_width, font_height;
//	m_pFont->GetFontSize( font_width, font_height );

//	const Vector2 vLeftTop = m_vTopLeftPos;
//	C2DRect rect( 
//		vLeftTop,
//		vLeftTop + Vector2( (float)(font_width * (num_max_chars + 2)), (float)(font_height * num_lines) ),
//		0x80000000 );
//	rect.Draw();

	// draw text
//	Vector2 vPos = m_vTopLeftPos;
//	FontBase *pFont = m_pFont;
	m_MultiLineTextBuffer.resize( 0 );
	for( i=0; i<num_lines; i++ )
	{
//		pFont->DrawText( rvecProfileText[i].c_str(), vPos, 0xFFFFFFFF );
//		vPos.y += font_height;
		m_MultiLineTextBuffer.push_back( rvecProfileText[i] );
	}
}


void DebugInfo_StateLog::UpdateDebugInfoText()
{
//	FontBase* pFont = m_pFont;

//	float fLetterHeight = (float)pFont->GetFontHeight();
//	Vector2 v2d = m_vTopLeftPos;

	m_MultiLineTextBuffer.resize( 0 );
	int row, num_rows = StateLog.GetNumLogs();
	for( row=2; row<num_rows; row++ )
	{
//		pFont->DrawText( StateLog.GetLog(row).c_str(), v2d );
//		v2d.y += fLetterHeight;
		m_MultiLineTextBuffer.push_back( StateLog.GetLog(row) );
	}
}



//======================================================================================
// CDebugOutput
//======================================================================================


singleton<CGlobalDebugOutput> CGlobalDebugOutput::m_obj;


CDebugOutput::CDebugOutput()
:
m_ItemIndex(0)
{
}


CDebugOutput::~CDebugOutput()
{
	Release();
}


void CDebugOutput::Release()
{
	SafeDeleteVector( m_vecpDebugItem );
}


void CDebugOutput::Render()
{
	if( !m_bDisplay )
		return;

	if( !m_pRenderer )
		return;

	const int index = m_ItemIndex;

	if( index < 0 || (int)m_vecpDebugItem.size() <= index )
		return;

	if( m_vecpDebugItem[index] )
	{
		DebugInfo& debug_info = *m_vecpDebugItem[index];

		debug_info.UpdateDebugInfoText();
	
		if( 0 < debug_info.GetTextBuffer().length() )
		{
			m_pRenderer->RenderDebugInfo( debug_info.GetTextBuffer() );
		}
		else
		{
			m_pRenderer->RenderDebugInfo( debug_info.GetMultiLineTextBuffer(), debug_info.GetMultiLineTextColors() );
		}
	}
}


void CDebugOutput::AddDebugItem( const std::string& item_name, DebugInfo *pDebugItem )
{
	m_vecpDebugItem.push_back( pDebugItem );
	m_vecpDebugItem.back()->SetName( item_name );

	// TODO: add offset for fps display
//	m_vecpDebugItem.back()->SetTopLeftPos( m_vTopLeftPos );
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


bool CDebugOutput::SetDebugItem( const std::string& item_name )
{
	size_t i, num_items = m_vecpDebugItem.size();
	for( i=0; i<num_items; i++ )
	{
		if( m_vecpDebugItem[i]->GetName() == item_name )
		{
			SetDebugItemIndex( (int)i );
			return true;
		}
	}

	return false;
}


} // namespace amorphous
