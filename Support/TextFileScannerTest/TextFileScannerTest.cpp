#include "gds/Support/TextFileScanner.hpp"
#include "gds/Utilities/TextFileScannerExtensions.hpp"
#include <ostream>

using namespace std;
//using namespace morph;


//#include "3DMath/Vector3.hpp"
//#include "Graphics/FloatRGBAColor.hpp"
/*
class ExTextFileScanner : public CTextFileScanner
{
public:

};
*/


int TestExtensions()
{
	CTextFileScanner scanner( "data/text_file_scanner_extensions_test.txt" );

	if( !scanner.IsReady() )
		return -1;

	Vector2 v2;
	Vector3 v3;
	Vector2 v2_2[2];
	Vector3 v3_2[2];
	SFloatRGBAColor c;
	SFloatRGBAColor c_2[2];
	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.TryScanLine( "VECTOR2", v2 );
		scanner.TryScanLine( "VECTOR3", v3 );
		scanner.TryScanLine( "RGBA", c );
		scanner.TryScanLine( "2_VEC2S", v2_2[0], v2_2[1] );
		scanner.TryScanLine( "2_VEC3S", v3_2[0], v3_2[1] );
		scanner.TryScanLine( "2_RGBAS", c_2[0],  c_2[1] );
	}

	return 0;
}

int Test()
{
	CTextFileScanner scanner( "data/text_file_scanner_test.txt" );

	if( !scanner.IsReady() )
	{
//		printf( "Failed to open file: %s\n", filepath.c_str() );
		return -1;
	}

	string name;
	int a, b, i3[3];
	short s;
	unsigned short us;
//	long l;
//	unsigned long ul;
	float fa, fb, f3[3], f4[4];
	double da, db, d3[3], d4[4];
	string str4[4];

	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.TryScanLine( "INT_PARAM_A",  a );
		scanner.TryScanLine( "INT_PARAM_B",  b );
		scanner.TryScanLine( "FLOAT_PARAM_A", fa );
		scanner.TryScanLine( "FLOAT_PARAM_B", fb );
		scanner.TryScanLine( "DOUBLE_PARAM_A", da );
		scanner.TryScanLine( "DOUBLE_PARAM_B", db );
		scanner.TryScanLine( "SHORT_PARAM",    s );
		scanner.TryScanLine( "USHORT_PARAM",   us );
//		scanner.TryScanLine( "LONG_PARAM",     l );
//		scanner.TryScanLine( "ULONG_PARAM",    ul );
		scanner.TryScanLine( "STRING", name );
		scanner.TryScanLine( "INTS",    i3[0], i3[1], i3[2] );
		scanner.TryScanLine( "3_FLOATS",  f3[0], f3[1], f3[2] );
		scanner.TryScanLine( "3_DOUBLES", d3[0], d3[1], d3[2] );
		scanner.TryScanLine( "4_FLOATS",  f4[0], f4[1], f4[2], f4[3] );
		scanner.TryScanLine( "4_DOUBLES", d4[0], d4[1], d4[2], d4[3] );
		scanner.TryScanLine( "4_STRINGS", str4[0], str4[1], str4[2], str4[3] );
	}

	bool tf;
	scanner.TryScanBool( "BOOL", "TRUE/FALSE", tf );

	double ref_d4[4] = { 0.05,  -3.20,  7.3,  -0.003 };
	float ref_f4[4]  = { 0.05f, -3.20f, 7.3f, -0.003f };

	return 0;
}


int main( int argc, char *argv[] )
{
	Test();

	TestExtensions();

	return 0;
}
