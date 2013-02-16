#ifndef  __SerializableStream_HPP__
#define  __SerializableStream_HPP__


#include "stream_buffer.hpp"
#include "Log/DefaultLog.hpp"
#include "Serialization/Serialization.hpp"
#include "Serialization/ArchiveObjectFactory.hpp"


namespace amorphous
{
using namespace serialization;


/**
 holds serializable stream
*/
class SerializableStream : public IArchiveObjectBase
{
public:

	stream_buffer m_Buffer;

public:

	SerializableStream() {}

	/// Returns true on success
	inline bool LoadBinaryStream( const std::string& filepath );

	inline bool LoadTextFile( const std::string& filepath );

	~SerializableStream() {}

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


inline bool SerializableStream::LoadBinaryStream( const std::string& filepath )
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


inline bool SerializableStream::LoadTextFile( const std::string& filepath )
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

} // namespace amorphous



#endif		/*  __SerializableStream_HPP__  */
