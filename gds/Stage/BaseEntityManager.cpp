#include "BaseEntityManager.hpp"
#include "BaseEntity.hpp"
#include "BaseEntityFactory.hpp"
#include "CoreBaseEntitiesLoader.hpp"
#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <boost/filesystem.hpp>


namespace amorphous
{

using namespace std;
using namespace boost;
using namespace boost::filesystem;


singleton<BaseEntityManager> BaseEntityManager::m_obj;


BaseEntityManager::BaseEntityManager()
: m_pBaseEntityDatabase(NULL)
{
	// register default base entity factory
	RegisterBaseEntityFactory( BaseEntityFactorySharedPtr( new BaseEntityFactory() ) );

//	m_strDatabaseFilename = "";

//	AddNameAndIDPair( "CBE_...", id ); 

//	m_mapIDtoClassName["BaseEntity"]			= BASEENTITY;
	m_mapIDtoClassName["CBE_AreaSensor"]		= BaseEntity::BE_AREASENSOR;
	m_mapIDtoClassName["CBE_Blast"]				= BaseEntity::BE_BLAST;
	m_mapIDtoClassName["CBE_Bullet"]			= BaseEntity::BE_BULLET;
	m_mapIDtoClassName["CBE_Cloud"]				= BaseEntity::BE_CLOUD;
	m_mapIDtoClassName["CBE_Decal"]				= BaseEntity::BE_DECAL;
	m_mapIDtoClassName["CBE_Door"]				= BaseEntity::BE_DOOR;
	m_mapIDtoClassName["CBE_DoorController"]	= BaseEntity::BE_DOORCONTROLLER;
	m_mapIDtoClassName["CBE_Enemy"]				= BaseEntity::BE_ENEMY;
	m_mapIDtoClassName["CBE_EventTrigger"]		= BaseEntity::BE_EVENTTRIGGER;
	m_mapIDtoClassName["CBE_ExplosionSmoke"]	= BaseEntity::BE_EXPLOSIONSMOKE;
	m_mapIDtoClassName["CBE_Explosive"]			= BaseEntity::BE_EXPLOSIVE;
	m_mapIDtoClassName["CBE_FixedTurretPod"]	= BaseEntity::BE_FIXEDTURRETPOD;
	m_mapIDtoClassName["CBE_Floater"]			= BaseEntity::BE_FLOATER;
	m_mapIDtoClassName["CBE_GeneralEntity"]		= BaseEntity::BE_GENERALENTITY;
	m_mapIDtoClassName["CBE_HomingMissile"]		= BaseEntity::BE_HOMINGMISSILE;
	m_mapIDtoClassName["CBE_LaserDot"]			= BaseEntity::BE_LASERDOT;
	m_mapIDtoClassName["CBE_MuzzleFlash"]		= BaseEntity::BE_MUZZLEFLASH;
	m_mapIDtoClassName["CBE_ParticleSet"]		= BaseEntity::BE_PARTICLESET;
	m_mapIDtoClassName["CBE_PhysicsBaseEntity"]	= BaseEntity::BE_PHYSICSBASEENTITY;
	m_mapIDtoClassName["CBE_Platform"]			= BaseEntity::BE_PLATFORM;
	m_mapIDtoClassName["CBE_Player"]			= BaseEntity::BE_PLAYER;
	m_mapIDtoClassName["CBE_PlayerPseudoAircraft"]		= BaseEntity::BE_PLAYERPSEUDOAIRCRAFT;
	m_mapIDtoClassName["CBE_PlayerPseudoLeggedVehicle"] = BaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE;
	m_mapIDtoClassName["CBE_PlayerShip"]		= BaseEntity::BE_PLAYERSHIP;
	m_mapIDtoClassName["CBE_PointLight"]		= BaseEntity::BE_POINTLIGHT;
	m_mapIDtoClassName["CBE_SmokeTrace"]		= BaseEntity::BE_SMOKETRACE;
	m_mapIDtoClassName["CBE_SupplyItem"]		= BaseEntity::BE_SUPPLYITEM;
	m_mapIDtoClassName["CBE_TextureAnimation"]	= BaseEntity::BE_TEXTUREANIMATION;
	m_mapIDtoClassName["CBE_Turret"]			= BaseEntity::BE_TURRET;
	m_mapIDtoClassName["CBE_EnemyAircraft"]		= BaseEntity::BE_ENEMYAIRCRAFT;
	m_mapIDtoClassName["CBE_DirectionalLight"]	= BaseEntity::BE_DIRECTIONALLIGHT;
	m_mapIDtoClassName["CBE_StaticParticleSet"]	= BaseEntity::BE_STATICPARTICLESET;
	m_mapIDtoClassName["CBE_NozzleExhaust"]		= BaseEntity::BE_NOZZLEEXHAUST;
	m_mapIDtoClassName["CBE_EnemyShip"]			= BaseEntity::BE_ENEMYSHIP;
	m_mapIDtoClassName["CBE_StaticGeometry"]	= BaseEntity::BE_STATICGEOMETRY;
	m_mapIDtoClassName["CBE_Skybox"]			= BaseEntity::BE_SKYBOX;
	m_mapIDtoClassName["CBE_ScriptedCamera"]	= BaseEntity::BE_SCRIPTEDCAMERA;
	m_mapIDtoClassName["CBE_CameraController"]	= BaseEntity::BE_CAMERACONTROLLER;
	m_mapIDtoClassName["CBE_IndividualEntity"]	= BaseEntity::BE_INDIVIDUALENTITY;
}


BaseEntityManager::~BaseEntityManager()
{
	SafeDelete( m_pBaseEntityDatabase );
}


bool BaseEntityManager::LoadAllBaseEntitiesFromRootTextFile( const string& strRootTextFile,
															  vector<BaseEntity *>& vecpBaseEntity,
															  std::string& db_filepath )
{
	CTextFileScanner scanner;
	if( !scanner.OpenFile( strRootTextFile ) )
	{
		LOG_PRINT_WARNING( " - Cannot open file: " + strRootTextFile );
		return false;
	}

	// change to the directory of 'strRootTextFile'
	path parent_path = path(strRootTextFile).parent_path();

	vector<string> vecSrcTextFile;
	string filename, relative_db_filepath;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		if( scanner.TryScanLine( "input", filename ) )
			vecSrcTextFile.push_back( path(parent_path / filename).string() );

		scanner.TryScanLine( "output", relative_db_filepath );
	}

	if( vecSrcTextFile.size() == 0 )		 { LOG_PRINT_ERROR( " - no input file is specified." ); return false; }

	if( 0 < relative_db_filepath.length() )
	{
		db_filepath = path( parent_path / relative_db_filepath ).string();
	}

	size_t i, num_input_files = vecSrcTextFile.size();
	for( i=0; i<num_input_files; i++ )
	{
		LoadAllBaseEntitiesFromTextFile( vecSrcTextFile[i], vecpBaseEntity );
	}

	return true;
}


bool BaseEntityManager::LoadAllBaseEntitiesFromTextFile( const string& strSrcTextFile,
														  vector<BaseEntity *>& vecpBaseEntity )
{

	CTextFileScanner scanner;
	if( !scanner.OpenFile( strSrcTextFile ) )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + strSrcTextFile );
		return false;
	}

	BaseEntity* pNewBaseEntity = NULL;
	string tag, classname = "";
	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();
		if( tag == "CLASSNAME" )
		{
			scanner.ScanLine( tag, classname );

			LOG_PRINT_VERBOSE( "loading a base entity: " + classname );

			pNewBaseEntity = CreateBaseEntity( classname );
			if( !pNewBaseEntity )
			{
				LOG_PRINT_ERROR( " - An invalid base entity class name: " + classname );
				continue;
			}

			pNewBaseEntity->LoadFromFile(scanner);

			// add to the dest buffer
			vecpBaseEntity.push_back( pNewBaseEntity );
		}
/*		else if( tag == "END" && pNewBaseEntity )
		{
//			pNewBaseEntity->Init();

			classname = "";
			pNewBaseEntity = NULL;
		}
		else if( pNewBaseEntity )*/
	}

	return true;
}


bool BaseEntityManager::OutputDatabaseFile( const string& database_filename,
											 vector<BaseEntity *>& vecpBaseEntity )
{
	CBinaryDatabase<string> db;

	if( !db.Open( database_filename.c_str(), CBinaryDatabase<string>::DB_MODE_NEW ) )
	{
		LOG_PRINT_ERROR( " - cannot open the base entity database file: " + database_filename );
		return false;
	}

	size_t i, num_entities = vecpBaseEntity.size();
	for( i=0; i<num_entities; i++ )
	{
		BaseEntity *pBaseEntity = vecpBaseEntity[i];
		db.AddPolymorphicData( pBaseEntity->GetNameString(), pBaseEntity, *m_pBaseEntityFactory );
	}

	return true;
}


BaseEntity *BaseEntityManager::LoadBaseEntity( const string& base_entity_name )
{
	if( base_entity_name.length() == 0 )
	{
		LOG_PRINT_ERROR( " - no base entity name" );
		return NULL;
	}

	LOG_PRINT_VERBOSE( " - loading a base entity '" + base_entity_name + "' from the database: " );

	BaseEntity *pEntity = NULL;
	bool res = m_pBaseEntityDatabase->GetPolymorphicData( base_entity_name, pEntity, *m_pBaseEntityFactory );

	if( !res )
	{
		LOG_PRINT_ERROR( " - failed to load a base entity: " + base_entity_name );
		return NULL;
	}
	else
		LOG_PRINT( " - loaded a base entity: " + base_entity_name );

	return pEntity;
}


BaseEntity* BaseEntityManager::CreateBaseEntity( const string& baseentity_classname )
{
	return m_pBaseEntityFactory->CreateBaseEntity( m_mapIDtoClassName[baseentity_classname] );
}


bool BaseEntityManager::OpenDatabase( const std::string& filename )
{
	SafeDelete( m_pBaseEntityDatabase );

	m_pBaseEntityDatabase = new CBinaryDatabase<string>;

	if( !m_pBaseEntityDatabase->Open( filename.c_str() ) )
		return false;

	return true;
}


bool BaseEntityManager::UpdateDatabase( const string& strSrcTextFile, string& db_filepath )
{
	vector<BaseEntity *> vecpBaseEntity;
	vecpBaseEntity.reserve( 64 );

	CoreBaseEntitiesLoader core_base_entity_loader;
	core_base_entity_loader.LoadCoreBaseEntities( vecpBaseEntity );

//	LoadAllBaseEntitiesFromTextFile( strSrcTextFile, vecpBaseEntity );
	LoadAllBaseEntitiesFromRootTextFile( strSrcTextFile, vecpBaseEntity, db_filepath );

	OutputDatabaseFile( db_filepath, vecpBaseEntity );

	SafeDeleteVector( vecpBaseEntity );

	//log->Print( "the base entity database has been updated" );

	return true;
}


} // namespace amorphous
