#ifndef  __amorphous_DefaultLogAux_HPP__
#define  __amorphous_DefaultLogAux_HPP__


#include "LogOutput.hpp"
#include "DefaultLog.hpp"


namespace amorphous
{

inline int InitTextlLog( const std::string& text_file_pathname )
{
	static LogOutput_TextFile s_TextLog( text_file_pathname );
	GlobalLog().AddLogOutputDevice( &s_TextLog );

	return 0;
}

inline int InitHTMLLog( const std::string& html_file_pathname )
{
	static LogOutput_HTML s_HTMLLog( html_file_pathname );
	GlobalLog().AddLogOutputDevice( &s_HTMLLog );

	return 0;
}

} // namespace amorphous


#endif  /*  __amorphous_DefaultLogAux_HPP__  */
