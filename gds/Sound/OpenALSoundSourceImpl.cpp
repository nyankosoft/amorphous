#include "OpenALSoundSourceImpl.h"
#include <vorbis/vorbisfile.h>
#include "Support/SafeDelete.h"
#include "../base.h"

using namespace std;
using namespace boost;


extern bool GetFormatFrequencyChannelsBufferSize( OggVorbis_File& ogg_vorbis_file,
										          ulong& ulFormat,
										          ulong& ulFrequency,
										          ulong& ulChannels,
										          ulong& ulBufferSize );

extern bool LoadOggVorbisSoundFromDisk( const std::string& resource_path, OggVorbis_File& sOggVorbisFile );

extern bool LoadOggVorbisSoundFromDisk( const std::string& resource_path,
					                    stream_buffer& src_buffer,
						                OggVorbis_File& sOggVorbisFile );

extern unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);


//====================================================================================
// COpenALSoundSourceImpl
//====================================================================================

COpenALSoundSourceImpl::COpenALSoundSourceImpl()
:
m_Released(false),
m_SourceType(CSoundSource::Type_Non3DSound)
{
	alGenSources( 1, &m_uiSource );
}

COpenALSoundSourceImpl::~COpenALSoundSourceImpl()
{
	alDeleteSources( 1, &m_uiSource );
}

void COpenALSoundSourceImpl::SetPosition( const Vector3& vPosition )
{
	alSource3f( m_uiSource, AL_POSITION, vPosition.x, vPosition.y, vPosition.z );
}


void COpenALSoundSourceImpl::SetVelocity( const Vector3& vVelocity )
{
	alSource3f( m_uiSource, AL_VELOCITY, vVelocity.x, vVelocity.y, vVelocity.z );
}


void COpenALSoundSourceImpl::SetPose( const Matrix34& pose )
{
	SetPosition( pose.vPosition );

	Vector3 vDirection = pose.matOrient.GetColumn(2);
	alSource3f( m_uiSource, AL_DIRECTION, vDirection.x, vDirection.y, vDirection.z );
}


void COpenALSoundSourceImpl::SetLoop( bool loop )
{
	alSourcei( m_uiSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
}


bool COpenALSoundSourceImpl::IsDone()
{
	if( m_ManagementType == CSoundSource::Manual )
	{
		// manual sound source has been released by the user
		return ( m_Released ? true : false );
	}
	else
	{
		// m_ManagementType == CSoundSource::Auto
		ALint state;
		alGetSourcei( m_uiSource, AL_SOURCE_STATE, &state );
		if( state == AL_STOPPED )
			return true;
		else
			return false;
	}
}


void COpenALSoundSourceImpl::OnReleased()
{
	m_Released = true;
}


CSoundSource::State COpenALSoundSourceImpl::GetState()
{
	ALint state;
	alGetSourcei( m_uiSource, AL_SOURCE_STATE, &state );

	switch( state )
	{
	case AL_PLAYING: return CSoundSource::State_Playing;
	case AL_STOPPED: return CSoundSource::State_Stopped;
	case AL_PAUSED:  return CSoundSource::State_Paused;
	default:         return CSoundSource::State_Invalid;
	}
}



//====================================================================================
// COpenALStreamedSoundSourceImpl
//====================================================================================

COpenALStreamedSoundSourceImpl::COpenALStreamedSoundSourceImpl()
:
m_NumBuffersForStreaming( NUM_DEFAULT_BUFFERS_FOR_STREAMING ),
m_ServiceUpdatePeriodMS( DEFAULT_SERVICE_UPDATE_PERIOD_MS ),
m_StreamMethod( StreamFromDisk )
{
}


void COpenALStreamedSoundSourceImpl::Play( double fadein_time )
{
	if( alIsSource( m_uiSource ) == AL_TRUE )
	{
		boost::mutex::scoped_lock scoped_lock(m_SoundOperationMutex);

		alSourcePlay( m_uiSource );
	}
}


void COpenALStreamedSoundSourceImpl::Stop( double fadeout_time )
{
}


void COpenALStreamedSoundSourceImpl::Pause( double fadeout_time )
{
	if( alIsSource( m_uiSource ) == AL_TRUE )
	{
		boost::mutex::scoped_lock scoped_lock(m_SoundOperationMutex);

		alSourcePause( m_uiSource );
	}
}


void COpenALStreamedSoundSourceImpl::Resume( double fadein_time )
{
}


int COpenALStreamedSoundSourceImpl::StreamMain()
{
	std::string resource_path;

	ALuint  uiBuffer;
	ALint   iState;
	ALint   iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;
	ulong   ulFrequency = 0;
	ulong   ulFormat = 0;
	ulong   ulChannels = 0;
	ulong   ulBufferSize;
	ulong   ulBytesWritten;

	// Open Ogg Stream
	stream_buffer src_buffer;
	OggVorbis_File sOggVorbisFile;

	bool resource_loaded = false;  
	switch( m_StreamMethod )
	{
	case StreamFromDisk:   resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, sOggVorbisFile ); break;
	case StreamFromMemory: resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, src_buffer, sOggVorbisFile ); break;
	default:
		break;
	}

	// sound data is decoded from either src_buffer or a file

	if( !resource_loaded )
		return 1;

	GetFormatFrequencyChannelsBufferSize( sOggVorbisFile, ulFormat, ulFrequency, ulChannels, ulBufferSize );

	// temporary buffer to store decoded data
	char *pDecodeBuffer = new char [ulBufferSize];

	if( ulFormat == 0 )
	{
		LOG_PRINT_ERROR( " Failed to find format information, or unsupported format." );
		return 1;
	}

	// Generate some AL Buffers for streaming
	alGenBuffers( m_NumBuffersForStreaming, m_uiBuffers );

	// Generate a Source to playback the Buffers
	// - Created in ctor
//	alGenSources( 1, &m_uiSource );

	// Fill all the Buffers with decoded audio data from the OggVorbis file
	for ( int iLoop = 0; iLoop < m_NumBuffersForStreaming; iLoop++ )
	{
		ulBytesWritten = DecodeOggVorbis( &sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels );
		if (ulBytesWritten)
		{
			alBufferData( m_uiBuffers[iLoop], ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency );
			alSourceQueueBuffers( m_uiSource, 1, &m_uiBuffers[iLoop] );
		}
	}

	// Start playing source
	Play( 0.0 );
//	alSourcePlay(m_uiSource);

	iTotalBuffersProcessed = 0;

	bool play = true;
	while( play/*!ALFWKeyPress()*/)
	{
		Sleep( m_ServiceUpdatePeriodMS );

		// Request the number of OpenAL Buffers have been processed (played) on the Source
		iBuffersProcessed = 0;
		alGetSourcei(m_uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

		// Keep a running count of number of buffers processed (for logging purposes only)
		iTotalBuffersProcessed += iBuffersProcessed;
//		ALFWprintf("Buffers Processed %d\r", iTotalBuffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (iBuffersProcessed)
		{
			// Remove the Buffer from the Queue. (uiBuffer contains the Buffer ID for the unqueued Buffer)
			uiBuffer = 0;
			alSourceUnqueueBuffers(m_uiSource, 1, &uiBuffer);

			// Read more audio data (if there is any)
			ulBytesWritten = DecodeOggVorbis( &sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels );
			if (ulBytesWritten)
			{
				alBufferData(uiBuffer, ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
				alSourceQueueBuffers(m_uiSource, 1, &uiBuffer);
			}

			iBuffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);
		if (iState != AL_PLAYING)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(m_uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
			if (iQueuedBuffers)
			{
				Play( 0.0 );
				// alSourcePlay(m_uiSource);
			}
			else
			{
				// Finished playing
				break;
			}
		}
	}

	// access source
	// - Need lock?
	{
		boost::mutex::scoped_lock scoped_lock(m_SoundOperationMutex);

		// Stop the Source and clear the Queue
		alSourceStop(m_uiSource);
		alSourcei(m_uiSource, AL_BUFFER, 0);

		// Clean up buffers and sources
		alDeleteSources( 1, &m_uiSource );
		alDeleteBuffers( m_NumBuffersForStreaming, m_uiBuffers );
	}

	// Release the temporary buffer to store decoded data
	SafeDeleteArray( pDecodeBuffer );

	// Close OggVorbis stream
	ov_clear(&sOggVorbisFile);

    return 0;
}



//====================================================================================
// COpenALNonStreamedSoundSourceImpl
//====================================================================================

void COpenALNonStreamedSoundSourceImpl::Play( double fadein_time )
{
	alSourcePlay( m_uiSource );
}


void COpenALNonStreamedSoundSourceImpl::Stop( double fadeout_time )
{
	alSourceStop( m_uiSource );
}


void COpenALNonStreamedSoundSourceImpl::Pause( double fadeout_time )
{
	alSourcePause( m_uiSource );
}


void COpenALNonStreamedSoundSourceImpl::Resume( double fadein_time )
{
	alSourcePlay( m_uiSource );
}
