#ifndef  __GAMESOUND_H__
#define  __GAMESOUND_H__


#include "3DMath/Vector3.h"


// define 'DWORD_PTR' for "dsound.h" in DX9 SDK Summer 2004
#ifndef DWORD_PTR
#define DWORD_PTR DWORD*
#endif

#include <dsound.h>

#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "dxerr9.lib" )


class CWaveFile;

//-----------------------------------------------------------------------------
// Name: class CGameSound
// Desc: Encapsulates functionality of a DirectSound buffer.
//-----------------------------------------------------------------------------
class CGameSound
{
protected:
	char				 m_acSoundName[64];
    LPDIRECTSOUNDBUFFER* m_apDSBuffer;
    DWORD                m_dwDSBufferSize;
    CWaveFile*           m_pWaveFile;
    DWORD                m_dwNumBuffers;
    DWORD                m_dwCreationFlags;

	float m_fDefaultMinDist;
	float m_fDefaultMaxDist;

    HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:

    CGameSound( const char* pcSoundName, LPDIRECTSOUNDBUFFER* apDSBuffer, DWORD dwDSBufferSize,
				DWORD dwNumBuffers, CWaveFile* pWaveFile, DWORD dwCreationFlags,
				float fDefaultMinDist, float fDefaultMaxDist );

	virtual ~CGameSound();

	const char* GetName() const { return m_acSoundName; }

	HRESULT Get3DBufferInterface( DWORD dwIndex, LPDIRECTSOUND3DBUFFER* ppDS3DBuffer );
	HRESULT FillBufferWithSound( LPDIRECTSOUNDBUFFER pDSB, BOOL bRepeatWavIfBufferLarger );
	LPDIRECTSOUNDBUFFER GetFreeBuffer();
	LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );

	/// plays non-3D sound?
	HRESULT Play( DWORD dwPriority = 0, DWORD dwFlags = 0,
		          LONG lVolume = DSBVOLUME_MAX, LONG lFrequency = -1, LONG lPan = 0 );

	HRESULT Play3D( LPDS3DBUFFER p3DBuffer, DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lFrequency = 0 );

	/// plays sound at a specified position
	HRESULT Play3D( LPDIRECTSOUND3DLISTENER pDSListener, const Vector3& vPosition,
					float fMinDist = 0, float fMaxDist = 0,
					DWORD dwPriority = 0, DWORD dwFlags = 0, LONG lFrequency = 0 );
	HRESULT Stop();
	HRESULT Reset();
	BOOL    IsSoundPlaying();
};

#endif // __GAMESOUND_H__