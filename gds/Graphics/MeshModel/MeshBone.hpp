#ifndef  __MeshBone_HPP__
#define  __MeshBone_HPP__


#include <vector>
#include <string>
#include "Graphics/fwd.hpp"
#include "Support/memory_helpers.hpp"
#include "3DMath/Transform.hpp"


//namespace GameLib1
//{

namespace MeshModel
{


/**
 bone for mesh that needs hierarchical transformation

 */
class CMeshBone
{
	/// holds bone name (for debugging)
	std::string m_strName;

	Vector3 m_vLocalOffset;

	/// vertex blend matrix
	Transform *m_pWorldTransform;

	int m_MatrixIndex;

	/// tranforms vertices from model space to bone space
	Matrix34 m_BoneTransform;

	/// bone space to local space
	Matrix34 m_LocalTransform;

	CMeshBone *m_paChild;

	int m_iNumChildren;

	/// for functions that finds a bone and returns its of bone,
	/// but has return null if the bone is not found
	static const CMeshBone ms_NullBone;

public:

	inline void CalculateWorldTransform( const Matrix34* pParentMatrix, const Matrix34 *paSrcMatrix, int& rIndex, Matrix34& dest_world_transform );

	inline void CalculateBlendTransform( const Transform *pParentTransform, const Transform& src_local_transform, Transform& dest_blend_transform );

public:

	inline CMeshBone();

	~CMeshBone() { SafeDeleteArray( m_paChild ); }

	void LoadBone_r( CMMA_Bone& rSrcBone, Transform *paBoneTransformMatrix, int &iNumRegisteredMatrices );

//	void Transform_EulerAngle( float *pafData );

	/// accepts an array of matrices that represents local transformation at each bone
	/// This function actually calculates world transforms
	void Transform_r( Matrix34 *pParentMatrix, Matrix34 *paSrcMatrix, int& rIndex );

	/// \param pParentMatrix [in]
	/// \param paSrcMatrix [in]
	/// \param pBlendTransforms [out]
	/// \param rIndex [in,out]
	void CalculateBlendTransforms_r( Transform *pParentTransform, const Transform *paSrcTransform, Transform *pBlendTransforms, int& rIndex );

//	void CalculateTransforms_r( const Matrix34 *pParentMatrix, const Matrix34 *paSrcMatrix, int& rIndex, Transform *paDest );

//	void CalculateBlendTransforms_r( const Transform *pParentTransform, const Transform& src_transform, int& rIndex, Transform *paDest );

	int GetBoneMatrixIndexByName_r( const char *pName );

	int GetMatrixIndex() const { return m_MatrixIndex; }

	const Vector3& GetLocalOffset() const { return m_vLocalOffset; }

	const Matrix34& GetBoneTransform() const { return m_BoneTransform; }

	const CMeshBone& GetBoneByName_r( const char *pName ) const;

	void SetBoneToArray_r( std::vector<CMeshBone *>& vecpDestArray );

//	static const CMeshBone& NullBone() const { return ms_NullBone; }
	static const CMeshBone& NullBone() { return ms_NullBone; }

	bool operator==( const CMeshBone& bone ) const
	{
		if( m_strName == bone.m_strName
		 && m_MatrixIndex == bone.m_MatrixIndex
		 && m_pWorldTransform == bone.m_pWorldTransform
		 && m_paChild == bone.m_paChild )
			return true;
		else
			return false;
	}

	bool operator!=( const CMeshBone& bone ) const
	{
		return !( (*this) == bone );
	}

	void DumpToTextFile( FILE* fp, int depth );

//	inline void SetLocalTransform( const Matrix34& local_transform );
};


//============================= inline implementations =============================

inline CMeshBone::CMeshBone()
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
