#include "LightWaveSceneLoader.hpp"
#include "Support/TextFileScanner.hpp"

using namespace std;
using namespace boost;


CLightWaveSceneLoader::CLightWaveSceneLoader()
{
	m_afAmbientColor[0] = 0;
	m_afAmbientColor[1] = 0;
	m_afAmbientColor[2] = 0;
	m_fAmbientIntensity = 0;
}


void CLightWaveSceneLoader::UpdateItemTrees()
{
	const int num_bones = (int)m_vecpBone.size();
	for( int i=0; i<num_bones; i++ )
	{
		CLWS_Bone& bone = *(m_vecpBone[i].get());

		if( bone.GetParentType() != 4 )
			continue;

		const int parent_index = bone.GetParentIndex();
		if( 0 <= parent_index && parent_index < num_bones )
			m_vecpBone[parent_index]->ChildBone().push_back( m_vecpBone[i] );
	}
}


bool CLightWaveSceneLoader::LoadFromFile( const char* pcLWS_Filename )
{
	CTextFileScanner scanner;

	if( !scanner.OpenFile( pcLWS_Filename ) )
		return false;

	string tag, strLine;

	for( ; !scanner.End(); scanner.NextLine() )
	{
		scanner.GetTagString( tag );
		scanner.GetCurrentLine( strLine );

/*		if( 0 < m_vecObjectLayer.size() &&
			m_vecObjectLayer.back().LoadFromFile( scanner ) )
			continue;

		if( 0 < m_vecLight.size() &&
			m_vecLight.back().LoadFromFile( scanner ) )
			continue;*/

		if( 0 < m_vecpItem.size() && m_vecpItem.back()->LoadFromFile( scanner ) )
		{
			continue;
		}

		if( tag == "LoadObjectLayer" )
		{
			m_vecObjectLayer.push_back( CLWS_ObjectLayer() );
			scanner.ScanLine( tag,
				              m_vecObjectLayer.back().m_iLayerNumber,
							  m_vecObjectLayer.back().m_strObjectFilename );

			m_vecpItem.push_back( &m_vecObjectLayer.back() );

			continue;
		}

		else if( tag == "AddNullObject" )
		{
			m_vecObjectLayer.push_back( CLWS_ObjectLayer() );
			scanner.ScanLine( tag, m_vecObjectLayer.back().m_strObjectFilename );
			m_vecObjectLayer.back().SetNullObject( true );

			m_vecpItem.push_back( &m_vecObjectLayer.back() );

			continue;
		}

		else if( tag == "AddBone" )
		{
			m_vecpBone.push_back( shared_ptr<CLWS_Bone>( new CLWS_Bone() ) );

			m_vecpItem.push_back( m_vecpBone.back().get() );

			continue;
		}

		else if( tag == "AddLight" )
		{
			m_vecLight.push_back( CLWS_Light() );

			m_vecpItem.push_back( &m_vecLight.back() );

			continue;
		}

/*		else if( tag == "AddCamera" )
		{
			m_vecCamera.push_back( CLWS_Camera() );
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

/*	FILE* fp = fopen( pcLWS_Filename, "r" );

	if(!fp)
		return false;

	char acLine[MAX_LINE_LENGTH], acSlag[MAX_LINE_LENGTH];
	while( fgets(acLine, MAX_LINE_LENGTH-1, fp) )
	{
		if( strncmp(acLine, "LoadObjectLayer", 15) == 0 )
		{
			CLWS_ObjectLayer new_objectlayer;
			new_objectlayer.LoadFromFile(acLine, fp);
			m_vecObjectLayer.push_back( new_objectlayer );
		}

		else if( strncmp(acLine, "AddLight", 8) == 0 )
		{
			CLWS_Light new_light;
			new_light.LoadFromFile(acLine, fp);
			m_vecLight.push_back( new_light );
		}

		TryLoadingFogDataBlock( acLine, fp );

	}
	
	fclose(fp);*/

	/// create links from parent items to child items
	AddParentToChildLinks();

	return true;
}


void CLightWaveSceneLoader::AddParentToChildLinks()
{
	int i, num_object_layers = (int)m_vecObjectLayer.size();

	for( i=0; i<num_object_layers; i++ )
	{
		if( m_vecObjectLayer[i].HasParent() )
		{
			int parent_index = m_vecObjectLayer[i].GetParentIndex();

			switch( m_vecObjectLayer[i].GetParentType() )
			{
			case CLWS_Item::TYPE_OBJECT:
				m_vecObjectLayer[parent_index].AddChildItemInfo( CLWS_Item::TYPE_OBJECT, i );
				break;
			case CLWS_Item::TYPE_LIGHT:
				m_vecLight[parent_index].AddChildItemInfo( CLWS_Item::TYPE_OBJECT, i );
				break;
/*			case CLWS_Item::TYPE_CAMERA:
				break;
			case CLWS_Item::TYPE_BONE:
				break;*/
			}
		}
	}

	int num_lights = (int)m_vecLight.size();
	for( i=0; i<num_lights; i++ )
	{
		if( m_vecLight[i].HasParent() )
		{
			int parent_index = m_vecLight[i].GetParentIndex();

			switch( m_vecLight[i].GetParentType() )
			{
			case CLWS_Item::TYPE_OBJECT:
				m_vecObjectLayer[parent_index].AddChildItemInfo( CLWS_Item::TYPE_LIGHT, i );
				break;
			case CLWS_Item::TYPE_LIGHT:
				m_vecLight[parent_index].AddChildItemInfo( CLWS_Item::TYPE_LIGHT, i );
				break;
/*			case CLWS_Item::TYPE_CAMERA:
				break;
			case CLWS_Item::TYPE_BONE:
				break;*/
			}
		}
	}
}


bool CLightWaveSceneLoader::LoadFogDataBlock( CTextFileScanner& scanner )
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


CLWS_ObjectLayer* CLightWaveSceneLoader::GetObjectLayer(int i)
{
	if( i<0 || (int)m_vecObjectLayer.size()<=i )
		return NULL;

	return &m_vecObjectLayer[i];
}


CLWS_Light* CLightWaveSceneLoader::GetLight(int i)
{
	if( i<0 || (int)m_vecLight.size()<=i )
		return NULL;

	return &m_vecLight[i];
}


shared_ptr<CLWS_Bone> CLightWaveSceneLoader::GetBone(int i)
{
	if( i<0 || (int)m_vecpBone.size()<=i )
		return shared_ptr<CLWS_Bone>();

	return m_vecpBone[i];
}


CLWS_Fog* CLightWaveSceneLoader::GetFog()
{
	if( m_Fog.iType == 0 )
		return NULL;	//no fog in this scene

	return &m_Fog;
}
