#include "SkeletalMeshTransform.hpp"
#include "amorphous/Graphics/Mesh/SkeletalMesh.hpp"


namespace amorphous
{


using namespace std;


namespace msynth
{


void UpdateMeshBoneTransforms_r( const msynth::Bone& bone,
                                 const msynth::TransformNode& node,
								 SkeletalMesh& mesh )
{
	// find the matrix index from the bone name (slow).
	int index = mesh.GetBoneMatrixIndexByName( bone.GetName() );
	if( index == -1 )
		return;

	Matrix34 local_transform;
	local_transform.vPosition = node.GetLocalTranslation();
	local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix();
	mesh.SetLocalTransformToCache( index, local_transform );

	const int num_child_bones = bone.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
//	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		UpdateMeshBoneTransforms_r(
			bone.GetChild(i),
			node.GetChildNode(i),
			mesh );
	}
}


void UpdateMeshBoneTransforms_r( const msynth::TransformNodeMap& map_node,
                                 const msynth::TransformNode& node,
								 SkeletalMesh& mesh )
{
	// find the matrix index from the bone name (slow).
	int index = map_node.m_DestIndex;
//	if( index == -1 )
//		return;

	if( 0 <= index )
	{
		Matrix34 local_transform;
		local_transform.vPosition = node.GetLocalTranslation();
		local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix();
		mesh.SetLocalTransformToCache( index, local_transform );
	}

	const int num_child_bones = map_node.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
//	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		UpdateMeshBoneTransforms_r(
			map_node.GetChild(i),
			node.GetChildNode(i),
			mesh );
	}
}


void UpdateMeshBoneTransforms_r( const msynth::Bone& bone,
                                 const msynth::TransformNode& node,
								 const SkeletalMesh& mesh,
								 std::vector<Transform>& mesh_bone_local_transforms )
{
	// find the matrix index from the bone name (slow).
	int index = mesh.GetBoneMatrixIndexByName( bone.GetName() );
	if( index == -1 )
		return;

	Matrix34 local_transform;
	local_transform.vPosition = node.GetLocalTranslation();
	local_transform.matOrient = node.GetLocalRotationQuaternion().ToRotationMatrix();

	if( 0 <= index && index < (int)mesh_bone_local_transforms.size() )
		mesh_bone_local_transforms[index].FromMatrix34( local_transform );

	const int num_child_bones = bone.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
//	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		UpdateMeshBoneTransforms_r(
			bone.GetChild(i),
			node.GetChildNode(i),
			mesh,
			mesh_bone_local_transforms );
	}
}


void UpdateMeshBoneTransforms_r( const msynth::TransformNodeMap& map_node,
                                 const msynth::TransformNode& node,
								 std::vector<Transform>& mesh_bone_local_transforms )
{
	int index = map_node.m_DestIndex;

	if( 0 <= index && index < (int)mesh_bone_local_transforms.size() )
	{
		mesh_bone_local_transforms[index] = node.GetLocalTransform();
	}

	const int num_child_bones = map_node.GetNumChildren();
	const int num_child_nodes = node.GetNumChildren();
	const int num_children = take_min( num_child_bones, num_child_nodes );
//	const int num_children = node.GetNumChildren();
	for( int i=0; i<num_children; i++ )
	{
		UpdateMeshBoneTransforms_r(
			map_node.GetChild(i),
			node.GetChildNode(i),
			mesh_bone_local_transforms );
	}
}


void CreateTransformMapTree_r( const msynth::Bone& src_bone,
							  msynth::TransformNodeMap& map_node,
							  const SkeletalMesh& mesh )
{
	// find the matrix index from the bone name (slow).
	map_node.m_DestIndex = mesh.GetBoneMatrixIndexByName( src_bone.GetName() );

	const int num_child_bones = src_bone.GetNumChildren();
//	const int num_child_nodes = node.GetNumChildren();
	const int num_children = num_child_bones;//take_min( num_child_bones, num_child_nodes );

	map_node.m_Children.resize( num_children );
	for( int i=0; i<num_children; i++ )
	{
		CreateTransformMapTree_r(
			src_bone.GetChild(i),
			map_node.Child(i),
			mesh );
	}
}


void CreateTransformMapTree( const msynth::Skeleton& src_skeleton, msynth::TransformNodeMap& root_map_node, const SkeletalMesh& mesh )
{
	CreateTransformMapTree_r( src_skeleton.GetRootBone(), root_map_node, mesh );
}


void CreateMSynthSkeletonFromMeshSkeleton_r( const MeshBone& src_bone, msynth::Bone& dest_bone )
{
	dest_bone.SetOffset( src_bone.GetLocalOffset() );
	dest_bone.SetOrient( Matrix33Identity() );
	dest_bone.SetName( src_bone.GetName() );
	dest_bone.Children().resize( src_bone.GetNumChildren() );

	for( uint i=0; i<src_bone.GetNumChildren(); i++ )
	{
		CreateMSynthSkeletonFromMeshSkeleton_r( src_bone.GetChild( (unsigned int)i ), dest_bone.Child(i) );
	}
}


void CreateSkeletonFromMeshSkeleton( const SkeletalMesh& src_skeletal_mesh, msynth::Skeleton& dest_skeleton )
{
	CreateMSynthSkeletonFromMeshSkeleton_r( src_skeletal_mesh.GetRootBone(), dest_skeleton.RootBone() );
}


std::shared_ptr<msynth::Skeleton> CreateSkeletonFromMeshSkeleton( const SkeletalMesh& src_skeletal_mesh )
{
	std::shared_ptr<msynth::Skeleton> pSkeleton( new msynth::Skeleton );
	CreateSkeletonFromMeshSkeleton( src_skeletal_mesh, *pSkeleton );
	return pSkeleton;
}



} // namespace msynth


} // namespace amorphous
