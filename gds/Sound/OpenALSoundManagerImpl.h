#ifndef  __OpenALSoundManagerImpl_H__
#define  __OpenALSoundManagerImpl_H__


#include "Sound/OpenALSoundSourceImpl.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <al.h>
#include <alut.h>
#include <list>
#include <map>

#include "Support/prealloc_pool.h"


#pragma comment( lib, "OpenAL32.lib" )
#pragma comment( lib, "alut.lib" )


#ifdef _DEBUG
	#pragma comment( lib, "ogg_static_d.lib" )
	#pragma comment( lib, "vorbis_static_d.lib" )
	#pragma comment( lib, "vorbisfile_static_d.lib" )
#else
	#pragma comment( lib, "ogg_static.lib" )
	#pragma comment( lib, "vorbis_static.lib" )
	#pragma comment( lib, "vorbisfile_static.lib" )
#endif

/// Can by used by more than one CSoundSource object
/// - Not used by CStreamSound
class CSoundBuffer
{
	std::string m_ResourcePath;

	ALuint m_uiBuffer;


	int m_StockIndex; ///< used by prealloc_pool
	int m_StockID;    ///< used by prealloc_pool

private:

	bool LoadOggVorbisSoundFromDisk( const std::string& resource_path );

public:

	CSoundBuffer();

	~CSoundBuffer();

	void Release();

	ALuint GetBufferID() { return m_uiBuffer; }

	bool LoadFromDisk( const std::string& resource_path );


	int GetStockIndex() const { return m_StockIndex; }
	void SetStockIndex( int index ) { m_StockIndex = index; }

	int GetStockID() const { return m_StockID; }
	void SetStockID( int id ) { m_StockID = id; }

	friend class COpenALSoundManagerImpl;
};


class COpenALSoundManagerImpl : public CSoundManagerImpl
{
	enum Params
	{
		NUM_DEFAULT_SOUND_BUFFERS = 256,
		NUM_DEFAULT_SOUND_SOURCES = 256,
		NUM_DEFAULT_STREAMED_SOURCE_IMPLS = 16,
		NUM_DEFAULT_NONSTREAMED_SOURCE_IMPLS = 256,
	};

	/// for non-streamed sounds
	prealloc_pool<CSoundBuffer> m_SoundBufferPool;

	/// stores borrowed references of sound sources currently being played
	std::list<CSoundSource *> m_ActiveSoundList;

	/// Stores borrowed references of sound buffers
	/// key: resource path (filename or db::keyname)
	std::map< std::string, CSoundBuffer *> m_ActiveSoundBuffer;

	prealloc_pool<CSoundSource> m_SoundSourcePool;

	prealloc_pool<COpenALStreamedSoundSourceImpl> m_StreamedSourceImplPool;

	prealloc_pool<COpenALNonStreamedSoundSourceImpl> m_NonStreamedSourceImplPool;

	Matrix34 m_ListenerPose;
	Vector3 m_vListenerVelocity;

private:

	COpenALSoundSourceImpl *GetSoundSourceImpl( CSoundSource::StreamType stream_type );

	void DetachImpl( CSoundSource* pSoundSource );

	CSoundBuffer *COpenALSoundManagerImpl::GetSoundBuffer( CSoundHandle& sound_handle );

public:

	COpenALSoundManagerImpl()
		:
	m_ListenerPose( Matrix34Identity() ),
	m_vListenerVelocity( Vector3(0,0,0) )
	{}

	~COpenALSoundManagerImpl() { Release(); }

	bool Init();

	void Release();

	/// preload sound data
	/// - Use this to load sound played throughout your app
	///   - e.g.) sound played when a button is pressed in a GUI menu
	bool LoadNonStreamedSound( const std::string& resource_path, int sound_group = 0 )
	{
		CSoundHandle handle( resource_path );
		return LoadNonStreamedSound( handle, sound_group );
	}

	bool LoadNonStreamedSound( CSoundHandle& sound_handle, int sound_group = 0 );

	void ReleaseNonStreamedSound( const std::string& resource_path );

	/// plays a non-streamed sound at a specified position
	void PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor );

	/// plays non-3D sound
	void Play( CSoundHandle& sound_handle );

	/// plays 3D or non-3D sound
	void Play( CSoundHandle& sound_handle, const CSoundDesc& desc );

	void PlayStream( CSoundHandle& sound_handle );

	CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
		                             const CSoundDesc& desc );

	void ReleaseSoundSource( CSoundSource*& pSoundSource );

	void PauseAllSounds();

	void ResumeAllSounds();


	void SetListenerPosition( const Vector3& vPosition );

	void SetListenerPose( const Vector3& vPosition,
		                  const Vector3& vLookAtDirection,
                          const Vector3& vUp
						  );

	void SetListenerVelocity( const Vector3& vVelocity );

	void Update();

//	void CommitDeferredSettings()

	CSoundSourceImpl *CreateSoundSourceImpl( CSoundSource::Type type, CSoundSource::StreamType stream_type );
};


#endif /* __OpenALSoundManagerImpl_H__ */
