#ifndef  __stream_buffer_HPP__
#define  __stream_buffer_HPP__


#include <string.h>
#include <string>
#include <vector>
#include <fstream>

#include "lfs.hpp"


namespace amorphous
{


class stream_buffer
{
protected:

	int m_pos;

	std::vector<char> m_buffer;

public:

	stream_buffer() : m_pos(0) {}

	~stream_buffer() {}

	inline bool LoadBinaryStream( const std::string& filepath );

	inline int read( void *dest_buffer, int size );
	inline int write( const void *src_buffer, int size );

	inline bool end() { return m_pos == m_buffer.size(); }
	inline bool error() { return false; }

	inline bool LoadTextFile( const std::string& filename );

	inline void resize( int size ) { m_buffer.resize( size, 0 ); }

	std::vector<char>& buffer() { return m_buffer; }

	const std::vector<char>& get_buffer() const { return m_buffer; }

	inline void seek_pos( int pos ) { m_pos = pos; }

	inline int get_current_pos() const { return m_pos; }

	inline void reset_pos() { seek_pos(0); }
};


inline bool stream_buffer::LoadBinaryStream( const std::string& filepath )
{
	FILE *fp = fopen( filepath.c_str(), "rb" );
	if( !fp )
	{
		return false;
	}

	int file_size = lfs::get_filesize(filepath);

	m_buffer.resize( file_size );

	fread( &m_buffer[0], file_size, 1, fp );

	fclose(fp);

	return true;
}


inline int stream_buffer::read( void *dest_buffer, int size )
{
	int left_size = (int)m_buffer.size() - m_pos;

	int copy_size = left_size < size ? left_size : size;
	memcpy( dest_buffer, &m_buffer[0] + m_pos, copy_size );

	m_pos += copy_size;

	return copy_size;
}


/**
 * add given data to the buffer
 * returns the size of the data actually written to the buffer
 */
inline int stream_buffer::write( const void *src_buffer, int size )
{
	int left_size = (int)m_buffer.size() - m_pos;

	// extend the buffer size if necessary
	if( left_size < size )
		m_buffer.insert( m_buffer.end(), size - left_size, 0 );

	memcpy( &m_buffer[0] + m_pos, src_buffer, size );

	m_pos += size;

	return size;
}


inline bool stream_buffer::LoadTextFile( const std::string& filepath )
{
	std::ifstream testfile;

	testfile.open( filepath.c_str() );

	if ( !testfile.is_open() ) 
	{
		return false;
	}

	// Get the length of the file
	testfile.seekg( 0, std::ios::end );
	int nLength = testfile.tellg();
	testfile.seekg( 0, std::ios::beg );

	m_buffer.resize( nLength + 1, 0 );

	testfile.read( &m_buffer[0], nLength );

	m_pos = 0;

	return true;
}

} // namespace amorphous



#endif		/*  __stream_buffer_HPP__  */
