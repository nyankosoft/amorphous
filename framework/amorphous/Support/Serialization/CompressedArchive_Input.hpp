#ifndef  __CompressedArchive_Input_H__
#define  __CompressedArchive_Input_H__

#include <vector>
#include <iostream>
#include <fstream>

#include "BinaryArchive_Input.hpp"
#include "ArchiveObjectBase.hpp"
#include "../SerializableStream.hpp"

#include "../ZLib/zpipe_stream.h"


namespace amorphous
{

namespace serialization
{


class CompressedArchive_Input : public BinaryArchive_Input
{
public:

	/// constructor
	CompressedArchive_Input( const std::string& filename,
		                  const char *pcStringID = nullptr,
						  unsigned int flag = 0 )
						  :
	BinaryArchive_Input(filename,pcStringID,flag),
	m_Filename(filename)
	{
	}

	virtual ~CompressedArchive_Input() {}

	/// load archive objects saved in binary format file
	bool operator>> ( IArchiveObjectBase& obj )
	{
		SerializableStream compressed_buffer;

		// load compressed data
		BinaryArchive_Input archive( m_Filename );
		bool res = archive >> compressed_buffer;

		if( !res )
			return false;

		compressed_buffer.m_Buffer.reset_pos();
		m_Stream.m_Buffer.reset_pos();

		// decompress data
		z_inf( compressed_buffer.m_Buffer, m_Stream.m_Buffer );

		m_Stream.m_Buffer.reset_pos();

		// serialize decompressed data
		(*this) & obj;

		return true;
	}

	// operators to serialize each object type

	///	serialize an archive object
/*	virtual IArchive& operator & (IArchiveObjectBase& rData);

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
*/
	virtual void HandleData( void *pData, const int size )
	{
		m_Stream.m_Buffer.read( pData, size );
	}


protected:

	std::string m_Filename;

	SerializableStream m_Stream;	///< buffer to temporarily hold uncompressed data

//	ifstream m_InputFileStream;
};


}  /*  Serialization  */


} // namespace amorphous


#endif		/*  __CompressedArchive_Input_H__  */
