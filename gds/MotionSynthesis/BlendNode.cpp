#include "BlendNode.hpp"
#include "Skeleton.hpp"
#include <map>

using namespace std;
using namespace boost;


namespace msynth
{


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


} // namespace msynth
