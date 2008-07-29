#include "OpenALSoundSourceImpl.h"
#include "Support/SafeDelete.h"
#include "Support/thread_starter.h"
#include "../base.h"

#include "OpenALSoundManagerImpl.h"

using namespace std;
using namespace boost;




/**

 - Looping streamed sound
   - alSourcei( source, AL_LOOPING,1 ) alone does not do the job.
     - Why?
	   - Probably because the whole audio data is not stored in a single buffer.
	   - Need to restart decoding the first data of the encoded buffer


 - threading
   - stream sound source has its own thread (stream sound thread)
     - only the stream sound thread calls OpenAL APIs
	 - e.g.) Play(), Stop() calls will change the requested state
	   and the requested state is processed by the stream sound thread
*/


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
m_SourceType(CSoundSource::Type_Non3DSound),
m_uiSource(0),
m_pManager(NULL)
{
}


COpenALSoundSourceImpl::~COpenALSoundSourceImpl()
{
	ReleaseSource();
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



void COpenALSoundSourceImpl::CreateSource()
{
	alGetError();
	if( alIsSource( m_uiSource ) == AL_TRUE )
		return; // already created

	alGetError();
	alGenSources( 1, &m_uiSource );

	ALenum ret = alGetError();
	if( ret != AL_NO_ERROR )
	{
		LOG_PRINT_ERROR( " alGenSources() failed." );
		m_uiSource = 0;
	}
}


void COpenALSoundSourceImpl::ReleaseSource()
{
	alDeleteSources( 1, &m_uiSource );
}


//====================================================================================
// COpenALStreamedSoundSourceImpl
//====================================================================================

COpenALStreamedSoundSourceImpl::COpenALStreamedSoundSourceImpl()
:
m_NumBuffersForStreaming( NUM_DEFAULT_BUFFERS_FOR_STREAMING ),
m_ServiceUpdatePeriodMS( DEFAULT_SERVICE_UPDATE_PERIOD_MS ),
m_StreamMethod( StreamFromDisk ),
m_ExitStreamThread(false),
m_Loop(false)
{
}


COpenALStreamedSoundSourceImpl::~COpenALStreamedSoundSourceImpl()
{
	EndStreamThread();
}


void COpenALStreamedSoundSourceImpl::OnCreated()
{
	COpenALSoundSourceImpl::OnCreated();

	EndStreamThread();

	m_ExitStreamThread = false;

	m_pThread = start_thread<COpenALStreamedSoundSourceImpl>( this );
}


void COpenALStreamedSoundSourceImpl::EndStreamThread()
{
	if( m_pThread )
	{
		m_ExitStreamThread = true;
		Stop( 0.0 );
		m_pThread->join();
		m_pThread.reset();
	}
}


void COpenALStreamedSoundSourceImpl::OnReleased()
{
	EndStreamThread();

	COpenALSoundSourceImpl::OnReleased();
}


void COpenALStreamedSoundSourceImpl::Release()
{
	m_pManager->m_StreamedSourceImplPool.release( this );
}


void COpenALStreamedSoundSourceImpl::Play( double fadein_time )
{
	SetRequestedState( CSoundSource::State_Playing );
}


void COpenALStreamedSoundSourceImpl::Stop( double fadeout_time )
{
	SetRequestedState( CSoundSource::State_Stopped );
}


void COpenALStreamedSoundSourceImpl::Pause( double fadeout_time )
{
	SetRequestedState( CSoundSource::State_Paused );
}


void COpenALStreamedSoundSourceImpl::Resume( double fadein_time )
{
	SetRequestedState( CSoundSource::State_Playing );
}


void COpenALStreamedSoundSourceImpl::SetLoop( bool loop )
{
	m_Loop = loop;
}

/*
void COpenALStreamedSoundSourceImpl::StartStreamThread()
{
	m_pThread = start_thread<COpenALStreamedSoundSourceImpl>( this );
}
*/

bool COpenALStreamedSoundSourceImpl::OpenOrLoadOggResource( const std::string& resource_path,
														    OggVorbis_File& sOggVorbisFile,
															stream_buffer& src_buffer )
{
	bool resource_loaded = false;
	switch( m_StreamMethod )
	{
	case StreamFromDisk:   resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, sOggVorbisFile ); break;
	case StreamFromMemory: resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, src_buffer, sOggVorbisFile ); break;
	default:
		resource_loaded = false;
	}

	return resource_loaded;
}


void COpenALStreamedSoundSourceImpl::StreamMain()
{
	while( !m_ExitStreamThread )
	{
		Sleep( m_ServiceUpdatePeriodMS );

		if( GetRequestedState() == CSoundSource::State_Playing )
		{
			PlayStream();
		}
	}
}



int COpenALStreamedSoundSourceImpl::PlayStream()
{
	std::string resource_path = m_ResourcePath;

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

	resource_loaded = OpenOrLoadOggResource( resource_path, sOggVorbisFile, src_buffer );

	// sound data is decoded from either src_buffer or a file

	if( !resource_loaded )
	{
		LOG_PRINT_ERROR( " LoadOggVorbisSoundFromDisk() failed." );
		return 1;
	}

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
	alSourcePlay(m_uiSource);

	iTotalBuffersProcessed = 0;

	ALint state;
	while( GetRequestedState() != CSoundSource::State_Stopped )
	{
		alGetSourcei( m_uiSource, AL_SOURCE_STATE, &state );
		if( GetRequestedState() == CSoundSource::State_Paused
		 && state == AL_PLAYING )
		{
			// pause
			alSourcePause( m_uiSource );
		}
		else if( GetRequestedState() == CSoundSource::State_Playing
		 && state == AL_PAUSED )
		{
			// resume
			alSourcePlay( m_uiSource );

		}

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
			else if( m_Loop )
			{
				// no more data to decode
				// - reached the end of the resource
				// - start again from the beginning
				ov_clear( &sOggVorbisFile );
				OpenOrLoadOggResource( resource_path, sOggVorbisFile, src_buffer );
				ulBytesWritten = DecodeOggVorbis( &sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels );
				if( ulBytesWritten )
				{
					alBufferData(uiBuffer, ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);
					alSourceQueueBuffers(m_uiSource, 1, &uiBuffer);
				}
				else
				{
					LOG_PRINT_ERROR( "Cannot loop the sound: " + resource_path );
				}
			}

			iBuffersProcessed--;
		}

		// Check the status of the Source.  If it is not playing, then playback was completed,
		// or the Source was starved of audio data, and needs to be restarted.
		alGetSourcei(m_uiSource, AL_SOURCE_STATE, &iState);
		if( iState != AL_PLAYING
		 && GetRequestedState() == CSoundSource::State_Playing )
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

	// Stop the Source and clear the Queue
	alSourceStop(m_uiSource);
	alSourcei(m_uiSource, AL_BUFFER, 0);

	// Clean up buffers and sources
	alDeleteSources( 1, &m_uiSource );
	alDeleteBuffers( m_NumBuffersForStreaming, m_uiBuffers );

	// Release the temporary buffer to store decoded data
	SafeDeleteArray( pDecodeBuffer );

	// Close OggVorbis stream
	ov_clear(&sOggVorbisFile);

	// end the stream thread
//	m_ExitStreamThread = true;

    return 0;
}



//====================================================================================
// COpenALNonStreamedSoundSourceImpl
//====================================================================================

void COpenALNonStreamedSoundSourceImpl::Release()
{
	m_pManager->m_NonStreamedSourceImplPool.release( this );
}


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


void COpenALNonStreamedSoundSourceImpl::SetLoop( bool loop )
{
	alSourcei( m_uiSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE );
}
