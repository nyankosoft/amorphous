#include "OpenALSoundManagerImpl.h"
#include "Support/Macro.h"
#include "Support/stream_buffer.h"
#include "Support/Log/DefaultLog.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

#include <vorbis/vorbisfile.h>



const static IDAndTextPair g_OpenALErrors[] =
{
	ID_AND_TEXT( AL_NO_ERROR ),
	ID_AND_TEXT( AL_INVALID_NAME ),
	ID_AND_TEXT( AL_INVALID_ENUM ),
	ID_AND_TEXT( AL_INVALID_VALUE ),
	ID_AND_TEXT( AL_INVALID_OPERATION ),
	ID_AND_TEXT( AL_OUT_OF_MEMORY )
};


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
static unsigned long DecodeOggVorbis( OggVorbis_File *psOggVorbisFile, unsigned long ulChannels, stream_buffer& dest_buffer )
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
/// \param [out] src_buffer buffer to store the loaded sound
/// \param [out] sOggVorbisFile contains callback function and pointer to src_buffer. They get registered for decoding the ogg file
bool LoadOggVorbisSoundFromDisk( const std::string& resource_path,
					             stream_buffer& src_buffer,
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

		bool retrieved = db.GetData( keyname, src_buffer );
		if( !retrieved )
		{
			LOG_PRINT_ERROR( "Could not find data with the key: " + keyname );
			return false;
		}
	}
	else
	{
		// load from a single file
		src_buffer.LoadBinaryStream( resource_path );
	}

	// Open Ogg Stream
	ov_callbacks	sCallbacks;
	sCallbacks.read_func  = ov_read_from_buffer;
	sCallbacks.seek_func  = ov_seek_on_buffer;
	sCallbacks.close_func = ov_close_buffer;
	sCallbacks.tell_func  = ov_tell_on_buffer;

	void *pDataSource = &src_buffer;

	// Create an OggVorbis file stream from file
	int result = ov_open_callbacks( pDataSource, &sOggVorbisFile, NULL, 0, sCallbacks );
	
	if( result != 0 )
	{
		LOG_PRINT_ERROR( "ov_open_callbacks() failed. resource: " + resource_path );
		return false;
	}

	return true;
}



//====================================================================================
// CSoundBuffer
//====================================================================================

CSoundBuffer::CSoundBuffer()
:
m_uiBuffer(0),
m_SoundGroup(0)
{
	// create a single buffer
	alGetError();
	alGenBuffers( 1, &m_uiBuffer );
	LOG_PRINT_AL_ERROR();
}


CSoundBuffer::~CSoundBuffer()
{
	Release();
}


void CSoundBuffer::Release()
{
	alDeleteBuffers( 1, &m_uiBuffer );
}


bool CSoundBuffer::LoadFromDisk( const std::string& resource_path )
{
	m_ResourcePath = resource_path;

	if( resource_path.length() == 0 )
	{
		LOG_PRINT_ERROR( "An invalid resource path (an empty string)" );
		return false;
	}

	if( fnop::get_ext(resource_path) == "ogg" )
	{
		// treat this as either of the following
		// - An ogg vorbis file
		// - An archived ogg vorbis file in a db
		return LoadOggVorbisSoundFromDisk( resource_path );
	}
	else
	{
		LOG_PRINT_ERROR( "A sound resource of unsupported type: " + resource_path );
		return false;
	}
}


bool CSoundBuffer::LoadOggVorbisSoundFromDisk( const std::string& resource_path )
{
	// create as a single buffer and a single source
	stream_buffer src_buffer;
	OggVorbis_File ogg_vorbis_file;

	// load on memory
	bool loaded = ::LoadOggVorbisSoundFromDisk( resource_path, src_buffer, ogg_vorbis_file );
	if( !loaded )
		return false;

	// encoded ogg vorbis file has been loaded to src_buffer
	// and it's ready to be decoded.

	ulong ulFormat, ulFrequency, ulChannels, ulBufferSize;

	bool res = GetFormatFrequencyChannelsBufferSize( ogg_vorbis_file, ulFormat, ulFrequency, ulChannels, ulBufferSize );
	if( !res )
		return false;

	stream_buffer decoded_buffer;
	ulong bytes_written = DecodeOggVorbis( &ogg_vorbis_file, ulChannels, decoded_buffer );

	// check if the current buffer is valid
	alGetError();
	alIsBuffer( m_uiBuffer );
	LOG_PRINT_AL_ERROR();


	// send the decoded sound data to the AL buffer
	alGetError();
	alBufferData( m_uiBuffer, ulFormat, &(decoded_buffer.buffer()[0]), bytes_written, ulFrequency );

	ALenum ret = alGetError();
	if( ret != AL_NO_ERROR )
		LOG_PRINT_ERROR( GET_TEXT_FROM_ID( ret, g_OpenALErrors ) );

	// Close OggVorbis stream
	ov_clear(&ogg_vorbis_file);

	return true;
}



//====================================================================================
// COpenALSoundManagerImpl
//====================================================================================

COpenALSoundManagerImpl::COpenALSoundManagerImpl()
:
m_ListenerPose( Matrix34Identity() ),
m_vListenerVelocity( Vector3(0,0,0) ),
m_ExitSoundManagerThread(false)
{}


COpenALSoundManagerImpl::~COpenALSoundManagerImpl()
{
	// Do not call Release() here.
	// - Release() is called through CSoundManager.
//	Release();
}


bool COpenALSoundManagerImpl::Init()
{
//	int error;

	// Init openAL
	alGetError();
	alutInit(0, NULL);
	LOG_PRINT_AL_ERROR();

	// Clear Error Code (so we can catch any new errors)
	alGetError();

	// - returns the handle to a buffer that contains loaded sound?

	m_SoundBufferPool.init( NUM_DEFAULT_SOUND_BUFFERS );

	m_SoundSourcePool.init( NUM_DEFAULT_SOUND_SOURCES );

	m_StreamedSourceImplPool.init( NUM_DEFAULT_SOUND_SOURCES );

	m_NonStreamedSourceImplPool.init( NUM_DEFAULT_NONSTREAMED_SOURCE_IMPLS );

	SetListenerPosition( Vector3(0,0,0) );
	SetListenerVelocity( Vector3(0,0,0) );

	// start the thread for sound manager
	ThreadStarter starter(this);
	m_pThread = shared_ptr<thread>( new thread(starter) );

	return true;
}


/// creates a sound buffer and store the requested sound in the buffer
bool COpenALSoundManagerImpl::LoadNonStreamedSound( CSoundHandle& sound_handle, int sound_group )
{
	const string& resource_path = sound_handle.GetResourceName();

	// search the buffers to see if the resource is already loaded
	map<string,CSoundBuffer *>::iterator itr = m_ActiveSoundBuffer.find( resource_path );
	if( itr != m_ActiveSoundBuffer.end() )
	{
		// create a handle from an existing sound buffer
		SetInternalSoundHandle( sound_handle, m_SoundBufferPool.get_handle( itr->second ) );
		// sound_handle.m_Handle = m_SoundBufferPool.get_handle( itr->second );
		return true;
	}
	else
	{
		CSoundBuffer *pNewBuffer = m_SoundBufferPool.get_new_object();
		bool loaded = pNewBuffer->LoadFromDisk( resource_path );
		if( loaded )
		{
			m_ActiveSoundBuffer[resource_path] = pNewBuffer;

			// create a handle from the new sound buffer
			SetInternalSoundHandle( sound_handle, m_SoundBufferPool.get_handle( pNewBuffer ) );
			// sound_handle.m_Handle = m_SoundBufferPool.get_handle( pNewBuffer );
			return true;
		}
		else
		{
			return false;
		}
	}
}


void COpenALSoundManagerImpl::Release()
{
	// release the thread
	m_ExitSoundManagerThread = true;
	m_pThread->join();
	m_pThread.reset();

	alutExit();
}


COpenALSoundSourceImpl *COpenALSoundManagerImpl::GetSoundSourceImpl( CSoundSource::StreamType stream_type )
{
	COpenALSoundSourceImpl *pImpl = NULL;
	switch(stream_type)
	{
	case CSoundSource::Streamed:    pImpl = m_StreamedSourceImplPool.get_new_object();    break;
	case CSoundSource::NonStreamed: pImpl = m_NonStreamedSourceImplPool.get_new_object(); break;
	default:
		break;
	}

	if( pImpl )
	{
		pImpl->m_pManager = this;
		return pImpl;
	}
	else
		return NULL;

}


CSoundBuffer *COpenALSoundManagerImpl::GetSoundBuffer( CSoundHandle& sound_handle )
{
	const string& resource_path = sound_handle.GetResourceName();
	CSoundBuffer *pBuffer = m_SoundBufferPool.get( GetInternalSoundHandle( sound_handle ) );
	if( !pBuffer )
	{
		// see if the requested sound has already been loaded
		map<string,CSoundBuffer *>::iterator itrBuffer = m_ActiveSoundBuffer.find( resource_path );
		if( itrBuffer == m_ActiveSoundBuffer.end() )
		{
			bool loaded = LoadNonStreamedSound( sound_handle );
			if( !loaded )
				return NULL;

			itrBuffer = m_ActiveSoundBuffer.find( resource_path );
		}

		// get the sound buffer that contains requested sound data
		pBuffer = itrBuffer->second;
	}

	return pBuffer;
}


void COpenALSoundManagerImpl::ThreadMain()
{
	while( !m_ExitSoundManagerThread )
	{
		Update();

		Sleep( 100 );
	}
}


void COpenALSoundManagerImpl::PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	CSoundDesc desc;
	desc.Position          = vPosition;
	desc.MaxDistance       = max_dist;
	desc.ReferenceDistance = ref_dist;
	desc.RollOffFactor     = rolloff_factor;

	CSoundSource *pSound = CreateSoundSource( sound_handle, desc );
	if( pSound )
		pSound->Play();

/*
	CSoundBuffer *pBuffer = GetSoundBuffer( sound_handle );
	if( !pBuffer )
		return;

	// take a sound source currently not in use (borrowed reference)
	CSoundSource *pSource = m_SoundSourcePool.get_new_object();

	// take a sound source impl currently not in use (borrowed reference)
	COpenALSoundSourceImpl *pImpl = GetSoundSourceImpl( CSoundSource::NonStreamed );

	// set sound source type (3D / non 3D sound)
	pImpl->m_SourceType = CSoundSource::Type_3DSound;

	// automatically release the sound after playing it
	pImpl->m_ManagementType = CSoundSource::Auto;

	// attach source to buffer
	alSourcei( pImpl->m_uiSource, AL_BUFFER, pBuffer->m_uiBuffer );

//	pImpl->AttachToBuffer( pBuffer );

	// attach impl
	SetImpl( pSource, pImpl );

	pSource->SetPosition( vPosition );

	pSource->Play();

	m_ActiveSoundList.push_back( pSource );
*/
}


/// plays non-3D sound
void COpenALSoundManagerImpl::Play( CSoundHandle& sound_handle )
{
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

	CSoundBuffer *pBuffer = GetSoundBuffer( sound_handle );
	if( !pBuffer )
		return;

	// take a sound source currently not in use
	CSoundSource *pSource = NULL;
	COpenALSoundSourceImpl *pImpl = NULL;

	// take a sound source currently not in use
	pSource = m_SoundSourcePool.get_new_object();
	if( !pSource )
		return;

	// get an impl
	pImpl = GetSoundSourceImpl( CSoundSource::NonStreamed );
	if( !pImpl )
		return;

	AddToActiveSourceList( pSource );

	// create OpenAL sound source
	// - do nothing if source already exists
	pImpl->CreateSource();

	// must be manually released by the caller
	pImpl->m_ManagementType = CSoundSource::Auto;

	// Always play the sound at the position of the listener
	pImpl->SetPosition( m_ListenerPose.vPosition );

	// clear any previous error
	alGetError();

	// attach source to buffer
	alSourcei( pImpl->m_uiSource, AL_BUFFER, pBuffer->m_uiBuffer );

//	LOG_PRINT_AL_ERROR();
	ALenum ret = alGetError();
	if( ret != AL_NO_ERROR )
		LOG_PRINT_ERROR( string("alSourcei() failed. - ") + GET_TEXT_FROM_ID( ret, g_OpenALErrors ) );

	// attach the impl to the source
	SetImpl( pSource, pImpl );

	pSource->Play();

	return;
}


/// plays 3D or non-3D sound
void COpenALSoundManagerImpl::Play( CSoundHandle& sound_handle, const CSoundDesc& desc )
{
}


void COpenALSoundManagerImpl::PlayStream( CSoundHandle& sound_handle )
{
//	loaded = LoadStreamedSound( resource_path );
}


CSoundSource *COpenALSoundManagerImpl::CreateSoundSource( CSoundHandle& sound_handle,
		                                                  const CSoundDesc& desc )
{
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

	CSoundBuffer *pBuffer = NULL;
	
	if( !desc.Streamed )
	{
		pBuffer = GetSoundBuffer( sound_handle );
		if( !pBuffer )
			return NULL;
	}

	// take a sound source currently not in use
	CSoundSource *pSource = NULL;
	COpenALSoundSourceImpl *pImpl = NULL;

	pSource = m_SoundSourcePool.get_new_object();
	if( !pSource )
		return NULL;

	// get an impl
	CSoundSource::StreamType stream_type = desc.Streamed ? CSoundSource::Streamed : CSoundSource::NonStreamed;
	pImpl = GetSoundSourceImpl( stream_type );

	if( !pImpl )
		return NULL;

	AddToActiveSourceList( pSource );

	pImpl->CreateSource();

	// must be manually released by the caller
	pImpl->m_ManagementType = CSoundSource::Manual;

	pImpl->SetResourcePath( sound_handle.GetResourceName() );

	if( desc.SoundSourceType == CSoundSource::Type_3DSound )
	{
		pImpl->SetPosition( desc.Position );
//		pImpl->SetMaxDistance( desc.MaxDistance );
//		pImpl->SetReferenceDistance( desc.ReferenceDistance );
//		pImpl->SetRollOffFactor( desc.RollOffFactor );
		alSourcef( pImpl->m_uiSource, AL_MAX_DISTANCE,       desc.MaxDistance );
		alSourcef( pImpl->m_uiSource, AL_REFERENCE_DISTANCE, desc.ReferenceDistance );
		alSourcef( pImpl->m_uiSource, AL_ROLLOFF_FACTOR,     desc.RollOffFactor );
	}
	else
	{
		pImpl->SetPosition( m_ListenerPose.vPosition );
	}

	// stream sound: the stream sound thread is created and started in this call.
	// non-streamed sound: does nothing
	pImpl->OnCreated();

	// attach source to buffer
	if( !desc.Streamed )
		alSourcei( pImpl->m_uiSource, AL_BUFFER, pBuffer->m_uiBuffer );

	LOG_PRINT_AL_ERROR();

	// attach the impl to the source
	SetImpl( pSource, pImpl );

	pSource->SetLoop( desc.Loop );

	return pSource;
}


void COpenALSoundManagerImpl::AddToActiveSourceList( CSoundSource *pSource )
{
//	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

	m_ActiveSoundList.push_back( pSource );
}


void COpenALSoundManagerImpl::DetachImpl( CSoundSource* pSoundSource )
{
	CSoundSourceImpl *pImpl = GetImpl( pSoundSource );

	pImpl->Stop( 0.0f );

	// detach the impl
	SetImpl( pSoundSource, NULL );
}


void COpenALSoundManagerImpl::ReleaseSoundSource( CSoundSource*& pSoundSource )
{
	if( !pSoundSource )
		return;

	// mark the released flag to true
	pSoundSource->OnReleased();

//	DetachImpl( pSoundSource ); - Dont do it here. Done in Update()

//	m_SoundSourcePool.release( pSoundSource ); - Dont do it here. Done in Update()

	pSoundSource = NULL;

	// detached later from m_ActiveSoundList
}


void COpenALSoundManagerImpl::PauseAllSounds()
{
	list<CSoundSource *>::iterator itr;
	for( itr = m_ActiveSoundList.begin();
		itr != m_ActiveSoundList.end();
		itr++ )
	{
		(*itr)->Pause();
	}
}


void COpenALSoundManagerImpl::ResumeAllSounds()
{
	list<CSoundSource *>::iterator itr;
	for( itr = m_ActiveSoundList.begin();
		itr != m_ActiveSoundList.end();
		itr++ )
	{
		(*itr)->Resume();
	}
}


void COpenALSoundManagerImpl::SetVolume( int volume_group, uint volume )
{
	if( 0 <= volume_group )
	{
		while( (int)m_vecVolume.size() <= volume_group )
			m_vecVolume.push_back( 0xFF );

		m_vecVolume[volume_group] = volume;

		{
			boost::mutex::scoped_lock scoped_lock(m_SourceListLock);
//			for( each active_sound_source )
		}
	}
}


void COpenALSoundManagerImpl::SetListenerPosition( const Vector3& vPosition )
{
	float afListenerPos[3];
	afListenerPos[0] = vPosition.x;
	afListenerPos[1] = vPosition.y;
	afListenerPos[2] = vPosition.z;
	alListenerfv( AL_POSITION, afListenerPos );

	m_ListenerPose.vPosition = vPosition;

	// move non-3D sound source to the listener position?
	{
		boost::mutex::scoped_lock scoped_lock(m_SourceListLock);
//		for(;;)
//		{
//		}
	}
}


void COpenALSoundManagerImpl::SetListenerPose( const Vector3& vPosition,
										       const Vector3& vLookAtDirection,
										       const Vector3& vUp )
{
	SetListenerPosition( vPosition );

	float afListenerOrient[6];
	afListenerOrient[0] = vLookAtDirection.x;
	afListenerOrient[1] = vLookAtDirection.y;
	afListenerOrient[2] = vLookAtDirection.z;
	afListenerOrient[3] = vUp.x;
	afListenerOrient[4] = vUp.y;
	afListenerOrient[5] = vUp.z;

	alListenerfv( AL_ORIENTATION, afListenerOrient );
}


void COpenALSoundManagerImpl::SetListenerVelocity( const Vector3& vVelocity )
{
	float afListenerVel[3];
	afListenerVel[0] = vVelocity.x;
	afListenerVel[1] = vVelocity.y;
	afListenerVel[2] = vVelocity.z;
	alListenerfv( AL_VELOCITY, afListenerVel );
}


void COpenALSoundManagerImpl::Update()
{
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

	list<CSoundSource *>::iterator itr;
	for( itr = m_ActiveSoundList.begin();
		itr != m_ActiveSoundList.end();
		/* Do not increment iterator here - see erase() below */ )
	{
		if( (*itr)->IsDone() )
		{
			// release the pool impl object
			// - calls prealloc_pool<T>::release(), where T is either streamed
			//   or non-streamed sound source class. See implementations
			(*itr)->Release();

			DetachImpl( (*itr) );

			// release routine in the object pool
			m_SoundSourcePool.release( *itr );

			// remove the pointer from the list - This does not release the instance.
			itr = m_ActiveSoundList.erase( itr );
		}
		else
			itr++;
	}
}

/*
CSoundSourceImpl *COpenALSoundManagerImpl::CreateSoundSourceImpl( CSoundSource::Type type, CSoundSource::StreamType stream_type )
{
	COpenALSoundSourceImpl *pImpl;
	switch(stream_type)
	{
	case CSoundSource::Streamed:    pImpl = new COpenALStreamedSoundSourceImpl();    break;
	case CSoundSource::NonStreamed: pImpl = new COpenALNonStreamedSoundSourceImpl(); break;
	default:
		LOG_PRINT_ERROR( " An invalid sound type" );
		return NULL;
	}

//	pImpl->m_pManager = this;

	return pImpl;
}
*/

/*
Based on samples included in OpenAL 1.1 SDK
*/
/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
#include "Vorbis/vorbisfile.h"

int m_NumBuffersForStreaming;   (4)

// Ogg Vorbis

// Ogg Voribis DLL Handle
//HINSTANCE g_hVorbisFileDLL = NULL;

// Functions
void InitVorbisFile();
void ShutdownVorbisFile();
unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
void Swap(short &s1, short &s2);

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
int ov_close_func(void *datasource);
long ov_tell_func(void *datasource);

// Function pointers
typedef int (*LPOVCLEAR)(OggVorbis_File *vf);
typedef long (*LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,int bigendianp,int word,int sgned,int *bitstream);
typedef ogg_int64_t (*LPOVPCMTOTAL)(OggVorbis_File *vf,int i);
typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf,int link);
typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf,int link);
typedef int (*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,char *initial, long ibytes, ov_callbacks callbacks);


bool g_bVorbisInit = false;
void ShutdownVorbisFile()
{
	if (g_hVorbisFileDLL)
	{
		FreeLibrary(g_hVorbisFileDLL);
		g_hVorbisFileDLL = NULL;
	}
	g_bVorbisInit = false;
}
*/
