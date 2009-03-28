#include "BaseEntityManager.hpp"
#include "BaseEntity.hpp"
#include "BaseEntityFactory.hpp"

#include "Support/memory_helpers.hpp"
#include "Support/Log/DefaultLog.hpp"
#include "Support/fnop.hpp"
using namespace fnop;


CSingleton<CBaseEntityManager> CBaseEntityManager::m_obj;


CBaseEntityManager::CBaseEntityManager()
: m_pBaseEntityDatabase(NULL)
{
	// register default base entity factory
	RegisterBaseEntityFactory( CBaseEntityFactorySharedPtr( new CBaseEntityFactory() ) );

//	m_strDatabaseFilename = "";

//	AddNameAndIDPair( "CBE_...", id ); 

//	m_mapIDtoClassName["CBaseEntity"]			= BASEENTITY;
	m_mapIDtoClassName["CBE_AreaSensor"]		= CBaseEntity::BE_AREASENSOR;
	m_mapIDtoClassName["CBE_Blast"]				= CBaseEntity::BE_BLAST;
	m_mapIDtoClassName["CBE_Bullet"]			= CBaseEntity::BE_BULLET;
	m_mapIDtoClassName["CBE_Cloud"]				= CBaseEntity::BE_CLOUD;
	m_mapIDtoClassName["CBE_Decal"]				= CBaseEntity::BE_DECAL;
	m_mapIDtoClassName["CBE_Door"]				= CBaseEntity::BE_DOOR;
	m_mapIDtoClassName["CBE_DoorController"]	= CBaseEntity::BE_DOORCONTROLLER;
	m_mapIDtoClassName["CBE_Enemy"]				= CBaseEntity::BE_ENEMY;
	m_mapIDtoClassName["CBE_EventTrigger"]		= CBaseEntity::BE_EVENTTRIGGER;
	m_mapIDtoClassName["CBE_ExplosionSmoke"]	= CBaseEntity::BE_EXPLOSIONSMOKE;
	m_mapIDtoClassName["CBE_Explosive"]			= CBaseEntity::BE_EXPLOSIVE;
	m_mapIDtoClassName["CBE_FixedTurretPod"]	= CBaseEntity::BE_FIXEDTURRETPOD;
	m_mapIDtoClassName["CBE_Floater"]			= CBaseEntity::BE_FLOATER;
	m_mapIDtoClassName["CBE_GeneralEntity"]		= CBaseEntity::BE_GENERALENTITY;
	m_mapIDtoClassName["CBE_HomingMissile"]		= CBaseEntity::BE_HOMINGMISSILE;
	m_mapIDtoClassName["CBE_LaserDot"]			= CBaseEntity::BE_LASERDOT;
	m_mapIDtoClassName["CBE_MuzzleFlash"]		= CBaseEntity::BE_MUZZLEFLASH;
	m_mapIDtoClassName["CBE_ParticleSet"]		= CBaseEntity::BE_PARTICLESET;
	m_mapIDtoClassName["CBE_PhysicsBaseEntity"]	= CBaseEntity::BE_PHYSICSBASEENTITY;
	m_mapIDtoClassName["CBE_Platform"]			= CBaseEntity::BE_PLATFORM;
	m_mapIDtoClassName["CBE_Player"]			= CBaseEntity::BE_PLAYER;
	m_mapIDtoClassName["CBE_PlayerPseudoAircraft"]		= CBaseEntity::BE_PLAYERPSEUDOAIRCRAFT;
	m_mapIDtoClassName["CBE_PlayerPseudoLeggedVehicle"] = CBaseEntity::BE_PLAYERPSEUDOLEGGEDVEHICLE;
	m_mapIDtoClassName["CBE_PlayerShip"]		= CBaseEntity::BE_PLAYERSHIP;
	m_mapIDtoClassName["CBE_PointLight"]		= CBaseEntity::BE_POINTLIGHT;
	m_mapIDtoClassName["CBE_SmokeTrace"]		= CBaseEntity::BE_SMOKETRACE;
	m_mapIDtoClassName["CBE_SupplyItem"]		= CBaseEntity::BE_SUPPLYITEM;
	m_mapIDtoClassName["CBE_TextureAnimation"]	= CBaseEntity::BE_TEXTUREANIMATION;
	m_mapIDtoClassName["CBE_Turret"]			= CBaseEntity::BE_TURRET;
	m_mapIDtoClassName["CBE_EnemyAircraft"]		= CBaseEntity::BE_ENEMYAIRCRAFT;
	m_mapIDtoClassName["CBE_DirectionalLight"]	= CBaseEntity::BE_DIRECTIONALLIGHT;
	m_mapIDtoClassName["CBE_StaticParticleSet"]	= CBaseEntity::BE_STATICPARTICLESET;
	m_mapIDtoClassName["CBE_NozzleExhaust"]		= CBaseEntity::BE_NOZZLEEXHAUST;
	m_mapIDtoClassName["CBE_EnemyShip"]			= CBaseEntity::BE_ENEMYSHIP;
	m_mapIDtoClassName["CBE_StaticGeometry"]	= CBaseEntity::BE_STATICGEOMETRY;
	m_mapIDtoClassName["CBE_Skybox"]			= CBaseEntity::BE_SKYBOX;
	m_mapIDtoClassName["CBE_ScriptedCamera"]	= CBaseEntity::BE_SCRIPTEDCAMERA;
	m_mapIDtoClassName["CBE_CameraController"]	= CBaseEntity::BE_CAMERACONTROLLER;
	m_mapIDtoClassName["CBE_IndividualEntity"]	= CBaseEntity::BE_INDIVIDUALENTITY;
}


CBaseEntityManager::~CBaseEntityManager()
{
	SafeDelete( m_pBaseEntityDatabase );
}


bool CBaseEntityManager::LoadAllBaseEntitiesFromRootTextFile( const string& strRootTextFile,
															  vector<CBaseEntity *>& vecpBaseEntity )
{
	CTextFileScanner scanner;
	if( !scanner.OpenFile( strRootTextFile ) )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + strRootTextFile );
		return false;
	}

	// change to the directory of 'strRootTextFile'
	dir_stack dir_stk( get_path( strRootTextFile ) );

	vector<string> vecSrcTextFile;
	string tag, filename, database_filename;
	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();

		if( scanner.TryScanLine( "input", filename ) )
			vecSrcTextFile.push_back( filename );

		scanner.TryScanLine( "output", database_filename );
	}

	if( database_filename.length() == 0 )	{ LOG_PRINT_ERROR( " - output filename is not specified." ); return false; }
	if( vecSrcTextFile.size() == 0 )		{ LOG_PRINT_ERROR( " - no input file is specified." ); return false; }

	size_t i, num_input_files = vecSrcTextFile.size();
	for( i=0; i<num_input_files; i++ )
	{
		LoadAllBaseEntitiesFromTextFile( vecSrcTextFile[i], vecpBaseEntity );
	}

	OutputDatabaseFile( database_filename, vecpBaseEntity );

	dir_stk.prevdir();

	return true;
}


bool CBaseEntityManager::LoadAllBaseEntitiesFromTextFile( const string& strSrcTextFile,
														  vector<CBaseEntity *>& vecpBaseEntity )
{

	CTextFileScanner scanner;
	if( !scanner.OpenFile( strSrcTextFile ) )
	{
		LOG_PRINT_ERROR( " - Cannot open file: " + strSrcTextFile );
		return false;
	}

	CBaseEntity* pNewBaseEntity = NULL;
	string tag, classname = "";
	for( ; !scanner.End(); scanner.NextLine() )
	{
		tag = scanner.GetTagString();
		if( tag == "CLASSNAME" )
		{
			scanner.ScanLine( tag, classname );

			LOG_PRINT( "loading a base entity: " + classname );

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


bool CBaseEntityManager::OutputDatabaseFile( const string& database_filename,
											 vector<CBaseEntity *>& vecpBaseEntity )
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
		CBaseEntity *pBaseEntity = vecpBaseEntity[i];
		db.AddPolymorphicData( pBaseEntity->GetNameString(), pBaseEntity, *m_pBaseEntityFactory );
	}

	return true;
}


CBaseEntity *CBaseEntityManager::LoadBaseEntity( const string& base_entity_name )
{
	if( base_entity_name.length() == 0 )
	{
		LOG_PRINT_ERROR( " - no base entity name" );
		return NULL;
	}

	LOG_PRINT( " - loading a base entity '" + base_entity_name + "' from the database: " );

	CBaseEntity *pEntity = NULL;
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


CBaseEntity* CBaseEntityManager::CreateBaseEntity( const string& baseentity_classname )
{
	return m_pBaseEntityFactory->CreateBaseEntity( m_mapIDtoClassName[baseentity_classname] );
}


bool CBaseEntityManager::OpenDatabase( const std::string& filename )
{
	SafeDelete( m_pBaseEntityDatabase );

	m_pBaseEntityDatabase = new CBinaryDatabase<string>;

	if( !m_pBaseEntityDatabase->Open( filename.c_str() ) )
		return false;

	return true;
}


bool CBaseEntityManager::UpdateDatabase( const string& strSrcTextFile )
{
	vector<CBaseEntity *> vecpBaseEntity;
	vecpBaseEntity.reserve( 64 );

//	LoadAllBaseEntitiesFromTextFile( strSrcTextFile, vecpBaseEntity );
	LoadAllBaseEntitiesFromRootTextFile( strSrcTextFile, vecpBaseEntity );

	SafeDeleteVector( vecpBaseEntity );

	//log->Print( "the base entity database has been updated" );

	return true;
}
