
#include "MeshBone.hpp"

//#include "Graphics/Direct3D9.hpp"
#include "Graphics/D3DXVec3Copy.hpp"

#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
using namespace MeshModel;
//using namespace GameLib1::MeshModel;

#include "Support/memory_helpers.hpp"


//=========================================================================================
// CMM_Bone
//=========================================================================================

const CMM_Bone CMM_Bone::ms_NullBone = CMM_Bone();


void CMM_Bone::LoadBone_r( CMMA_Bone& rSrcBone,
						   D3DXMATRIX *paBoneTransformMatrix,
						   int &iNumRegisteredMatrices )
{
	m_strName = rSrcBone.strName;

	m_iNumChildren = (int)rSrcBone.vecChild.size();

	m_paChild = new CMM_Bone [m_iNumChildren];

	// set pointer to the transform matrix array
	m_pWorldTransform = paBoneTransformMatrix + iNumRegisteredMatrices;

	m_MatrixIndex = iNumRegisteredMatrices;

	iNumRegisteredMatrices++;

	// get a matrix that transforms vertices from local model space to local bone space
	rSrcBone.BoneTransform.GetRowMajorMatrix44( (float *)&m_matBoneTransform );

	m_BoneTransform = rSrcBone.BoneTransform;

	m_vLocalOffset = rSrcBone.vLocalOffset;


	// bone space to local model space
	D3DXMatrixInverse( &m_matLocalTransform, NULL, &m_matBoneTransform );

	m_BoneTransform.GetInverseROT( m_LocalTransform );


	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].LoadBone_r( rSrcBone.vecChild[i], paBoneTransformMatrix, iNumRegisteredMatrices );
	}
}


void CMM_Bone::Transform_r( Matrix34* pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex )
{
	D3DXMATRIX matLocal;

/*	paMatrix[rIndex++].GetRowMajorMatrix44( (float *)&matLocal );

	D3DXMatrixMultiply( matLocal, &m_matBoneTransform, &matLocal );
	D3DXMatrixMultiply( m_pWorldTransform, &matLocal, &m_matLocalTransform );
*/

	Matrix34 local_rot = paSrcMatrix[rIndex++];
	if( pParentMatrix )
		local_rot.vPosition = Vector3(0,0,0);

	Matrix34 mat;

//	mat = m_LocalTransform * paSrcMatrix[rIndex++] * m_BoneTransform;
	mat = m_LocalTransform * local_rot * m_BoneTransform;

//----------------------------

	Matrix34 mat2 = m_LocalTransform * ( local_rot * m_BoneTransform );
	Matrix34 mat3 = m_BoneTransform * local_rot * m_LocalTransform;

	Vector3 v1, v2, v3, v4, v0 = Vector3( 2.1f, 1.0f, -11.0f );
	v1 = m_BoneTransform * v0;
	v2 = local_rot * v1;
	v3 = m_LocalTransform * v2;

	v4 = mat * v0;

//----------------------------

	if( pParentMatrix )
		mat = (*pParentMatrix) * mat;

	mat.GetRowMajorMatrix44( (float *)m_pWorldTransform );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].Transform_r( &mat, paSrcMatrix, rIndex );
	}
}


void CMM_Bone::Transform_Quaternion( float *pafData, int& rIndex )
{
/*	D3DXMATRIX matWorldTrans, matLocalTrans;
	D3DXMatrixIdentity( &matLocalTrans );

	matLocalTrans.vPosition = m_vLocalOffset;

	if( 0 < m_iNumChildren )
	{
		if( !pParentMatrix )
		{	// root node needs translation
			matLocalTrans._41 += paframeData[rIndex++];
			matLocalTrans._42 += paframeData[rIndex++];
			matLocalTrans._43 += paframeData[rIndex++];
		}

		D3DXQUATERNION qLocalRot;
		qLocalRot.x = paframeData[rIndex++];
		qLocalRot.y = paframeData[rIndex++];
		qLocalRot.z = paframeData[rIndex++];
		qLocalRot.w = paframeData[rIndex++];

		// convert quaternion to rotation matrix
		D3DXMATRIX matLocalRot;

//		qLocalRot.ToRotationMatrix( matLocalRot );

		matLocalTrans = matLocalRot * matLocalTrans;
	}
//	else
//		EndSite - no rotation

	if( pParentMatrix )
		matWorldTrans = (*pParentMatrix) * matLocalTrans;
	else
		matWorldTrans = matLocalTrans;	// root node

	int i;
	for( i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i]->Transform_Quaternion( &matWorldTrans, pafData, rIndex );
	}*/
}


int CMM_Bone::GetBoneMatrixIndexByName_r( const char *pName )
{
/*	const CMM_Bone& bone = GetBoneByName_r( pName );

	if( bone == NullBone() )
		return -1;
	else
		return bone.m_MatrixIndex;
*/
	if( m_strName == pName )
		return m_MatrixIndex;
	else
	{
		if( m_iNumChildren == 0 )
			return -1;
		else
		{
			int i, index;
			for( i=0; i<m_iNumChildren; i++ )
			{
				index = m_paChild[i].GetBoneMatrixIndexByName_r( pName );
				if( 0 <= index )
					return index;	// found the bone with the specified name
			}

			return -1;	// not found in this sub-tree
		}
	}
}


const CMM_Bone& CMM_Bone::GetBoneByName_r( const char *pName ) const
{
	if( m_strName == pName )
		return *this;
	else
	{
		if( m_iNumChildren == 0 )
			return NullBone();
		else
		{
			for( int i=0; i<m_iNumChildren; i++ )
			{
				const CMM_Bone& bone = m_paChild[i].GetBoneByName_r( pName );
				if( bone != NullBone() )
					return bone;	// found the bone with the specified name
			}

			return NullBone();	// not found in this sub-tree
		}
	}
}


void CMM_Bone::SetBoneToArray_r( vector<CMM_Bone *>& vecpDestArray )
{
	vecpDestArray.push_back( this );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].SetBoneToArray_r( vecpDestArray );
	}
}


