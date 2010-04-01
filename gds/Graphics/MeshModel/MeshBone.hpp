#ifndef  __MeshBone_HPP__
#define  __MeshBone_HPP__


#include <vector>
#include <string>
#include <d3dx9.h>
#include "Graphics/fwd.hpp"
#include "Support/memory_helpers.hpp"
#include "3DMath/Transform.hpp"
//#include "3DMeshModelArchive.hpp"


//namespace GameLib1
//{

namespace MeshModel
{


/**
 bone for mesh that needs hierarchical transformation

 */
class CMM_Bone
{
	/// holds bone name (for debugging)
	std::string m_strName;

	Vector3 m_vLocalOffset;

	/// vertex blend matrix
	D3DXMATRIX *m_pWorldTransform;

	int m_MatrixIndex;

	/// tranforms vertices from model space to bone space
	Matrix34 m_BoneTransform;

	/// bone space to local space
	Matrix34 m_LocalTransform;

	CMM_Bone *m_paChild;

	int m_iNumChildren;

	/// for functions that finds a bone and returns its of bone,
	/// but has return null if the bone is not found
	static const CMM_Bone ms_NullBone;

public:

//	void CalculateWorldTransform( const Matrix34* pParentMatrix, const Matrix34 *paSrcMatrix, int& rIndex, Matrix34& dest_world_transform );

public:

	inline CMM_Bone();

	~CMM_Bone() { SafeDeleteArray( m_paChild ); }

	void LoadBone_r( CMMA_Bone& rSrcBone, D3DXMATRIX *paBoneTransformMatrix, int &iNumRegisteredMatrices );

//	void Transform_EulerAngle( float *pafData );

	/// accepts an array of matrices that represents local transformation at each bone
	/// This function actually calculates world transforms
	void Transform_r( Matrix34 *pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex );

//	void CalculateTransforms_r( const Matrix34 *pParentMatrix, const Matrix34 *paSrcMatrix, int& rIndex, Transform *paDest );

	int GetBoneMatrixIndexByName_r( const char *pName );

	int GetMatrixIndex() const { return m_MatrixIndex; }

	const Vector3& GetLocalOffset() const { return m_vLocalOffset; }

	const CMM_Bone& GetBoneByName_r( const char *pName ) const;

	void SetBoneToArray_r( std::vector<CMM_Bone *>& vecpDestArray );

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
};


//============================= inline implementations =============================

inline CMM_Bone::CMM_Bone()
:
m_vLocalOffset( Vector3(0,0,0) ),
m_pWorldTransform(NULL),
m_MatrixIndex(0),
m_BoneTransform( Matrix34Identity() ),
m_LocalTransform( Matrix34Identity() ),
m_paChild(NULL),
m_iNumChildren(0)
{}



}	//  MeshModel

//}	// GameLib1


#endif		/*  __MeshBone_HPP__  */
