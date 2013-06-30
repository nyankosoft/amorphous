#include "../../../Project1/amorphous/Support/clipboard_utils.hpp"


int main( int argc, char *argv[] )
{
	std::string buffer;
	get_from_clipboard( buffer );

	printf( "clipboard: %s\n", buffer.c_str() );

	send_to_clipboard( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" );

	std::string text = get_text_from_clipboard();

	return 0;
}
