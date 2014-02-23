#include "OggVorbisAux.hpp"
#include "../base.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/SerializableStream.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{
using namespace serialization;

using namespace std;
using namespace boost;


static inline void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

size_t ov_read_from_file( void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

int ov_seek_on_file( void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

int ov_close_file( void *datasource )
{
   return fclose((FILE*)datasource);
}

long ov_tell_on_file( void *datasource )
{
	return ftell((FILE*)datasource);
}


size_t ov_read_from_buffer( void *ptr, size_t size, size_t nmemb, void *datasource )
{
	return ((stream_buffer *)datasource)->read( ptr, (int)(size * nmemb) );
}

int ov_seek_on_buffer( void *datasource, ogg_int64_t offset, int whence )
{
	stream_buffer *pStreamBuffer = (stream_buffer *)datasource;
	switch(whence)
	{
	case SEEK_SET: pStreamBuffer->seek_pos( (int)offset ); break;
	case SEEK_CUR: pStreamBuffer->seek_pos( pStreamBuffer->get_current_pos() + (int)offset ); break;
	case SEEK_END: pStreamBuffer->seek_pos( (int)pStreamBuffer->buffer().size() + (int)offset ); break;
	default:
		return 0;
	}

	return 0;
}

int ov_close_buffer( void *datasource )
{
	return 0;
}

long ov_tell_on_buffer( void *datasource )
{
	return ((stream_buffer *)datasource)->get_current_pos();
}


static void ReorderOggVorbisSamples( ulong ulChannels, void* pBuffer, ulong ulBufferSize )
{
	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	unsigned long ulSamples;
	short *pSamples;
	if (ulChannels == 6)
	{		
		pSamples = (short*)pBuffer;
		for (ulSamples = 0; ulSamples < (ulBufferSize>>1); ulSamples+=6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			Swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
			Swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
			Swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
		}
	}
}


/// for short, non-streamed sound
/// - decode the entire sound data with a single call of this function
///   and store the decoded data in a single buffer
/// \param psOggVorbisFile [in]	
unsigned long DecodeOggVorbis( OggVorbis_File *psOggVorbisFile, unsigned long ulChannels, stream_buffer& dest_buffer )
{
	int current_section;
	ulong bytes_read = 0;
	ulong total_bytes_read = 0;

	static const int s_BufferSize = 1024;
	char buffer[s_BufferSize];
	while(1)
	{
		bytes_read = ov_read( psOggVorbisFile, buffer, s_BufferSize, 0, 2, 1, &current_section );
		if( 0 < bytes_read )
		{
			dest_buffer.write( buffer, bytes_read );
			total_bytes_read += bytes_read;
		}
		else
		{
			break;
		}
	}

	ReorderOggVorbisSamples( ulChannels, &(dest_buffer.buffer()[0]), total_bytes_read );

	return total_bytes_read;
}


/// for treamed sound
/// - decode up to ulBufferSize bytes of sound data and store the result to the dest buffer
unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
	int current_section;
	long lDecodeSize;

	ulong ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	ReorderOggVorbisSamples( ulChannels, pDecodeBuffer, ulBufferSize );

	return ulBytesDone;
}


/// Opens the file and prepare to load
/// - Does not load the resource on the memory. Just opens the file and get it ready for loading
bool LoadOggVorbisSoundFromDisk( const std::string& resource_path, OggVorbis_File& sOggVorbisFile )
{
	if( is_db_filepath_and_keyname(resource_path) )
		return false;

	// load from a single file
	ov_callbacks	sCallbacks;
	sCallbacks.read_func  = ov_read_from_file;
	sCallbacks.seek_func  = ov_seek_on_file;
	sCallbacks.close_func = ov_close_file;
	sCallbacks.tell_func  = ov_tell_on_file;

	// Open the OggVorbis file
	FILE *pOggVorbisFile = fopen( resource_path.c_str(), "rb" );
	if ( !pOggVorbisFile )
	{
		LOG_PRINT_ERROR( "Could not find open: " + resource_path );
		return false;
	}
	
	int result = ov_open_callbacks( pOggVorbisFile, &sOggVorbisFile, NULL, 0, sCallbacks );

	return true;
}


/// Loads the sound resource from the disk and stores it on memory
/// \param [out] sound_stream buffer to store the loaded sound
/// \param [out] sOggVorbisFile contains callback function and pointer to src_buffer. They get registered for decoding the ogg file
bool LoadOggVorbisSoundFromDisk( const std::string& resource_path,
					             SerializableStream& sound_stream,
						         OggVorbis_File& sOggVorbisFile )
{
	if( is_db_filepath_and_keyname(resource_path) )
	{
		// load from db
		const string db_filepath = "";
		const string keyname = "";
		CBinaryDatabase<std::string> db;
		bool db_open = db.Open( db_filepath );
		if( !db_open )
		{
			return false;
		}

		bool retrieved = db.GetData( keyname, sound_stream );
		if( !retrieved )
		{
			LOG_PRINT_ERROR( "Could not find data with the key: " + keyname );
			return false;
		}
	}
	else
	{
		// load from a single file
		bool loaded = sound_stream.LoadBinaryStream( resource_path );
		if( !loaded )
		{
			LOG_PRINT_ERROR( "Failed to load a file as a binary stream: " + resource_path );
			return false;
		}
	}

	// Open Ogg Stream
	ov_callbacks	sCallbacks;
	sCallbacks.read_func  = ov_read_from_buffer;
	sCallbacks.seek_func  = ov_seek_on_buffer;
	sCallbacks.close_func = ov_close_buffer;
	sCallbacks.tell_func  = ov_tell_on_buffer;

	void *pDataSource = &sound_stream.m_Buffer;

	// Create an OggVorbis file stream from file
	int result = ov_open_callbacks( pDataSource, &sOggVorbisFile, NULL, 0, sCallbacks );
	
	if( result != 0 )
	{
		LOG_PRINT_ERROR( "ov_open_callbacks() failed. resource: " + resource_path );
		return false;
	}

	return true;
}


} // namespace amorphous
