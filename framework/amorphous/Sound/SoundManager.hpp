#ifndef  __SoundManager_H__
#define  __SoundManager_H__

#include <string>
#include <map>

#include "SoundHandle.hpp"
#include "SoundDesc.hpp"
#include "SoundManagerImpl.hpp"
#include "../base.hpp"
#include "../Support/singleton.hpp"


namespace amorphous
{


class SoundManager
{
private:

	SoundManagerImpl *m_pSoundManagerImpl;

	bool m_bMute;

	std::map<std::string,SoundSource *> m_mapNameToSoundSource;

protected:

	SoundManager();		// singleton

	inline bool UpdateSoundHandle( SoundHandle& sound_handle );

protected:

	static singleton<SoundManager> m_obj;

public:

	static SoundManager* Get() { return m_obj.get(); }

	~SoundManager();

	void Release();

	bool Init( const std::string& library_name = "OpenAL" );

	bool LoadSoundsFromList( const std::string& sound_list_file );

	/// Simplified interface for playing a 3D sound
	/// - Plays a sound at a specified position
	/// - The sound is automatically released after being played
	inline void PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist = 1000.0f, float ref_dist = 100.0f, float rolloff_factor = 1.0f );

	/// Simplified interface for playing a 3D sound
	/// - Plays a sound at a specified position
	inline void PlayAt( const std::string& resource_path, const Vector3& vPosition, float max_dist = 1000.0f, float ref_dist = 100.0f, float rolloff_factor = 1.0f );

	/// Simplified interface for playing a non-3D sound
	/// - The sound is automatically released after being played
	inline void Play( SoundHandle& sound_handle );

	/// Simplified interface for playing a non-3D sound
	/// - The sound is automatically released after being played
	inline void Play( const std::string& resource_path );

	/// General interface for playing a 3D / non-3D sound
	/// \param sound_handle [in] sound resource
	/// \param desc [in] sound properties (position, volume, 3D / Non-3D, looped or not, range, etc.)
	inline void Play( SoundHandle& sound_handle, const SoundDesc& desc );

	/// A convenience function for playing a non-3D, streamed sound
	/// - Plays a non-3D, streamed sound
	/// - The sound is automatically released after being played
	bool PlayStream( const std::string& resource_path, double fadein_time = 0.0, bool looped = false, int sound_group = 0, U8 volume = 0xFF );

	bool StopStream( const std::string& resource_path, double fadeout_time = 0.0 );

	/// create a sound source
	/// - Caller is responsible for releasing the sound by ReleaseSoundSource()
	/// \param type [in] 3D / non 3D sound
	/// \param stream_type [in] streamed / non-streamed sound
	inline SoundSource *CreateSoundSource( SoundHandle& sound_handle,
		                                    const SoundDesc& desc );

	inline SoundSource *CreateSoundSource( const std::string& resource_path,
		                                    const SoundDesc& desc );

	inline SoundSource *CreateSoundSource( SoundHandle& sound_handle,
		                                    SoundSource::Type type,
											SoundSource::StreamType stream_type );

	inline SoundSource *CreateSoundSource( const std::string& resource_path,
		                                    SoundSource::Type type,
											SoundSource::StreamType stream_type );

	inline void ReleaseSoundSource( SoundSource*& pSoundSource );


	inline void PauseAllSounds();

	inline void ResumeAllSounds();

	/// NOT IMPLEMENTED YET
	/// \param volume_group
	/// \param volume [0,255]
	inline void SetVolume( int volume_group, uint volume );


	inline void SetListenerPosition( const Vector3& vPosition );

	inline void SetListenerPose( const Matrix34& pose );

	inline void SetListenerPose( const Vector3& vPosition,
		                         const Vector3& vLookAtDirection,
                                 const Vector3& vUp );

	inline void SetListenerVelocity( const Vector3& vVelocity );

	inline void CommitDeferredSettings();

	inline void SetMute( bool mute ) { m_bMute = mute; }

	inline void GetTextInfo( std::string& dest_buffer );

	friend class singleton<SoundManager>;
};


//================================= inline implementations ============================================

inline void SoundManager::PlayAt( const std::string& resource_path, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	SoundHandle sound_handle( resource_path );
	m_pSoundManagerImpl->PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void SoundManager::PlayAt( SoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	m_pSoundManagerImpl->PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void SoundManager::Play( SoundHandle& sound_handle )
{
	m_pSoundManagerImpl->Play( sound_handle );
}


inline void SoundManager::Play( const std::string& resource_path )
{
	SoundHandle sound_handle( resource_path );
	m_pSoundManagerImpl->Play( sound_handle );
}


inline void SoundManager::Play( SoundHandle& sound_handle, const SoundDesc& desc )
{
	m_pSoundManagerImpl->Play( sound_handle, desc );
}


inline SoundSource *SoundManager::CreateSoundSource( SoundHandle& sound_handle,
	                                                   const SoundDesc& desc )
{
	return m_pSoundManagerImpl->CreateSoundSource( sound_handle, desc );
}


inline SoundSource *SoundManager::CreateSoundSource( const std::string& resource_path,
	                                                   const SoundDesc& desc )
{
	SoundHandle sound_handle( resource_path );
	return m_pSoundManagerImpl->CreateSoundSource( sound_handle, desc );
}


inline SoundSource *SoundManager::CreateSoundSource( SoundHandle& sound_handle,
		                                               SoundSource::Type type,
											           SoundSource::StreamType stream_type )
{
	SoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == SoundSource::Streamed );

	return CreateSoundSource( sound_handle, desc );
}


inline SoundSource *SoundManager::CreateSoundSource( const std::string& resource_path,
		                                               SoundSource::Type type,
											           SoundSource::StreamType stream_type )
{
	SoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == SoundSource::Streamed );

	return CreateSoundSource( resource_path, desc );
}


inline void SoundManager::ReleaseSoundSource( SoundSource*& pSoundSource )
{
	m_pSoundManagerImpl->ReleaseSoundSource( pSoundSource );
}


inline void SoundManager::PauseAllSounds()
{
	m_pSoundManagerImpl->PauseAllSounds();
}


inline void SoundManager::ResumeAllSounds()
{
	m_pSoundManagerImpl->ResumeAllSounds();
}


inline void SoundManager::SetVolume( int volume_group, uint volume )
{
	m_pSoundManagerImpl->SetVolume( volume_group, volume );
}


inline void SoundManager::SetListenerPosition( const Vector3& vPosition )
{
	m_pSoundManagerImpl->SetListenerPosition( vPosition );
}


inline void SoundManager::SetListenerPose( const Vector3& vPosition,
											const Vector3& vLookAtDirection,
		                                    const Vector3& vUp )
{
	m_pSoundManagerImpl->SetListenerPose( vPosition, vLookAtDirection, vUp );
}


inline void SoundManager::SetListenerPose( const Matrix34& pose )
{
	m_pSoundManagerImpl->SetListenerPose(
		pose.vPosition,
		pose.matOrient.GetColumn(2),
		pose.matOrient.GetColumn(1)
		);
}


inline void SoundManager::SetListenerVelocity( const Vector3& vVelocity )
{
	m_pSoundManagerImpl->SetListenerVelocity( vVelocity );
}


inline void SoundManager::CommitDeferredSettings()
{
	m_pSoundManagerImpl->CommitDeferredSettings();
}


inline void SoundManager::GetTextInfo( std::string& dest_buffer )
{
	m_pSoundManagerImpl->GetTextInfo( dest_buffer );
}


inline SoundManager& GetSoundManager()
{
	return *(SoundManager::Get());
}

} // namespace amorphous



#endif		/*  __SoundManager_H__  */
