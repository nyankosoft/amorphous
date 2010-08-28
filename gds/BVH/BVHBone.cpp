#include "BVHBone.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "Graphics/Shader/ShaderManager.hpp"
#include "Graphics/Shader/FixedFunctionPipelineManager.hpp"
#include "Graphics/FVF_ColorVertex.h"
#include "Graphics/UnitCube.hpp"
#include "Graphics/Direct3D/Direct3D9.hpp"

using namespace std;


SFloatRGBAColor CBVHBone::ms_dwSkeletonColor;
CMeshObjectHandle CBVHBone::ms_TestCube;
CUnitCube *CBVHBone::ms_pUnitCube = NULL;


CBVHBone::CBVHBone()
:
m_vOffset( Vector3(0,0,0) ),
m_iNumChannels(0),
m_matLocalPose( Matrix34Identity() ),
m_matWorldPose( Matrix34Identity() ),
m_qLocalRot( Matrix33Identity() )
{
}


CBVHBone::~CBVHBone()
{
}


void CBVHBone::Reset()
{
	m_strName = "";
	m_vOffset = Vector3(0,0,0);
	m_iNumChannels = 0;
	m_matLocalPose = Matrix34Identity();
	m_matWorldPose = Matrix34Identity();
	m_vecChild.clear();
}


void CBVHBone::LoadFromFile( char* pcTypeAndName, FILE* fp )
{
	char acNextTypeAndName[512];
	char acOffset[512], acStr[64], acName[256];
	char acChannel[512];
	char acLine[512];

	while( fgets(acLine, 511, fp) )
	{
		sscanf( acLine, "%s", acOffset );

		if( !strcmp(acOffset, "OFFSET") )
			break;	// found offset
	}

	sscanf( acLine, "%s %f %f %f\n", acStr, &m_vOffset.x, &m_vOffset.y, &m_vOffset.z );

//	// skip "{"
//	fgets( acLine, 511, fp );

	// 4/13/2008 commented out
	//  
//	m_vOffset *= 0.01f;		//unit conversion: cm -> m

	if( fabs(m_vOffset.x) < 0.0001 ) m_vOffset.x = 0;
	if( fabs(m_vOffset.y) < 0.0001 ) m_vOffset.y = 0;
	if( fabs(m_vOffset.z) < 0.0001 ) m_vOffset.z = 0;

	// flip X to change coordinate system from right-hand to left-hand
	m_vOffset.x *= (-1);

	if( strstr(pcTypeAndName, "End Site") )
	{
		// set name
		m_strName = "End Site";

		// End Site has no channels
		this->m_iNumChannels = 0;

		//skip "}"
		fgets( acLine, 511, fp );

		return;
	}
	else if( strstr(pcTypeAndName, "JOINT") || strstr(pcTypeAndName, "ROOT") )
	{
		// set name
		sscanf( pcTypeAndName, "%s%s", acStr, acName );
		m_strName = acName;

		// read and set channels
		fgets( acChannel, 511, fp );
		SetChannels( acChannel );

		while(1)
		{
			// read next line
			// this will be either the type and name of next child 
			// or "}", which indicates there is no more child of this bone
			fgets( acNextTypeAndName, 511, fp );

			if( strstr( acNextTypeAndName, "}" ) )
			{
				return;	// no more child bone
			}
			else
			{
				acStr[0] = '\0'; acName[0] = '\0';
				sscanf( acNextTypeAndName, "%s %s\n", acStr, acName );

				if( !strcmp(acStr,"JOINT") || strstr(acNextTypeAndName, "End Site") )	// acNextTypeAndName
				{
					// create a new child bone
					CBVHBone new_childbone;
					new_childbone.LoadFromFile( acNextTypeAndName, fp );
					m_vecChild.push_back( new_childbone );
				}
			}
		}
	}

}


void CBVHBone::SetChannels( char* pcChannel )
{
	char acStr[64];
	int iNumChannels;
	
	sscanf( pcChannel, "%s%d", acStr, &iNumChannels );

	if( iNumChannels == 3 )
	{
		// rotation
		this->m_iNumChannels = 3;
	}
	else if( iNumChannels == 6 )
	{
		// translation & rotation
		this->m_iNumChannels = 6;
	}
	else
	{
		return;
	}
}


void CBVHBone::GetLocalTransformMatrices_r( vector<Matrix34>* pvecLocalTransform, Vector3 vParentBoneGlobalOffset )
{
	Matrix34 local_transform( Matrix34Identity() );

	local_transform.vPosition = -vParentBoneGlobalOffset;

	pvecLocalTransform->push_back( local_transform );

	Vector3 vGlobalOffset = m_vOffset + vParentBoneGlobalOffset;

	for(size_t i=0; i<this->m_vecChild.size(); i++)
	{
		m_vecChild[i].GetLocalTransformMatrices_r( pvecLocalTransform, vGlobalOffset );
	}
}


void CBVHBone::GetLocalTransforms_r( Matrix34* paDestTransforms, int& rIndex ) const
{
//	if( m_vecChild.size() == 0 )
//		return;

	paDestTransforms[rIndex++] = m_matLocalPose;

	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].GetLocalTransforms_r( paDestTransforms, rIndex );
	}
}


void CBVHBone::Scale_r( float factor )
{
	m_vOffset *= factor;

	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].Scale_r( factor );
	}
}


void CBVHBone::SetMatrixFromBVHData_r( Matrix34* pParentMatrix, float* pafFrameData, int& riCount)
{
	Matrix33 matRotX, matRotY, matRotZ;
	Vector3 vTranslation = Vector3(0,0,0);
	float fRotAngleX, fRotAngleY, fRotAngleZ;

	if( m_vecChild.size() == 0 )
	{
		m_matLocalPose.Identity();
		m_matLocalPose.vPosition = m_vOffset;

		m_matWorldPose = (*pParentMatrix) * m_matLocalPose;
		return;
	}

	if(m_iNumChannels == 6)
	{	// root node needs translation
		vTranslation.x = pafFrameData[riCount++];
		vTranslation.y = pafFrameData[riCount++];
		vTranslation.z = pafFrameData[riCount++];
	}

	fRotAngleZ = pafFrameData[riCount++];
	fRotAngleX = pafFrameData[riCount++];
	fRotAngleY = pafFrameData[riCount++];

	// rotation around Y-axis (angle in radians)
	matRotY.SetRotationY( fRotAngleY );

	// rotation around X-axis (angle in radians)
	matRotX.SetRotationX( fRotAngleX );

	// rotation around Z-axis (angle in radians)
	matRotZ.SetRotationZ( fRotAngleZ );

	m_matLocalPose.matOrient = matRotZ * matRotX * matRotY;

	// quaternion for rotation
	m_qLocalRot.FromRotationMatrix( m_matLocalPose.matOrient );

	// translation
	m_matLocalPose.vPosition = m_vOffset + vTranslation;

	if( pParentMatrix )
	{
//		m_matWorldPose = m_matLocalPose * (*pParentMatrix);
		m_matWorldPose = (*pParentMatrix) * m_matLocalPose;
	}
	else
	{	// root node
		m_matWorldPose = m_matLocalPose;
	}

	size_t i, iNumChildren = m_vecChild.size();
	for( i=0; i<iNumChildren; i++ )
	{
		m_vecChild[i].SetMatrixFromBVHData_r( &m_matWorldPose, pafFrameData, riCount );
	}

}


void CBVHBone::Draw_r( Vector3* pvPrevPosition, Matrix34* pParentMatrix )
{
	COLORVERTEX avBoneVertex[2];
	Vector3 vWorldPosition;

	//' m_matLocalPose' is the local transformation matrix for this bone
	// pParentMatrix * m_matLocalPose = matWorldTransform = world transformation matrix

	m_matWorldPose.Transform( vWorldPosition, Vector3(0,0,0) );

//	D3DXVec3TransformCoord( &vLocalTransformedPosition, &m_vOffset, &m_matLocalPose );
//	if( pParentMatrix )
//		D3DXVec3TransformCoord( &vWorldPosition, &vLocalTransformedPosition, pParentMatrix );
//	else
//		vWorldPosition = vLocalTransformedPosition;

	if( pvPrevPosition )
	{
		if( ms_pUnitCube )
		{
			Matrix44 matWorldTransform = ToMatrix44( m_matWorldPose );

			Matrix44 matParentTransform = pParentMatrix ? ToMatrix44( *pParentMatrix ) : Matrix44Identity();

			// draw bone as a box
			DrawBoxForBone( matParentTransform, matWorldTransform );
		}
		else
		{
			// draw bone as a simple line segment
			memcpy( &(avBoneVertex[0].vPosition), pvPrevPosition, sizeof(float) * 3 );
			memcpy( &(avBoneVertex[1].vPosition), &vWorldPosition, sizeof(float) * 3 );
//			avBoneVertex[0].vPosition = *pvPrevPosition;
//			avBoneVertex[1].vPosition = vWorldPosition;
			avBoneVertex[0].color = ms_dwSkeletonColor.GetARGB32();
			avBoneVertex[1].color = ms_dwSkeletonColor.GetARGB32();

			DIRECT3D9.GetDevice()->SetFVF( D3DFVF_COLORVERTEX );
			DIRECT3D9.GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
			DIRECT3D9.GetDevice()->DrawPrimitiveUP( D3DPT_LINELIST, 1, avBoneVertex, sizeof(COLORVERTEX) );
		}
	}

	// recurse down to children
	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].Draw_r( &vWorldPosition, &m_matWorldPose );
	}

}


void CBVHBone::DrawBoxForBone( Matrix44 &rmatParent, Matrix44 &rmatWorldTransform )
{

//	DIRECT3D9.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	Matrix44 matTrans( Matrix44Identity() );
	float fScale[3];
	int i;

	// set translation matrix to stretch the cube in the direction of the bone 
//	D3DXMatrixIdentity( &matTrans );
	
	for( i=0; i<3; i++ )
	{
		if( 0 < m_vOffset[i] )
			matTrans(i,3) = 0.2f;
//			((float *)(&matTrans._41))[i] = 0.2f;
		else if( m_vOffset[i] < 0 )
			matTrans(i,3) = -0.2f;
//			((float *)(&matTrans._41))[i] = -0.2f;
	}

	// scale the cube to the size of each body part
	for( i=0; i<3; i++ )
	{
		if( m_vOffset[i] == 0 )
			fScale[i] = 0.04f;	// bone direction is not aligned to this axis
		else
			fScale[i] = ((float)fabs(m_vOffset[i]) * 25.0f) * 0.04f;
	}

	Matrix44 matScale = Matrix44Scaling( fScale[0], fScale[1], fScale[2] );


	// combine the matrices to create the world matrix ( = matTrans * matScale * rmatParent )
	matTrans = rmatParent * matScale * matTrans;
/*	D3DXMatrixMultiply( &matTrans, &matTrans, &matScale );

//	D3DXMatrixMultiply( &matScale, &matScale, &rmatWorldTransform );
	D3DXMatrixMultiply( &matTrans, &matTrans, &rmatParent );
*/
	FixedFunctionPipelineManager().SetWorldTransform( matTrans );
//	ms_pTestCube->Draw();
	ms_pUnitCube->Draw();

	// reset the world transformation matrix
	FixedFunctionPipelineManager().SetWorldTransform( Matrix44Identity() );

}


void CBVHBone::GetGlobalPositions_r( vector<Vector3>& rvecDestGlobalPositions,
									 Matrix34* pParentMatrix )
{
	Vector3 vWorldPosition;
	Matrix34 matWorldTransform;

	// 'm_matLocal' is the local transformation matrix for this bone
	// m_matLocalPose * pParentMatrix = matWorldTransform = world transformation matrix

	if( pParentMatrix )
		matWorldTransform = (*pParentMatrix) * m_matLocalPose;
	else
		matWorldTransform = m_matLocalPose;

	matWorldTransform.Transform( vWorldPosition, Vector3(0,0,0) );

/*	if(pParentMatrix)
		D3DXMatrixMultiply( &matWorldTransform, &m_matLocal, pParentMatrix );
	else
		matWorldTransform = m_matLocal;

	D3DXVec3TransformCoord( &vWorldPosition, &Vector3(0,0,0), &matWorldTransform );
*/
	//store the global position of this bone
	rvecDestGlobalPositions.push_back( vWorldPosition );
	
	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].GetGlobalPositions_r( rvecDestGlobalPositions, &matWorldTransform );
	}
}


void CBVHBone::GetChannelType_r( vector<int>* pvecChannelType )
{
	if( this->m_iNumChannels == 0 )
		return;

	if( this->m_iNumChannels == 6 )
	{	//this bone has both translation & rotation channels
		pvecChannelType->push_back( CT_POSITION_X );
		pvecChannelType->push_back( CT_POSITION_Y );
		pvecChannelType->push_back( CT_POSITION_Z );

		pvecChannelType->push_back( CT_ROTATION_Z );
		pvecChannelType->push_back( CT_ROTATION_X );
		pvecChannelType->push_back( CT_ROTATION_Y );
	}
	else if( this->m_iNumChannels == 3 )
	{	//this bone has both translation & rotation channels
		pvecChannelType->push_back( CT_ROTATION_Z );
		pvecChannelType->push_back( CT_ROTATION_X );
		pvecChannelType->push_back( CT_ROTATION_Y );
	}

	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].GetChannelType_r( pvecChannelType );
	}
	
}


int CBVHBone::GetNumBones_r()
{
	int i = 0;

	for(size_t j=0; j<m_vecChild.size(); j++)
		i += m_vecChild[j].GetNumBones_r();
	
	return i+1;		// 'i'(descendants) and '1'(myself)
}


void CBVHBone::SetPointersToLocalTransformMatrix_r( vector<Matrix34 *> *pvecpLocalTransform )
{
	pvecpLocalTransform->push_back( &m_matLocalPose );

	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].SetPointersToLocalTransformMatrix_r( pvecpLocalTransform );
	}
}


void CBVHBone::SetPointersToGlobalTransformMatrix_r( vector<Matrix34 *> *pvecpGlobalTransform )
{
	pvecpGlobalTransform->push_back( &m_matWorldPose );

	for(size_t i=0; i<m_vecChild.size(); i++)
	{
		m_vecChild[i].SetPointersToGlobalTransformMatrix_r( pvecpGlobalTransform );
	}
}
