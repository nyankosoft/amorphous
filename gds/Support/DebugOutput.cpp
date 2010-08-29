#include "DebugOutput.hpp"

#include "Graphics/2DPrimitive/2DRect.hpp"
//#include "Graphics/Font/Font.hpp"
#include "Graphics/Font/TextureFont.hpp"
#include "Graphics/Font/TrueTypeTextureFont.hpp"
#include "Graphics/Font/UTFFont.hpp"
#include "Graphics/Font/BuiltinFonts.hpp"
#include "Graphics/LogOutput_OnScreen.hpp"
#include "Graphics/GraphicsResourceManager.hpp"
#include "Support/Timer.hpp"
#include "Sound/SoundManager.hpp"
#include "Input/InputDevice.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/StringAux.hpp"
#include "Support/Profile.hpp"
#include "Support/Log/StateLog.hpp"

using namespace std;


class CFontFactory
{
public:

	CFontBase* CreateFontRawPtr( CFontBase::FontType type );

	/// Create a font
	/// Determines the font class from the font_name
	/// If font_name is a pathname of an image file, CTextureFont is created
	/// and the image file is loaded as a texture that contains the fixed-pitch ascii characters.
	/// If font_name is a *.ttf or *.otf file, CUTFFont is created.
	/// If font_name begins with the string, "BuiltinFont::" CTextureFont is created
	/// and the specified built-in font is loaded
	CFontBase* CreateFontRawPtr( const std::string& font_name, int font_width = 16, int font_height = 32 );

	/// returns a owned ref
	CFontBase* CreateFontRawPtr( CFontBase::FontType type, const std::string& font_name, int font_width, int font_height );
};


CFontBase* CFontFactory::CreateFontRawPtr( CFontBase::FontType type )
{
	switch( type )
	{
	case CFontBase::FONTTYPE_TEXTURE:
		return new CTextureFont;
	case CFontBase::FONTTYPE_TRUETYPETEXTURE:
		return new CTrueTypeTextureFont;
	case CFontBase::FONTTYPE_UTF:
		return new CUTFFont;
//	case CFontBase::FONTTYPE_DIRECT3D:
//		return new CD3DFont;
	default:
		LOG_PRINT_ERROR( " An unsupported font type." );
		return NULL;
	}
}


CFontBase* CFontFactory::CreateFontRawPtr( const std::string& font_name, int font_width, int font_height )
{
	if( font_name.find(".ia")  != string::npos
	 || font_name.find(".dds") != string::npos
	 || font_name.find(".bmp") != string::npos
	 || font_name.find(".tga") != string::npos
	 || font_name.find(".jpg") != string::npos )
	{
		return new CTextureFont( font_name, font_width, font_height );
	}
	else if( font_name.find( "BuiltinFont::" ) == 0 )
	{
		const string builtin_font_name = font_name.substr( strlen("BuiltinFont::") );
		CTextureFont *pFont = new CTextureFont;
		bool initialized = pFont->InitFont( GetBuiltinFontData( builtin_font_name ) );

		if( initialized )
		{
			pFont->SetFontSize( font_width, font_height );
			return pFont;
		}
		else
		{
			SafeDelete( pFont );
			return NULL;
		}
	}
	else if( 4 < font_name.length() )
	{
		const string dot_and_ext = font_name.substr(font_name.length() - 4);
		if( dot_and_ext == ".ttf"
		 && dot_and_ext == ".otf" )
		{
			// Consider font_name as a filename
			CUTFFont *pUTFFont = new CUTFFont;
			pUTFFont->InitFont( font_name );
			return pUTFFont;
		}
		else
			return NULL;
	}
	else
		return NULL;

//	CTextureFont *pTexFont = new CTextureFont;
//	pTexFont->InitFont( font_name, 16, 32 );
//	return pTexFont;

	return NULL;
}


CFontBase* CFontFactory::CreateFontRawPtr( CFontBase::FontType type, const string& font_name, int font_width, int font_height )
{
	LOG_PRINT_ERROR( " Not implemented." );
	return NULL;
}



CDebugItem_Log::CDebugItem_Log( CLogOutput_ScrolledTextBuffer* pLogOutput )
:
m_pLogOutput(pLogOutput)
{
//	m_pLogOutput = new CLogOutput_ScrolledTextBuffer()// "Texture/...", w, h, rows, chars );
}


void CDebugItem_Log::Render()
{
	CFontBase *pFont = m_pFont;

	const int font_width  = pFont->GetFontWidth();
	const int font_height = pFont->GetFontHeight();
//	const int num_rows = ???;
	const int num_current_rows = m_pLogOutput->GetNumCurrentRows();

	// draw rect for background
	const int sx = (int)m_vTopLeftPos.x;
	const int sy = (int)m_vTopLeftPos.y;
	C2DRect bg_rect( sx, sy, sx + font_width * 80/*m_NumChars*/, sy + font_height * num_current_rows, 0x80000000 );
	bg_rect.Draw();


	Vector2 vTextPos = m_vTopLeftPos;
	for( int i=0; i<num_current_rows; i++, vTextPos.y += font_height )
	{
		const char *pText = m_pLogOutput->GetText( i );
		const U32 color   = m_pLogOutput->GetTextColor( i );

		pFont->DrawText( pText, vTextPos, color );
	}

/*	Vector2 pos;
	pos.x = (float)( sx + font_width * 0.5f );
	int i, row_index;
	int num_rows = m_NumRows;
//	const int end_row_index = m_EndRowIndex;
	int start_row_index = (m_EndRowIndex);// % m_NumRows;
//	if( m_TextBuffer[start_row_index].length() == 0 )
	if( m_NumOutputLines < num_rows )
	{
		// not wrapped yet - start rendering from the first row of the buffer
		num_rows = start_row_index;
		start_row_index = 0;
	}

	for( i=0, row_index = start_row_index;
		 i<num_rows;
		 i++, row_index = (row_index+1) % num_rows )	// wrap the row index
	{
		pos.y = (float)(sy + i * font_height);
//		pFont->CacheText( m_TextBuffer[i].c_str(), pos, m_TextColor[i] );
		pFont->DrawText( m_TextBuffer[row_index].c_str(), pos, m_TextColor[row_index] );
	}
*/

//	m_pLogOutput->Render();
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

	const Vector2 vLeftTop = m_vTopLeftPos;
	C2DRect rect( 
		vLeftTop,
		vLeftTop + Vector2( (float)(font_width * (num_max_chars + 2)), (float)(font_height * num_lines) ),
		0x80000000 );
	rect.Draw();

	// draw text
	Vector2 vPos = m_vTopLeftPos;
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
	GraphicsResourceManager().GetStatus( GraphicsResourceType::Texture, m_TextBuffer );
}


void CDebugItem_SoundManager::GetTextInfo()
{
	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
	SoundManager().GetTextInfo( m_TextBuffer );
}


void CDebugItem_InputDevice::GetTextInfo()
{
	InputDeviceHub().GetInputDeviceStatus( m_vecTextBuffer );

	memset( m_TextBuffer, 0, sizeof(m_TextBuffer) );
	int left = sizeof(m_TextBuffer) - 1;
	for( size_t i=0; i<m_vecTextBuffer.size() && 0 < left; i++ )
	{
		strncat( m_TextBuffer, m_vecTextBuffer[i].c_str(), left );
		left -= (int)m_vecTextBuffer[i].size();
	}
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
	m_pFont = factory.CreateFontRawPtr( font_name );
	m_pFont->SetFontSize( w, h );
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

///	StateLog.Update( 0, "FPS: " + to_string(GlobalTimer().GetFPS()) );
//	StateLog.Update( 1, "AVE. FPS: " + to_string(FPS.GetAverageFPS()) );
	string fps_text = "FPS: " + to_string(GlobalTimer().GetFPS());

	U32 color = ( GlobalTimer().GetFPS() < 40.0f ) ? 0xFFFF0000 /* fps low - red */ : 0xFF00FF00; /* green */

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

	SetRenderStatesForTextureFont( AlphaBlend::One );

	RenderFPS();

	m_vecpDebugItem[index]->Render();
}


void CDebugOutput::AddDebugItem( const std::string& item_name, CDebugItemBase *pDebugItem )
{
	m_vecpDebugItem.push_back( pDebugItem );
	m_vecpDebugItem.back()->SetFont( m_pFont );
	m_vecpDebugItem.back()->SetName( item_name );

	// TODO: add offset for fps display
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


void CDebugOutput::ReleaseGraphicsResources()
{
	m_pFont->Release();
}


void CDebugOutput::LoadGraphicsResources( const CGraphicsParameters& rParam )
{
	m_pFont->Reload();
}
