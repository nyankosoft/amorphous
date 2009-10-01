#include "LWS_Items.hpp"
#include "Support/Macro.h"
#include "Support/TextFileScanner.hpp"


#define MAX_LINE_LENGTH	1024


using namespace std;


void CLWS_Channel::Load( FILE* fp )
{
	int i, iNumKeys = 0;
	CLWS_Keyframe key;
	char acSlag[MAX_LINE_LENGTH];
	fgets(acSlag, MAX_LINE_LENGTH-1, fp);	// skip "{ Envelope"
	fscanf( fp, "%d", &iNumKeys );			// get how many keys in this envelope
	for(i=0; i<iNumKeys; i++)
	{
		fscanf( fp, "%s %f %f %d %f %f %f %f %f %f", acSlag, &key.fValue, &key.fTime, &key.iSpantype,
			&key.fCurveParam[0], &key.fCurveParam[1], &key.fCurveParam[2],
			&key.fCurveParam[3], &key.fCurveParam[4], &key.fCurveParam[5] );
		vecKey.push_back( key );
	}
	fscanf( fp, "%s %d %d", acSlag, &iPreBehavior, &iPostBehavior );
	fgets(acSlag, MAX_LINE_LENGTH-1, fp);	// skip "}"

}


//=====================================================================================
// CLWS_Item
//=====================================================================================

CLWS_Item::CLWS_Item()
{
	m_iNumChannels	= 0;
	memset( m_aChannel, 0, sizeof(m_aChannel) );

	m_iParentType	= -1;
	m_iParentIndex	= -1;

	for( int i=0; i<numof(m_afPivotRotationAngle); i++ )
		m_afPivotRotationAngle[i] = 0.0f;
}


bool CLWS_Item::LoadChannels( CTextFileScanner& scanner )
{
	int num_keys = 0, num_channels = m_iNumChannels;
	CLWS_Keyframe key;
	string tag, strLine;
	char acSlag[256];

	int num_channels_loaded = 0;
	int channel_index = 0;

	for( ; num_channels_loaded < num_channels; scanner.NextLine() )
	{
		scanner.GetTagString( tag );
		scanner.GetCurrentLine( strLine );

		if( tag == "Channel" )
		{
			scanner.ScanLine( tag, channel_index );
			num_channels_loaded++;
		}
		else if( tag == "Behaviors" )
		{
			scanner.ScanLine( tag, m_aChannel[channel_index].iPreBehavior,
				                   m_aChannel[channel_index].iPostBehavior );
		}
		else if( tag == "{" )
		{
			// TODO: a proper routine to load the number of keyframes
			scanner.NextLine();
			scanner.GetCurrentLine( strLine );	// the keyframe conut should be recorded in this line
			sscanf( strLine.c_str(), "%d", &num_keys );
			m_aChannel[channel_index].vecKey.reserve( num_keys );
		}
		else if( tag == "Key" )
		{
			sscanf( strLine.c_str(), "%s %f %f %d %f %f %f %f %f %f",
				    acSlag,
					&key.fValue,
					&key.fTime,
					&key.iSpantype,
					&key.fCurveParam[0], &key.fCurveParam[1], &key.fCurveParam[2],
					&key.fCurveParam[3], &key.fCurveParam[4], &key.fCurveParam[5] );

			m_aChannel[channel_index].vecKey.push_back( key );
		}
	}

/*	int i, j;
	for( i=0; i<num_channels; i++ )
	{
		CLWS_Channel& rChannel = m_aChannel[i];

		int channel;
		scanner.NextLine();
		scanner.ScanLine( tag, channel );

		scanner.NextLine();
		scanner.NextLine();	// skip "{ Envelope"

		/// get how many keyrames in this envelope
		/// the keyframe count does not have a tag string,
		/// so we use sscanf() to retrive a single digit instead of scanner.ScanLine()
		scanner.GetCurrentLine( strLine );
		sscanf( strLine.c_str(), "%d", &num_keys );
		rChannel.vecKey.resize( num_keys );

		// load keyframes
		for( j=0; j<num_keys; j++ )
		{
			scanner.NextLine();
			scanner.GetCurrentLine( strLine );
			sscanf( strLine.c_str(), "%s %f %f %d %f %f %f %f %f %f",
				    acSlag,
					&key.fValue,
					&key.fTime,
					&key.iSpantype,
					&key.fCurveParam[0], &key.fCurveParam[1], &key.fCurveParam[2],
					&key.fCurveParam[3], &key.fCurveParam[4], &key.fCurveParam[5] );

			rChannel.vecKey[j] = key;
		}

		scanner.NextLine();
		scanner.ScanLine( tag, rChannel.iPreBehavior, rChannel.iPostBehavior );

		scanner.NextLine();	// skip "}"
	}*/

	return true;
}


bool CLWS_Item::LoadFromFile( CTextFileScanner& scanner )
{
	string strLine, tag, strParentItem;
	scanner.GetCurrentLine( strLine );

	scanner.GetTagString( tag );

	if( tag == "NumChannels" )
	{
		scanner.ScanLine( tag, m_iNumChannels );
		LoadChannels( scanner );
		return true;
	}
	else if( tag == "ParentItem" )
	{
		scanner.ScanLine( tag, strParentItem );

		char acItemType[8], acItemIndex[12];

		strncpy( acItemType, strParentItem.c_str(), 1 );
		sscanf( acItemType, "%x", &m_iParentType );

		strcpy( acItemIndex, strParentItem.c_str() + 1 );
		sscanf( acItemIndex, "%x", &m_iParentIndex );
		
//		m_iParentIndex /= 10000;

		// parent index: 0-origin index for the list of objects of the same type?

		return true;
	}
	else if( tag == "PivotRotation" )
	{
		scanner.ScanLine( tag, m_afPivotRotationAngle[0], m_afPivotRotationAngle[1], m_afPivotRotationAngle[2] );
		return true;
	}

	return false;
}


void CLWS_Item::LoadChannelBlocksFromFile( int iNumChannels, FILE* fp )
{
	m_iNumChannels = iNumChannels;
	int iChannelNumber, iCount = 0;
	char acLine[MAX_LINE_LENGTH];
	char acSlag[256];
	while( fgets(acLine, MAX_LINE_LENGTH-1, fp) && iCount < iNumChannels )
	{
		if( strncmp(acLine, "Channel", 7) == 0 )
		{
			sscanf(acLine, "%s %d", acSlag, &iChannelNumber );
			m_aChannel[iChannelNumber].Load(fp);
			iCount++;
		}
	}
}

void CLWS_Item::CheckChannelBlock( char* pcFirstLine, FILE* fp )
{
	if( strncmp( pcFirstLine, "NumChannels", 11) != 0 )
		return;

	// Here is the channel data
	int iCount = 0;
	int iChannelNumber;
	char acLine[MAX_LINE_LENGTH];
	char acSlag[256];

	sscanf( pcFirstLine, "%s %d", acSlag, &m_iNumChannels );

	while( fgets(acLine, MAX_LINE_LENGTH-1, fp) && iCount < m_iNumChannels );
	{
		if( strncmp(acLine, "Channel", 7) == 0 )
		{
			sscanf(acLine, "%s %d", acSlag, &iChannelNumber );
			m_aChannel[iChannelNumber].Load(fp);
			iCount++;
		}
	}
}

Vector3 CLWS_Item::GetPositionAt( float fTime )
{
	return GetPositionAtKeyframe( 0 );

/*	Vector3 vPosition;

	for(int i=0; i<3; i++)
	{
		vPosition[i] = m_aChannel[i].vecKey[0].fValue;
	}

	return vPosition;*/
}


Vector3 CLWS_Item::GetPositionAtKeyframe( int keyframe )
{
	Vector3 vPosition;

	if( keyframe < 0 || 
		(int)m_aChannel[0].vecKey.size() <= keyframe ||
		(int)m_aChannel[1].vecKey.size() <= keyframe ||
		(int)m_aChannel[2].vecKey.size() <= keyframe )
		return Vector3(0,0,0);

	vPosition.x = m_aChannel[0].vecKey[keyframe].fValue;
	vPosition.y = m_aChannel[1].vecKey[keyframe].fValue;
	vPosition.z = m_aChannel[2].vecKey[keyframe].fValue;

	return vPosition;
}


void CLWS_Item::GetOrientationAt( float fTime, Matrix33& matOrient )
{
	// TODO: support animation time
	// returns the position at frame 0 for the moment

	GetOrientationAtKeyframe( 0, matOrient );

/*	Matrix33 matRotX, matRotY, matRotZ;

	float fHeading	= m_aChannel[3].vecKey[0].fValue;
	float fPitch	= m_aChannel[4].vecKey[0].fValue;
	float fBank		= m_aChannel[5].vecKey[0].fValue;

	matRotY = Matrix33RotationY( fHeading );
	matRotX = Matrix33RotationX( fPitch );
	matRotZ = Matrix33RotationZ( fBank );

	// rotations are applied to a vector in a heading -> pitch -> bank order

	matOrient = matRotZ * matRotX * matRotY;*/
}


void CLWS_Item::GetOrientationAtKeyframe( int keyframe, Matrix33& matOrient )
{
	Matrix33 matRotX, matRotY, matRotZ;

	float fHeading	= m_aChannel[3].vecKey[keyframe].fValue;
	float fPitch	= m_aChannel[4].vecKey[keyframe].fValue;
	float fBank		= m_aChannel[5].vecKey[keyframe].fValue;

	matRotY = Matrix33RotationY( fHeading );
	matRotX = Matrix33RotationX( fPitch );
	matRotZ = Matrix33RotationZ( fBank );

	// rotations are applied to a vector in a heading -> pitch -> bank order

	matOrient = matRotZ * matRotX * matRotY;
}


void CLWS_Item::GetPoseAt( float fTime, Matrix34& rDestPose )
{
	GetOrientationAt( fTime, rDestPose.matOrient );
	rDestPose.vPosition = GetPositionAt( fTime );
}


void CLWS_Item::GetPoseAtKeyframe( int keyframe, Matrix34& rDestPose )
{
	GetOrientationAtKeyframe( keyframe, rDestPose.matOrient );
	rDestPose.vPosition = GetPositionAtKeyframe( keyframe );
}

/*
//Right now, this function returns the rotation marix at frame 0. The value of 'fTime' currently has no effect. 
D3DXMATRIX CLWS_Item::GetRotationMatrixAt( float fTime )
{
	D3DXMATRIX matRotation, matRotTemp;
	D3DXMatrixIdentity( &matRotation );

	float fHeading	= m_aChannel[3].vecKey[0].fValue;
	float fPitch	= m_aChannel[4].vecKey[0].fValue;
	float fBank		= m_aChannel[5].vecKey[0].fValue;

	D3DXMatrixRotationY( &matRotTemp, fHeading );
	D3DXMatrixMultiply( &matRotation, &matRotation, &matRotTemp );

	D3DXMatrixRotationX( &matRotTemp, fPitch );
	D3DXMatrixMultiply( &matRotation, &matRotation, &matRotTemp );

	D3DXMatrixRotationZ( &matRotTemp, fBank );
	D3DXMatrixMultiply( &matRotation, &matRotation, &matRotTemp );

	return matRotation;
}
*/

int CLWS_Item::GetNumKeyFrames()
{
	int i, num = m_iNumChannels;
	int num_max_keyframes = 0;
	for( i=0; i<num; i++ )
	{
		if( num_max_keyframes < (int)m_aChannel[i].vecKey.size() )
			num_max_keyframes = (int)m_aChannel[i].vecKey.size();
	}

	return num_max_keyframes;
}


Vector3 CLWS_Item::GetPositionAtKeyFrame( int iKeyFrame )
{
	if( (int)m_aChannel[0].vecKey.size() <= iKeyFrame ||
		(int)m_aChannel[1].vecKey.size() <= iKeyFrame ||
		(int)m_aChannel[2].vecKey.size() <= iKeyFrame )
		return Vector3(0,0,0);

	Vector3 vPosition;
	vPosition.x = m_aChannel[0].vecKey[iKeyFrame].fValue;
	vPosition.y = m_aChannel[1].vecKey[iKeyFrame].fValue;
	vPosition.z = m_aChannel[2].vecKey[iKeyFrame].fValue;

	return vPosition;
}


float CLWS_Item::GetTimeAtKeyFrame( int iKeyFrame )
{
	if( (int)m_aChannel[0].vecKey.size() <= iKeyFrame )
		return 0;

	return m_aChannel[0].vecKey[iKeyFrame].fTime;
}


CLWS_ObjectLayer::CLWS_ObjectLayer()
{
	m_iLayerNumber = 0;

	m_bNullObject = false;
}


bool CLWS_ObjectLayer::LoadFromFile( CTextFileScanner& scanner )
{
	if( CLWS_Item::LoadFromFile( scanner ) )
		return true;

	return false;
}



//=====================================================================================
// CLWS_Bone
//=====================================================================================

CLWS_Bone::CLWS_Bone()
:
m_fBoneRestLength(0),
m_vBoneRestPosition(Vector3(0,0,0)),
m_vBoneRestDirection(Vector3(0,0,0))
{
}


bool CLWS_Bone::LoadFromFile( CTextFileScanner& scanner )
{
	string tag;
	scanner.GetTagString( tag );

	if( CLWS_Item::LoadFromFile( scanner ) )
		return true;

	else if( tag == "BoneName" )
	{
		scanner.ScanLine( tag, m_strBoneName );
		return true;
	}

	else if( tag == "BoneRestPosition" )
	{
		scanner.ScanLine( tag, m_vBoneRestPosition );
		return true;
	}

	else if( tag == "BoneRestDirection" )
	{
		scanner.ScanLine( tag, m_vBoneRestDirection );
		return true;
	}

	else if( tag == "BoneRestLength" )
	{
		scanner.ScanLine( tag, m_fBoneRestLength );
		return true;
	}

	else if( tag == "BoneWeightMapName" )
	{
		scanner.ScanLine( tag, m_strBoneWeightMapName );
		return true;
	}

	return false;
}



//=====================================================================================
// CLWS_Light
//=====================================================================================

CLWS_Light::CLWS_Light()
:
m_fLightIntensity(0),
m_LightType(TYPE_INVALID)

{
	memset( m_afLightColor, 0, sizeof(m_afLightColor) );
}


bool CLWS_Light::LoadFromFile( CTextFileScanner& scanner )
{
	string tag, strLine;
	scanner.GetTagString( tag );

	if( CLWS_Item::LoadFromFile( scanner ) )
		return true;

	else if( tag == "LightName" )
	{
		scanner.ScanLine( tag, m_strLightName );
		return true;
	}

	else if( tag == "LightColor" )
	{
		scanner.ScanLine( tag, m_afLightColor[0], m_afLightColor[1], m_afLightColor[2] );
		return true;
	}

	else if( tag == "LightIntensity" )
	{
		scanner.ScanLine( tag, m_fLightIntensity );
		return true;
	}

	else if( tag == "LightType" )
	{
		scanner.ScanLine( tag, m_LightType );
		return true;
	}

	return false;
}



//=====================================================================================
// CLWS_Fog
//=====================================================================================

CLWS_Fog::CLWS_Fog()
{
	iType = 0;	// (iType == 0) indicates that there is no fog in the scene
	fMinDist = 0;
	fMaxDist = 0;
	fMinAmount = 0;
	fMaxAmount = 0;
	afColor[0] = 0;	// red
	afColor[1] = 0;	// green
	afColor[2] = 0;	// blue
}
