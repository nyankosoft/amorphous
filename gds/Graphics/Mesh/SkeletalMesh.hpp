#ifndef __SkeletalMesh_HPP__
#define __SkeletalMesh_HPP__


#include "ProgressiveMesh.hpp"
#include "../MeshModel/MeshBone.hpp"


namespace amorphous
{


class SkeletalMesh : public ProgressiveMesh
{
public:

	SkeletalMesh();

	SkeletalMesh( const std::string& filename );

	~SkeletalMesh() {}

//	virtual void Release();

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags );

//	virtual bool LoadFromArchive( C3DMeshModelArchive& archive, const std::string& filename, U32 option_flags, int num_pmeshes );

	inline int GetNumBones() const { return m_pImpl->GetNumBones(); }

	inline int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return m_pImpl->GetBoneMatrixIndexByName( bone_name ); }

//	inline const CMeshBone& GetBone( int index ) const;

	inline const CMeshBone& GetBone( const std::string& bone_name ) const { return m_pImpl->GetBone( bone_name ); }

	inline const CMeshBone& GetRootBone() const { return m_pImpl->GetRootBone(); }

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// calculate hierarchical transforms by handiing an array of matrices
	/// that represents local transformations at each bone
	inline void SetLocalTransforms( Matrix34 *paLocalTransform );

	/// set local transformation for a bone to a cache
	/// NOTE: do not updates transformation for the bone
	/// use is responsible for calling SetLocalTransformsFromCache() later to updates the transformations for bones
	inline void SetLocalTransformToCache( int index, const Matrix34& local_transform ) { m_pImpl->SetLocalTransformToCache(index,local_transform); }

	inline void CalculateBlendTransformsFromCachedLocalTransforms() { m_pImpl->CalculateBlendTransformsFromCachedLocalTransforms(); }

	inline void ResetLocalTransformsCache() { m_pImpl->ResetLocalTransformsCache(); }

	inline void CalculateBlendTransforms( const std::vector<Transform>& src_local_transforms, std::vector<Transform>& dest_blend_transforms ) { m_pImpl->CalculateBlendTransforms( src_local_transforms, dest_blend_transforms ); }

	/// returns the pointer to the array of vertex blend matrices (world transforms)
//	inline void GetBlendMatrices( D3DXMATRIX* paDestMatrix ) { paDestMatrix = m_paBoneMatrix; }
	inline Transform* GetBlendTransforms() { return m_pImpl->GetBlendTransforms(); }

	/// Get vertex blend transforms
	/// - The returned vertex blend transforms are in world coordinates
	/// - Use the local transforms currently stored in cache.
	inline void GetBlendTransforms( std::vector<Transform>& dest_transforms ) { m_pImpl->GetBlendTransforms( dest_transforms ); }

	inline void DumpSkeletonToTextFile( const std::string& output_filepath ) const { m_pImpl->DumpSkeletonToTextFile( output_filepath ); }

	virtual MeshType::Name GetMeshType() const { return MeshType::SKELETAL; }
};


} // namespace amorphous



#endif /* __SkeletalMesh_HPP__ */
