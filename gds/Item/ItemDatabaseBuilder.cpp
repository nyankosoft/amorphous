
#include "ItemDatabaseBuilder.h"

using namespace std;

#include "Support/TextFileScanner.h"

#include "Item/GameItemObjectFactory.h"
#include "Item/GameItem.h"
#include "Item/GI_Weapon.h"
#include "Item/GI_Ammunition.h"
#include "Item/GI_GravityGun.h"
//#include "Item/GI_NightVision.h"
#include "Item/GI_MissileLauncher.h"
#include "Item/GI_Aircraft.h"
#include "Item/GameItemDatabase.h"

#include "GameCommon/MeshBoneController_Aircraft.h"

#include "Support/Log/DefaultLog.h"
#include "Support/SafeDeleteVector.h"
#include "Support/fnop.h"
using namespace fnop;
#include "Support/Serialization/Serialization.h"
#include "Support/Serialization/ArchiveObjectFactory.h"
#include "Support/Serialization/BinaryDatabase.h"
using namespace GameLib1::Serialization;


CItemDatabaseBuilder::CItemDatabaseBuilder()
{
}


CItemDatabaseBuilder::~CItemDatabaseBuilder()
{
	SafeDeleteVector( m_vecpItem );
}


void CItemDatabaseBuilder::LoadGameItemSharedProperty( CTextFileScanner& scanner, CGameItem* pItem )
{
	scanner.TryScanLine( "name",			pItem->m_strName );
	scanner.TryScanLine( "max_quantity",	pItem->m_iMaxQuantity );
	scanner.TryScanLine( "price",			pItem->m_Price );

//	scanner.TryScanLine( "desc_jp",			pItem->m_Desc.text[Lang::Japanese] );
//	scanner.TryScanLine( "desc_en",			pItem->m_Desc.text[Lang::English] );
	scanner.TryScanSentence( "desc_jp",			pItem->m_Desc.text[Lang::Japanese] );
	scanner.TryScanSentence( "desc_en",			pItem->m_Desc.text[Lang::English] );

	scanner.TryScanLine( "model_file",		pItem->m_MeshObjectContainer.m_MeshDesc.ResourcePath );

	string type_str;
	if( scanner.TryScanLine( "mesh_type",	type_str ) )
	{
		CMeshType::Name mesh_type = CMeshType::BASIC;
		if( type_str == "skeletal" )         mesh_type = CMeshType::SKELETAL;
		else if( type_str == "progressive" ) mesh_type = CMeshType::PROGRESSIVE;
		else if( type_str == "normal" )      mesh_type = CMeshType::BASIC;
		pItem->m_MeshObjectContainer.m_MeshDesc.MeshType = mesh_type;
	}
}


void CItemDatabaseBuilder::LoadFirearms( CTextFileScanner& scanner, CGI_Weapon* pWeapon )
{
	LoadGameItemSharedProperty( scanner, pWeapon );

	string str;

	scanner.TryScanLine( "grouping",			pWeapon->m_fGrouping );
	scanner.TryScanLine( "fire_rate",			pWeapon->m_fFireInterval );
	scanner.TryScanLine( "muzzle_speed_factor",	pWeapon->m_fMuzzleSpeedFactor );
	scanner.TryScanLine( "num_bursts",			pWeapon->m_iNumBursts );
//	scanner.TryScanLine( "burst_interval",		pWeapon->m_fBurstInterval );
	scanner.TryScanLine( "local_recoil_force",	pWeapon->m_vLocalRecoilForce );
	scanner.TryScanLine( "muzzle_end_pos",		pWeapon->m_MuzzleEndLocalPose.vPosition );
	if( scanner.TryScanLine("ammo_type",str) || scanner.TryScanLine("caliber",str) )
	{
		strcpy( pWeapon->m_acAmmoType, str.c_str() );
	}
	if( scanner.TryScanLine( "fire_sound", str ) )
	{
		pWeapon->m_FireSound.SetResourceName( str.c_str() );
	}
}


void CItemDatabaseBuilder::LoadGravityGun( CTextFileScanner& scanner, CGI_GravityGun* pGravityGun )
{
	LoadGameItemSharedProperty( scanner, pGravityGun );

	scanner.TryScanLine( "grasp_range",	pGravityGun->m_fGraspRange );
//	scanner.TryScanLine( "power",		pGravityGun->m_fPower );
}


void CItemDatabaseBuilder::LoadAmmunition( CTextFileScanner& scanner, CGI_Ammunition* pAmmo )
{
	LoadGameItemSharedProperty( scanner, pAmmo );

	string str;

	scanner.TryScanLine( "muzzle_speed",	pAmmo->m_fMuzzleSpeed );
	scanner.TryScanLine( "power",			pAmmo->m_fPower );
	scanner.TryScanLine( "num_pellets",		pAmmo->m_iNumPellets );
	if( scanner.TryScanLine("ammo_type",str) || scanner.TryScanLine("caliber",str) )
	{
		strcpy( pAmmo->m_acAmmoType, str.c_str() );
	}
	if( scanner.TryScanLine( "ammo_entity", str ) )
	{
		pAmmo->m_AmmoBaseEntity.SetBaseEntityName( str.c_str() );
	}
	if( scanner.TryScanLine( "muzzle_flash_entity", str ) )
	{
		pAmmo->m_MuzzleFlashBaseEntity.SetBaseEntityName( str.c_str() );
	}

	// range - used by missile item
	scanner.TryScanLine( "range",	pAmmo->m_fRange );
}


void CItemDatabaseBuilder::LoadBinocular( CTextFileScanner& scanner, CGI_Binocular* pBinocular )
{
	LoadGameItemSharedProperty( scanner, pBinocular );

	scanner.TryScanLine( "max_zoom",		pBinocular->m_fMaxZoom );
}


void CItemDatabaseBuilder::LoadNightVision( CTextFileScanner& scanner, CGI_NightVision* pNV )
{
	LoadGameItemSharedProperty( scanner, pNV );

	string tag;
	scanner.GetTagString( tag );

	if( scanner.TryScanLine( "battery_life", pNV->m_fMaxBatteryLife ) )
	{
		pNV->m_fBatteryLeft = pNV->m_fMaxBatteryLife;	// battery is charged full by default
	}
	scanner.TryScanLine( "charge_speed",		pNV->m_fChargeSpeed );
}


void CItemDatabaseBuilder::LoadMissileLauncher( CTextFileScanner& scanner, CGI_MissileLauncher* pItem )
{
	LoadFirearms( scanner, pItem );
//	LoadGameItemSharedProperty( scanner, pItem );

	string tag;
	scanner.GetTagString( tag );
	int num_release_positions;

	CCamera m_SensorCamera;

	scanner.TryScanLine( "sensor_angle",	pItem->m_fValidSensorAngle );

	scanner.TryScanLine( "sensor_range",	pItem->m_fMaxSensorRange );

	/// the number of targets this launcher can simultaneously lock on
	scanner.TryScanLine( "max_simul_targets",		pItem->m_NumMaxSimulTargets );

	if( scanner.TryScanLine( "num_release_positions",	num_release_positions ) )
	{
		pItem->SetNumReleasePositions( num_release_positions );
	}
}

void CItemDatabaseBuilder::AddMeshBoneControllerForAircraft( CGI_Aircraft& aircraft,
															 CTextFileScanner& scanner,
						                                     vector<CMeshBoneController_AircraftBase *>& vecpMeshController )
{
	string tag, type;
	char _tag[32], _type[32];
	float angle_per_accel, angle_per_pitchaccel, angle_per_rollaccel;
	int dir;
	char dir_name[32];
//	string flapR_name, flapL_name, vflap0_name, vflap1_name, rotor_name;
	char flapR_name[64], flapL_name[64], vflap0_name[64], vflap1_name[64], rotor_name[64];
//	const char* pCurrentLine = scanner.GetCurrentLine().c_str();
	string current_line = scanner.GetCurrentLine();

	scanner.ScanLine( tag, type );
	if( type == "flap" )
	{
//		scanner.ScanLine( tag, type, angle_per_pitchaccel, angle_per_rollaccel, flapR_name, flapL_name );
		sscanf( current_line.c_str(), "%s %s %f %f %s %s",
			_tag, _type, &angle_per_pitchaccel, &angle_per_rollaccel, flapR_name, flapL_name );
		CMeshBoneController_Flap* pFlapControl = new CMeshBoneController_Flap();
		pFlapControl->m_fAnglePerPitchAccel = angle_per_pitchaccel;
		pFlapControl->m_fAnglePerRollAccel = angle_per_rollaccel;
		pFlapControl->m_vecBoneControlParam.resize( 2 );
		pFlapControl->m_vecBoneControlParam[0].Name = flapR_name;
		pFlapControl->m_vecBoneControlParam[1].Name = flapL_name;
		aircraft.m_vecpMeshController.push_back( pFlapControl );
	}
	else if( type == "tvflap" )
	{
//		scanner.ScanLine( tag, type, vflap_type, vflap0_name, vflap1_name );
		sscanf( current_line.c_str(), "%s %s %f %s %s",
			_tag, _type, &angle_per_accel, vflap0_name, vflap1_name );
		CMeshBoneController_VFlap* pFlapControl = new CMeshBoneController_VFlap();
		pFlapControl->m_fAnglePerAccel = angle_per_accel;
		pFlapControl->m_vecBoneControlParam.resize( 2 );
		pFlapControl->m_vecBoneControlParam[0].Name = vflap0_name;
		pFlapControl->m_vecBoneControlParam[1].Name = vflap1_name;
		aircraft.m_vecpMeshController.push_back( pFlapControl );
	}
	else if( type == "svflap" )
	{
//		scanner.ScanLine( tag, type, angle_per_accel, vflap0_name );
		sscanf( current_line.c_str(), "%s %s %f %s", _tag, _type, &angle_per_accel, vflap0_name );
		CMeshBoneController_VFlap* pFlapControl = new CMeshBoneController_VFlap();
		pFlapControl->m_fAnglePerAccel = angle_per_accel;
		pFlapControl->m_vecBoneControlParam.resize( 1 );
		pFlapControl->m_vecBoneControlParam[0].Name = vflap0_name;
		aircraft.m_vecpMeshController.push_back( pFlapControl );
	}
	else if( type == "rotor" )
	{
//		scanner.ScanLine( tag, type, rotation_direction, rotor_name );
		sscanf( current_line.c_str(), "%s %s %s %s", _tag, _type, dir_name, rotor_name );
		CMeshBoneController_Rotor* pRotorControl = new CMeshBoneController_Rotor();
		if( dir_name == "ccw" )		dir = CMeshBoneController_Rotor::DIR_CCW;
		else if( dir_name == "cw" )	dir = CMeshBoneController_Rotor::DIR_CW;
		else dir = CMeshBoneController_Rotor::DIR_CW;
		pRotorControl->m_RotationDirection = dir;
		pRotorControl->m_vecBoneControlParam.resize( 1 );
		pRotorControl->m_vecBoneControlParam[0].Name = rotor_name;
		aircraft.m_vecpMeshController.push_back( pRotorControl );
	}
}


void CItemDatabaseBuilder::LoadAircraft( CTextFileScanner& scanner, CGI_Aircraft* pItem )
{
	LoadGameItemSharedProperty( scanner, pItem );

	string tag;
	scanner.GetTagString( tag );

	int index;
	Vector3 pos;

	string ammo_name;
	int weapon_slot=0, max_quantity=0;

	scanner.TryScanLine( "default_accel",	pItem->m_fAccel );
	scanner.TryScanLine( "boost_accel",		pItem->m_fBoostAccel );
	scanner.TryScanLine( "brake_accel",		pItem->m_fBrakeAccel );

	scanner.TryScanLine( "max_pitch_accel",	pItem->m_fMaxPitchAccel );
	scanner.TryScanLine( "max_roll_accel",	pItem->m_fMaxRollAccel );
	scanner.TryScanLine( "max_yaw_accel",	pItem->m_fMaxYawAccel );

	scanner.TryScanLine( "gun_muzzle_end_pos",pItem->m_vGunMuzzleEndLocalPos );

	scanner.TryScanLine( "cockpit_pos",	pItem->m_CockpitLocalPose.vPosition );
	scanner.TryScanLine( "3rd_person_view",	pItem->m_vThirdPersonViewOffset );
	scanner.TryScanLine( "ceiling",		pItem->m_fCeiling );
	scanner.TryScanLine( "armor",		pItem->m_fArmor );
	scanner.TryScanLine( "rcs",			pItem->m_fRCS );
/*	scanner.TryScanLine( "gear_height",	pItem->m_fGearUnitHeight );

	if( scanner.TryScanLine( "nozzle_pos", index, pos ) )
	{
		for( int i=pItem->m_vecNozzleExhaustPose.size(); i<=index; i++ )
			pItem->m_vecNozzleExhaustPose.push_back( Matrix34Identity() );

		pItem->m_vecNozzleExhaustPose[index].vPosition = pos;
	}
*/
	if( scanner.TryScanLine( "ammo_release_pos", index, pos ) )
	{
		if( index < (int)pItem->m_AmmoReleaseLocalPose.size() )
			pItem->m_AmmoReleaseLocalPose[index].vPosition = pos;
	}

	if( scanner.TryScanLine( "payload", ammo_name, weapon_slot, max_quantity ) )
	{
//		MsgBoxFmt( "setting payload info to %s: %s, %d, %d",
//			pItem->GetName().c_str(), ammo_name.c_str(), weapon_slot, max_quantity );
		pItem->m_vecSupportedAmmo.push_back( CGI_Aircraft::AmmoPayload( ammo_name, weapon_slot, max_quantity ) );
	}

	static NozzleFlameParams s_NozzleFlameParams = NozzleFlameParams();

	if( tag == "nozzle_flame_sld" )
	{
		// start position, length and diameter (round nozzle flame)
		// this is interpreted as a new nozzle flame param set
		// - stored to the static variable and used as shared properties for the following nozzle_flame_pos params
		s_NozzleFlameParams.LoadFromFile( scanner );
		s_NozzleFlameParams.LocalPose.matOrient = Matrix33RotationY( 3.141592f );	// direction is set to backwards by default
		return;
	}

	if( tag == "nozzle_flame_slwh" )
	{
		// start position, length, width and height (rectangular nozzle flame)
		// this is interpreted as a new nozzle flame param set
		// temporarily store it to the static variable
		// - stored to the static variable and used as shared properties for the following nozzle_flame_pos params
		s_NozzleFlameParams.LoadFromFile( scanner );
		s_NozzleFlameParams.LocalPose.matOrient = Matrix33RotationY( 3.141592f );	// direction is set to backwards by default
		return;
	}

	if( tag == "nozzle_flame_pos" )
	{
		// nozzle flame position - must be defined after (start,length,...) params and before any other params
		pItem->m_vecNozzleFlameParams.push_back( s_NozzleFlameParams );
		if( pItem->m_vecNozzleFlameParams.back().LoadFromFile( scanner ) )
			return;
	}

	if( tag == "mesh_ctrl" )
	{
		AddMeshBoneControllerForAircraft( *pItem, scanner, pItem->m_vecpMeshController );
	}

	CAircraftRotor rotor = CAircraftRotor();
	if( scanner.TryScanLine( "rotor", rotor.fRotationSpeed, rotor.fAngleOffset ) )
		pItem->m_vecRotor.push_back( rotor );
}


bool CItemDatabaseBuilder::LoadItemsFromTextFile( const std::string filename )
{
	CTextFileScanner scanner;
	
	if( !scanner.OpenFile( filename ) )
		return false;

	CGameItem *pObject = NULL;

	CGameItemObjectFactory factory;

	string tag, strClassName, strLine;
//	unsigned int id;
	int id = IArchiveObjectBase::INVALID_ID;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.GetCurrentLine( strLine );

		if( !scanner.GetTagString( tag ) )
			break;

		if( strLine.find( "\\begin:" ) == 0 )
		{
			// a new item desc has started
			strClassName = strLine.substr( 7, 1024 );
			size_t len = strClassName.length();
			if( len == 0 )
				continue;

//			if( strClassName[len-1] == '\n' )
			while( strClassName[len-1] == '\n' || strClassName[len-1] == '\t' || strClassName[len-1] == ' ' )
			{
//				strClassName[len-1] = '\0';
				strClassName = strClassName.substr( 0, strClassName.length() - 1 );
				len--;
			}

			id = GetItemID( strClassName );

//			if( id < 0 )
			if( id == IArchiveObjectBase::INVALID_ID )
				continue;

			pObject = (CGameItem *)factory.CreateObject( id );

			if( !pObject )
				continue;

//			result = LoadCharacter( scanner );
//			if( !result )
//				return false;
		}

		else if( strncmp(strLine.c_str(), "\\end:", 5) == 0 )
		{
			if( pObject )
			{
				m_vecpItem.push_back( pObject );
				pObject = NULL;
			}
			id = -1;
		}

		// load properties according to the type of item 
		switch( id )
		{
		case CGameItem::ID_AMMUNITION:
			LoadAmmunition( scanner, (CGI_Ammunition *)pObject );
			break;
		case CGameItem::ID_FIREARMS:
			LoadFirearms( scanner, (CGI_Weapon *)pObject );
			break;
		case CGameItem::ID_GRAVITY_GUN:
			LoadGravityGun( scanner, (CGI_GravityGun *)pObject );
			break;
		case CGameItem::ID_BINOCULAR:
			LoadBinocular( scanner, (CGI_Binocular *)pObject );
			break;
		case CGameItem::ID_NIGHT_VISION:
			LoadNightVision( scanner, (CGI_NightVision *)pObject );
			break;
		case CGameItem::ID_MISSILELAUNCHER:
			LoadMissileLauncher( scanner, (CGI_MissileLauncher *)pObject );
			break;
		case CGameItem::ID_AIRCRAFT:
			LoadAircraft( scanner, (CGI_Aircraft *)pObject );
			break;
		}
	}

//	OutputDatabaseFile( pcDestFilename );

	return true;
}

int CItemDatabaseBuilder::GetItemID( const string& class_name )
{
/*	if( !strcmp(class_name.c_str(),"Firearm") )			return CGameItem::ID_FIREARMS;
	else if( !strcmp(class_name.c_str(),"Ammunition") )	return CGameItem::ID_AMMUNITION;
	else if( !strcmp(class_name.c_str(),"GravityGun") )	return CGameItem::ID_GRAVITY_GUN;
	else if( !strcmp(class_name.c_str(),"Binocular") )	return CGameItem::ID_BINOCULAR;
	else if( !strcmp(class_name.c_str(),"NightVision") )	return CGameItem::ID_NIGHT_VISION;
	else if( !strcmp(class_name.c_str(),"Key") )			return CGameItem::ID_KEY;
	else if( !strcmp(class_name.c_str(),"CamouflageDevice") )		return CGameItem::ID_CAMFLOUGE_DEVICE;
	else if( !strcmp(class_name.c_str(),"Suppressor") )	return CGameItem::ID_SUPPRESSOR;
	else if( !strcmp(class_name.c_str(),"Aircraft") )		return CGameItem::ID_AIRCRAFT;
	else if( !strcmp(class_name.c_str(),"MissileLauncher") )		return CGameItem::ID_MISSILELAUNCHER;
*/
	if( class_name == "Firearm" )			return CGameItem::ID_FIREARMS;
	else if( class_name == "Ammunition" )	return CGameItem::ID_AMMUNITION;
	else if( class_name == "GravityGun" )	return CGameItem::ID_GRAVITY_GUN;
	else if( class_name == "Binocular" )	return CGameItem::ID_BINOCULAR;
	else if( class_name == "NightVision" )	return CGameItem::ID_NIGHT_VISION;
	else if( class_name == "Key" )			return CGameItem::ID_KEY;
	else if( class_name == "CamouflageDevice" )		return CGameItem::ID_CAMFLOUGE_DEVICE;
	else if( class_name == "Suppressor" )	return CGameItem::ID_SUPPRESSOR;
	else if( class_name == "Aircraft" )		return CGameItem::ID_AIRCRAFT;
	else if( class_name == "MissileLauncher" )		return CGameItem::ID_MISSILELAUNCHER;

	else
	{
		g_Log.Print( "invalid item name:" + class_name );
		return IArchiveObjectBase::INVALID_ID;
	}
}	


/*
== operator of stl::string class does not return true if the numbers of null characters are different
unsigned int CItemDatabaseBuilder::GetItemID( string& strClassName )
{
	if( strClassName == "Firearms" )			return CGameItem::ID_FIREARMS;
	else if( strClassName == "Ammunition" )		return CGameItem::ID_AMMUNITION;
	else if( strClassName == "GravityGun" )		return CGameItem::ID_GRAVITY_GUN;
	else if( strClassName == "Binocular" )		return CGameItem::ID_BINOCULAR;
	else if( strClassName == "NightVision" )	return CGameItem::ID_NIGHT_VISION;
	else return -1;
}*/


bool CItemDatabaseBuilder::OutputDatabaseFile( const std::string output_filename )
{
///	string strBodyFilename;
///	CFileNameOperation::GetBodyFilename( strBodyFilename, output_filename );

///	string strDatabaseFilename = m_strOutputPath + strBodyFilename;

//	return OutputSeparateFiles( strDatabaseFilename );

	return OutputSingleDBFile( output_filename );
}


bool CItemDatabaseBuilder::OutputSingleDBFile( const string& strDBFilename )
{
	CBinaryDatabase<string> m_ItemDB;

	CGameItemObjectFactory factory;

	bool db_open = m_ItemDB.Open( strDBFilename.c_str(), CBinaryDatabase<string>::DB_MODE_NEW );
	if( !db_open )
	{
		LOG_PRINT_ERROR( "Cannot open file: " + strDBFilename );
		return false;
	}

	size_t i, num_items = m_vecpItem.size();
	for( i=0; i<num_items; i++ )
	{
		CGameItem* pItem = m_vecpItem[i];

		m_ItemDB.AddPolymorphicData( pItem->GetName(), pItem, factory );
	}

	return true;
}


bool CItemDatabaseBuilder::CreateItemDatabaseFile( const string& src_filename )
{
	vector<string> input_filename_list;
	string input_filename;
	string output_filename;

	CTextFileScanner scanner( src_filename );
	if( scanner.IsReady() )
	{
		for( ; !scanner.End(); scanner.NextLine() )
		{
			if( scanner.TryScanLine( "input", input_filename ) )
			{
				input_filename_list.push_back( input_filename );
			}

			scanner.TryScanLine( "output", output_filename );
		}
	}
	else
		return false;

	if( input_filename_list.size() == 0 )
		return false;	// no input file - cannot create an item database

	// input&output filenames are given with relative paths
	// - set the path of src_filename as working directory
	dir_stack dirstk( get_path(src_filename) );

	// load item parameters from text
	size_t i, num_input_files = input_filename_list.size();
	for( i=0; i<num_input_files; i++ )
	{
		if( input_filename_list[i].length() ==  0 )
			continue;

/*		if( !has_abspath(input_filename_list[i]) )
		{
			dir_stack dirstk( get_path(src_filename) );
			LoadItemsFromTextFile( input_filename_list[i] );
			dirstk.prevdir();	// back to the previous work directory
		}
		else*/
            LoadItemsFromTextFile( input_filename_list[i] );
	}

	// output a database file
	bool db_created = OutputDatabaseFile( output_filename );
	if( !db_created )
	{
	    dirstk.prevdir();	// back to the previous work directory
		return false;
	}

    dirstk.prevdir();	// back to the previous work directory

	return true;
}

/*
bool CItemDatabaseBuilder::OutputSeparateFiles( const string& strDBFilename )
{	
//	CBinaryArchive_Output archive( strDatabaseFilename.c_str() );

	CGameItemDatabase database;

	string strFilename;
	char acItemBodyFilename[256];

	size_t i, num_items = m_vecpItem.size();
	for( i=0; i<num_items; i++ )
	{
		// make a filename for an item object
		sprintf( acItemBodyFilename, "A%03d.dat", i );
		strFilename = m_strItemFilePath + acItemBodyFilename;

		// store the pair of the filename & the corresponding item name to the item database
		database.AddItemInfo( strFilename, m_vecpItem[i]->GetName() );

		// make a filename for an item file
		strFilename = m_strOutputPath + acItemBodyFilename;

		// save the item object into a separate binary file
		database.SaveItem( strFilename.c_str(), m_vecpItem[i] );
//		CGameItemSerializer temp_serializer;
//		CBinaryArchive_Output archive( strFilename.c_str() );
//		temp_serializer.pItem = m_vecpItem[i];
//		archive << temp_serializer;
	}

	// save only the filenames and item names to the item database
	return database.SaveToFile( strDBFilename.c_str() );
//	return archive << database;
}
*/
