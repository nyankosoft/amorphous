#ifndef  __OpenALSoundManagerImpl_H__
#define  __OpenALSoundManagerImpl_H__


#include "Sound/SoundManagerImpl.hpp"
#include "Sound/OpenALSoundSourceImpl.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <al.h>
#include <alut.h>
#include <list>
#include <map>

#include "amorphous/Support/prealloc_pool.hpp"


namespace amorphous
{


#pragma comment( lib, "OpenAL32.lib" )
#pragma comment( lib, "alut.lib" )


/// Can be used by more than one SoundSource object
/// - Not used by CStreamSound
class SoundBuffer
{
	std::string m_ResourcePath;

	ALuint m_uiBuffer;

	int m_SoundGroup; ///< defualt sound group of the source attached to this buffer (music, effect, etc.)


	int m_StockIndex; ///< used by prealloc_pool
	int m_StockID;    ///< used by prealloc_pool

private:

	bool LoadOggVorbisSoundFromDisk( const std::string& resource_path );

public:

	SoundBuffer();

	~SoundBuffer();

	void Release();

	ALuint GetBufferID() { return m_uiBuffer; }

	bool LoadFromDisk( const std::string& resource_path );


	int GetStockIndex() const { return m_StockIndex; }
	void SetStockIndex( int index ) { m_StockIndex = index; }

	int GetStockID() const { return m_StockID; }
	void SetStockID( int id ) { m_StockID = id; }

	friend class OpenALSoundManagerImpl;
};


class OpenALSoundManagerImpl : public SoundManagerImpl
{
	enum Params
	{
		NUM_DEFAULT_SOUND_BUFFERS = 256,

		/// Under default settings done by alutInit(), my PC can create only 30 sources
		/// How to use them for sound sources?
		/// - 20 for non-streamed, short sound
		/// -  5 for streamed sound (such as background music)
		/// -> Changed: call alGenSources() in Play() / CreateSoundSource()
		NUM_DEFAULT_SOUND_SOURCES = 25,

		NUM_DEFAULT_STREAMED_SOURCE_IMPLS = 2,

		NUM_DEFAULT_NONSTREAMED_SOURCE_IMPLS = 20,
	};

	/// for non-streamed sounds
	prealloc_pool<SoundBuffer> m_SoundBufferPool;

	/// stores borrowed references of sound sources currently being played
	std::list<SoundSource *> m_ActiveSoundList;

	/// Stores borrowed references of sound buffers
	/// key: resource path (filename or db::keyname)
	std::map< std::string, SoundBuffer *> m_ActiveSoundBuffer;

	prealloc_pool<SoundSource> m_SoundSourcePool;

	prealloc_pool<COpenALStreamedSoundSourceImpl> m_StreamedSourceImplPool;

	prealloc_pool<COpenALNonStreamedSoundSourceImpl> m_NonStreamedSourceImplPool;

	Matrix34 m_ListenerPose;
	Vector3 m_vListenerVelocity;

	std::vector<U8> m_vecVolume;

	// thread to release finished sound sources

	boost::shared_ptr<boost::thread> m_pThread;

	boost::mutex m_SourceListLock;

	bool m_ExitSoundManagerThread;

	struct ThreadStarter
	{
	public:

		ThreadStarter( OpenALSoundManagerImpl *p ) : pTarget(p) {}

		OpenALSoundManagerImpl *pTarget;

		void operator()()
		{
			pTarget->ThreadMain();
		}
	};

private:

	COpenALSoundSourceImpl *GetSoundSourceImpl( SoundSource::StreamType stream_type );

	void DetachImpl( SoundSource* pSoundSource );

	SoundBuffer *GetSoundBuffer( SoundHandle& sound_handle );

	void AddToActiveSourceList( SoundSource *pSource );

public:

	OpenALSoundManagerImpl();

	~OpenALSoundManagerImpl();

	bool Init();

	void Release();

	/// preload sound data
	/// - Use this to load sound played throughout your app
	///   - e.g.) sound played when a button is pressed in a GUI menu
	bool LoadNonStreamedSound( const std::string& resource_path, int sound_group = 0 )
	{
		SoundHandle handle( resource_path );
		return LoadNonStreamedSound( handle, sound_group );
	}

	bool LoadNonStreamedSound( SoundHandle& sound_handle, int sound_group = 0 );

	void ReleaseNonStreamedSound( const std::string& resource_path );

	/// plays a non-streamed sound at a specified position
	void PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor );

	/// plays non-3D sound
	void Play( SoundHandle& sound_handle );

	/// plays 3D or non-3D sound
	void Play( SoundHandle& sound_handle, const SoundDesc& desc );

	void PlayStream( SoundHandle& sound_handle );

	SoundSource *CreateSoundSource( SoundHandle& sound_handle,
		                             const SoundDesc& desc );

	void ReleaseSoundSource( SoundSource*& pSoundSource );

	void PauseAllSounds();

	void ResumeAllSounds();

	void SetVolume( int volume_group, uint volume );

	void SetListenerPosition( const Vector3& vPosition );

	void SetListenerPose( const Vector3& vPosition,
		                  const Vector3& vLookAtDirection,
                          const Vector3& vUp
						  );

	void SetListenerVelocity( const Vector3& vVelocity );

	void Update();

//	void CommitDeferredSettings()

	void GetTextInfo( std::string& dest_buffer );

//	SoundSourceImpl *CreateSoundSourceImpl( SoundSource::Type type, SoundSource::StreamType stream_type );

	void ThreadMain();

	friend class COpenALStreamedSoundSourceImpl;
	friend class COpenALNonStreamedSoundSourceImpl;
};

} // namespace amorphous



#endif /* __OpenALSoundManagerImpl_H__ */
