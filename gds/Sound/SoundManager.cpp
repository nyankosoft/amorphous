#include "SoundManager.h"
#include "OpenALSoundManagerImpl.h"
#include "Support/TextFileScanner.h"
#include "Support/Log/DefaultLog.h"
#include "Support/SafeDelete.h"

using namespace std;


/// define the singleton instance
CSingleton<CSoundManager> CSoundManager::m_obj;


CSoundManager::CSoundManager()
:
m_pSoundManagerImpl(NULL)
{
	m_pSoundManagerImpl = new CNullSoundManagerImpl();
}


CSoundManager::~CSoundManager()
{
	Release();
}


void CSoundManager::Release()
{
	m_pSoundManagerImpl->Release();

	SafeDelete( m_pSoundManagerImpl );
}


bool CSoundManager::Init( const std::string& library_name )
{
	Release();

	if( library_name == "OpenAL" )
	{
		m_pSoundManagerImpl = new COpenALSoundManagerImpl();
	}
	else if( library_name == "DirectSound" )
	{
		LOG_PRINT_ERROR( "Sound manager for DirectSound has not been implemented yet." );
		m_pSoundManagerImpl = NULL;//new CDirectSoundManager();
	}
	else
	{
		m_pSoundManagerImpl = new CNullSoundManagerImpl();
		return false;
	}

	return m_pSoundManagerImpl->Init();
}

/*
<Sound>
	<Streamed>no</Streamed>
	<3D>true</3D>
	<ResourceName>filepath_or_dbname_2colons_keyname_pair</ResourceName>
</Sound>
*/


bool CSoundManager::LoadSoundsFromList( const std::string& sound_list_file )
{
/*
	CTextFileScanner scanner( sound_list_file );

	if( !scanner.IsReady() )
		return false;

	string current_line;
	char acWaveFilename[512];
	int iIs3DSound;
	int num_buffers;
	float fDefaultMinDist, fDefaultMaxDist;
	DWORD dwCreationFlags;
	GUID guid3DAlgorithm;
	CGameSound* pNewSound;
	HRESULT hr;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( NUM_MAX_SOUNDS <= m_iNumGameSounds )
			break;

		scanner.GetCurrentLine( current_line );

		if( current_line.find( "#" ) == 0 )
		{
			// the line starts with "#". - skip
			continue;
		}

		iIs3DSound = 0;
		num_buffers = 1;
		fDefaultMinDist	= 1.0f;
		fDefaultMaxDist = 1000.0f;

		sscanf( current_line.c_str(), "%s %d %d %f %f",
			acWaveFilename, &iIs3DSound, &num_buffers, &fDefaultMinDist, &fDefaultMaxDist );

		if( iIs3DSound )
		{
			dwCreationFlags = DSBCAPS_CTRL3D;
			guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		}
		else
		{
			dwCreationFlags = DSBCAPS_CTRLPAN;
			guid3DAlgorithm = DS3DALG_DEFAULT;
		}

		// create a suond object
		hr = CreateSound( &pNewSound,
						  acWaveFilename,
						  dwCreationFlags,
						  guid3DAlgorithm,
						  num_buffers,
						  fDefaultMinDist,
						  fDefaultMaxDist );

		if( !FAILED(hr) )
		{
//			PrintLog( std::string( std::string("created a sound object from file: ") + std::string(acWaveFilename) ) );
			m_apGameSound[ m_iNumGameSounds++ ] = pNewSound;
		}
	}
*/
	return true;
}
