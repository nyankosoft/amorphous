int test_fixed_string( int argc, char *argv[] );;
int test_clipboard_utils( int argc, char *argv[] );
int test_BitmapImage( int argc, char *argv[] );


int main( int argc, char *argv[] )
{
	test_fixed_string( argc, argv );

	test_clipboard_utils( argc, argv );

	test_BitmapImage( argc, argv );

	return 0;
}
