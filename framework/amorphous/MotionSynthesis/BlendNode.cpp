#include "BlendNode.hpp"
#include "Skeleton.hpp"
#include <map>


namespace amorphous
{

using namespace boost;


namespace msynth
{


BlendNode::BlendNode()
:
m_Priority(0)
{
}


void BlendNode::CreateFromSkeleton( const Bone& src_bone )
{
	m_Name = src_bone.GetName();
	m_vecpChild.resize( src_bone.GetNumChildren() );
	for( int i=0; i<(int)m_vecpChild.size(); i++ )
	{
		m_vecpChild[i] = shared_ptr<BlendNode>( new BlendNode );
		m_vecpChild[i]->m_pSelf = m_vecpChild[i];
		m_vecpChild[i]->CreateFromSkeleton( src_bone.GetChild(i) );
	}
}


shared_ptr<BlendNode> BlendNode::GetBlendNodeByName( const std::string& name )
{
	if( m_Name == name )
	{
		return m_pSelf.lock();
	}

	for( int i=0; i<(int)m_vecpChild.size(); i++ )
	{
		shared_ptr<BlendNode> pNode = m_vecpChild[i]->GetBlendNodeByName( name );

		if( pNode )
			return pNode;
	}

	return shared_ptr<BlendNode>();
}


} // namespace msynth


} // namespace amorphous
