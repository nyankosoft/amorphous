#include "MeshBone.hpp"
#include "3DMath/MatrixConversions.hpp"
#include "Support/Vec3_StringAux.hpp"
#include "3DMeshModelArchive.hpp"


namespace amorphous
{

using namespace std;



/**
How to replace D3DXMATRIX MeshBone::m_pWorldTransform
-----------------------------------------------------
- Replace with Matrix44
  - pros:
    - Direct3D and OpenGL implementations may be able to share MeshBone.
  - cons:
    - ID3DXEffect::SetMatrixTranspose() is slower than ID3DXEffect::SetMatrix()
Replace with Transform
  - pros:
    - Less data to send to shader
  - cons:
    - No support for scaling
	- Need to convert to Matrix34 unless MeshBone::m_BoneTransform
      and MeshBone::m_LocalTransform are also Transform.
*/


//=========================================================================================
// MeshBone
//=========================================================================================

const MeshBone MeshBone::ms_NullBone = MeshBone();


void MeshBone::LoadBone_r( CMMA_Bone& rSrcBone,
						   Transform *paBlendTransforms,
						   int &iNumRegisteredMatrices )
{
	m_strName = rSrcBone.strName;

	m_iNumChildren = (int)rSrcBone.vecChild.size();

	m_paChild = new MeshBone [m_iNumChildren];

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


inline void MeshBone::CalculateWorldTransform( const Matrix34* pParentMatrix,
									           const Matrix34 *paSrcMatrix,
									           int& rIndex,
									           Matrix34& dest_world_transform )
{
	Matrix34 local_rot = paSrcMatrix[rIndex];
	if( pParentMatrix )
		local_rot.vPosition = Vector3(0,0,0);

	Matrix34 mat;

//	mat = m_LocalTransform * paSrcMatrix[rIndex++] * m_BoneTransform;
	mat = m_LocalTransform * local_rot * m_BoneTransform;

	if( pParentMatrix )
		mat = (*pParentMatrix) * mat;

	dest_world_transform = mat;
}


inline void MeshBone::CalculateBlendTransform( const Transform *pParentTransform,
									            const Transform& src_local_transform,
									            Transform& dest_blend_transform )
{
	Transform local_rot = src_local_transform;
	if( pParentTransform )
		local_rot.vTranslation = Vector3(0,0,0);

	Matrix34 mat;

//	mat = m_LocalTransform * paSrcMatrix[rIndex++] * m_BoneTransform;
	mat = m_LocalTransform * local_rot.ToMatrix34() * m_BoneTransform;

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

	if( pParentTransform )
		dest_blend_transform = (*pParentTransform) * Transform(mat);
	else
		dest_blend_transform = Transform(mat);
}


void MeshBone::Transform_r( Matrix34* pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex )
{
	Matrix34 world_transform( Matrix34Identity() );
	CalculateWorldTransform( pParentMatrix, paSrcMatrix, rIndex, world_transform );

	if( m_pWorldTransform )
		m_pWorldTransform->FromMatrix34( world_transform );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		rIndex++;
		m_paChild[i].Transform_r( &world_transform, paSrcMatrix, rIndex );
	}
}


void MeshBone::CalculateBlendTransforms_r( Transform* pParentTransform, const Transform *paSrcTransform, Transform *pBlendTransforms, int& rIndex )
{
	CalculateBlendTransform( pParentTransform, paSrcTransform[rIndex], pBlendTransforms[rIndex] );

	int current_index = rIndex;
	for( int i=0; i<m_iNumChildren; i++ )
	{
		rIndex++;
		m_paChild[i].CalculateBlendTransforms_r( &pBlendTransforms[current_index], paSrcTransform, pBlendTransforms, rIndex );
	}
}

/*
void MeshBone::CalculateBlendTransforms_r( const Transform *pParentTransform,
									        const Transform& src_transform,
									        int& rIndex,
									        Transform *paDest )
{
	CalculateBlendTransform( pParentTransform, paSrcMatrix, paDest[m_MatrixIndex] );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		rIndex++;
		m_paChild[i].CalculateBlendTransforms_r( &blend_transform, src_transform, rIndex, paDest );
	}
}
*/

int MeshBone::GetBoneMatrixIndexByName_r( const char *pName )
{
/*	const MeshBone& bone = GetBoneByName_r( pName );

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


const MeshBone& MeshBone::GetBoneByName_r( const char *pName ) const
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
				const MeshBone& bone = m_paChild[i].GetBoneByName_r( pName );
				if( bone != NullBone() )
					return bone;	// found the bone with the specified name
			}

			return NullBone();	// not found in this sub-tree
		}
	}
}


void MeshBone::SetBoneToArray_r( vector<MeshBone *>& vecpDestArray )
{
	vecpDestArray.push_back( this );

	for( int i=0; i<m_iNumChildren; i++ )
	{
		m_paChild[i].SetBoneToArray_r( vecpDestArray );
	}
}


void MeshBone::DumpToTextFile( FILE* fp, int depth )
{
	for( int i=0; i<depth; i++ ) fprintf( fp, "  " );
	fprintf( fp, "%s: t%s, q%s\n", m_strName.c_str(), to_string(m_LocalTransform.vPosition).c_str(), to_string(Quaternion(m_LocalTransform.matOrient)).c_str() );

	for( int i=0; i<m_iNumChildren; i++ )
		m_paChild[i].DumpToTextFile( fp, depth + 1 );
}


} // namespace amorphous
