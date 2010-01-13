#ifndef  __BlendNode_HPP__
#define  __BlendNode_HPP__


#include <map>
#include <gds/3DMath/Quaternion.hpp>
#include <gds/Support/FixedVector.hpp>
#include <gds/MotionSynthesis/fwd.hpp>
#include <gds/MotionSynthesis/MotionPrimitive.hpp>
#include <gds/MotionSynthesis/MotionPrimitiveBlender.hpp>


class Transform
{
public:

	Vector3 t;
	Quaternion q;

public:

	Transform( const Vector3& _t = Vector3(0,0,0), const Quaternion& _q = Quaternion( Matrix33Identity() ) )
		:
	t(_t),
	q(_q)
	{
	}

	Matrix34 ToMatrix34() const
	{
		return Matrix34( t, q.ToRotationMatrix() );
	}

	void FromMatrix34( const Matrix34& src )
	{
		t = src.vPosition;
		q.FromRotationMatrix( src.matOrient );
	}
};


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

	/// Stores shared pointers to let motion primitives use this as entry point
	std::vector< boost::shared_ptr<CBlendNode> > m_vecpChild;

	// -1: no transform has been set.
	int m_Priority;

	TCFixedVector<Transform,NUM_MAX_BLENDS> m_vecTransformNode;

public:

	void SetSelf( boost::weak_ptr<CBlendNode> pSelf ) { m_pSelf = pSelf; }

	void CreateFromSkeleton( const CBone& src_bone );

	/// Recursively sets transform nodes
	void SetTransformNodes( const CTransformNode& src_node )
	{
		if( m_vecTransformNode.size() < NUM_MAX_BLENDS )
		{
			m_vecTransformNode.push_back(
				Transform(
				src_node.GetLocalTranslation(),
				src_node.GetLocalRotationQuaternion()
				)
			);
		}
	}

	boost::shared_ptr<CBlendNode> GetBlendNodeByName( const std::string& name )
	{
		using namespace boost;

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

	void GetTransformNode( CTransformNode& dest )
	{
		// blend transforms?
/*		for( int i=0; i<m_vecTransformNode.size(); i++ )
		{
			dest.
		}*/

		if( 0 < m_vecTransformNode.size() )
		{
			dest.SetRotation(    m_vecTransformNode[0].q );
			dest.SetTranslation( m_vecTransformNode[0].t );
		}

//		int num_children = take_min( dest.GetNumChildren(), (int)m_vecpChild.size() );
		int num_children = (int)m_vecpChild.size();
		dest.Child().resize( num_children );
		for( int i=0; i<num_children; i++ )
			m_vecpChild[i]->GetTransformNode( dest.ChildNode(i) );
	}

	/// Extract keyframe
	void GetKeyframe( CKeyframe& dest )
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


#endif /* __BlendNode_HPP__ */
