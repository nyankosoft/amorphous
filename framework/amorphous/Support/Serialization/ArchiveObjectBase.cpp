#include "ArchiveObjectBase.hpp"
#include "BinaryArchive_Input.hpp"
#include "BinaryArchive_Output.hpp"
#include "CompressedArchive_Input.hpp"
#include "CompressedArchive_Output.hpp"

using namespace amorphous::serialization;

using namespace std;


bool IArchiveObjectBase::SaveToFile( const string& filename, unsigned int archive_option_flags )
{
	BinaryArchive_Output archive( filename, "", archive_option_flags );
	return ( archive << *this );

//	return true;
}


bool IArchiveObjectBase::LoadFromFile( const string& filename )
{
	BinaryArchive_Input archive( filename );
	return ( archive >> *this );

//	return true;
}


bool IArchiveObjectBase::SaveToCompressedFile( const std::string& filename )
{
	CompressedArchive_Output archive( filename );
	return ( archive << *this );
}


bool IArchiveObjectBase::LoadFromCompressedFile( const std::string& filename )
{
	CompressedArchive_Input archive( filename );
	return ( archive >> *this );
}
