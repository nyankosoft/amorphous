#ifndef __OggVorbisAux_HPP__
#define __OggVorbisAux_HPP__


#include <string>
#include <vorbis/vorbisfile.h>


class CSerializableStream;
class stream_buffer;


#ifdef _DEBUG
	#pragma comment( lib, "libogg_static_d.lib" )
	#pragma comment( lib, "libvorbis_static_d.lib" )
	#pragma comment( lib, "libvorbisfile_static_d.lib" )
#else
	#pragma comment( lib, "libogg_static.lib" )
	#pragma comment( lib, "libvorbis_static.lib" )
	#pragma comment( lib, "libvorbisfile_static.lib" )
#endif


extern bool LoadOggVorbisSoundFromDisk( const std::string& resource_path, OggVorbis_File& sOggVorbisFile );

extern bool LoadOggVorbisSoundFromDisk( const std::string& resource_path,
					                    CSerializableStream& src_stream,
						                OggVorbis_File& sOggVorbisFile );

extern unsigned long DecodeOggVorbis( OggVorbis_File *psOggVorbisFile, unsigned long ulChannels, stream_buffer& dest_buffer );

extern unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);



#endif /* __OggVorbisAux_HPP__ */
