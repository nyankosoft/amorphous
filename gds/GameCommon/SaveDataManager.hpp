#ifndef  __SAVEDATAMANAGER_H__
#define  __SAVEDATAMANAGER_H__


#include <vector>
#include <string>

#include "Support/StringAux.hpp"

#include "Support/Serialization/BinaryDatabase.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
using namespace GameLib1::Serialization;

#include "Support/Singleton.hpp"
using namespace NS_KGL;





/*
// component object needs to acccess component manager in its ctor & dtor
// > component manager needs to singleton
// - How can this be made as template class?
template<class T>
class ComponentManager
{
protected:

	std::vector<T *> m_vecpComponent;

public:

//	void AddComponent( T* pComponent );
//	bool DeleteComponent( T* pComponent );


	void AddComponent( T* pComponent )
	{
		m_vecpComponent.push_back( pComponent );
	}


	bool DeleteComponent( T* pComponent )
	{
		size_t i, num_components = m_vecpComponent.size();
		for( i=0; i<num_components; i++ )
		{
			if( m_vecpComponent[i] == pComponent )
			{
				m_vecpComponent.erase( m_vecpGraphicsComponent.begin() + i );
				return true;
			}
		}

		return false;	// the requested component was not found
	}
};
*/


/**
 * instance of derived class will be saved by calling SaveDataManager.SaveState()
 * - derived calss must implement Serialize()
 * - All the necessary instances must be present
 *   when SaveState() or LoadSavedData() is called
 *   
 */
class CSaveDataComponent : public IArchiveObjectBase
{
public:

	CSaveDataComponent();
	virtual ~CSaveDataComponent();

//	virtual const std::string& GetKeyString() const = 0;
	virtual const char *GetKeyString() const = 0;

	virtual void LoadFrom( CBinaryDatabase<std::string>& db );

	virtual void SaveTo( CBinaryDatabase<std::string>& db );

};


#define SaveDataManager ( (*CSaveDataManager::Get()) )

/// save & load a state of the game to files
/// - singleton
/// - holds a borrowed references of save data components
/// - save data components need to be registered by the user
/// - DON'T FORGET to call RemoveAllComponents() after loading / saving
///
/// TODO: How to access CSaveDataManager from ctor & dtor of CSaveDataComponent?
class CSaveDataManager// : public ComponentManager<>
{
	// borrowed references
	std::vector<CSaveDataComponent *> m_vecpComponent;

	std::string m_strSaveDataDirectory;

	int m_MaxNumSaveDataFiles;

	static CSingleton<CSaveDataManager> m_obj;

public:

	static CSaveDataManager* Get() { return m_obj.get(); }

	CSaveDataManager() : m_MaxNumSaveDataFiles(8) {}

	int GetMaxNumSaveDataFiles() const { return m_MaxNumSaveDataFiles; }

	void SetMaxNumSaveDataFiles( int max_num_savedata_files ) { m_MaxNumSaveDataFiles = max_num_savedata_files; }

	int GetNumSaveDataFiles();

	std::string GetSaveDataFilename( int index );

	bool SaveCurrentStateToFile( int index );

	bool LoadSavedStateFromFile( int index );

	void SetSaveDataDirectory( const std::string& directory_path );

	void AddComponent( CSaveDataComponent* pComponent );

	bool RemoveComponent( CSaveDataComponent* pComponent );

	/// release all the borrowed references of save data components
	/// - note this does not release any actual memory
	void RemoveAllComponents() { m_vecpComponent.resize( 0 ); }
};


/*
class CSaveDataHeader : public IArchiveObjectBase
{
public:

	std::string m_Title;
	std::string m_Text;

	unsigned long m_PlayTime;	/// total played time	[sec]
	unsigned long m_OwnedMoney;

public:

	CSaveDataHeader() : m_PlayTime(0), m_OwnedMoney(0) {}

	virtual void Serialize( IArchive& ar, const unsigned int version )
	{
		ar & m_Title & m_Text;
		ar & m_PlayTime;
		ar & m_OwnedMoney;
	};
};
*/


#endif /* __SAVEDATAMANAGER_H__ */
