#ifndef  __SoundManagerImpl_H__
#define  __SoundManagerImpl_H__


#include "SoundHandle.hpp"
#include "SoundDesc.hpp"
#include "amorphous/3DMath/Matrix34.hpp"


namespace amorphous
{


class SoundSourceImpl;


class SoundManagerImpl
{
protected:

	void SetImpl( SoundSource *pSource, SoundSourceImpl *pImpl ) { pSource->SetImpl( pImpl ); }

	SoundSourceImpl *GetImpl( SoundSource *pSource ) { return pSource->GetImpl(); }

	void SetInternalSoundHandle( SoundHandle& sound_handle, const pooled_object_handle& handle ) { sound_handle.m_Handle = handle; }

	pooled_object_handle& GetInternalSoundHandle( SoundHandle& handle ) { return handle.m_Handle; }

public:

	SoundManagerImpl() {}

	virtual ~SoundManagerImpl() {}

	virtual void Release() {}

	virtual bool Init() = 0;

	/// plays a sound at a specified position
	virtual void PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor ) = 0;

	/// plays non-3D sound
	virtual void Play( SoundHandle& sound_handle ) = 0;

	/// plays 3D or non-3D sound
	virtual void Play( SoundHandle& sound_handle, const SoundDesc& desc ) = 0;

	virtual void PlayStream( SoundHandle& sound_handle ) = 0;

	virtual SoundSource *CreateSoundSource( SoundHandle& sound_handle,
		                                     const SoundDesc& desc ) = 0;

	virtual void ReleaseSoundSource( SoundSource*& pSoundSource ) = 0;

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

	virtual void GetTextInfo( std::string& dest_buffer ) {}

//	virtual SoundSourceImpl *CreateSoundSourceImpl( SoundSource::Type type, SoundSource::StreamType stream_type ) = 0;
};


class CNullSoundManagerImpl : public SoundManagerImpl
{
public:

	// TODO: add empty implementations of pure virtual functions declared by SoundManagerImpl 

	CNullSoundManagerImpl() {}

	~CNullSoundManagerImpl() {}

	void Release() {}

	bool Init() { return false; }

	void PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor ) {}

	void Play( SoundHandle& sound_handle ) {}

	void Play( SoundHandle& sound_handle, const SoundDesc& desc ) {}

	void PlayStream( SoundHandle& sound_handle ) {}

	SoundSource *CreateSoundSource( SoundHandle& sound_handle,
	                                 const SoundDesc& desc ) { return NULL; }

	void ReleaseSoundSource( SoundSource*& pSoundSource ) {}

	void PauseAllSounds() {}

	void ResumeAllSounds() {}

	void SetVolume( int volume_group, uint volume ) {}

	void SetListenerPosition( const Vector3& vPosition ) {}

	void SetListenerPose( const Vector3& vPosition,
	                      const Vector3& vLookAtDirection,
                          const Vector3& vUp ) {}

	void SetListenerVelocity( const Vector3& vVelocity ) {}

//	SoundSourceImpl *CreateSoundSourceImpl( SoundSource::Type type, SoundSource::StreamType stream_type ) { return NULL; }
};

} // namespace amorphous



#endif		/*  __SoundManagerImpl_H__  */
