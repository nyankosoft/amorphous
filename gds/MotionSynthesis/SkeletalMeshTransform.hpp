#ifndef  __SkeletalMeshTransform_HPP__
#define  __SkeletalMeshTransform_HPP__


#include "Skeleton.hpp"
#include "TransformNode.hpp"
#include "MotionPrimitive.hpp"
#include "Graphics/fwd.hpp"


namespace msynth
{


void UpdateMeshBoneTransforms_r( const CBone& bone, const CTransformNode& node, CSkeletalMesh& mesh );

/// Set transforms to mesh bones.
/// For each bone, search the corresponding mesh bone with CSkeletalMesh::GetBoneMatrixIndexByName() (slow)
/// \param keyframe [in] source keyframe
/// \param skeleton [in] tree of bones
/// \param target_skeletal_mesh [out] skeletal mesh
inline void UpdateMeshBoneTransforms( const CKeyframe& keyframe, const CSkeleton& skeleton, CSkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), target_skeletal_mesh );
}


} // namespace msynth


#endif /* __SkeletalMeshTransform_HPP__ */
