#include "DebugOutput.hpp"

#include "amorphous/Graphics/LogOutput_OnScreen.hpp"
#include "amorphous/Support/memory_helpers.hpp"
#include "amorphous/Support/StringAux.hpp"
#include "amorphous/Support/Profile.hpp"
#include "amorphous/Support/Log/StateLog.hpp"


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
	const int num_current_rows = m_pLogOutput->GetNumCurrentRows();

	SFloatRGBAColor color;
	for( int i=0; i<num_current_rows; i++ )
	{
		const char *pText = m_pLogOutput->GetText( i );
		const U32 argb_color   = m_pLogOutput->GetTextColor( i );

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

	// Store text to the buffer
	m_MultiLineTextBuffer.resize( 0 );
	for( i=0; i<num_lines; i++ )
	{
		m_MultiLineTextBuffer.push_back( rvecProfileText[i] );
	}
}


void DebugInfo_StateLog::UpdateDebugInfoText()
{
	m_MultiLineTextBuffer.resize( 0 );
	int row, num_rows = StateLog.GetNumLogs();
	for( row=2; row<num_rows; row++ )
	{
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
