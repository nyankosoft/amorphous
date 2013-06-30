#include "amorphous/Support/clipboard_utils.hpp"

using namespace amorphous;


int test_clipboard_utils( int argc, char *argv[] )
{
	std::string buffer;
	get_from_clipboard( buffer );

	printf( "clipboard: %s\n", buffer.c_str() );

	send_to_clipboard( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" );

	std::string text = get_text_from_clipboard();

	return 0;
}
