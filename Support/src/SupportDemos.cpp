int test_fixed_string( int argc, char *argv[] );;
int test_TextFileScanner( int argc, char *argv[] );;
int test_clipboard_utils( int argc, char *argv[] );
int test_BitmapImage( int argc, char *argv[] );
int test_ImageSplitter( int argc, char *argv[] );


int main( int argc, char *argv[] )
{
	test_fixed_string( argc, argv );

//	test_TextFileScanner( argc, argv );

//	test_clipboard_utils( argc, argv );

	test_BitmapImage( argc, argv );

//	test_ImageSplitter( argc, argv );

	return 0;
}
