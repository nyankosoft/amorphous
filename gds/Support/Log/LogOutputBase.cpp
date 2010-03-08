#include "LogOutputBase.hpp"
#include <gds/Support/SafeDelete.hpp>
#include <gds/base.hpp>

using namespace std;


//=================================================================================
// CLogOutputBase
//=================================================================================

CLogOutputBase::CLogOutputBase()
{
}


CLogOutputBase::~CLogOutputBase()
{
}



//=================================================================================
// CLogOutput_TextFile
//=================================================================================

CLogOutput_TextFile::CLogOutput_TextFile( const string& filename )
{
//	m_pFile = fopen( pFilename, "w" );

	m_OutputFileStream.open( filename.c_str(), ios_base::out );
	if( !m_OutputFileStream.is_open() )
	{
		// Failed to open the log file, which we need to log this error. How should we report this error?
//		printf( "Failed to open a log file: %s\n", filename.c_str() );
	}

	SetNewLineCharacterOption( NLCO_ADD_ALWAYS );
}


CLogOutput_TextFile::~CLogOutput_TextFile()
{
//	if( m_pFile )
//		fclose( m_pFile );

	m_OutputFileStream.close();
}


void CLogOutput_TextFile::SetNewLineCharacterOption( int option )
{
	m_NewLineCharacterOption = option;
}


void CLogOutput_TextFile::Print( const CLogMessage& msg )
{
//	if( !m_pFile )
//		return;
//	fprintf( m_pFile, "%s %s", msg.m_Time.c_str(), msg.m_Text.c_str() );

	m_OutputFileStream << "[" << msg.m_Time << "] " << msg.m_Text;

	switch( m_NewLineCharacterOption )
	{
	case NLCO_ADD_ALWAYS:
		m_OutputFileStream << "\n";
		break;

	case NLCO_ADD_IF_ONT_FOUND:
		if( msg.m_Text[ msg.m_Text.length() - 1 ] != '\n' )
			m_OutputFileStream << "\n";
		break;

	case NLCO_DONT_ADD:
	default:
		break;
	}

	m_OutputFileStream.flush();
}



//=================================================================================
// CLogOutput_Console
//=================================================================================

void CLogOutput_Console::Print( const CLogMessage& msg )
{
//	printf( "%s %s", msg.m_Time.c_str(), msg.m_Text.c_str() );
	cout << "[" << msg.m_Time << "] " << msg.m_Text;
}



//=================================================================================
// CLogOutput_HTML
//=================================================================================

#define NUM_TEMPLATE_LINES	10
#define NUM_BEGINNING_LINES	7

const char s_HTMLTemplate[NUM_TEMPLATE_LINES][256] = 
{
	"<html>",
	"	<head>",
	"		<title></title>",
	"	</head>",
	"	<body text=\"#F0F0F0\" bgcolor=\"#080808\">",
	"	<table width=\"90%\" align=\"center\" border=\"1\" bordercolor=\"#CCCCCC\" cellpadding=\"3\" cellspacing=\"1\">",
	"	<tr><td width=\"15%\" align=\"center\">TIME</td><td>Message</td></tr>",
	"	</table>",
	"	</body>",
	"</html>"
};

static const char *s_LogTextColor[] =
{
	"E6E6E6",	// verbose
	"F8F808",	// caution
	"FF8F06",	// warning
	"FF0000",	// error
	"F8F8FF"	// info
};


CLogOutput_HTML::CLogOutput_HTML( const std::string& filename )
: CLogOutput_TextFile( filename )
{
	for( int i=0; i<NUM_BEGINNING_LINES; i++ )
		m_OutputFileStream << s_HTMLTemplate[i];

	m_OutputFileStream.flush();
}


CLogOutput_HTML::~CLogOutput_HTML()
{
	for( int i=NUM_BEGINNING_LINES; i<NUM_TEMPLATE_LINES; i++ )
		m_OutputFileStream << s_HTMLTemplate[i];
}


void CLogOutput_HTML::Print( const CLogMessage& msg )
{
	string strText;
	if( msg.m_Text[ msg.m_Text.size() - 1 ] == '\n' )
		strText = msg.m_Text.substr( 0, msg.m_Text.size() - 1 );


	int color_index = msg.m_FilterVal & 0x000000FF;	// take out the warning level index from the lowest 8-bits
	clamp( color_index, 0, NUM_LOGWARNINGLEVELS - 1 );
	const char *text_color = s_LogTextColor[color_index];
	string font_tag = "<font color=\"#" + string(text_color) + "\">";

	m_OutputFileStream <<
		"	<tr><td align=\"center\">" + font_tag + msg.m_Time
		+ "</font></td><td>" + font_tag + msg.m_Text + "</font></td></tr>\n";

	m_OutputFileStream.flush();
}



CLogOutput_Overlay::CLogOutput_Overlay()
{
}


CLogOutput_Overlay::~CLogOutput_Overlay()
{
}


void CLogOutput_Overlay::Print( const CLogMessage& msg )
{
}
