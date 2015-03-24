#ifndef  __BINARYARCHIVE_INPUT_H__
#define  __BINARYARCHIVE_INPUT_H__

#include <iostream>
#include <fstream>
#include "BinaryArchive.hpp"


namespace amorphous
{

namespace serialization
{


class BinaryArchive_Input : public BinaryArchive
{
public:

	/// constructor
	BinaryArchive_Input( const std::string& filename,
		                  const char *pcStringID = nullptr,
						  unsigned int archive_option_flags = 0 );

	virtual ~BinaryArchive_Input() { m_InputFileStream.close(); }

	/// load archive objects saved in binary format file
	bool operator>> ( IArchiveObjectBase& obj );

	// operators to serialize each object type

	///	serialize an archive object
	virtual IArchive& operator & (IArchiveObjectBase& rData);

	///	serialize the primitive data types
	virtual IArchive& operator & (int& nData);
	virtual IArchive& operator & (unsigned int& nData);
	virtual IArchive& operator & (bool& bData);
	virtual IArchive& operator & (char& cData);
	virtual IArchive& operator & (unsigned char& cData);
	virtual IArchive& operator & (short& sData);
	virtual IArchive& operator & (unsigned short& usData);
	virtual IArchive& operator & (long& ulData);
	virtual IArchive& operator & (unsigned long& ulData);
	virtual IArchive& operator & (float& fData);
	virtual IArchive& operator & (double& fData);
	virtual IArchive& operator & (std::string& strData);

	virtual void HandleData( void *pData, const int size );


protected:

	std::ifstream m_InputFileStream;

};


} // namespace serialization


} // namespace amorphous


#endif		/*  __BINARYARCHIVE_INPUT_H__  */
