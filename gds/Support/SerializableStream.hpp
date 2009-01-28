#ifndef  __SerializableStream_HPP__
#define  __SerializableStream_HPP__


#include "Support/stream_buffer.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
using namespace GameLib1::Serialization;


/**
 holds serializable stream
*/
class CSerializableStream : public IArchiveObjectBase
{
public:

	stream_buffer m_Buffer;

public:

	CSerializableStream() {}

	/// Returns true on success
	inline bool LoadBinaryStream( const std::string& filepath );

	inline bool LoadTextFile( const std::string& filepath );

	~CSerializableStream() {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Buffer.buffer();

		if( ar.GetMode() == IArchive::MODE_INPUT )
		{
			// set the new stream
			// - reset the cursor position
			m_Buffer.reset_pos();
		}
	}
};


inline bool CSerializableStream::LoadBinaryStream( const std::string& filepath )
{
	bool res = m_Buffer.LoadBinaryStream( filepath );

	if( res )
		return true;
	else
	{
		LOG_PRINT_ERROR( "Failed to open a file: " + filepath );
		return false;
	}
}


inline bool CSerializableStream::LoadTextFile( const std::string& filepath )
{
	bool res = m_Buffer.LoadTextFile( filepath );

	if( res )
		return true;
	else
	{
		LOG_PRINT_ERROR( "Failed to open a file: " + filepath );
		return false;
	}
}


#endif		/*  __SerializableStream_HPP__  */
