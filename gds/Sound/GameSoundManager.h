#ifndef  __GAMESOUNDMANAGER_H__
#define  __GAMESOUNDMANAGER_H__

#include "3DMath/Matrix34.h"

#include <string>


// define 'DWORD_PTR' for "dsound.h" in DX9 SDK Summer 2004
#ifndef DWORD_PTR
#define DWORD_PTR DWORD*
#endif

#include <dsound.h>

#include "SoundHandle.h"
#include "GameSound.h"

#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "dxerr9.lib" )
#pragma comment( lib, "dxguid.lib" )


#define GAMESOUNDMANAGER ( CGameSoundManager::ms_SingletonInstance_ )

class CGameSound;

class CGameSoundManager
{
private:

	enum param{ NUM_MAX_SOUNDS = 256 };

    LPDIRECTSOUND8 m_pDirectSound;

   /// 3D listener object
	LPDIRECTSOUND3DLISTENER m_pDSListener;

	CGameSound *m_apGameSound[NUM_MAX_SOUNDS];
	int m_iNumGameSounds;

	bool m_bMute;

protected:

	CGameSoundManager();		//singleton

	HRESULT CreateSound( CGameSound** ppSound, LPTSTR strWaveFileName,
		                 DWORD dwCreationFlags, GUID guid3DAlgorithm, DWORD dwNumBuffers,
						 float fDefaultMinDist, float fDefaultMaxDist );

	inline bool UpdateSoundHandle( CSoundHandle& rSoundHandle );

public:

	static CGameSoundManager ms_SingletonInstance_;	//single instance of 'CGameSoundManager'

	~CGameSoundManager();

	HRESULT Init( HWND hWnd );

	HRESULT SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                    DWORD dwPrimaryFreq, 
                                    DWORD dwPrimaryBitRate );

	HRESULT Set3DListenerInterface();

	bool LoadSoundsFromList( const std::string& sound_list_file );

	/// plays a sound at a specified position
	inline void Play3D( const char* pcSoundName, const Vector3& rvPosition );

	/// plays a sound at a specified position
	inline void Play3D( int index, const Vector3& rvPosition );

	/// plays a sound at a specified position
	inline void Play3D( CSoundHandle& rSoundHandle, const Vector3& rvPosition );

	/// plays non-3D sound
	inline void Play( CSoundHandle& rSoundHandle );

	int GetSoundIndex( const char* pcSoundName );

	inline void SetListenerPosition( const Vector3& rvPosition );

	inline void UpdateListenerPose( const Matrix34& pose );

	inline void UpdateListenerPose( const Vector3& rvPosition,
		                            const Vector3& rvFrontDir,
                                    const Vector3& rvUp /*,
									const Vector3& rvVelocity = Vector3(0,0,0) */ );

	inline void UpdateListenerVelocity( const Vector3& vVelocity );

	inline void CommitDeferredSettings()
	{
		if( !m_pDirectSound )
			return;

		m_pDSListener->CommitDeferredSettings();
	}

	inline void SetMute( bool mute ) { m_bMute = mute; }
};

//================================= inline implementations ============================================


inline bool CGameSoundManager::UpdateSoundHandle( CSoundHandle& rSoundHandle )
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
//			PrintLog( "CGameSoundManager::UpdateSoundHandle() - invalid sound handle" );
	}
	else
		return true;

	return false;
}


inline void CGameSoundManager::Play3D( const char* pcSoundName, const Vector3& rvPosition )
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
			m_apGameSound[i]->Play3D( m_pDSListener, rvPosition );
		}
	}
}


inline void CGameSoundManager::Play3D( int index, const Vector3& rvPosition )
{
	if( !m_pDirectSound )
		return;

	if( m_bMute )
		return;

	if( index < 0 || m_iNumGameSounds <= index )
		return;

	m_apGameSound[index]->Play3D( m_pDSListener, rvPosition );
}


inline void CGameSoundManager::Play3D( CSoundHandle& rSoundHandle, const Vector3& rvPosition )
{
	if( !UpdateSoundHandle( rSoundHandle ) )
		return;

	Play3D( rSoundHandle.m_iIndex, rvPosition );
}


inline void CGameSoundManager::Play( CSoundHandle& rSoundHandle )
{
	if( !m_pDirectSound )
		return;

	if( m_bMute )
		return;

	if( !UpdateSoundHandle( rSoundHandle ) )
		return;

	m_apGameSound[rSoundHandle.m_iIndex]->Play();
}


inline void CGameSoundManager::SetListenerPosition( const Vector3& rvPosition )
{
	m_pDSListener->SetPosition(	rvPosition.x, rvPosition.y, rvPosition.z, DS3D_DEFERRED );
}


inline void CGameSoundManager::UpdateListenerPose( const Vector3& rvPosition, const Vector3& rvFrontDir,
		                                       const Vector3& rvUp/* , const Vector3& rvVelocity */ )
{
	if( !m_pDirectSound )
		return;

	SetListenerPosition( rvPosition );

	m_pDSListener->SetOrientation(
		rvFrontDir.x, rvFrontDir.y, rvFrontDir.z,
		rvUp.x,		  rvUp.y,		rvUp.z,
		DS3D_DEFERRED );
}


inline void CGameSoundManager::UpdateListenerPose( const Matrix34& pose )
{
	if( !m_pDirectSound )
		return;

	SetListenerPosition( pose.vPosition );

	const Vector3& vFront = pose.matOrient.GetColumn( 2 );
	const Vector3& vTop = pose.matOrient.GetColumn( 1 );

	m_pDSListener->SetOrientation( vFront.x, vFront.y, vFront.z, vTop.x, vTop.y, vTop.z, DS3D_DEFERRED ); 
}


inline void CGameSoundManager::UpdateListenerVelocity( const Vector3& vVelocity )
{
	if( !m_pDirectSound )
		return;

	m_pDSListener->SetVelocity( vVelocity.x, vVelocity.y, vVelocity.z, DS3D_DEFERRED );
}


#endif		/*  __GAMESOUNDMANAGER_H__  */
