#include "OpenALSoundSourceImpl.hpp"
#include "OpenALSoundManagerImpl.hpp"
#include "../base.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"
#include "Support/thread_starter.hpp"
#include "Support/SerializableStream.hpp"
#include "OggVorbisAux.hpp"

using namespace std;
using namespace boost;


inline static void sleep_milliseonds( int64_t ms )
{
	boost::this_thread::sleep( boost::posix_time::milliseconds(ms) );
}



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


/// \param [in] ogg_vorbis_file,
/// \param [out] ulFormat,
/// \param [out] ulFrequency,
/// \param [out] ulChannels,
/// \param [out] ulBufferSize
bool GetFormatFrequencyChannelsBufferSize( OggVorbis_File& ogg_vorbis_file,
										   ulong& ulFormat,
										   ulong& ulFrequency,
										   ulong& ulChannels,
										   ulong& ulBufferSize )
{
	ulFormat     = 0;
	ulFrequency  = 0;
	ulChannels   = 0;
	ulBufferSize = 0;

	// Get some information about the file (Channels, Format, and Frequency)
	vorbis_info *psVorbisInfo = ov_info(&ogg_vorbis_file, -1);

	if( !psVorbisInfo )
	{
		LOG_PRINT_ERROR( "ov_info() failed." );
		return false;
	}

	// determine the frequency, channels, and format
	ulFrequency = psVorbisInfo->rate;
	ulChannels  = psVorbisInfo->channels;
	if (psVorbisInfo->channels == 1)
	{
		ulFormat = AL_FORMAT_MONO16;
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		ulBufferSize = ulFrequency >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		ulBufferSize -= (ulBufferSize % 2);
	}
	else if (psVorbisInfo->channels == 2)
	{
		ulFormat = AL_FORMAT_STEREO16;
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		ulBufferSize = ulFrequency;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		ulBufferSize -= (ulBufferSize % 4);
	}
	else if (psVorbisInfo->channels == 4)
	{
		ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		ulBufferSize = ulFrequency * 2;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		ulBufferSize -= (ulBufferSize % 8);
	}
	else if (psVorbisInfo->channels == 6)
	{
		ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
		// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		ulBufferSize = ulFrequency * 3;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		ulBufferSize -= (ulBufferSize % 12);
	}
	else
	{
		LOG_PRINT_ERROR( "An unsupported number of channels. (channels: " + to_string(psVorbisInfo->channels) + ")" );
		return false;
	}

	return true;
}


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
	m_uiSource = 0;
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
m_Loop(false),
m_NumTotalBuffersProcessed(0)
{
	for( int i=0; i<NUM_MAX_BUFFERS_FOR_STREAMING; i++ )
		m_uiBuffers[i] = 0;
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
															CSerializableStream& src_stream )
{
	bool resource_loaded = false;
	switch( m_StreamMethod )
	{
	case StreamFromDisk:   resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, sOggVorbisFile ); break;
	case StreamFromMemory: resource_loaded = LoadOggVorbisSoundFromDisk( resource_path, src_stream, sOggVorbisFile ); break;
	default:
		resource_loaded = false;
	}

	return resource_loaded;
}


void COpenALStreamedSoundSourceImpl::StreamMain()
{
	while( !m_ExitStreamThread )
	{
		sleep_milliseonds( m_ServiceUpdatePeriodMS );

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
	ALint   iBuffersProcessed;
	ulong   ulFrequency = 0;
	ulong   ulFormat = 0;
	ulong   ulChannels = 0;
	ulong   ulBufferSize = 0;
	ulong   ulBytesWritten = 0;

	m_NumTotalBuffersProcessed = 0;

	// Open Ogg Stream
	CSerializableStream src_stream;
	OggVorbis_File sOggVorbisFile;

	bool resource_loaded = false;

	resource_loaded = OpenOrLoadOggResource( resource_path, sOggVorbisFile, src_stream );

	// sound data is decoded from either src_buffer or a file

	if( !resource_loaded )
	{
		LOG_PRINT_ERROR( " LoadOggVorbisSoundFromDisk() failed." );
		return 1;
	}

	GetFormatFrequencyChannelsBufferSize( sOggVorbisFile, ulFormat, ulFrequency, ulChannels, ulBufferSize );

	if( ulBufferSize == 0 )
		return 1;

	if( ulFormat == 0 )
	{
		LOG_PRINT_ERROR( " Failed to find format information, or unsupported format." );
		return 1;
	}

	// temporary buffer to store decoded data
	char *pDecodeBuffer = new char [ulBufferSize];

	alGetError();

	// Generate some AL Buffers for streaming
	alGenBuffers( m_NumBuffersForStreaming, m_uiBuffers );

	ALenum al_error = alGetError();
	if( al_error != AL_NO_ERROR )
		LOG_PRINT_ERROR( "alGenBuffers() failed." );

	// Check if the current source is valid
	if( alIsSource( m_uiSource ) == AL_FALSE )
		LOG_PRINT_ERROR( "An invalid sound source." );


	// Fill all the Buffers with decoded audio data from the OggVorbis file
	for ( int iLoop = 0; iLoop < m_NumBuffersForStreaming; iLoop++ )
	{
		ulBytesWritten = DecodeOggVorbis( &sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels );
		if (ulBytesWritten)
		{
			alBufferData( m_uiBuffers[iLoop], ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency );

			al_error = alGetError();
			if( al_error != AL_NO_ERROR )
				LOG_PRINT_ERROR( "alBufferData() failed. Error: " + string(GET_TEXT_FROM_ID(al_error,g_OpenALErrors)) );

			alSourceQueueBuffers( m_uiSource, 1, &m_uiBuffers[iLoop] );

			al_error = alGetError();
			if( al_error != AL_NO_ERROR )
				LOG_PRINT_ERROR( "alSourceQueueBuffers() failed. Error: " + string(GET_TEXT_FROM_ID(al_error,g_OpenALErrors)) );
		}
	}

	// Start playing source
	alGetError();
	alSourcePlay(m_uiSource);

	al_error = alGetError();
	if( al_error != AL_NO_ERROR )
		LOG_PRINT_ERROR( "alSourcePlay() failed. Error: " + string(GET_TEXT_FROM_ID(al_error,g_OpenALErrors)) );

	m_NumTotalBuffersProcessed = 0;

	ALint state;
	while( GetRequestedState() != CSoundSource::State_Stopped )
	{
		sleep_milliseonds( m_ServiceUpdatePeriodMS );

		alGetSourcei( m_uiSource, AL_SOURCE_STATE, &state );
		if( GetRequestedState() == CSoundSource::State_Paused
		 && state == AL_PLAYING )
		{
			// pause
			alSourcePause( m_uiSource );
			while( GetRequestedState() == CSoundSource::State_Paused )
			{
				sleep_milliseonds( m_ServiceUpdatePeriodMS );
			}

			if( GetRequestedState() == CSoundSource::State_Playing )
				alSourcePlay( m_uiSource );

			// Sometimes sound is not correctly played after this
			// - The sound turns into some continuous noise
			// - It also affects other sound sources
			//   - e.g., When this bug happens, UI event sounds played afterwards gets some noise

			// What about sleeping here a little?
			sleep_milliseonds( 100 );
		}

/*		if( GetRequestedState() == CSoundSource::State_Playing
		 && state == AL_PAUSED )
		{
			// resume
			alSourcePlay( m_uiSource );

		}
*/
		// Request the number of OpenAL Buffers have been processed (played) on the Source
		iBuffersProcessed = 0;
		alGetSourcei(m_uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

		// Keep a running count of number of buffers processed (for logging purposes only)
		m_NumTotalBuffersProcessed += iBuffersProcessed;
//		ALFWprintf("Buffers Processed %d\r", m_NumTotalBuffersProcessed);

		// For each processed buffer, remove it from the Source Queue, read next chunk of audio
		// data from disk, fill buffer with new data, and add it to the Source Queue
		while (iBuffersProcessed)
		{
			// Remove the Buffer from the Queue. (uiBuffer contains the Buffer ID for the unqueued Buffer)
			uiBuffer = 0;
			alSourceUnqueueBuffers(m_uiSource, 1, &uiBuffer);

			// Read more audio data (if there is any)
			ulBytesWritten = DecodeOggVorbis( &sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels );
			alGetError();
			if (ulBytesWritten)
			{
				alBufferData(uiBuffer, ulFormat, pDecodeBuffer, ulBytesWritten, ulFrequency);

				al_error = alGetError();
				if( al_error != AL_NO_ERROR )
					LOG_PRINT_ERROR( "alBufferData() failed. Error: " + string(GET_TEXT_FROM_ID(al_error,g_OpenALErrors)) );

				alSourceQueueBuffers(m_uiSource, 1, &uiBuffer);

				al_error = alGetError();
				if( al_error != AL_NO_ERROR )
					LOG_PRINT_ERROR( "alSourceQueueBuffers() failed. Error: " + string(GET_TEXT_FROM_ID(al_error,g_OpenALErrors)) );
			}
			else if( m_Loop )
			{
				// no more data to decode
				// - reached the end of the resource
				// - start again from the beginning
				ov_clear( &sOggVorbisFile );
				OpenOrLoadOggResource( resource_path, sOggVorbisFile, src_stream );
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
			ALint iQueuedBuffers = 0;
			alGetSourcei(m_uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
			if (iQueuedBuffers)
			{
				// Play( 0.0 );
				alSourcePlay(m_uiSource);
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
	m_uiSource = 0;
	alDeleteBuffers( m_NumBuffersForStreaming, m_uiBuffers );

	// Release the temporary buffer to store decoded data
	SafeDeleteArray( pDecodeBuffer );

	// Close OggVorbis stream
	ov_clear(&sOggVorbisFile);

	// end the stream thread
//	m_ExitStreamThread = true;

    return 0;
}


void COpenALStreamedSoundSourceImpl::GetTextInfo( std::string& dest_buffer )
{
	char buffer[1024];
	sprintf( buffer, "streamed /     source id: %d / resource: %s / req. state: %d / %d buffers processed\n",
		m_uiSource,
		m_ResourcePath.c_str(),
		GetRequestedState(),
		m_NumTotalBuffersProcessed
		);

	dest_buffer = buffer;
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


void COpenALNonStreamedSoundSourceImpl::GetTextInfo( std::string& dest_buffer )
{
	char buffer[1024];
	sprintf( buffer, "non-streamed / source id: %d\n",
		m_uiSource );

	dest_buffer = buffer;
}
