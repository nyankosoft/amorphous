
#include "SaveDataManager.h"
#include <sys/stat.h>

using namespace std;


//======================================================================================
// CSaveDataComponent
//======================================================================================

CSaveDataComponent::CSaveDataComponent()
{
//	SaveDataManager.AddComponent( this );
}


CSaveDataComponent::~CSaveDataComponent()
{
//	SaveDataManager.DeleteComponent( this );
}


void CSaveDataComponent::LoadFrom( CBinaryDatabase<std::string>& db )
{
	db.GetData( GetKeyString(), *this );
}


void CSaveDataComponent::SaveTo( CBinaryDatabase<std::string>& db )
{
	db.AddData( GetKeyString(), *this );
}



//======================================================================================
// CSaveDataManager
//======================================================================================

CSingleton<CSaveDataManager> CSaveDataManager::m_obj;


/**
 note: file with the returned filename may not exist
   - check file_exist(filename) if you are trying to read data
     from the returned file
*/
std::string CSaveDataManager::GetSaveDataFilename( int index )
{
	return fmt_string( "%s/sd%03d.bin", m_strSaveDataDirectory.c_str(), index );
}


int CSaveDataManager::GetNumSaveDataFiles()
{
	int num_max_save_data_files = 8;

	struct stat filestat;

	int result = 0;
	int index, num_save_data_files = 0;
	for( index=0; index<num_max_save_data_files; index++ )
	{
		result = stat( GetSaveDataFilename(index).c_str(), &filestat );
		if( result == 0 )
			num_save_data_files++;
	}

	return num_save_data_files;
}


void CSaveDataManager::SetSaveDataDirectory( const string& directory_path )
{
	m_strSaveDataDirectory = directory_path;
}


bool CSaveDataManager::SaveCurrentStateToFile( int index )
{
//	string filename = "sd.bin";
	string filename = GetSaveDataFilename( index );
	CBinaryDatabase<string> db;

	if( !db.Open( filename, CBinaryDatabase<string>::DB_MODE_NEW ) )
		return false;

//	CSaveDataHeader sd_header;
//	db.AddData( "header", sd_header );

	const size_t num_components = m_vecpComponent.size();
	for( size_t i=0; i<num_components; i++ )
	{
		m_vecpComponent[i]->SaveTo( db );
	}

	db.Close();

	return true;
}


bool CSaveDataManager::LoadSavedStateFromFile( int index )
{
	string filename = GetSaveDataFilename( index );
	CBinaryDatabase<string> db;

	if( !db.Open( filename, CBinaryDatabase<string>::DB_MODE_APPEND ) )
		return false;

	const size_t num_components = m_vecpComponent.size();
	for( size_t i=0; i<num_components; i++ )
	{
		m_vecpComponent[i]->LoadFrom( db );
	}

	db.Close();

	return true;
}


void CSaveDataManager::AddComponent( CSaveDataComponent* pComponent )
{
	m_vecpComponent.push_back( pComponent );
}


bool CSaveDataManager::RemoveComponent( CSaveDataComponent* pComponent )
{
	size_t i, num_components = m_vecpComponent.size();
	for( i=0; i<num_components; i++ )
	{
		if( m_vecpComponent[i] == pComponent )
		{
			m_vecpComponent.erase( m_vecpComponent.begin() + i );
			return true;
		}
	}
	return false;
}


