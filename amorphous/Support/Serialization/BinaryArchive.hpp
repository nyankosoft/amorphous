#ifndef  __amorphous_BinaryArchive_HPP__
#define  __amorphous_BinaryArchive_HPP__


#include "amorphous/base.hpp"
#include "Archive.hpp"
#include "ArchiveObjectBase.hpp"


namespace amorphous
{

namespace serialization
{


class BinaryArchive : public IArchive
{
protected:

	enum Params
	{
		NUM_OBFUSCATION_BYTES = 32
	};

	int m_ObfucationBitIndex;

	static bool ms_IsInitialized;

	static unsigned char ms_ObfucationBits[NUM_OBFUSCATION_BYTES];

	// The following two strings are used by binary archive input & output classes.
	// - The first 13 characters, "BinaryArchive", is used as a verification code.
	// - The last 2 digits are used as a version number

	// Identifier
	static const char *sg_pBinaryArchiveIdentifierString;

	// Version number
	// - Framework developer increments the last 2 digits when the archive header is changed.
	static const char *sg_pBinaryArchiveVersionString;

	static void Initialize();

public:

	BinaryArchive();

	virtual ~BinaryArchive() {}

	/// \param[in] bits must be the array of size 8.
	static void SetObfuscationBits( U32 *bits );
};


} // namespace serialization


} // namespace amorphous


#endif		/*  __amorphous_BinaryArchive_HPP__  */
