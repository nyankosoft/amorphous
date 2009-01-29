#ifndef  __DirectSoundManager_H__
#define  __DirectSoundManager_H__

#include "3DMath/Matrix34.h"

#include <string>


// define 'DWORD_PTR' for "dsound.h" in DX9 SDK Summer 2004
#ifndef DWORD_PTR
#define DWORD_PTR DWORD*
#endif

#include <dsound.h>

#include "SoundManagerImpl.h"
#include "SoundHandle.h"
#include "GameSound.h"

#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "dxerr9.lib" )
#pragma comment( lib, "dxguid.lib" )


class CDirectSoundManager : public CSoundManagerImpl
{
private:

	enum param{ NUM_MAX_SOUNDS = 256 };

    LPDIRECTSOUND8 m_pDirectSound;

   /// 3D listener object
	LPDIRECTSOUND3DLISTENER m_pDSListener;

	CGameSound *m_apGameSound[NUM_MAX_SOUNDS];
	int m_iNumGameSounds;

protected:

	CDirectSoundManager();		//singleton

	HRESULT CreateSound( CGameSound** ppSound, LPTSTR strWaveFileName,
		                 DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNumBuffers,
						 float fDefaultMinDist, float fDefaultMaxDist );

	inline bool UpdateSoundHandle( CSoundHandle& rSoundHandle );

public:

	static CDirectSoundManager ms_SingletonInstance_;	//single instance of 'CDirectSoundManager'

	~CDirectSoundManager();

	HRESULT Init( HWND hWnd );

	HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                    DWORD dwPrimaryFreq, 
                                    DWORD dwPrimaryBitRate );

	HRESULT Set3DListenerInterface();

	bool LoadSoundsFromList( const std::string& sound_list_file );

	/// plays a sound at a specified position
	inline void PlayAt( const char* pcSoundName, const Vector3& vPosition );

	/// plays a sound at a specified position
	inline void PlayAt( int index, const Vector3& vPosition );

	/// plays a sound at a specified position
	inline void PlayAt( CSoundHandle& rSoundHandle, const Vector3& vPosition );

	/// plays non-3D sound
	inline void Play( CSoundHandle& rSoundHandle );

	int GetSoundIndex( const char* pcSoundName );

	inline void SetListenerPosition( const Vector3& vPosition );

	inline void SetListenerPose( const Matrix34& pose );

	inline void SetListenerPose( const Vector3& vPosition,
		                            const Vector3& vLookAtDirection,
                                    const Vector3& vUp /*,
									const Vector3& rvVelocity = Vector3(0,0,0) */ );

	inline void SetListenerVelocity( const Vector3& vVelocity );

	inline void CommitDeferredSettings()
	{
		if( !m_pDirectSound )
			return;

		m_pDSListener->CommitDeferredSettings();
	}

	inline void SetMute( bool mute ) { m_bMute = mute; }
};

//================================= inline implementations ============================================


inline bool CDirectSoundManager::UpdateSoundHandle( CSoundHandle& rSoundHandle )
{
	if( rSoundHandle.m_iIndex < 0 )
	{
		if( rSoundHandle.m_iIndex == CSoundHandle::INVALID_INDEX )
		{
            return false;	// sound name is invalid
		}
		else if( rSoundHandle.m_iIndex == CSoundHandle::UNINITIALIZED )
		{
			int index = rSoundHandle.m_iIndex = GetSoundIndex( rSoundHandle.GetSoundName().c_str() );
			if( index == CSoundHandle::INVALID_INDEX )
				return false;	// sound name was not found and is invalid
			else
				return true;
		}
//		else
//			PrintLog( "CDirectSoundManager::UpdateSoundHandle() - invalid sound handle" );
	}
	else
		return true;

	return false;
}


inline void CDirectSoundManager::PlayAt( const char* pcSoundName, const Vector3& vPosition )
{
	if( !m_pDirectSound )
		return;

	if( m_bMute )
		return;

	int i, num_sounds = m_iNumGameSounds;
	for( i=0; i<num_sounds; i++ )
	{
		if( strcmp( pcSoundName, m_apGameSound[i]->GetName() ) == 0 )
		{
			m_apGameSound[i]->PlayAt( m_pDSListener, rvPosition );
		}
	}
}


inline void CDirectSoundManager::PlayAt( int index, const Vector3& vPosition )
{
	if( !m_pDirectSound )
		return;

	if( m_bMute )
		return;

	if( index < 0 || m_iNumGameSounds <= index )
		return;

	m_apGameSound[index]->PlayAt( m_pDSListener, rvPosition );
}


inline void CDirectSoundManager::PlayAt( CSoundHandle& rSoundHandle, const Vector3& vPosition )
{
	if( !UpdateSoundHandle( rSoundHandle ) )
		return;

	PlayAt( rSoundHandle.m_iIndex, rvPosition );
}


inline void CDirectSoundManager::Play( CSoundHandle& rSoundHandle )
{
	if( !m_pDirectSound )
		return;

	if( m_bMute )
		return;

	if( !UpdateSoundHandle( rSoundHandle ) )
		return;

	m_apGameSound[rSoundHandle.m_iIndex]->Play();
}


inline void CDirectSoundManager::SetListenerPosition( const Vector3& vPosition )
{
	m_pDSListener->SetPosition(	rvPosition.x, rvPosition.y, rvPosition.z, DS3D_DEFERRED );
}


inline void CDirectSoundManager::SetListenerPose( const Vector3& vPosition, const Vector3& vLookAtDirection,
		                                       const Vector3& vUp/* , const Vector3& rvVelocity */ )
{
	if( !m_pDirectSound )
		return;

	SetListenerPosition( rvPosition );

	m_pDSListener->SetOrientation(
		vLookAtDirection.x, vLookAtDirection.y, vLookAtDirection.z,
		rvUp.x,		  rvUp.y,		rvUp.z,
		DS3D_DEFERRED );
}


inline void CDirectSoundManager::SetListenerPose( const Matrix34& pose )
{
	if( !m_pDirectSound )
		return;

	SetListenerPosition( pose.vPosition );

	const Vector3& vFront = pose.matOrient.GetColumn( 2 );
	const Vector3& vTop = pose.matOrient.GetColumn( 1 );

	m_pDSListener->SetOrientation( vFront.x, vFront.y, vFront.z, vTop.x, vTop.y, vTop.z, DS3D_DEFERRED ); 
}


inline void CDirectSoundManager::SetListenerVelocity( const Vector3& vVelocity )
{
	if( !m_pDirectSound )
		return;

	m_pDSListener->SetVelocity( vVelocity.x, vVelocity.y, vVelocity.z, DS3D_DEFERRED );
}


#endif		/*  __DirectSoundManager_H__  */
