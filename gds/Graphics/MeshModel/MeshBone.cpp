#include "MeshBone.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "3DMeshModelArchive.hpp"
using namespace MeshModel;

using namespace std;



/**
How to replace D3DXMATRIX CMM_Bone::m_pWorldTransform
-----------------------------------------------------
- Replace with Matrix44
  - pros:
    - Direct3D and OpenGL implementations may be able to share CMM_Bone.
  - cons:
    - ID3DXEffect::SetMatrixTranspose() is slower than ID3DXEffect::SetMatrix()
Replace with Transform
  - pros:
    - Less data to send to shader
  - cons:
    - No support for scaling
	- Need to convert to Matrix34 unless CMM_Bone::m_BoneTransform
      and CMM_Bone::m_LocalTransform are also Transform.
*/


//=========================================================================================
// CMM_Bone
//=========================================================================================

const CMM_Bone CMM_Bone::ms_NullBone = CMM_Bone();


void CMM_Bone::LoadBone_r( CMMA_Bone& rSrcBone,
						   Transform *paBlendTransforms,
						   int &iNumRegisteredMatrices )
{
	m_strName = rSrcBone.strName;

	m_iNumChildren = (int)rSrcBone.vecChild.size();

	m_paChild = new CMM_Bone [m_iNumChildren];

	// set pointer to the transform matrix array
	m_pWorldTransform = paBlendTransforms + iNumRegisteredMatrices;

	m_MatrixIndex = iNumRegisteredMatrices;

	iNumRegisteredMatrices++;

	// get a matrix that transforms vertices from local model space to local bone space
	m_BoneTransform = rSrcBone.BoneTransform;

	m_vLocalOffset = rSrcBone.vLocalOffset;

	m_BoneTransform.GetInverseROT( m_LocalTransform );


	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].LoadBone_r( rSrcBone.vecChild[i], paBlendTransforms, iNumRegisteredMatrices );
	}
}


inline void CMM_Bone::CalculateWorldTransform( const Matrix34* pParentMatrix,
									           const Matrix34 *paSrcMatrix,
									           int& rIndex,
									           Matrix34& dest_world_transform )
{
	Matrix34 local_rot = paSrcMatrix[rIndex++];
	if( pParentMatrix )
		local_rot.vPosition = Vector3(0,0,0);

	Matrix34 mat;

//	mat = m_LocalTransform * paSrcMatrix[rIndex++] * m_BoneTransform;
	mat = m_LocalTransform * local_rot * m_BoneTransform;

//>>>------------ test calculations ----------------
/*
	Matrix34 mat2 = m_LocalTransform * ( local_rot * m_BoneTransform );
	Matrix34 mat3 = m_BoneTransform * local_rot * m_LocalTransform;

	Vector3 v1, v2, v3, v4, v0 = Vector3( 2.1f, 1.0f, -11.0f );
	v1 = m_BoneTransform * v0;
	v2 = local_rot * v1;
	v3 = m_LocalTransform * v2;

	v4 = mat * v0;
*/
//<<<------------ test calculations ----------------

	if( pParentMatrix )
		mat = (*pParentMatrix) * mat;

	dest_world_transform = mat;
}


void CMM_Bone::Transform_r( Matrix34* pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex )
{
	Matrix34 world_transform( Matrix34Identity() );
	CalculateWorldTransform( pParentMatrix, paSrcMatrix, rIndex, world_transform );

	if( m_pWorldTransform )
		m_pWorldTransform->FromMatrix34( world_transform );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].Transform_r( &world_transform, paSrcMatrix, rIndex );
	}
}


void CMM_Bone::CalculateTransforms_r( const Matrix34 *pParentMatrix,
									  const Matrix34 *paSrcMatrix,
									  int& rIndex,
									  Transform *paDest )
{
	Matrix34 world_transform;
	CalculateWorldTransform( pParentMatrix, paSrcMatrix, rIndex, world_transform );

	paDest[m_MatrixIndex] = Transform( world_transform );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].CalculateTransforms_r( &world_transform, paSrcMatrix, rIndex, paDest );
	}
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


void CMM_Bone::DumpToTextFile( FILE* fp, int depth )
{
	for( int i=0; i<depth; i++ ) fprintf( fp, "  " );
	fprintf( fp, "%s: t%s, q%s\n", m_strName.c_str(), to_string(m_LocalTransform.vPosition).c_str(), to_string(Quaternion(m_LocalTransform.matOrient)).c_str() );

	for( int i=0; i<m_iNumChildren; i++ )
		m_paChild[i].DumpToTextFile( fp, depth + 1 );
}
