#ifndef  __BASEENTITYMANAGER_H__
#define  __BASEENTITYMANAGER_H__


#include <vector>
#include <map>
#include "fwd.hpp"
#include "Support/Singleton.hpp"
#include "Support/Serialization/Serialization.hpp"
#include "Support/Serialization/ArchiveObjectFactory.hpp"
#include "Support/Serialization/BinaryDatabase.hpp"


namespace amorphous
{

using namespace serialization;


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

	bool LoadAllBaseEntitiesFromRootTextFile( const std::string& strRootTextFile,
											  std::vector<CBaseEntity *>& vecpBaseEntity,
											  std::string& db_filepath );

	bool LoadAllBaseEntitiesFromTextFile( const std::string& strDestDBFile,
		                                  std::vector<CBaseEntity *>& vecpBaseEntity );

	bool OutputDatabaseFile( const std::string& database_filename, std::vector<CBaseEntity *>& vecpBaseEntity );

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
	 * The filename of the output database can be overriden set with 'output' keyword in the source text file.
	 */
	bool UpdateDatabase( const std::string& strSrcTextFile, std::string& db_filepath );

	bool OpenDatabase( const std::string& filename );
};


//============================== inline implementations ==============================

inline void CBaseEntityManager::AddBaseEntityClassNameToIDMap( const std::string& base_entity_name,
															   int base_entity_id )
{
	m_mapIDtoClassName[base_entity_name] = base_entity_id;
}


//============================== global function ==============================
inline CBaseEntityManager& BaseEntityManager()
{
	return *(CBaseEntityManager::Get());
}

} // namespace amorphous



#endif		/*  __BASEENTITYMANAGER_H__  */
