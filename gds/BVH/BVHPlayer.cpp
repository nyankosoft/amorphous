#include "BVHPlayer.h"

#include "Graphics/FVF_ColorVertex.h"
#include "Support/memory_helpers.h"

using namespace std;


#define BVH_DEFAULT_FRAMETIMME 0.03333333333f


CBVHPlayer::CBVHPlayer()
:
m_fFrameTime(BVH_DEFAULT_FRAMETIMME)
{
}


CBVHPlayer::~CBVHPlayer()
{
}


void CBVHPlayer::Reset()
{
	m_strBVHFilename = "";
	m_fFrameTime = BVH_DEFAULT_FRAMETIMME;
	//m_RootBone.Destroy();
	m_vecFrame.clear();
	m_vecChannelType.clear();
}


#define MAX_LINE_LENGTH 4096

bool CBVHPlayer::LoadBVHFile( const string& filename )
{
	//  check the file extention
	if( strncmp(filename.c_str() + strlen(filename.c_str()) - 4, ".bvh", 4) != 0 )
		return false;	// extention is not ".bvh"

	FILE* fp = fopen( filename.c_str(), "r");
	if(!fp)
		return false;	// file not found

	// confirmed a valid filepath

	Reset();

	m_strBVHFilename = filename;

	LoadSkeleton( fp );

	LoadFrameData( fp );

	fclose(fp);

	// set up unit cube for bone

	return true;
}


void CBVHPlayer::LoadSkeleton( FILE *fp )
{
	char acLine[MAX_LINE_LENGTH];
	fgets(acLine, MAX_LINE_LENGTH - 1, fp);		// "HIERARCHY"
	fgets(acLine, MAX_LINE_LENGTH - 1, fp);		// "ROOT Hips"


	// ----------------------------- load skeleton -----------------------------

	// load the skelton structure
	this->m_RootBone.LoadFromFile( acLine, fp );

	this->m_RootBone.GetChannelType_r( &m_vecChannelType );
	int iNumTotalChannels = m_vecChannelType.size();
}


void CBVHPlayer::LoadFrameData( FILE *fp )
{
	char acLine[MAX_LINE_LENGTH];
	char acSlag[256], acSlag2[256];

	while(1)
	{
		fgets(acLine, MAX_LINE_LENGTH - 1, fp);
		if( strncmp(acLine, "MOTION", 6) == 0 )
			break;
	}

	// get the total number of frames in this .bvh file
	int iNumTotalFrames;
	fgets(acLine, MAX_LINE_LENGTH - 1, fp);
	sscanf( acLine, "%s%d", acSlag, &iNumTotalFrames );

	// get "time per frame" in second.
	fgets(acLine, MAX_LINE_LENGTH - 1, fp);
	sscanf( acLine, "%s%s%f", acSlag, acSlag2, &this->m_fFrameTime );

	int frame_count = 0, channel = 0;
	float fChannelValue;

	SBVHFrameData new_frame;
	new_frame.m_vecfChannelValue.reserve(256);

	while(1)
	{
		if( !fgets(acLine, MAX_LINE_LENGTH - 1, fp) )
			break;	// reached the end of the file

		// clear the previous data
		new_frame.m_vecfChannelValue.resize(0);

		channel = 0;
		for( size_t j=0; j<strlen(acLine)-2; j++ )
		{
			// locate a next floating point value by finding
			// a character which is neither a space nor a tab
			if( j==0 || acLine[j] == ' ' || acLine[j] == '\t' )
			{

//			if( acLine[j] == ' ' || acLine[j] == '\t' )
//				continue;

				// skip redundant spaces and tabs between floating point values
				while( acLine[j] == ' ' || acLine[j] == '\t' )
				{
					j++;
				}

				// new float value
			
				sscanf( acLine + j, "%f", &fChannelValue );

				if( m_vecChannelType[channel] == CT_POSITION_X
					|| m_vecChannelType[channel] == CT_POSITION_Y 
					|| m_vecChannelType[channel] == CT_POSITION_Z )
				{
					// convert unit from  [cm] to [m]
					// 4/13/2008 commented out
					// - Should do be done separately for each bvh file
					// - Although all the bvh files that I've seen use 'inch' for length,
					//   There is no guarantee that this is always true and there may be
					//   some others that use a different metric, so I decided not do any conversion here
///					fChannelValue *= 0.01f;
				}

				if( m_vecChannelType[channel] == CT_ROTATION_X
					|| m_vecChannelType[channel] == CT_ROTATION_Y 
					|| m_vecChannelType[channel] == CT_ROTATION_Z )
				{
					//convert unit from [deg] to [rad]
					fChannelValue *= (2.0f * 3.141592f / 360.0f);
				}

				// flip the values used for translation along x-axis and for rotation
				// around y & z axis to convert coordinate system from right hand
				// to left hand
				if( m_vecChannelType[channel] == CT_POSITION_X
					|| m_vecChannelType[channel] == CT_ROTATION_Y
					|| m_vecChannelType[channel] == CT_ROTATION_Z )
					fChannelValue *= (-1);

				if( fabs(fChannelValue) < 0.0001 )
					fChannelValue = 0;

				new_frame.m_vecfChannelValue.push_back( fChannelValue );

				channel++;
			}

		}

		m_vecFrame.push_back( new_frame );

		frame_count++;
	}

	if( 0 < m_vecFrame.size() )
		m_vecfInterpolatedFrame.reserve( m_vecFrame[0].m_vecfChannelValue.size() );
}


bool CBVHPlayer::SetWorldTransformation( float fTime )
{
	if( m_vecFrame.size() == 0 )
		return false;

	float fFraction, fDelta, fOverflow, fUnderflow, fVal;
	int iPrevFrame;

	if( fTime < 0 || m_fFrameTime * (m_vecFrame.size()-1) <= fTime )
		return false;	// invalid time

	m_vecfInterpolatedFrame.resize(0);

	iPrevFrame = (int)(fTime / m_fFrameTime);

	fFraction = fTime / m_fFrameTime - (float)iPrevFrame;

	// calculate linear interpolation
	// and store the interpolated frame data in 'afInterpolatedFrame[]'
	const int iNumTotalChannels = (int)m_vecChannelType.size();
	for( int iChannel=0; iChannel<iNumTotalChannels; iChannel++ )
	{
		fDelta = m_vecFrame[iPrevFrame+1].GetValue(iChannel)
		 	   - m_vecFrame[iPrevFrame].GetValue(iChannel);

		if( m_vecChannelType[iChannel] == CT_ROTATION_X ||
			m_vecChannelType[iChannel] == CT_ROTATION_Y ||
			m_vecChannelType[iChannel] == CT_ROTATION_Z )
		{
			// need to check verflow and underflow
			fOverflow = fDelta + 2.0f * 3.14159f;
			fUnderflow = fDelta - 2.0f * 3.14159f;
			fDelta = fabs(fDelta) < fabs(fOverflow) ? fDelta : fOverflow;
			fDelta = fabs(fDelta) < fabs(fUnderflow) ? fDelta : fUnderflow;
		}

		fVal = m_vecFrame[iPrevFrame].GetValue(iChannel) + fDelta * fFraction;

		m_vecfInterpolatedFrame.push_back( fVal );
	}

	int count = 0;
	m_RootBone.SetMatrixFromBVHData_r(NULL, &(m_vecfInterpolatedFrame[0]), count);

	return true;
}


bool CBVHPlayer::Render()
{

	LPDIRECT3DDEVICE9 pd3dDev = DIRECT3D9.GetDevice();

//	pd3dDev->SetVertexShader( NULL );
//	pd3dDev->SetFVF( D3DFVF_COLORVERTEX );
//	pd3dDev->SetRenderState( D3DRS_LIGHTING, FALSE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	pd3dDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	pd3dDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDev->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );


	CBVHBone::ms_pUnitCube = NULL;

	m_RootBone.SetSkeletonColor( 0xFFFFFFFF );
	m_RootBone.Draw_r();

	return true;
}


bool CBVHPlayer::Render(float fTime)
{
	SetWorldTransformation( fTime );

	return Render();
}


void CBVHPlayer::GetGlobalPositionsAtFrame( int iFrame, vector<Vector3>& rvecDestGlobalPositions )
{
	if( iFrame < 0 || (int)m_vecFrame.size() <= iFrame )
		return;

	int count = 0;
	m_RootBone.SetMatrixFromBVHData_r( NULL, &(m_vecFrame[iFrame].m_vecfChannelValue[0]), count );

	m_RootBone.GetGlobalPositions_r( rvecDestGlobalPositions );
}


// returns the matrix which transforms a vertex into the local coodinate
// whose origin is the center of the body
D3DXMATRIX CBVHPlayer::GetBodyCenterTransformationMatrixAt( int iFrame )
{
	SBVHFrameData& rFrame = this->m_vecFrame[iFrame];

	D3DXMATRIX matTrans, matRot, matLocal;

	float fRotAngleZ = rFrame.GetValue(3);
	float fRotAngleX = rFrame.GetValue(4);
	float fRotAngleY = rFrame.GetValue(5);

	// rotation around Y-axis
	D3DXMatrixRotationY( &matRot, fRotAngleY );
	matTrans = matRot;

	// rotation around X-axis
	D3DXMatrixRotationX( &matRot, fRotAngleX );
	D3DXMatrixMultiply( &matTrans, &matTrans, &matRot);

	// rotation around Z-axis
	D3DXMatrixRotationZ( &matRot, fRotAngleZ );
	D3DXMatrixMultiply( &matTrans, &matTrans, &matRot);

	// translation
	matTrans._41 = rFrame.GetValue(0);
	matTrans._42 = rFrame.GetValue(1);
	matTrans._43 = rFrame.GetValue(2);

	//make the inverse matrix
	D3DXMatrixInverse( &matLocal, NULL, &matTrans );

	return matLocal;
}


Vector3 CBVHPlayer::GetBodyCenterPosition( int iFrame )
{
	SBVHFrameData& rFrame = this->m_vecFrame[iFrame];
	return Vector3( rFrame.GetValue(0), rFrame.GetValue(1), rFrame.GetValue(2) );
}


void CBVHPlayer::CopyFramesTo( int iStartFrame, int iEndFrame, CBVHPlayer* pDestBVHPlayer )
{
	if( iStartFrame < 0 ) iStartFrame = 0;
	if( iEndFrame < 0 ) iEndFrame = 0;
	if( GetNumTotalFrames() <= iStartFrame ) iStartFrame = GetNumTotalFrames() - 1;
	if( GetNumTotalFrames() <= iEndFrame ) iEndFrame = GetNumTotalFrames() - 1;

	int iFrame;
	for(iFrame=iStartFrame; iFrame<=iEndFrame; iFrame++)
	{
		pDestBVHPlayer->m_vecFrame.push_back( this->m_vecFrame[iFrame] );
	}
}


// translate the player's position to make a motion between 'iStartFrame' and 'iEndFrame' start from
// the origin of the world coordinate
void CBVHPlayer::ClearStartPositionOffset( int iStartFrame, int iEndFrame )
{
	if( iStartFrame < 0 || (int)m_vecFrame.size() <= iStartFrame ||
		  iEndFrame < 0 || (int)m_vecFrame.size() <= iEndFrame )
		  return;

	D3DXVECTOR3 vOffset;
	float fRotAngleX, fRotAngleY, fRotAngleZ;

	// get the position and the rotation of start frame
	SBVHFrameData& rStartFrame = this->m_vecFrame[iStartFrame];
	vOffset.x = rStartFrame.GetValue(0);
	vOffset.y = 0; //rStartFrame.GetValue(1);
	vOffset.z = rStartFrame.GetValue(2);
	fRotAngleZ = 0; //rStartFrame.GetValue(3);
	fRotAngleX = 0; //rStartFrame.GetValue(4);
	fRotAngleY = 0; //rStartFrame.GetValue(5);

	int iFrame;
	for(iFrame = iStartFrame; iFrame <= iEndFrame; iFrame++)
	{
		SBVHFrameData& rFrame = this->m_vecFrame[iFrame];
		rFrame.m_vecfChannelValue[0] -= vOffset.x;
		rFrame.m_vecfChannelValue[1] -= vOffset.y;
		rFrame.m_vecfChannelValue[2] -= vOffset.z;
		rFrame.m_vecfChannelValue[3] -= fRotAngleZ;
		rFrame.m_vecfChannelValue[4] -= fRotAngleX;
		rFrame.m_vecfChannelValue[5] -= fRotAngleY;
	}
}


void CBVHPlayer::CopySkeletonTo(CBVHPlayer* pDestBVHPlayer)
{
	pDestBVHPlayer->m_RootBone = this->m_RootBone;
}


void CBVHPlayer::DeleteFrames( int iStartFrame, int iEndFrame )
{
	if( iStartFrame < 0 ) iStartFrame = 0;
	if( iEndFrame < 0 ) iEndFrame = 0;
	if( GetNumTotalFrames() <= iStartFrame ) iStartFrame = GetNumTotalFrames() - 1;
	if( GetNumTotalFrames() <= iEndFrame ) iEndFrame = GetNumTotalFrames() - 1;

	m_vecFrame.erase(
		m_vecFrame.begin() + iStartFrame,
		m_vecFrame.begin() + iEndFrame );
}


// move the player's body to the specified place at the specified frame
void CBVHPlayer::MoveOffset( int iFrame, Vector3 vOffset,
		float fRotAngleZ, float fRotAngleX, float fRotAngleY )
{
	if(iFrame < 0 || GetNumTotalFrames() <= iFrame )
		return;

	SBVHFrameData& rFrame = m_vecFrame[iFrame];

	rFrame.m_vecfChannelValue[0] += vOffset.x;
	rFrame.m_vecfChannelValue[1] += vOffset.y;
	rFrame.m_vecfChannelValue[2] += vOffset.z;

	rFrame.m_vecfChannelValue[3] += fRotAngleZ;
	rFrame.m_vecfChannelValue[4] += fRotAngleX;
	rFrame.m_vecfChannelValue[5] += fRotAngleY;
}



void CBVHPlayer::GetLocalTransforms( Matrix34* paDestTransform ) const
{
	int index = 0;

	m_RootBone.GetLocalTransforms_r( paDestTransform, index );
}


void CBVHPlayer::Scale( float factor )
{
	// scale frame data
	vector<int> pos_channels_index;
	const size_t num_channels = m_vecChannelType.size();
	for( size_t i=0; i<num_channels; i++ )
	{
		if( m_vecChannelType[i] == CT_POSITION_X
		 || m_vecChannelType[i] == CT_POSITION_Y
		 || m_vecChannelType[i] == CT_POSITION_Z
		 )
		{
			pos_channels_index.push_back( i );
		}
	}

	const size_t num_frames = m_vecFrame.size();
	const size_t num_pos_channels = pos_channels_index.size();

	for( size_t i=0; i<num_frames; i++ )
	{
		for( size_t j=0; j<num_pos_channels; j++ )
		{
			m_vecFrame[i].m_vecfChannelValue[ pos_channels_index[j] ] *= factor;
		}
	}

	// scale bone lengths
	m_RootBone.Scale_r( factor );
}



/*
void CBVHPlayer::SetBoneOffsetMatricesForXMesh( char* pcFilename )
{
	FILE* fp = fopen( pcFilename, "r" ); // only the text .x file is supported

	if(!fp)
		return;

	char acFrameNamesInPreorder[64][128];
	int i, j, iNumFrames = 0;
	vector<SXMeshBoneInfo> vecXMeshBoneInfo;
	bool bEnteredSkeletonArea = false;

	char acLine[MAX_LINE_LENGTH], acName1[256], acName2[256];
	while( fgets(acLine, MAX_LINE_LENGTH - 1, fp) )
	{
		sscanf( acLine, "%s %s", acName1, acName2 );

		if( !strncmp(acName1, "SkinWeights", 11) )
		{
			SXMeshBoneInfo bone_info;
			char acTemp[256];

			// get the next line assuming it contains the name of the corresponding frame
			fgets(acLine, MAX_LINE_LENGTH - 1, fp);

			sscanf( acLine, "%s", acTemp );
			strcpy( bone_info.acBoneName, acTemp + 1);
			bone_info.acBoneName[ strlen(bone_info.acBoneName) - 2 ] = '\0';

			while( fgets(acLine, MAX_LINE_LENGTH - 1, fp) )
			{
				D3DXMATRIX& mat = bone_info.matBoneOffsetMatrix;
				if( strstr(acLine, ";;") )
				{	// Here is the bone offset matrix.
					sscanf( acLine, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
						&mat._11, &mat._12, &mat._13, &mat._14,
						&mat._21, &mat._22, &mat._23, &mat._24,
						&mat._31, &mat._32, &mat._33, &mat._34,
						&mat._41, &mat._42, &mat._43, &mat._44 );
					vecXMeshBoneInfo.push_back( bone_info );
					break;
				}
			}
		}

		else if( !strcmp(acName1, "Frame") )
		{
			if( strstr(acName2, "RootBone") )
				bEnteredSkeletonArea = true;
			else if( bEnteredSkeletonArea && iNumFrames < 64 )
			{
				if( acName2[strlen(acName2)-1] == '\n' )
					acName2[strlen(acName2)-1] =  '\0';
				strcpy( acFrameNamesInPreorder[iNumFrames++], acName2 );
			}
		}

		// clear name holder strings
		acName1[0] = '\0'; acName2[0] = '\0';
	}
	fclose(fp);

	// set the bone offset matrices in preorder
	vector<SXMeshBoneInfo> vecXMeshBoneInPreorder;
	
	// first, set the root bone
	SXMeshBoneInfo root_bone;
	strcpy( root_bone.acBoneName, "RootBone" );
	D3DXMatrixIdentity( &root_bone.matBoneOffsetMatrix );
	vecXMeshBoneInPreorder.push_back( root_bone );

	// next, set all the other bones in preorder
	for(i=0; i<iNumFrames; i++)
	{
		for(j=0; j<vecXMeshBoneInfo.size(); j++)
		{
			if( !strcmp(acFrameNamesInPreorder[i], vecXMeshBoneInfo[j].acBoneName) )
			{
				vecXMeshBoneInPreorder.push_back( vecXMeshBoneInfo[j] );
				break;
			}
		}
	}
	
	int count = 0;
	m_RootBone.SetBoneOffsetMetricesForXMesh(
		&vecXMeshBoneInPreorder, count, D3DXVECTOR3(0,0,0) );
}*/
