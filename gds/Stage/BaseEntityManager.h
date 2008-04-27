#ifndef  __BASEENTITYMANAGER_H__
#define  __BASEENTITYMANAGER_H__

#include <vector>
#include <map>

#include "Support/Singleton.h"
using namespace NS_KGL;

#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;

#include "fwd.h"

// for quick access to the singleton instance
#define BaseEntityManager ( *CBaseEntityManager::Get() )



//===================================================================================
// CBaseEntityManager (singleton)
//===================================================================================

/// manages the binary database of base entities (singleton)
/* 
 * allows the user to load the base entity by name from the database
 * the database file is opened during the first call of LoadBaseEntity()
 * the database is kept open until the application is closed
 */
class CBaseEntityManager
{
private:

	CBaseEntityFactorySharedPtr m_pBaseEntityFactory;

	CBinaryDatabase<std::string> *m_pBaseEntityDatabase;

	std::map<std::string,unsigned int> m_mapIDtoClassName;

//	std::string m_strDatabaseFilename;

	CBaseEntity* CreateBaseEntity( const std::string& baseentity_classname );

    bool LoadAllBaseEntitiesFromRootTextFile( const string& strRootTextFile,
											  std::vector<CBaseEntity *>& vecpBaseEntity );

	bool LoadAllBaseEntitiesFromTextFile( const std::string& strDestDBFile,
		                                  std::vector<CBaseEntity *>& vecpBaseEntity );

	bool OutputDatabaseFile( const string& database_filename, std::vector<CBaseEntity *>& vecpBaseEntity );

protected:

	static CSingleton<CBaseEntityManager> m_obj;

public:

	static CBaseEntityManager* Get() { return m_obj.get(); }

	CBaseEntityManager();

	~CBaseEntityManager();

	CBaseEntity *LoadBaseEntity( const std::string& base_entity_name );

	void RegisterBaseEntityFactory( CBaseEntityFactorySharedPtr pFactory ) { m_pBaseEntityFactory = pFactory; }

	inline void AddBaseEntityClassNameToIDMap( const std::string& base_entity_name, int base_entity_id );

	/**
	 * updates binary database of base entities
	 * \param strSrcTextFile a text file which contains input filenames and an output database filename
	 * the filename of the output database is set with 'output' keyword in the source text file.
	 */
	bool UpdateDatabase( const std::string& strSrcTextFile/*, const std::string& strDestDBFile = ""*/ );

	bool OpenDatabase( const std::string& filename );
};


//============================== inline implementations ==============================

inline void CBaseEntityManager::AddBaseEntityClassNameToIDMap( const std::string& base_entity_name,
															   int base_entity_id )
{
	m_mapIDtoClassName[base_entity_name] = base_entity_id;
}


#endif		/*  __BASEENTITYMANAGER_H__  */
