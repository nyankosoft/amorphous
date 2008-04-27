
#ifndef  __CompressedArchive_Output_H__
#define  __CompressedArchive_Output_H__

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "BinaryArchive_Output.h"
#include "ArchiveObjectBase.h"

#include "../Zlib/zpipe_stream.h"


namespace GameLib1
{

namespace Serialization
{


class CCompressedArchive_Output : public CBinaryArchive_Output
{
public:

//	CCompressedArchive_Output( const char *pcFilename, const unsigned long archive_id = 0, unsigned int flag = 0 );

	CCompressedArchive_Output( const std::string& filename,
		                       const char *pStringID = NULL,
							   unsigned int flag = 0 )
                               :
	CBinaryArchive_Output(filename,pStringID,flag),
	m_Filename(filename)
	{
	}

	virtual ~CCompressedArchive_Output() {}

	/// load archive objects saved in binary format file
	bool operator<< ( IArchiveObjectBase& obj )
	{
		m_Buffer.buffer().reserve( 0xffff );
		m_Buffer.reset_pos();

		// serialize to temporary buffer (m_Buffer)
		(*this) & obj;

		// reset the pointer position of the buffer
		m_Buffer.reset_pos();

		stream_buffer compressed_buffer;
		compressed_buffer.buffer().reserve( m_Buffer.buffer().size() );

		// compress data
		z_def( m_Buffer, compressed_buffer );

		// save compressed data as a binary archive
		CBinaryArchive_Output archive( m_Filename );
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
		m_Buffer.write( pData, size );
	}


protected:

	std::string m_Filename;

	stream_buffer m_Buffer;	///< buffer to temporarily hold uncompressed data

//	ofstream m_OutputFileStream;

};


}  /*  Serialization  */


}  /*  GameLib1  */


#endif		/*  __CompressedArchive_Output_H__  */
