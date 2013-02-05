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
// BaseEntityManager (singleton)
//===================================================================================

/// manages the binary database of base entities (singleton)
/* 
 * allows the user to load the base entity by name from the database
 * the database file is opened during the first call of LoadBaseEntity()
 * the database is kept open until the application is closed
 */
class BaseEntityManager
{
private:

	BaseEntityFactorySharedPtr m_pBaseEntityFactory;

	CBinaryDatabase<std::string> *m_pBaseEntityDatabase;

	std::map<std::string,unsigned int> m_mapIDtoClassName;

//	std::string m_strDatabaseFilename;

	BaseEntity* CreateBaseEntity( const std::string& baseentity_classname );

	bool LoadAllBaseEntitiesFromRootTextFile( const std::string& strRootTextFile,
											  std::vector<BaseEntity *>& vecpBaseEntity,
											  std::string& db_filepath );

	bool LoadAllBaseEntitiesFromTextFile( const std::string& strDestDBFile,
		                                  std::vector<BaseEntity *>& vecpBaseEntity );

	bool OutputDatabaseFile( const std::string& database_filename, std::vector<BaseEntity *>& vecpBaseEntity );

protected:

	static CSingleton<BaseEntityManager> m_obj;

public:

	static BaseEntityManager* Get() { return m_obj.get(); }

	BaseEntityManager();

	~BaseEntityManager();

	BaseEntity *LoadBaseEntity( const std::string& base_entity_name );

	void RegisterBaseEntityFactory( BaseEntityFactorySharedPtr pFactory ) { m_pBaseEntityFactory = pFactory; }

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

inline void BaseEntityManager::AddBaseEntityClassNameToIDMap( const std::string& base_entity_name,
															   int base_entity_id )
{
	m_mapIDtoClassName[base_entity_name] = base_entity_id;
}


//============================== global function ==============================
inline BaseEntityManager& GetBaseEntityManager()
{
	return *(BaseEntityManager::Get());
}

} // namespace amorphous



#endif		/*  __BASEENTITYMANAGER_H__  */
