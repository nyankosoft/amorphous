#ifndef __clipboard_utils_linux_HPP__
#define __clipboard_utils_linux_HPP__


#include <string>
#include "Log/DefaultLog.hpp"


namespace amorphous
{


inline void get_from_clipboard( std::string& text )
{
	LOG_PRINT_ERROR( "Not implemented yet." );
}


inline std::string get_text_from_clipboard()
{
	std::string text;
	LOG_PRINT_ERROR( "Not implemented yet." );
	return text;
}


inline void send_to_clipboard( const std::string& text )
{
	LOG_PRINT_ERROR( "Not implemented yet." );
}


} // amorphous



#endif /* __clipboard_utils_linux_HPP__ */
