#include "SoundManager.hpp"
#include "OpenALSoundManagerImpl.hpp"
#include "Support/TextFileScanner.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/SafeDelete.hpp"


namespace amorphous
{

using namespace std;


/**
 Usage
 1. Play short, non-looping sounds (gunshots, explosions, etc.)
   - Call GetSoundManager().PlayAt()
   - No release operation is necessary
     - Sound source is automatically released
 2. Play background music
   - Call GetSoundManager().PlayStream() to play
   - Call GetSoundManager().StopStream() to stop
     - These two functions must be called in pairs
 3. Create a sound source object in 3D environment
   - Call GetSoundManager().CreateSoundSource()
     - Call SoundSource::Play() to play the sound from the created sound source
     - Call SoundSource::Stop() to stop the sound
   - Call GetSoundManager().ReleaseSoundSource() to release the sound source
     - GetSoundManager().CreateSoundSource() and GetSoundManager().ReleaseSoundSource() must be called in pairs
*/


/// define the singleton instance
singleton<SoundManager> SoundManager::m_obj;


SoundManager::SoundManager()
:
m_pSoundManagerImpl(NULL)
{
	m_pSoundManagerImpl = new CNullSoundManagerImpl();
}


SoundManager::~SoundManager()
{
	Release();
}


void SoundManager::Release()
{
	m_pSoundManagerImpl->Release();

	SafeDelete( m_pSoundManagerImpl );
}


bool SoundManager::Init( const std::string& library_name )
{
	Release();

	if( library_name == "OpenAL" )
	{
		m_pSoundManagerImpl = new OpenALSoundManagerImpl();
	}
	else if( library_name == "DirectSound" )
	{
		LOG_PRINT_ERROR( "Sound manager for DirectSound has not been implemented yet." );
		m_pSoundManagerImpl = NULL;//new DirectSoundManager();
	}
	else
	{
		m_pSoundManagerImpl = new CNullSoundManagerImpl();
		return false;
	}

	return m_pSoundManagerImpl->Init();
}


bool SoundManager::PlayStream( const std::string& resource_path, double fadein_time, bool looped, int sound_group, U8 volume )
{
//	m_pSoundManagerImpl->PlayStream( resource_path, fadein_time, looped, sound_group, volume );

	if( m_mapNameToSoundSource.find( resource_path ) != m_mapNameToSoundSource.end() )
	{
		// A stream sound with the same name has already been loaded.
		return false;
	}

	SoundDesc desc;
	desc.Loop             = looped == 1 ? true : false;
	desc.Streamed         = true;
	desc.SourceManagement = SoundSource::Manual;

	SoundSource *pSource = CreateSoundSource( resource_path, desc );
	if( pSource )
	{
		m_mapNameToSoundSource[resource_path] = pSource;
		pSource->Play();
		return true;
	}
	else
	{
		LOG_PRINT_ERROR( "Cannot play streamed sound of: " + resource_path );
		return false;
	}
}


bool SoundManager::StopStream( const std::string& resource_path, double fadeout_time )
{
	map<string,SoundSource *>::iterator itr
		= m_mapNameToSoundSource.find( resource_path );

	if( itr == m_mapNameToSoundSource.end() )
	{
		return false;
	}

	SoundSource *pSource = itr->second;
	if( pSource )
	{
		pSource->Stop();
		GetSoundManager().ReleaseSoundSource( pSource );
	}

	m_mapNameToSoundSource.erase( itr );

	return true;
}


/*
<Sound>
	<Streamed>no</Streamed>
	<3D>true</3D>
	<ResourceName>filepath_or_dbname_2colons_keyname_pair</ResourceName>
</Sound>
*/


bool SoundManager::LoadSoundsFromList( const std::string& sound_list_file )
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
	GameSound* pNewSound;
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


} // namespace amorphous
