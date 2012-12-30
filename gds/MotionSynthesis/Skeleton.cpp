#include "Skeleton.hpp"
#include "Keyframe.hpp"
#include "../Support/Vec3_StringAux.hpp"


namespace amorphous
{

using namespace msynth;


//==================================================================
// CBone
//==================================================================


void CBone::Scale_r( float factor )
{
	m_vOffset *= factor;

	const size_t num_children = m_vecChild.size();
	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChild[i].Scale_r( factor );
	}
}


bool CBone::CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const
{
	const size_t num_children = m_vecChild.size();

/*	if( bone_name == GetName() )
		return true;
*/
	if( num_children == 0 )
		return false;

	for( size_t i=0; i<num_children; i++ )
	{
		if( m_vecChild[i].GetName() == bone_name )
		{
			locator.clear();
			locator.push_back( (int)i );
			return true;
		}
	}

	// The target bone was not found in the immediate children
	for( size_t i=0; i<num_children; i++ )
	{
		bool found = m_vecChild[i].CreateLocator( bone_name, locator );
		if( found )
		{
			locator.insert( locator.begin(), (int)i );
			return true;
		}
	}

	return false;
}


void CBone::CreateEmptyTransformNodeTree( CTransformNode& parent_transform_node )
{
	const size_t num_children = m_vecChild.size();
	parent_transform_node.m_vecChildNode.resize( 0 );
	parent_transform_node.m_vecChildNode.resize( num_children );
	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChild[i].CreateEmptyTransformNodeTree( parent_transform_node.m_vecChildNode[i] );
	}
}


Vector3 CBone::CalculateNodePositionInSkeletonSpace(
	const std::vector<int>& node_locator,
	uint& index,
	const Transform& parent_transform,
	const CTransformNode& parent_transform_node
	) const
{
	if( (uint)node_locator.size() == index )
	{
		return parent_transform * m_vOffset;
	}

	int bone_and_node_index = node_locator[index];
	const int num_children = (int)m_vecChild.size();
	if( num_children <= bone_and_node_index
	 || parent_transform_node.GetNumChildren() <= bone_and_node_index )
	{
		return Vector3(0,0,0);
	}

	index++;

	const CTransformNode& child_transform_node = parent_transform_node.GetChildNode(bone_and_node_index);
	Transform offset_transform = Transform( Quaternion().FromRotationMatrix(Matrix33Identity()), m_vOffset );
	Matrix34 transform;
	CalculateWorldTransform( transform, parent_transform.ToMatrix34(), child_transform_node );
	return m_vecChild[bone_and_node_index].CalculateNodePositionInSkeletonSpace(
		node_locator,
		index,
		parent_transform * child_transform_node.GetLocalTransform() * offset_transform,
		child_transform_node
		);
}


void CBone::DumpToTextFile( FILE* fp, int depth )
{
	for( int i=0; i<depth; i++ ) fprintf( fp, "  " );
	fprintf( fp, "%s: t%s, q%s\n", m_Name.c_str(), to_string(m_vOffset).c_str(), to_string(Quaternion(m_matOrient)).c_str() );

	for( size_t i=0; i<m_vecChild.size(); i++ )
		m_vecChild[i].DumpToTextFile( fp, depth + 1 );
}


void CBone::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Name;
	ar & m_vOffset;

	if( 1 <= version )
		ar & m_matOrient;

	ar & m_vecChild;
}

int g_htrans_rev = 3;

void CBone::CalculateWorldTransform( Matrix34& dest_transform, const Matrix34& parent_transform, const CTransformNode& input_node ) const
{
  if( g_htrans_rev == 3 )
  {
	if( false/*m_TransformStyle & APPLY_LOCAL_ROTATION_TO_OFFSET*/ )
	{
		const Matrix33 matLocalRot = input_node.GetLocalRotationQuaternion().ToRotationMatrix();
		const Vector3 vLocalTrans = input_node.GetLocalTranslation() + m_vOffset;
/*		dest_transform.vPosition = parent_transform.matOrient * matLocalRot * vLocalTrans + parent_transform.vPosition;
		dest_transform.matOrient = parent_transform.matOrient * matLocalRot;
*/
		dest_transform
			= parent_transform
			* Matrix34( input_node.GetLocalTranslation(), matLocalRot );
//			* Matrix34( -m_vOffset, Matrix33Transpose(m_matOrient) );
//			* Matrix34( m_vOffset, m_matOrient ).GetInverseROT();
//			* Matrix34( Vector3(0,0,0), m_matOrient );
	}
	else
	{
		Matrix33 matRotation = input_node.GetLocalRotationQuaternion().ToRotationMatrix() * m_matOrient;
		dest_transform
			= parent_transform
//			* Matrix34( input_node.GetLocalTranslation(), input_node.GetLocalRotationQuaternion().ToRotationMatrix() )
//			* Matrix34( m_vOffset, Matrix33Identity() );
///			* Matrix34( input_node.GetLocalTranslation() + m_vOffset, input_node.GetLocalRotationQuaternion().ToRotationMatrix() );
///			* Matrix34( input_node.GetLocalTranslation() + m_vOffset, matRotation );
///			* Matrix34( Vector3(0,0,0), matRotation ) * Matrix34( input_node.GetLocalTranslation() + m_vOffset, Matrix33Identity() );
///			* Matrix34( Vector3(0,0,0), m_matOrient ) * Matrix34( input_node.GetLocalTranslation() + m_vOffset, input_node.GetLocalRotationQuaternion().ToRotationMatrix() );
//			* Matrix34( input_node.GetLocalTranslation(), input_node.GetLocalRotationQuaternion().ToRotationMatrix() ) * Matrix34( m_vOffset, m_matOrient );
//			* Matrix34( input_node.GetLocalTranslation(), input_node.GetLocalRotationQuaternion().ToRotationMatrix() ) * Matrix34( Vector3(0,0,0), m_matOrient ) * Matrix34( m_vOffset, Matrix33Identity() );

			// Transforms for arms are not correctly calculated.
			// The root and other nodes are not correctly transformed for run motion.
			* Matrix34( m_vOffset, m_matOrient ) * Matrix34( input_node.GetLocalTranslation(), input_node.GetLocalRotationQuaternion().ToRotationMatrix() );
	}
  }
  else // if( sg_rev == 2 )
  {
	//
  }
}



//==================================================================
// CSkeleton
//==================================================================

bool CSkeleton::CreateLocator( const std::string& bone_name, std::vector<int>& locator ) const
{
	locator.resize( 0 );

	if( m_RootBone.GetName() == bone_name )
	{
		// The root bone is the target
		// - Return the empty locator, which points to the root bone.
		return true;
	};

	return m_RootBone.CreateLocator( bone_name, locator );
}


void CSkeleton::DumpToTextFile( const std::string& output_filepath )
{
	FILE *fp = fopen( output_filepath.c_str(), "w" );
	if( !fp )
		return;

	m_RootBone.DumpToTextFile( fp, 0 );

	fclose(fp);
}


Vector3 CSkeleton::CalculateNodePositionInSkeletonSpace( const std::vector<int> node_locator, const CKeyframe& keyframe ) const
{
	unsigned int index = 0;
	Transform transform = Transform( Matrix34Identity() );
	return m_RootBone.CalculateNodePositionInSkeletonSpace( node_locator, index, transform, keyframe.GetRootNode() );
}


} // namespace amorphous
