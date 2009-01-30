#include "BVH/BVHBone.hpp"
#include "Skeleton.hpp"

using namespace msynth;


CBone::CBone( CBVHBone& bvh_bone )
:
m_vOffset(Vector3(0,0,0))
{
	CopyBones_r( bvh_bone );
}


void CBone::CopyBones_r( CBVHBone& src_bone )
{
	m_vOffset = src_bone.GetLocalOffset();

	m_Name = src_bone.GetName();

	const int num_children = src_bone.GetNumChildren();
	m_vecChild.resize( num_children );

	for( int i=0; i<num_children; i++ )
	{
		m_vecChild[i].CopyBones_r( *src_bone.GetChild(i) );
	}
}


void CBone::Scale_r( float factor )
{
	m_vOffset *= factor;

	const size_t num_children = m_vecChild.size();
	for( size_t i=0; i<num_children; i++ )
	{
		m_vecChild[i].Scale_r( factor );
	}
}


void CBone::Serialize( IArchive & ar, const unsigned int version )
{
	ar & m_Name;
	ar & m_vOffset;
	ar & m_vecChild;
}

