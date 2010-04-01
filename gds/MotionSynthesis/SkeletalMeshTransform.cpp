#include "SkeletalMeshTransform.hpp"
#include "gds/Graphics/Mesh/SkeletalMesh.hpp"


using namespace std;


namespace msynth
{


void UpdateMeshBoneTransforms_r( const msynth::CBone& bone,
                                 const msynth::CTransformNode& node,
								 CSkeletalMesh& mesh )
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


void UpdateMeshBoneTransforms_r( const msynth::CTransformNodeMap& map_node,
                                 const msynth::CTransformNode& node,
								 CSkeletalMesh& mesh )
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


void CreateTransformMapTree_r( const msynth::CBone& src_bone,
							  msynth::CTransformNodeMap& map_node,
							  const CSkeletalMesh& mesh )
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


void CreateTransformMapTree( const msynth::CSkeleton& src_skeleton, msynth::CTransformNodeMap& root_map_node, const CSkeletalMesh& mesh )
{
	CreateTransformMapTree_r( src_skeleton.GetRootBone(), root_map_node, mesh );
}


} // namespace msynth
