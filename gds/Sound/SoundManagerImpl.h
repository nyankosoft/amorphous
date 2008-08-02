#ifndef  __SoundManagerImpl_H__
#define  __SoundManagerImpl_H__


#include "SoundHandle.h"
#include "SoundDesc.h"
#include "3DMath/Matrix34.h"


class CSoundSourceImpl;


class CSoundManagerImpl
{
protected:

	void SetImpl( CSoundSource *pSource, CSoundSourceImpl *pImpl ) { pSource->SetImpl( pImpl ); }

	CSoundSourceImpl *GetImpl( CSoundSource *pSource ) { return pSource->GetImpl(); }

	void SetInternalSoundHandle( CSoundHandle& sound_handle, const pooled_object_handle& handle ) { sound_handle.m_Handle = handle; }

	pooled_object_handle& GetInternalSoundHandle( CSoundHandle& handle ) { return handle.m_Handle; }

public:

	CSoundManagerImpl() {}

	virtual ~CSoundManagerImpl() {}

	virtual void Release() {}

	virtual bool Init() = 0;

	/// plays a sound at a specified position
	virtual void PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor ) = 0;

	/// plays non-3D sound
	virtual void Play( CSoundHandle& sound_handle ) = 0;

	/// plays 3D or non-3D sound
	virtual void Play( CSoundHandle& sound_handle, const CSoundDesc& desc ) = 0;

	virtual void PlayStream( CSoundHandle& sound_handle ) = 0;

	virtual CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
		                                     const CSoundDesc& desc ) = 0;

	virtual void ReleaseSoundSource( CSoundSource*& pSoundSource ) = 0;

	virtual void PauseAllSounds() = 0;

	virtual void ResumeAllSounds() = 0;

	virtual void SetVolume( int volume_group, uint volume ) = 0;

	virtual void SetListenerPosition( const Vector3& vPosition ) = 0;

	virtual void SetListenerPose( const Vector3& vPosition,
		                          const Vector3& vLookAtDirection,
                                  const Vector3& vUp ) = 0;

	virtual void SetListenerVelocity( const Vector3& vVelocity ) = 0;

	/// DirectSound needs a function like this
	virtual void CommitDeferredSettings() {}

	virtual void GetTextInfo( char *pDestBuffer ) {}

//	virtual CSoundSourceImpl *CreateSoundSourceImpl( CSoundSource::Type type, CSoundSource::StreamType stream_type ) = 0;
};


class CNullSoundManagerImpl : public CSoundManagerImpl
{
public:

	// TODO: add empty implementations of pure virtual functions declared by CSoundManagerImpl 

	CNullSoundManagerImpl() {}

	~CNullSoundManagerImpl() {}

	void Release() {}

	bool Init() { return false; }

	void PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor ) {}

	void Play( CSoundHandle& sound_handle ) {}

	void Play( CSoundHandle& sound_handle, const CSoundDesc& desc ) {}

	void PlayStream( CSoundHandle& sound_handle ) {}

	CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
	                                 const CSoundDesc& desc ) { return NULL; }

	void ReleaseSoundSource( CSoundSource*& pSoundSource ) {}

	void PauseAllSounds() {}

	void ResumeAllSounds() {}

	void SetVolume( int volume_group, uint volume ) {}

	void SetListenerPosition( const Vector3& vPosition ) {}

	void SetListenerPose( const Vector3& vPosition,
	                      const Vector3& vLookAtDirection,
                          const Vector3& vUp ) {}

	void SetListenerVelocity( const Vector3& vVelocity ) {}

//	CSoundSourceImpl *CreateSoundSourceImpl( CSoundSource::Type type, CSoundSource::StreamType stream_type ) { return NULL; }
};


#endif		/*  __SoundManagerImpl_H__  */
