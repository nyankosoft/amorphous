#ifndef  __ARCHIVE_OUTPUT_H__
#define  __ARCHIVE_OUTPUT_H__


#include <iostream>
#include <fstream>
#include "BinaryArchive.hpp"


namespace amorphous
{

namespace serialization
{


class BinaryArchive_Output : public BinaryArchive
{
public:

//	BinaryArchive_Output( const char *pcFilename, const unsigned long archive_id = 0, unsigned int flag = 0 );

	BinaryArchive_Output(
		const std::string& filename,
		const char *pStringID = nullptr,
		unsigned int archive_option_flags = 0 );

	virtual ~BinaryArchive_Output() { m_OutputFileStream.close(); }

	/// load archive objects saved in binary format file
	bool operator<< ( IArchiveObjectBase& obj );

	// operators to serialize each object type

	///	serialize an archive object
	virtual IArchive& operator & (IArchiveObjectBase& vData);

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

	std::ofstream m_OutputFileStream;

};


} // namespace serialization


} // namespace amorphous


#endif		/*  __ARCHIVE_OUTPUT_H__  */
