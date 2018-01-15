#include "BinaryArchive.hpp"
#include <cstring>


using namespace amorphous::serialization;

using namespace std;


bool BinaryArchive::ms_IsInitialized = false;
unsigned char BinaryArchive::ms_ObfucationBits[NUM_OBFUSCATION_BYTES];
const char *BinaryArchive::sg_pBinaryArchiveIdentifierString = "BinaryArchive";
const char *BinaryArchive::sg_pBinaryArchiveVersionString = "01";

BinaryArchive::BinaryArchive()
:
m_ObfucationBitIndex(0)
{
	Initialize();
}


void BinaryArchive::Initialize()
{
	if( !ms_IsInitialized )
	{
		std::memset( ms_ObfucationBits, sizeof(ms_ObfucationBits), 0 );
		ms_IsInitialized = true;
	}
}


void BinaryArchive::SetObfuscationBits( U32 *bits )
{
	// This funciton may be called before the first instantiation of BinaryArchive
	Initialize();

//	U32 u[] = {u0,u1,u2,u3};
	U32 *u = bits;
	U32 masks[] = {0x000000FF,0x0000FF00,0x00FF0000,0xFF000000};
	for( int i=0; i<sizeof(u) / sizeof(U32); i++ )
	{
		for( int j=0; j<sizeof(U32); j++ )
		{
			ms_ObfucationBits[i*4+j] = (unsigned char)( (u[i] & masks[j]) >> (j*8) );
		}
	}
}
