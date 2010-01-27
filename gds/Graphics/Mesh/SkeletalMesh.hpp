#ifndef __SkeletalMesh_HPP__
#define __SkeletalMesh_HPP__


#include "ProgressiveMesh.hpp"


class CSkeletalMesh : public CProgressiveMesh
{
public:

	CSkeletalMesh();

	CSkeletalMesh( const std::string& filename );

	~CSkeletalMesh() {}

//	virtual void Release();

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes );

	inline int GetNumBones() const { return m_pImpl->GetNumBones(); }

	inline int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return m_pImpl->GetBoneMatrixIndexByName( bone_name ); }

//	inline const CMM_Bone& GetBone( int index ) const;

	inline const CMM_Bone& GetBone( const std::string& bone_name ) const { return m_pImpl->GetBone( bone_name ); }

	inline const CMM_Bone& GetRootBone() const { return m_pImpl->GetRootBone(); }

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	inline void SetLocalTransforms( Matrix34 *paLocalTransform );

	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	inline void SetLocalTransformToCache( int index, const Matrix34& local_transform ) { m_pImpl->SetLocalTransformToCache(index,local_transform); }

	inline void SetLocalTransformsFromCache() { m_pImpl->SetLocalTransformsFromCache(); }

	inline void ResetLocalTransformsCache() { m_pImpl->ResetLocalTransformsCache(); }

	/// returns the pointer to the array of vertex blend matrices (world transforms)
//	inline void GetBlendMatrices( D3DXMATRIX* paDestMatrix ) { paDestMatrix = m_paBoneMatrix; }
	inline D3DXMATRIX* GetBlendMatrices() { return m_pImpl->GetBlendMatrices(); }

	/// Get vertex blend transforms
	/// - The returned vertex blend transforms are in world coordinates
	/// - Use the local transforms currently stored in cache.
	inline void GetBlendTransforms( std::vector<Transform>& dest_transforms ) { m_pImpl->GetBlendTransforms( dest_transforms ); }

	virtual CMeshType::Name GetMeshType() const { return CMeshType::SKELETAL; }
};



#endif /* __SkeletalMesh_HPP__ */
