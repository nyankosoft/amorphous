#include <vector>
#include <string>
#include <boost/integer.hpp>


int main( int argc, char *argv[] )
{
	if( argc < 2 )
		return 0;

	const char *image_pathname = argv[1];

	FILE *fp = fopen(image_pathname,"rb");
	if( !fp )
		return 0;

	size_t dot_pos = std::string(image_pathname).rfind( "." );

	int last_bytes = -1;
	unsigned char dest = 0;
	std::vector<unsigned char> dest_buffer;
	while(1)
	{
		int read_count = (int)fread( &dest, sizeof(char), 1, fp );
		if( read_count < 1 )
			break;

		dest_buffer.push_back( dest );
	}

	int ret = fclose(fp);

	printf( "static const U32 s_ImageSize = %d;\n", (int)dest_buffer.size() );
	printf( "static const U32 s_ImageData[] =\n" );
	printf( "{\n" );

	for( size_t i=0; i<dest_buffer.size(); i += 4 )
	{
		bool first_column = (i % 16 == 0)  ? true : false;
		bool last_column  = (i % 16 == 12) ? true : false;
		bool last_u32     = (dest_buffer.size() - i <= 4) ? true : false;

		if( first_column )
			printf( "\t" );

		printf( "0x%02x%02x%02x%02x%s ",
			i+3 < dest_buffer.size() ? dest_buffer[i+3] : 0,
			i+2 < dest_buffer.size() ? dest_buffer[i+2] : 0,
			i+1 < dest_buffer.size() ? dest_buffer[i+1] : 0,
			i   < dest_buffer.size() ? dest_buffer[i]   : 0,
			last_u32 ? "" : ","
			);

		if( last_column || last_u32 )
			printf( "\n" );
	}

	printf( "};\n\n" );

	const char *image_data_variable_name = "image_data_variable_name";
	std::string ext;
	if( dot_pos != std::string::npos && dot_pos < std::string(image_pathname).length() )
		ext = std::string(image_pathname).substr( dot_pos + 1 );
	else
		ext = "insert_image_extension_here";

	printf( "CBuiltinImage s_ImageName =\n" );
	printf( "{\n" );
	printf( "	\"%s\", insert_image_width_here, insert_image_height_here, %s, sizeof(%s)\n",
		ext.c_str(),
		image_data_variable_name,
		image_data_variable_name
		);
	printf( "}\n" );

	return 0;
}
