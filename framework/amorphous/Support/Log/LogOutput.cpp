#include "LogOutput.hpp"
#include "amorphous/base.hpp"
#include "amorphous/Support/lfs.hpp"
#include "amorphous/Support/StringAux.hpp"
#include <thread>


namespace amorphous
{

using namespace std;

//=================================================================================
// LogOutput_TextFile
//=================================================================================

LogOutput_TextFile::LogOutput_TextFile( const string& filename )
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


LogOutput_TextFile::~LogOutput_TextFile()
{
//	if( m_pFile )
//		fclose( m_pFile );

	m_OutputFileStream.close();
}


void LogOutput_TextFile::SetNewLineCharacterOption( int option )
{
	m_NewLineCharacterOption = option;
}


void LogOutput_TextFile::Print( const LogMessage& msg )
{
//	if( !m_pFile )
//		return;
//	fprintf( m_pFile, "%s %s", msg.m_Time.c_str(), msg.m_Text.c_str() );

	m_OutputFileStream << "[" << msg.m_Time << "] ";

	// A hashed thread id
	std::hash<std::thread::id> hasher;
	size_t id = hasher(std::this_thread::get_id());
	m_OutputFileStream << (id % 10000) << " "; // id is long so we just print the last four digits.
	
	m_OutputFileStream << msg.m_Text;

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
// LogOutput_Console
//=================================================================================

void LogOutput_Console::Print( const LogMessage& msg )
{
//	printf( "%s %s", msg.m_Time.c_str(), msg.m_Text.c_str() );
	cout << "[" << msg.m_Time << "] " << msg.m_Text;
}



//=================================================================================
// LogOutput_HTML
//=================================================================================

static const char *sg_forms =
	"	<form>\n"\
	"	<input id=\"I\" type=\"checkbox\" onclick='handleClick(this);' name=\"level\" value=\"i\" checked>Information\n"\
	"	<input id=\"E\" type=\"checkbox\" onclick='handleClick(this);' name=\"level\" value=\"e\" checked>Error\n"\
	"	<input id=\"W\" type=\"checkbox\" onclick='handleClick(this);' name=\"level\" value=\"w\" checked>Warning\n"\
	"	<input id=\"C\" type=\"checkbox\" onclick='handleClick(this);' name=\"level\" value=\"c\" checked>Caution\n"\
	"	<input id=\"V\" type=\"checkbox\" onclick='handleClick(this);' name=\"level\" value=\"v\" checked>Verbose\n"\
	"	</form>\n";

static const char *sg_body_and_table_end =
	"	</table>\n"\
	"	</body>\n";

static const char *js_source =
	"	<script>\n"\
	"	function getCheckedClasses()\n"\
	"	{\n"\
	"		var text = \"\";\n"\
	"		if( document.getElementById(\"I\").checked ) text += \"I\";\n"\
	"		if( document.getElementById(\"E\").checked ) text += \"E\";\n"\
	"		if( document.getElementById(\"W\").checked ) text += \"W\";\n"\
	"		if( document.getElementById(\"C\").checked ) text += \"C\";\n"\
	"		if( document.getElementById(\"V\").checked ) text += \"V\";\n"\
	"		return text;\n"\
	"	}\n"\
	"	function handleClick(arg)\n"\
	"	{\n"\
	"		var enabled_classes = getCheckedClasses();\n"\
	"		var rows = document.getElementsByTagName(\"tr\");\n"\
	"		for( var i=0; i<rows.length; i++ )\n"\
	"		{\n"\
	"			var index = enabled_classes.indexOf( rows[i].className );\n"\
	"			rows[i].style.display = (index == -1) ? \"none\" : \"table-row\"\n"\
	"		}\n"\
	"	}\n"\
	"	</script>\n";

static const char *s_LogTextColor[] =
{
	"E6E6E6",	// verbose
	"F8F808",	// caution
	"FF8F06",	// warning
	"FF0000",	// error
	"F8F8FF"	// info
};


LogOutput_HTML::LogOutput_HTML( const std::string& filename )
: LogOutput_TextFile( filename )
{
	bool support_filtering = true;

	std::ofstream& ofs = m_OutputFileStream;

	ofs << "<html>\n";
	ofs << "	<head>\n";
	ofs << "	<title>" << lfs::path(filename).leaf().stem().string() << "</title>\n";

	if( support_filtering )
		ofs << js_source;

	ofs << "	</head>\n";
	ofs << "	<body text=\"#F0F0F0\" bgcolor=\"#080808\">\n";

	if( support_filtering )
		ofs << sg_forms;

	ofs << "	<table width=\"90%\" align=\"center\" border=\"1\" bordercolor=\"#CCCCCC\" cellpadding=\"3\" cellspacing=\"1\">\n";
	ofs << "	<tr><td width=\"15%\" align=\"center\">TIME</td><td>Message</td></tr>\n";

	ofs.flush();
}


LogOutput_HTML::~LogOutput_HTML()
{
	std::ofstream& ofs = m_OutputFileStream;

	ofs << sg_body_and_table_end;

	ofs << "</html>\n";
}


static const char *get_row_class( int warning_level )
{
	switch( warning_level )
	{
	case 0: return "V";
	case 1: return "C";
	case 2: return "W";
	case 3: return "E";
	case 4: return "I";
	default:
		return "";
	}

	return "";
}


void LogOutput_HTML::Print( const LogMessage& msg )
{
	string text_in_html( msg.m_Text );

	if( text_in_html[ text_in_html.size() - 1 ] == '\n' )
		text_in_html = text_in_html.substr( 0, text_in_html.size() - 1 );

	replace_all( text_in_html, "<", "&lt;" );
	replace_all( text_in_html, ">", "&gt;" );

	int color_index = msg.m_FilterVal & 0x000000FF;	// take out the warning level index from the lowest 8-bits
	clamp( color_index, 0, NUM_LOGWARNINGLEVELS - 1 );
	const char *text_color = s_LogTextColor[color_index];
	string font_tag = "<font color=\"#" + string(text_color) + "\">";

	m_OutputFileStream <<
		"	<tr class=\"" + string(get_row_class(color_index)) + "\"><td align=\"center\">" + font_tag + msg.m_Time
		+ "</font></td><td>" + font_tag + text_in_html + "</font></td></tr>\n";

	m_OutputFileStream.flush();
}



LogOutput_Overlay::LogOutput_Overlay()
{
}


LogOutput_Overlay::~LogOutput_Overlay()
{
}


void LogOutput_Overlay::Print( const LogMessage& msg )
{
}


} // namespace amorphous
