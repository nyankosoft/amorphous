#include "OpenALSoundManagerImpl.hpp"
#include "OggVorbisAux.hpp"
#include "Support/Macro.h"
#include "Support/SerializableStream.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{
using namespace serialization;

using namespace std;
using namespace boost;



const IDAndTextPair g_OpenALErrors[6] =
{
	ID_AND_TEXT( AL_NO_ERROR ),
	ID_AND_TEXT( AL_INVALID_NAME ),
	ID_AND_TEXT( AL_INVALID_ENUM ),
	ID_AND_TEXT( AL_INVALID_VALUE ),
	ID_AND_TEXT( AL_INVALID_OPERATION ),
	ID_AND_TEXT( AL_OUT_OF_MEMORY )
};


inline static void ReportALError( const char *fname )
{
	ALenum al_error = alGetError();
	if( al_error != AL_NO_ERROR )
	{
		LOG_PRINT_ERROR( string(fname) + "() An OpenAL error (code: " + to_string(al_error) + ")." );
	}
}

#define REPORT_AL_ERROR() ReportALError(__FUNCTION__)


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

	if( lfs::get_ext(resource_path) == "ogg" )
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
	SerializableStream src_buffer;
	OggVorbis_File ogg_vorbis_file;

	// load on memory
	bool loaded = amorphous::LoadOggVorbisSoundFromDisk( resource_path, src_buffer, ogg_vorbis_file );
	if( !loaded )
		return false;

	LOG_PRINT( fmt_string(" Loaded an ogg vorbis sound: %s (loaded data size: %d)", resource_path.c_str(), src_buffer.m_Buffer.get_buffer().size()) );

	// encoded ogg vorbis file has been loaded to src_buffer
	// and it's ready to be decoded.

	ulong ulFormat = 0, ulFrequency = 0, ulChannels = 0, ulBufferSize = 0;

	bool res = GetFormatFrequencyChannelsBufferSize( ogg_vorbis_file, ulFormat, ulFrequency, ulChannels, ulBufferSize );
	if( !res )
		return false;

	stream_buffer decoded_buffer;
	ulong bytes_written = DecodeOggVorbis( &ogg_vorbis_file, ulChannels, decoded_buffer );

	LOG_PRINT( fmt_string(" Decoded an ogg vorbis sound: %s (decoded data size: %d)", resource_path.c_str(), decoded_buffer.get_buffer().size()) );

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
	m_pThread.reset( new thread(starter) );

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

		boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
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


/// Just mark the sound source as 'released'
/// - COpenALSoundManagerImpl::Update() later releases the sound source
void COpenALSoundManagerImpl::ReleaseSoundSource( CSoundSource*& pSoundSource )
{
	if( !pSoundSource )
		return;

	// mark the released flag to true
	pSoundSource->OnReleased();

	pSoundSource = NULL;

	// detached later from m_ActiveSoundList
}


void COpenALSoundManagerImpl::PauseAllSounds()
{
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

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
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

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
	alGetError();

	float afListenerPos[3];
	afListenerPos[0] = vPosition.x;
	afListenerPos[1] = vPosition.y;
	afListenerPos[2] = vPosition.z;
	alListenerfv( AL_POSITION, afListenerPos );

	REPORT_AL_ERROR();

	m_ListenerPose.vPosition = vPosition;

	// move non-3D sound source to the listener position?
/*	{
		boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

		list<CSoundSource *>::iterator itr;
		for( itr = m_ActiveSoundList.begin();
			itr != m_ActiveSoundList.end();
			itr++)
		{
			if( itr->GetType() == CSoundSource::Type_Non3DSound )
				itr->SetPosition( m_ListenerPose.vPosition );
		}
	}*/
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


void COpenALSoundManagerImpl::GetTextInfo( std::string& dest_buffer )
{
	boost::mutex::scoped_lock scoped_lock(m_SourceListLock);

	const size_t num_buffers = m_ActiveSoundBuffer.size();

	dest_buffer = fmt_string( "Sound Buffers (%d in total)\n----------------------------------------\n", num_buffers );

	map< string, CSoundBuffer *>::iterator itr;
	for( itr =  m_ActiveSoundBuffer.begin();
		 itr != m_ActiveSoundBuffer.end();
		 itr++ )
	{
		dest_buffer += "name: ";
		dest_buffer += itr->second->m_ResourcePath;
		dest_buffer += " / buffer id: ";
		dest_buffer += to_string(itr->second->m_uiBuffer);
		dest_buffer += "\n";
	}

	dest_buffer += "\n";

	string text_buffer;
	const size_t num_sources = m_ActiveSoundList.size();

	dest_buffer += fmt_string( "Sound Sources (%d in total)\n----------------------------------------\n", num_sources );

	std::list<CSoundSource *>::iterator itrSource;
	for( itrSource =  m_ActiveSoundList.begin();
		 itrSource != m_ActiveSoundList.end();
		 itrSource++ )
	{
		(*itrSource)->GetTextInfo( text_buffer );
		dest_buffer += text_buffer;
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

*/


} // namespace amorphous
