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


} // namespace msynth
