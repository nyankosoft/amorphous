#ifndef  __SkeletalMeshTransform_HPP__
#define  __SkeletalMeshTransform_HPP__


#include "Skeleton.hpp"
#include "TransformNode.hpp"
#include "MotionPrimitive.hpp"
#include "gds/Graphics/fwd.hpp"


namespace msynth
{


class CTransformNodeMap
{
public:

	int m_DestIndex;

	std::vector<CTransformNodeMap> m_Children;

public:

	CTransformNodeMap()
		:
	m_DestIndex(-1)
	{}

	int GetNumChildren() const { return (int)m_Children.size(); }

	const CTransformNodeMap& GetChild( int i ) const { return m_Children[i]; }

	CTransformNodeMap& Child( int i ) { return m_Children[i]; }
};


// ---------------------------- Global Functions ----------------------------

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const CBone& bone, const CTransformNode& node, CSkeletalMesh& mesh );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const CTransformNodeMap& map_node, const CTransformNode& node, CSkeletalMesh& mesh );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const CBone& bone, const CTransformNode& node, const CSkeletalMesh& mesh, std::vector<Transform>& mesh_bone_local_transforms );

/// This is a private function of the msynth module
/// Client code should not call this.
void UpdateMeshBoneTransforms_r( const CTransformNodeMap& map_node, const CTransformNode& node, std::vector<Transform>& mesh_bone_local_transforms );

/// Set transforms to mesh bones.
/// For each bone, search the corresponding mesh bone with CSkeletalMesh::GetBoneMatrixIndexByName() (slow)
/// \param keyframe [in] source keyframe
/// \param skeleton [in] tree of bones
/// \param target_skeletal_mesh [out] skeletal mesh
inline void UpdateMeshBoneTransforms( const CKeyframe& keyframe, const CSkeleton& skeleton, CSkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), target_skeletal_mesh );
}


inline void UpdateMeshBoneTransforms( const CKeyframe& keyframe, const CTransformNodeMap& root_map_node, CSkeletalMesh& target_skeletal_mesh )
{
	UpdateMeshBoneTransforms_r( root_map_node, keyframe.GetRootNode(), target_skeletal_mesh );
}


inline void UpdateMeshBoneTransforms( const CKeyframe& keyframe, const CSkeleton& skeleton, const CSkeletalMesh& mesh, std::vector<Transform>& mesh_bone_local_transforms )
{
	UpdateMeshBoneTransforms_r( skeleton.GetRootBone(), keyframe.GetRootNode(), mesh, mesh_bone_local_transforms );
}


inline void UpdateMeshBoneTransforms( const CKeyframe& keyframe, const CTransformNodeMap& root_map_node, std::vector<Transform>& mesh_bone_local_transforms )
{
	UpdateMeshBoneTransforms_r( root_map_node, keyframe.GetRootNode(), mesh_bone_local_transforms );
}


/// Creates a tree of CTransformNodeMap that has the same hierarchical structure with that of src_skeleton
/// Each created node of CTransformNodeMap is provided with the index
/// to its corresponding mesh bone transform in the transform array of the skeletal mesh
void CreateTransformMapTree( const CSkeleton& src_skeleton, CTransformNodeMap& root_map_node, const CSkeletalMesh& mesh );


} // namespace msynth


#endif /* __SkeletalMeshTransform_HPP__ */
