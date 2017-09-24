#ifndef  __SkeletalMeshTransform_HPP__
#define  __SkeletalMeshTransform_HPP__


#include "Skeleton.hpp"
#include "TransformNode.hpp"
#include "MotionPrimitive.hpp"
#include "amorphous/Graphics/fwd.hpp"


namespace amorphous
{


namespace msynth
{


class TransformNodeMap
{
public:

	int m_DestIndex;

	std::vector<TransformNodeMap> m_Children;

public:

	TransformNodeMap()
		:
	m_DestIndex(-1)
	{}

	int GetNumChildren() const { return (int)m_Children.size(); }

	const TransformNodeMap& GetChild( int i ) const { return m_Children[i]; }

	TransformNodeMap& Child( int i ) { return m_Children[i]; }
};


// ---------------------------- Global Functions ----------------------------

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const Bone& bone, const TransformNode& node, SkeletalMesh& mesh );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const TransformNodeMap& map_node, const TransformNode& node, SkeletalMesh& mesh );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const Bone& bone, const TransformNode& node, const SkeletalMesh& mesh, std::vector<Transform>& mesh_bone_local_transforms );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const TransformNodeMap& map_node, const TransformNode& node, std::vector<Transform>& mesh_bone_local_transforms );

/// Set transforms to mesh bones.
/// For each bone, search the corresponding mesh bone with SkeletalMesh::GetBoneMatrixIndexByName() (slow)
/// \param keyframe [in] source keyframe
/// \param skeleton [in] tree of bones
/// \param target_skeletal_mesh [out] skeletal mesh
inline void UpdateMeshBoneTransforms( const Keyframe& keyframe, const Skeleton& skeleton, SkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), target_skeletal_mesh );
}


inline void UpdateMeshBoneTransforms( const Keyframe& keyframe, const TransformNodeMap& root_map_node, SkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( root_map_node, keyframe.GetRootNode(), target_skeletal_mesh );
}


inline void UpdateMeshBoneTransforms( const Keyframe& keyframe, const Skeleton& skeleton, const SkeletalMesh& mesh, std::vector<Transform>& mesh_bone_local_transforms )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), mesh, mesh_bone_local_transforms );
}


inline void UpdateMeshBoneTransforms( const Keyframe& keyframe, const TransformNodeMap& root_map_node, std::vector<Transform>& mesh_bone_local_transforms )
{
	UpdateMeshBoneTransforms_r( root_map_node, keyframe.GetRootNode(), mesh_bone_local_transforms );
}


/// Creates a tree of TransformNodeMap that has the same hierarchical structure with that of src_skeleton
/// Each created node of TransformNodeMap is provided with the index
/// to its corresponding mesh bone transform in the transform array of the skeletal mesh
void CreateTransformMapTree( const Skeleton& src_skeleton, TransformNodeMap& root_map_node, const SkeletalMesh& mesh );

/// \param src_skeletal_mesh [in] source
/// \param dest_skeleton [out] destination
void CreateSkeletonFromMeshSkeleton( const SkeletalMesh& src_skeletal_mesh, msynth::Skeleton& dest_skeleton );

std::shared_ptr<msynth::Skeleton> CreateSkeletonFromMeshSkeleton( const SkeletalMesh& src_skeletal_mesh );

} // namespace msynth

} // namespace amorphous



#endif /* __SkeletalMeshTransform_HPP__ */
