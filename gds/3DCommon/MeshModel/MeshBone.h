
#ifndef  __MESHBONE_H__
#define  __MESHBONE_H__


//#include <vector>
#include <string>
using namespace std;


#include <d3dx9.h>

#include "Support/memory_helpers.h"

#include "3DMeshModelArchive.h"


//namespace GameLib1
//{

namespace MeshModel
{


/**
 * bone for mesh that needs hierarchical transformation
 *
 */
class CMM_Bone
{
	/// holds bone name (for debugging)
	string m_strName;

	D3DXVECTOR3 m_vLocalOffset;

	D3DXMATRIX *m_pWorldTransform;

	int m_MatrixIndex;

	/// tranforms vertices from model space to bone space
	D3DXMATRIX m_matBoneTransform;
	Matrix34 m_BoneTransform;

	/// bone space to local space
	D3DXMATRIX m_matLocalTransform;
	Matrix34 m_LocalTransform;

	CMM_Bone *m_paChild;

	int m_iNumChildren;

	/// for functions that finds a bone and returns its of bone,
	/// but has return null if the bone is not found
	static const CMM_Bone ms_NullBone;

public:

	CMM_Bone() : m_iNumChildren(0), m_paChild(NULL), m_pWorldTransform(NULL), m_MatrixIndex(0) {}

	~CMM_Bone() { SafeDeleteArray( m_paChild ); }

	void LoadBone_r( CMMA_Bone& rSrcBone, D3DXMATRIX *paBoneTransformMatrix, int &iNumRegisteredMatrices );

//	void Transform_EulerAngle( float *pafData );

	void Transform_Quaternion( float *pafData, int& rIndex );

	/// accepts an array of matrices that represents local transformation at each bone
	void Transform_r( Matrix34* pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex );

	int GetBoneMatrixIndexByName_r( const char *pName );

	int GetMatrixIndex() const { return m_MatrixIndex; }

	const Vector3& GetLocalOffset() const { return m_vLocalOffset; }

	const CMM_Bone& GetBoneByName_r( const char *pName ) const;

	void SetBoneToArray_r( vector<CMM_Bone *>& vecpDestArray );

//	static const CMM_Bone& NullBone() const { return ms_NullBone; }
	static const CMM_Bone& NullBone() { return ms_NullBone; }

	bool operator==( const CMM_Bone& bone ) const
	{
		if( m_strName == bone.m_strName
		 && m_MatrixIndex == bone.m_MatrixIndex
		 && m_pWorldTransform == bone.m_pWorldTransform
		 && m_paChild == bone.m_paChild )
			return true;
		else
			return false;
	}

	bool operator!=( const CMM_Bone& bone ) const
	{
		return !( (*this) == bone );
	}

//	inline void SetLocalTransform( const Matrix34& local_transform );

//	friend class CD3DXMeshModel;
};


}	/*  3DMesh  */

//}	/* GameLib1   */


#endif		/*  __MESHBONE_H__  */