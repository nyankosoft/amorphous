#include "LightWaveSceneLoader.hpp"
#include "amorphous/Support/TextFileScanner.hpp"
#include "amorphous/Support/StringAux.hpp"


namespace amorphous
{

using std::string;
using std::vector;
using namespace boost;


bool LWSceneInfo::LoadSceneInfo( CTextFileScanner& scanner )
{
	if( scanner.GetTagString() == "LWSC" )
	{
		// Assume the next line is the version number (expects no empty lines).
		scanner.NextLine();
		m_Version = to_int( scanner.GetCurrentLine() );
		return true;
	}
	else if( scanner.TryScanLine( "RenderRangeType",      m_RenderRangeType      ) ) return true;
	else if( scanner.TryScanLine( "FirstFrame",           m_FirstFrame           ) ) return true;
	else if( scanner.TryScanLine( "LastFrame",            m_LastFrame            ) ) return true;
	else if( scanner.TryScanLine( "FrameStep",            m_FrameStep            ) ) return true;
//	else if( scanner.TryScanLine( "RenderRangeArbitrary", m_RenderRangeArbitrary ) ) return true;
	else if( scanner.TryScanLine( "PreviewFirstFrame",    m_PreviewFirstFrame    ) ) return true;
	else if( scanner.TryScanLine( "PreviewLastFrame",     m_PreviewLastFrame     ) ) return true;
	else if( scanner.TryScanLine( "PreviewFrameStep",     m_PreviewFrameStep     ) ) return true;
	else if( scanner.TryScanLine( "CurrentFrame",         m_CurrentFrame         ) ) return true;
	else if( scanner.TryScanLine( "FramesPerSecond",      m_FramesPerSecond      ) ) return true;
	else
		return false;
}


LightWaveSceneLoader::LightWaveSceneLoader()
{
	m_afAmbientColor[0] = 0;
	m_afAmbientColor[1] = 0;
	m_afAmbientColor[2] = 0;
	m_fAmbientIntensity = 0;

	m_vecObjectLayer.reserve( 64 );
	m_vecLight.reserve( 64 );
	m_vecpBone.reserve( 256 );
}


boost::shared_ptr<LWS_Bone> GetBone( int bone_id, std::vector< boost::shared_ptr<LWS_Bone> >& vecpBone )
{
	const int num_bones = (int)vecpBone.size();
	for( int i=0; i<num_bones; i++ )
	{
		if( vecpBone[i]->GetItemID() == bone_id )
			return vecpBone[i];
	}

	return boost::shared_ptr<LWS_Bone>();
}


void LightWaveSceneLoader::UpdateItemTrees()
{
	const int num_bones = (int)m_vecpBone.size();
	for( int i=0; i<num_bones; i++ )
	{
		LWS_Bone& bone = *(m_vecpBone[i].get());

		if( bone.GetParentType() != 4 )
			continue;

		const int parent_index = bone.GetParentIndex();
		if( 0 <= parent_index && parent_index < num_bones )
			m_vecpBone[parent_index]->ChildBone().push_back( m_vecpBone[i] );

		bone.m_pParentBone = GetBone( bone.GetParentIndex() );
	}
}


bool LightWaveSceneLoader::LoadFromFile( const std::string& filepath )
{
	CTextFileScanner scanner;

	if( !scanner.OpenFile( filepath ) )
		return false;

	// clear the scene currently stored
	Clear();

	string tag, strLine;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		// used in scene file with version 5 or later
		string item_id_in_hex;
//		int item_id = -1;

		scanner.GetTagString( tag );
		scanner.GetCurrentLine( strLine );

/*		if( 0 < m_vecObjectLayer.size() &&
			m_vecObjectLayer.back().LoadFromFile( scanner ) )
			continue;

		if( 0 < m_vecLight.size() &&
			m_vecLight.back().LoadFromFile( scanner ) )
			continue;*/

		if( m_SceneInfo.LoadSceneInfo( scanner ) )
			continue;

		if( 0 < m_vecpItem.size() && m_vecpItem.back()->LoadFromFile( scanner ) )
		{
			continue;
		}

		if( tag == "LoadObjectLayer" )
		{
			m_vecObjectLayer.push_back( shared_ptr<LWS_ObjectLayer>( new LWS_ObjectLayer() ) );
			if( 5 <= m_SceneInfo.m_Version )
			{
				scanner.ScanLine( tag,
								  m_vecObjectLayer.back()->m_iLayerNumber,
								  item_id_in_hex,
								  m_vecObjectLayer.back()->m_strObjectFilename );
			}
			else
			{
				scanner.ScanLine( tag,
								  m_vecObjectLayer.back()->m_iLayerNumber,
								  m_vecObjectLayer.back()->m_strObjectFilename );
			}

			m_vecpItem.push_back( m_vecObjectLayer.back().get() );
			m_vecpItem.back()->SetItemID( from_hex_to_int(item_id_in_hex) );

			continue;
		}

		else if( tag == "AddNullObject" )
		{
			m_vecObjectLayer.push_back( shared_ptr<LWS_ObjectLayer>( new LWS_ObjectLayer() ) );
			scanner.ScanLine( tag, m_vecObjectLayer.back()->m_strObjectFilename );
			m_vecObjectLayer.back()->SetNullObject( true );

			m_vecpItem.push_back( m_vecObjectLayer.back().get() );

			continue;
		}

		else if( tag == "AddBone" )
		{
			m_vecpBone.push_back( shared_ptr<LWS_Bone>( new LWS_Bone() ) );

			if( 5 <= m_SceneInfo.m_Version )
				scanner.ScanLine( tag, item_id_in_hex );

			m_vecpItem.push_back( m_vecpBone.back().get() );
			m_vecpItem.back()->SetItemID( from_hex_to_int(item_id_in_hex) );

			continue;
		}

		else if( tag == "AddLight" )
		{
			m_vecLight.push_back( shared_ptr<LWS_Light>( new LWS_Light() ) );

			if( 5 <= m_SceneInfo.m_Version )
				scanner.ScanLine( tag, item_id_in_hex );

			m_vecpItem.push_back( m_vecLight.back().get() );
			m_vecpItem.back()->SetItemID( from_hex_to_int(item_id_in_hex) );

			continue;
		}

/*		else if( tag == "AddCamera" )
		{
			m_vecpCamera.push_back( shared_ptr<LWS_Camera>( new LWS_Camera() ) );

			if( 5 <= m_SceneInfo.m_Version )
				scanner.ScanLine( tag, item_id );

			m_vecpItem.push_back( m_vecpCamera.back().get() );
			m_vecpItem.back()->SetItemID( item_id );

			continue;
		}
*/
		else if( tag == "AmbientColor" )
		{
			scanner.ScanLine( tag, m_afAmbientColor[0], m_afAmbientColor[1], m_afAmbientColor[2] );
			continue;
		}

		else if( tag == "AmbientIntensity" )
		{
			scanner.ScanLine( tag, m_fAmbientIntensity );
			continue;
		}

		else if( LoadFogDataBlock(scanner) )
			continue;

	}

	UpdateItemTrees();

	/// create links from parent items to child items
	AddParentToChildLinks();

	return true;
}


void LightWaveSceneLoader::AddParentToChildLinks()
{
	int i, num_object_layers = (int)m_vecObjectLayer.size();

	for( i=0; i<num_object_layers; i++ )
	{
		if( m_vecObjectLayer[i]->HasParent() )
		{
			int parent_index = m_vecObjectLayer[i]->GetParentIndex();

			switch( m_vecObjectLayer[i]->GetParentType() )
			{
			case LWS_Item::TYPE_OBJECT:
				m_vecObjectLayer[parent_index]->AddChildItemInfo( LWS_Item::TYPE_OBJECT, i );
				break;
			case LWS_Item::TYPE_LIGHT:
				m_vecLight[parent_index]->AddChildItemInfo( LWS_Item::TYPE_OBJECT, i );
				break;
/*			case LWS_Item::TYPE_CAMERA:
				break;
			case LWS_Item::TYPE_BONE:
				break;*/
			default:
				break;
			}
		}
	}

	int num_lights = (int)m_vecLight.size();
	for( i=0; i<num_lights; i++ )
	{
		if( m_vecLight[i]->HasParent() )
		{
			int parent_index = m_vecLight[i]->GetParentIndex();

			switch( m_vecLight[i]->GetParentType() )
			{
			case LWS_Item::TYPE_OBJECT:
				m_vecObjectLayer[parent_index]->AddChildItemInfo( LWS_Item::TYPE_LIGHT, i );
				break;
			case LWS_Item::TYPE_LIGHT:
				m_vecLight[parent_index]->AddChildItemInfo( LWS_Item::TYPE_LIGHT, i );
				break;
/*			case LWS_Item::TYPE_CAMERA:
				break;
			case LWS_Item::TYPE_BONE:
				break;*/
			}
		}
	}

	const int num_items = (int)m_vecpItem.size();
	for( i=0; i<num_items; i++ )
	{
		m_vecpItem[i]->m_pParent = GetItemByID( m_vecpItem[i]->GetParentID() );
	}
}


bool LightWaveSceneLoader::LoadFogDataBlock( CTextFileScanner& scanner )
{
	string tag, strLine;

	scanner.GetTagString( tag );
	scanner.GetCurrentLine( strLine );

	if( tag == "FogType" )
	{
		scanner.ScanLine( tag, m_Fog.iType );

//		if( m_Fog.iType == 0 )
//			// Fog is not enabled

		return true;
	}

	else if( tag == "FogMinDistance" )
	{
		scanner.ScanLine( tag, m_Fog.fMinDist );
		return true;
	}

	else if( tag == "FogMaxDistance" )
	{
		scanner.ScanLine( tag, m_Fog.fMaxDist );
		return true;
	}

	else if( tag == "FogMinAmount" )
	{
		scanner.ScanLine( tag, m_Fog.fMinAmount );
		return true;
	}

	else if( tag == "FogMaxAmount" )
	{
		scanner.ScanLine( tag, m_Fog.fMaxAmount );
		return true;
	}

	else if( tag == "FogColor" )
	{
		scanner.ScanLine( tag, m_Fog.afColor[0], m_Fog.afColor[1], m_Fog.afColor[2] );
		return true;
	}

	else
		return false;

/*	char acSlag[256], cType;

	if( strncmp( pcLine, "FogType", 7 ) != 0 )
		return;	//This line is not a fog block

	sscanf(pcLine, "%s %d", acSlag, &cType);
	if( cType == 0 )
		return;	//Fog is not enabled

	m_Fog.iType = cType;
	fscanf(fp, "%s %f", acSlag, &m_Fog.fMinDist);
	fscanf(fp, "%s %f", acSlag, &m_Fog.fMaxDist);
	fscanf(fp, "%s %f", acSlag, &m_Fog.fMinAmount);
	fscanf(fp, "%s %f", acSlag, &m_Fog.fMaxAmount);
	fscanf(fp, "%s %f %f %f", acSlag, &m_Fog.afColor[0], &m_Fog.afColor[1], &m_Fog.afColor[2] );

	return;*/
}


LWS_ObjectLayer* LightWaveSceneLoader::GetObjectLayer(int i)
{
	if( i<0 || (int)m_vecObjectLayer.size()<=i )
		return NULL;

	return m_vecObjectLayer[i].get();
}


LWS_Light* LightWaveSceneLoader::GetLight(int i)
{
	if( i<0 || (int)m_vecLight.size()<=i )
		return NULL;

	return m_vecLight[i].get();
}


shared_ptr<LWS_Bone> LightWaveSceneLoader::GetBone(int i)
{
	if( i<0 || (int)m_vecpBone.size()<=i )
		return shared_ptr<LWS_Bone>();

	return m_vecpBone[i];
}


std::vector< boost::shared_ptr<LWS_Bone> > LightWaveSceneLoader::GetRootBones()
{
	vector< shared_ptr<LWS_Bone> > vecpRootBone;
	vecpRootBone.reserve( 8 );
	const int num_bones = (int)m_vecpBone.size();
	for( int i=0; i<num_bones; i++ )
	{
		if( m_vecpBone[i]->GetParentType() == 1 )
			vecpRootBone.push_back( m_vecpBone[i] );
	}

	return vecpRootBone;
}


LWS_Fog* LightWaveSceneLoader::GetFog()
{
	if( m_Fog.iType == 0 )
		return NULL;	//no fog in this scene

	return &m_Fog;
}


void LightWaveSceneLoader::Clear()
{
	m_vecpItem.resize( 0 );
	m_vecObjectLayer.resize( 0 );
	m_vecLight.resize( 0 );
	m_vecpBone.resize( 0 );
}


} // amorphous
