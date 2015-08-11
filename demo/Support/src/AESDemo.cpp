#include "amorphous/Support/AES.hpp"
#include "amorphous/base.hpp"

using namespace std;
using namespace amorphous;


int RunAESDemo()
{
	AES aes;

	vector<unsigned char> key;
	aes.GenerateKey( 256, key );

	printf( "key.size(): %d\n", (int)key.size() );

//	unsigned char buffer[50000]; // 50KB
	size_t read_bytes = 0;

	vector<unsigned char> plaintext;
	plaintext.resize(50000,0);

//	string plaintext_file = "input_data/plaintext/plaintext_example.txt";
//	string plaintext_file = "input_data/plaintext/0s.txt";
	string plaintext_file = "input_data/plaintext/more0s.txt";
	FILE *fp = fopen( plaintext_file.c_str(), "rb" );
	if(fp)
	{
//		read_bytes = fread(buffer,sizeof(buffer),1,fp);
		read_bytes = fread(&plaintext[0], sizeof(unsigned char), plaintext.size(), fp);
		printf( "read_bytes: %d\n", (int)read_bytes );
		fclose(fp);
	}

	vector<unsigned char> ciphertext;
	aes.Encrypt( plaintext, ciphertext );

	printf( "ciphertext.size(): %d\n", (int)ciphertext.size() );

	if( 0 < ciphertext.size() )
	{
		fp = fopen( string(plaintext_file + ".encrypted").c_str(), "wb" );
		if(fp)
		{
			size_t written_bytes = fwrite(&ciphertext[0], sizeof(unsigned char), ciphertext.size(), fp);
			printf( "written_bytes: %d\n", (int)written_bytes );
			fclose(fp);
		}
	}

	return 0;
}


int demo_AES( int argc, char *argv[] )
{
	return RunAESDemo();
}
