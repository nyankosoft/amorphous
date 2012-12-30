#include <stdio.h>
#include <windows.h>
#include <dxerr9.h>

#include "Support/TextFileScanner.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"

#include "DirectSoundManager.hpp"
#include "WaveFile.hpp"


namespace amorphous
{

using namespace std;



CDirectSoundManager::CDirectSoundManager()
{
	m_pDirectSound = NULL;
	m_pDSListener  = NULL;
	m_iNumGameSounds = 0;

	m_bMute = false;
}


CDirectSoundManager::~CDirectSoundManager()
{
	for(int i=0; i<m_iNumGameSounds; i++)
	{
		SafeDelete( m_apGameSound[i] );
	}

	SAFE_RELEASE( m_pDSListener );
	SAFE_RELEASE( m_pDirectSound );
}


static const char *GetDSoundError( HRESULT hr )
{
	switch(hr)
	{
	case DSERR_ALLOCATED:     return "DSERR_ALLOCATED";
	case DSERR_INVALIDPARAM:  return "DSERR_INVALIDPARAM";
	case DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION";
	case DSERR_NODRIVER:      return "DSERR_NODRIVER";
	case DSERR_OUTOFMEMORY:   return "DSERR_OUTOFMEMORY";
	default: return "unknown";
	}
}


HRESULT CDirectSoundManager::Init( HWND hWnd )
{
    HRESULT hr;

	SAFE_RELEASE( m_pDirectSound );

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &m_pDirectSound, NULL ) ) )
	{
		LOG_PRINT_ERROR( " - DirectSoundCreate8() failed. Error: " + string(GetDSoundError(hr)) );
        return hr;
	}

    // Set DirectSound coop level 
    if( FAILED( hr = m_pDirectSound->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) ) )
	{
		LOG_PRINT_ERROR( " - SetCooperativeLevel() failed." );
        return hr;
	}

	if( FAILED( hr = SetPrimaryBufferFormat(2, 22050, 16) ) )
	{
		LOG_PRINT_ERROR( " - SetPrimaryBufferFormat() failed." );
        return hr;
	}

	if( FAILED( hr = Set3DListenerInterface() ) )
	{
		LOG_PRINT_ERROR( " - Set3DListenerInterface() failed." );
        return hr;
	}

    return S_OK;
}

HRESULT CDirectSoundManager::SetPrimaryBufferFormat( DWORD dwPrimaryChannels, 
                                                   DWORD dwPrimaryFreq, 
                                                   DWORD dwPrimaryBitRate )
{
    HRESULT             hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDirectSound == NULL )
        return CO_E_NOTINITIALIZED;

    // Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if( FAILED( hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
	{
		LOG_PRINT_ERROR( " - CreateSoundBuffer() failed." );
        return hr;
	}

    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
	{
		LOG_PRINT_ERROR( " - SetFormat() failed." );
        return hr;
	}

    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}

HRESULT CDirectSoundManager::Set3DListenerInterface()
{
    HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    if( m_pDirectSound == NULL )
        return CO_E_NOTINITIALIZED;

    // Obtain primary buffer, asking it for 3D control
    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

    if( FAILED( hr = m_pDirectSound->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
        return DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)&m_pDSListener ) ) )
    {
        SAFE_RELEASE( pDSBPrimary );
        return DXTRACE_ERR( TEXT("QueryInterface"), hr );
    }

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return S_OK;
}
HRESULT CDirectSoundManager::CreateSound( CGameSound** ppSound, 
										LPTSTR strWaveFileName, 
									    DWORD dwCreationFlags, 
									    GUID guid3DAlgorithm,
									    DWORD dwNumBuffers,
										float fDefaultMinDist,
										float fDefaultMaxDist )
{
    HRESULT hr;
    HRESULT hrRet = S_OK;
    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = 0;
    CWaveFile*           pWaveFile      = NULL;

    if( m_pDirectSound == NULL )
        return CO_E_NOTINITIALIZED;

    if( strWaveFileName == NULL || ppSound == NULL || dwNumBuffers < 1 )
        return E_INVALIDARG;

    apDSBuffer = new LPDIRECTSOUNDBUFFER[dwNumBuffers];
    if( apDSBuffer == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->Open( strWaveFileName, NULL, WAVEFILE_READ );

    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        hr = E_FAIL;
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;
    
    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &apDSBuffer[0], NULL );

    // Be sure to return this error code if it occurs so the
    // callers knows this happened.
    if( hr == DS_NO_VIRTUALIZATION )
        hrRet = DS_NO_VIRTUALIZATION;
            
    if( FAILED(hr) )
    {
        // DSERR_BUFFERTOOSMALL will be returned if the buffer is
        // less than DSBSIZE_FX_MIN and the buffer is created
        // with DSBCAPS_CTRLFX.
        
        // It might also fail if hardware buffer mixing was requested
        // on a device that doesn't support it.
        DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
                    
        goto LFail;
    }

    // Default to use DuplicateSoundBuffer() when created extra buffers since always 
    // create a buffer that uses the same memory however DuplicateSoundBuffer() will fail if 
    // DSBCAPS_CTRLFX is used, so use CreateSoundBuffer() instead in this case.
    if( (dwCreationFlags & DSBCAPS_CTRLFX) == 0 )
    {
        for( DWORD i=1; i<dwNumBuffers; i++ )
        {
            if( FAILED( hr = m_pDirectSound->DuplicateSoundBuffer( apDSBuffer[0], &apDSBuffer[i] ) ) )
            {
                DXTRACE_ERR( TEXT("DuplicateSoundBuffer"), hr );
                goto LFail;
            }
        }
    }
    else
    {
        for( DWORD i=1; i<dwNumBuffers; i++ )
        {
            hr = m_pDirectSound->CreateSoundBuffer( &dsbd, &apDSBuffer[i], NULL );
            if( FAILED(hr) )
            {
                DXTRACE_ERR( TEXT("CreateSoundBuffer"), hr );
                goto LFail;
            }
        }
   }

	// cut the path and the extention of the wave file to extract the body filename
	char acSoundName[256];
	for( size_t i=strlen(strWaveFileName) - 1; 0<i; i--)
	{	// cut the path
		if( strWaveFileName[i] == '\\' )
		{
			strcpy( acSoundName, &strWaveFileName[i+1] );
			break;
		}
	}

	// cut the extention
	acSoundName[strlen(acSoundName) - 4] = '\0';
    
    // Create the sound
    *ppSound = new CGameSound( acSoundName,
							   apDSBuffer,
						       dwDSBufferSize,
						       dwNumBuffers,
						       pWaveFile,
						       dwCreationFlags,
							   fDefaultMinDist,
							   fDefaultMaxDist );
    
    SafeDelete( apDSBuffer );
    return hrRet;

LFail:
    // Cleanup
    SafeDelete( pWaveFile );
    SafeDelete( apDSBuffer );
    return hr;
}


} // amorphous
