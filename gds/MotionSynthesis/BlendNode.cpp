#include "BlendNode.hpp"
#include "Skeleton.hpp"
#include <map>

using namespace std;
using namespace boost;


namespace msynth
{


CBlendNode::CBlendNode()
:
m_Priority(0)
{
}


void CBlendNode::CreateFromSkeleton( const CBone& src_bone )
{
	m_Name = src_bone.GetName();
	m_vecpChild.resize( src_bone.GetNumChildren() );
	for( int i=0; i<(int)m_vecpChild.size(); i++ )
	{
		m_vecpChild[i] = shared_ptr<CBlendNode>( new CBlendNode );
		m_vecpChild[i]->m_pSelf = m_vecpChild[i];
		m_vecpChild[i]->CreateFromSkeleton( src_bone.GetChild(i) );
	}
}


shared_ptr<CBlendNode> CBlendNode::GetBlendNodeByName( const std::string& name )
{
	if( m_Name == name )
	{
		return m_pSelf.lock();
	}

	for( int i=0; i<(int)m_vecpChild.size(); i++ )
	{
		shared_ptr<CBlendNode> pNode = m_vecpChild[i]->GetBlendNodeByName( name );

		if( pNode )
			return pNode;
	}

	return shared_ptr<CBlendNode>();
}


} // namespace msynth
