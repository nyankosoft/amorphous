#include "ArchiveObjectBase.h"
#include "BinaryArchive_Input.h"
#include "BinaryArchive_Output.h"
#include "CompressedArchive_Input.h"
#include "CompressedArchive_Output.h"

using namespace GameLib1::Serialization;

using namespace std;


bool IArchiveObjectBase::SaveToFile( const string& filename )
{
	CBinaryArchive_Output archive( filename );
	return ( archive << *this );

//	return true;
}


bool IArchiveObjectBase::LoadFromFile( const string& filename )
{
	CBinaryArchive_Input archive( filename );
	return ( archive >> *this );

//	return true;
}


bool IArchiveObjectBase::SaveToCompressedFile( const std::string& filename )
{
	CCompressedArchive_Output archive( filename );
	return ( archive << *this );
}


bool IArchiveObjectBase::LoadFromCompressedFile( const std::string& filename )
{
	CCompressedArchive_Input archive( filename );
	return ( archive >> *this );
}
