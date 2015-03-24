#ifndef  __CompressedArchive_Output_H__
#define  __CompressedArchive_Output_H__

#include <vector>
#include <iostream>
#include <fstream>

#include "BinaryArchive_Output.hpp"
#include "ArchiveObjectBase.hpp"

#include "../Zlib/zpipe_stream.h"


namespace amorphous
{

namespace serialization
{


class CompressedArchive_Output : public BinaryArchive_Output
{
public:

//	CompressedArchive_Output( const char *pcFilename, const unsigned long archive_id = 0, unsigned int flag = 0 );

	CompressedArchive_Output( const std::string& filename,
		                       const char *pStringID = nullptr,
							   unsigned int flag = 0 )
                               :
	BinaryArchive_Output(filename,pStringID,flag),
	m_Filename(filename)
	{
	}

	virtual ~CompressedArchive_Output() {}

	/// load archive objects saved in binary format file
	bool operator<< ( IArchiveObjectBase& obj )
	{
		m_Stream.m_Buffer.buffer().reserve( 0xffff );
		m_Stream.m_Buffer.reset_pos();

		// serialize to temporary buffer (m_Stream.m_Buffer)
		(*this) & obj;

		// reset the pointer position of the buffer
		m_Stream.m_Buffer.reset_pos();

		SerializableStream compressed_buffer;
		compressed_buffer.m_Buffer.buffer().reserve( m_Stream.m_Buffer.buffer().size() );

		// compress data
		z_def( m_Stream.m_Buffer, compressed_buffer.m_Buffer );

		// save compressed data as a binary archive
		BinaryArchive_Output archive( m_Filename );
		return archive << compressed_buffer;
	}

	// operators to serialize each object type

	///	serialize an archive object
/*	virtual IArchive& operator & (IArchiveObjectBase& vData);

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
		m_Stream.m_Buffer.write( pData, size );
	}


protected:

	std::string m_Filename;

	SerializableStream m_Stream;	///< buffer to temporarily hold uncompressed data

//	ofstream m_OutputFileStream;

};


}  /*  Serialization  */


} // namespace amorphous


#endif		/*  __CompressedArchive_Output_H__  */
