#ifndef  __BlendNode_HPP__
#define  __BlendNode_HPP__


#include <map>
#include "gds/3DMath/Quaternion.hpp"
#include "gds/3DMath/Transform.hpp"
#include "gds/Support/FixedVector.hpp"
#include "gds/MotionSynthesis/fwd.hpp"
#include "gds/MotionSynthesis/MotionPrimitive.hpp"
#include "gds/MotionSynthesis/MotionPrimitiveBlender.hpp"


namespace amorphous
{


namespace msynth
{


/// Need to share the same hiearchy with the skeleton which uses keyframes blended on this tree
class CBlendNode
{
	enum Params
	{
		NUM_MAX_BLENDS = 4,
	};

	boost::weak_ptr<CBlendNode> m_pSelf;

	std::string m_Name;

//	std::vector<CBlendNode> m_vecChild;

	/// Stores children as shared pointers to let motion primitives hold a pointer
	/// to a blend node as an entry point
	std::vector< boost::shared_ptr<CBlendNode> > m_vecpChild;

	// -1: no transform has been set.
	int m_Priority;

	TCFixedVector<Transform,NUM_MAX_BLENDS> m_vecTransformNode;

public:

	CBlendNode();

	void SetSelf( boost::weak_ptr<CBlendNode> pSelf ) { m_pSelf = pSelf; }

	void CreateFromSkeleton( const CBone& src_bone );

	/// Recursively sets transform nodes from src_node
	void SetTransformNodes( const CTransformNode& src_node )
	{
		if( m_vecTransformNode.size() < NUM_MAX_BLENDS )
		{
			m_vecTransformNode.push_back(
				Transform(
				src_node.GetLocalRotationQuaternion(),
				src_node.GetLocalTranslation()
				)
			);
		}

		int num_children = take_min( src_node.GetNumChildren(), (int)m_vecpChild.size() );
		for( int i=0; i<num_children; i++ )
			m_vecpChild[i]->SetTransformNodes( src_node.GetChildNode(i) );
	}

	boost::shared_ptr<CBlendNode> GetBlendNodeByName( const std::string& name );

	/// Recursively sets the transforms to a tree of transform nodes.
	/// Resizes the child buffers of dest nodes if necessary
	void GetTransformNode( CTransformNode& dest ) const
	{
		// blend transforms?
/*		for( int i=0; i<m_vecTransformNode.size(); i++ )
		{
			dest.
		}*/

		if( 0 < m_vecTransformNode.size() )
		{
			dest.SetRotation(    m_vecTransformNode[0].qRotation );
			dest.SetTranslation( m_vecTransformNode[0].vTranslation );
		}

//		int num_children = take_min( dest.GetNumChildren(), (int)m_vecpChild.size() );
		int num_children = (int)m_vecpChild.size();
		dest.Child().resize( num_children );
		for( int i=0; i<num_children; i++ )
			m_vecpChild[i]->GetTransformNode( dest.ChildNode(i) );
	}

	/// Extract keyframe
	void GetKeyframe( CKeyframe& dest ) const
	{
		GetTransformNode( dest.RootNode() );
	}

	void Clear()
	{
		m_vecTransformNode.resize( 0 );

		int num_children = (int)m_vecpChild.size();
		for( int i=0; i<num_children; i++ )
			m_vecpChild[i]->Clear();
	}
};



} // msynth

} // namespace amorphous



#endif /* __BlendNode_HPP__ */
