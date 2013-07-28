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

	inline int GetNumBones() const { return m_pImpl->GetNumBones(); }

	inline int GetBoneMatrixIndexByName( const std::string& bone_name ) const { return m_pImpl->GetBoneMatrixIndexByName( bone_name ); }

//	inline const MeshBone& GetBone( int index ) const;

	inline const MeshBone& GetBone( const std::string& bone_name ) const { return m_pImpl->GetBone( bone_name ); }

	inline const MeshBone& GetRootBone() const { return m_pImpl->GetRootBone(); }

//	inline void SetLocalTransform( int matrix_index, const Matrix34 *local_transform );

	/// \brief Sets a local transform for a bone to a cache
	/// NOTE: this does not update the transform for the bone.
	/// The user is responsible for calling SetLocalTransformsFromCache() later to update the transforms for bones.
	inline void SetLocalTransformToCache( int index, const Matrix34& local_transform ) { m_pImpl->SetLocalTransformToCache(index,local_transform); }

	inline void CalculateBlendTransformsFromCachedLocalTransforms() { m_pImpl->CalculateBlendTransformsFromCachedLocalTransforms(); }

	inline void ResetLocalTransformsCache() { m_pImpl->ResetLocalTransformsCache(); }

	/// \param[in] src_local_transforms   The Local transforms for bones. These represent, in the case of a humanoid character,
	///                                   how many degrees hands, arms, legs, etc. joints are bent
	/// \param[out] dest_blend_transforms The transforms which are usually set to the vertex shader.
	inline void CalculateBlendTransforms( const std::vector<Transform>& src_local_transforms, std::vector<Transform>& dest_blend_transforms ) { m_pImpl->CalculateBlendTransforms( src_local_transforms, dest_blend_transforms ); }

	/// \brief Returns the pointer to the array of vertex blend matrices (world transforms)
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
