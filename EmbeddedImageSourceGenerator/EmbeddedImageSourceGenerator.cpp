#include <vector>
#include <boost/integer.hpp>


int main( int argc, char *argv[] )
{
	if( argc < 2 )
		return 0;

	FILE *fp = fopen(argv[1],"rb");
	if( !fp )
		return 0;

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

	printf( "static const U32 ClassName::m_MemberVariableName[] =\n" );
	printf( "{\n" );

	for( size_t i=0; i<dest_buffer.size(); i += 4 )
	{
		bool first_column = (i % 16 == 0)  ? true : false;
		bool last_column  = (i % 16 == 12) ? true : false;
		bool last_u32     = (dest_buffer.size() - i <= 4) ? true : false;

		if( first_column )
			printf( "\t" );

		printf( "0x%02x%02x%02x%02x%s ",
			i   < dest_buffer.size() ? dest_buffer[i+3] : 0,
			i+1 < dest_buffer.size() ? dest_buffer[i+2] : 0,
			i+2 < dest_buffer.size() ? dest_buffer[i+1] : 0,
			i+3 < dest_buffer.size() ? dest_buffer[i] : 0,
			last_u32 ? "" : ","
			);

		if( last_column || last_u32 )
			printf( "\n" );
	}

	printf( "};\n" );

	return 0;
}
