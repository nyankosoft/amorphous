#ifndef  __SoundManager_H__
#define  __SoundManager_H__

#include <string>
#include <map>

#include "SoundHandle.hpp"
#include "SoundDesc.hpp"
#include "SoundManagerImpl.hpp"
#include "../base.hpp"
#include "../Support/Singleton.hpp"


namespace amorphous
{


class CSoundManager
{
private:

	CSoundManagerImpl *m_pSoundManagerImpl;

	bool m_bMute;

	std::map<std::string,CSoundSource *> m_mapNameToSoundSource;

protected:

	CSoundManager();		// singleton

	inline bool UpdateSoundHandle( CSoundHandle& sound_handle );

protected:

	static CSingleton<CSoundManager> m_obj;

public:

	static CSoundManager* Get() { return m_obj.get(); }

	~CSoundManager();

	void Release();

	bool Init( const std::string& library_name = "OpenAL" );

	bool LoadSoundsFromList( const std::string& sound_list_file );

	/// Simplified interface for playing a 3D sound
	/// - Plays a sound at a specified position
	/// - The sound is automatically released after being played
	inline void PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist = 1000.0f, float ref_dist = 100.0f, float rolloff_factor = 1.0f );

	/// Simplified interface for playing a 3D sound
	/// - Plays a sound at a specified position
	inline void PlayAt( const std::string& resource_path, const Vector3& vPosition, float max_dist = 1000.0f, float ref_dist = 100.0f, float rolloff_factor = 1.0f );

	/// Simplified interface for playing a non-3D sound
	/// - The sound is automatically released after being played
	inline void Play( CSoundHandle& sound_handle );

	/// Simplified interface for playing a non-3D sound
	/// - The sound is automatically released after being played
	inline void Play( const std::string& resource_path );

	/// General interface for playing a 3D / non-3D sound
	/// \param sound_handle [in] sound resource
	/// \param desc [in] sound properties (position, volume, 3D / Non-3D, looped or not, range, etc.)
	inline void Play( CSoundHandle& sound_handle, const CSoundDesc& desc );

	/// A convenience function for playing a non-3D, streamed sound
	/// - Plays a non-3D, streamed sound
	/// - The sound is automatically released after being played
	bool PlayStream( const std::string& resource_path, double fadein_time = 0.0, bool looped = false, int sound_group = 0, U8 volume = 0xFF );

	bool StopStream( const std::string& resource_path, double fadeout_time = 0.0 );

	/// create a sound source
	/// - Caller is responsible for releasing the sound by ReleaseSoundSource()
	/// \param type [in] 3D / non 3D sound
	/// \param stream_type [in] streamed / non-streamed sound
	inline CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
		                                    const CSoundDesc& desc );

	inline CSoundSource *CreateSoundSource( const std::string& resource_path,
		                                    const CSoundDesc& desc );

	inline CSoundSource *CreateSoundSource( CSoundHandle& sound_handle,
		                                    CSoundSource::Type type,
											CSoundSource::StreamType stream_type );

	inline CSoundSource *CreateSoundSource( const std::string& resource_path,
		                                    CSoundSource::Type type,
											CSoundSource::StreamType stream_type );

	inline void ReleaseSoundSource( CSoundSource*& pSoundSource );


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

	friend class CSingleton<CSoundManager>;
};


//================================= inline implementations ============================================

inline void CSoundManager::PlayAt( const std::string& resource_path, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	CSoundHandle sound_handle( resource_path );
	m_pSoundManagerImpl->PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void CSoundManager::PlayAt( CSoundHandle& sound_handle, const Vector3& vPosition, float max_dist, float ref_dist, float rolloff_factor )
{
	m_pSoundManagerImpl->PlayAt( sound_handle, vPosition, max_dist, ref_dist, rolloff_factor );
}


inline void CSoundManager::Play( CSoundHandle& sound_handle )
{
	m_pSoundManagerImpl->Play( sound_handle );
}


inline void CSoundManager::Play( const std::string& resource_path )
{
	CSoundHandle sound_handle( resource_path );
	m_pSoundManagerImpl->Play( sound_handle );
}


inline void CSoundManager::Play( CSoundHandle& sound_handle, const CSoundDesc& desc )
{
	m_pSoundManagerImpl->Play( sound_handle, desc );
}


inline CSoundSource *CSoundManager::CreateSoundSource( CSoundHandle& sound_handle,
	                                                   const CSoundDesc& desc )
{
	return m_pSoundManagerImpl->CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CSoundManager::CreateSoundSource( const std::string& resource_path,
	                                                   const CSoundDesc& desc )
{
	CSoundHandle sound_handle( resource_path );
	return m_pSoundManagerImpl->CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CSoundManager::CreateSoundSource( CSoundHandle& sound_handle,
		                                               CSoundSource::Type type,
											           CSoundSource::StreamType stream_type )
{
	CSoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == CSoundSource::Streamed );

	return CreateSoundSource( sound_handle, desc );
}


inline CSoundSource *CSoundManager::CreateSoundSource( const std::string& resource_path,
		                                               CSoundSource::Type type,
											           CSoundSource::StreamType stream_type )
{
	CSoundDesc desc;
	desc.SoundSourceType = type;
	desc.Streamed = ( stream_type == CSoundSource::Streamed );

	return CreateSoundSource( resource_path, desc );
}


inline void CSoundManager::ReleaseSoundSource( CSoundSource*& pSoundSource )
{
	m_pSoundManagerImpl->ReleaseSoundSource( pSoundSource );
}


inline void CSoundManager::PauseAllSounds()
{
	m_pSoundManagerImpl->PauseAllSounds();
}


inline void CSoundManager::ResumeAllSounds()
{
	m_pSoundManagerImpl->ResumeAllSounds();
}


inline void CSoundManager::SetVolume( int volume_group, uint volume )
{
	m_pSoundManagerImpl->SetVolume( volume_group, volume );
}


inline void CSoundManager::SetListenerPosition( const Vector3& vPosition )
{
	m_pSoundManagerImpl->SetListenerPosition( vPosition );
}


inline void CSoundManager::SetListenerPose( const Vector3& vPosition,
											const Vector3& vLookAtDirection,
		                                    const Vector3& vUp )
{
	m_pSoundManagerImpl->SetListenerPose( vPosition, vLookAtDirection, vUp );
}


inline void CSoundManager::SetListenerPose( const Matrix34& pose )
{
	m_pSoundManagerImpl->SetListenerPose(
		pose.vPosition,
		pose.matOrient.GetColumn(2),
		pose.matOrient.GetColumn(1)
		);
}


inline void CSoundManager::SetListenerVelocity( const Vector3& vVelocity )
{
	m_pSoundManagerImpl->SetListenerVelocity( vVelocity );
}


inline void CSoundManager::CommitDeferredSettings()
{
	m_pSoundManagerImpl->CommitDeferredSettings();
}


inline void CSoundManager::GetTextInfo( std::string& dest_buffer )
{
	m_pSoundManagerImpl->GetTextInfo( dest_buffer );
}


inline CSoundManager& SoundManager()
{
	return *(CSoundManager::Get());
}

} // namespace amorphous



#endif		/*  __SoundManager_H__  */
