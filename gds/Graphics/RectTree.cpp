#include "Support/memory_helpers.hpp"
#include "RectTree.hpp"


namespace amorphous
{

//==================================================================================
// RectTree
//==================================================================================

RectTree::RectTree()
{
	m_pRootNode = NULL;
}


RectTree::RectTree( const SRect& rect )
{
	SetRectangle( rect );
}


void RectTree::SetRectangle( const SRect& rect )
{
	SafeDelete( m_pRootNode );
	m_pRootNode = new RectNode;
	m_pRootNode->m_Rectangle = rect;
}


RectTree::~RectTree()
{
	SafeDelete( m_pRootNode );
}

int RectTree::Insert( const SRect& rect, const int index )
{
	RectNode *pNode = m_pRootNode->Insert( rect );

	if( pNode != NULL )
	{
		pNode->m_iIndex = index;
		return 0;
	}
	else
		return RectNode::INVALID_INDEX;	// couldn't find any place to put the rectangle
}




//==================================================================================
// RectNode
//==================================================================================

RectNode::RectNode()
{
	m_pChild[0] = m_pChild[1] = NULL;

	m_iIndex = INVALID_INDEX;
}


RectNode::~RectNode()
{
	SafeDelete( m_pChild[0] );
	SafeDelete( m_pChild[1] );
}

bool RectNode::IsLeaf() const
{
	if( m_pChild[0] == NULL && m_pChild[1] == NULL )
		return true;
	else
		return false;
}

RectNode *RectNode::Insert( const SRect& rect )
{
	RectNode *pNewNode = NULL;

	if( !IsLeaf() )
	{
		// try inserting to first child
		pNewNode = m_pChild[0]->Insert( rect );
		if( pNewNode != NULL )
			return pNewNode;
		
		return m_pChild[1]->Insert( rect );
	}
	else
	{
		// if there's already a rectangle here, return
		if( m_iIndex != INVALID_INDEX )
			return NULL;

		// if we are too small, return 
		if( this->m_Rectangle.GetWidth() < rect.GetWidth() || this->m_Rectangle.GetHeight() < rect.GetHeight() )
			return NULL;

		int dw, dh;

		dw = m_Rectangle.GetWidth()  - rect.GetWidth();
		dh = m_Rectangle.GetHeight() - rect.GetHeight();

		// if we're just right, accept
		if( dw == 0 && dh == 0 )
			return this;

		// otherwise gotta split this node and create some kids
		m_pChild[0] = new RectNode;
		m_pChild[1] = new RectNode;

		// decide which way to split

		SRect& rc = m_Rectangle;
		if( dw > dh )
		{
			m_pChild[0]->m_Rectangle = SRect( rc.left, rc.top,
				                              rc.left + rect.GetWidth() - 1, rc.bottom );

			m_pChild[1]->m_Rectangle = SRect( rc.left + rect.GetWidth(), rc.top,
				                              rc.right, rc.bottom );
		}
		else
		{
			m_pChild[0]->m_Rectangle = SRect( rc.left,  rc.top,
				                              rc.right, rc.top + rect.GetHeight() - 1 );

			m_pChild[1]->m_Rectangle = SRect( rc.left,  rc.top + rect.GetHeight(),
				                              rc.right, rc.bottom );

		}

		// insert into first child that we created
		return m_pChild[0]->Insert( rect );
	}
}


RectNode *RectNode::GetNode( const int index )
{
	if( !IsLeaf() )
	{
		RectNode *pNode = m_pChild[0]->GetNode( index );

		if( pNode )
			return pNode;	// found the rectangle in the first sub-tree
		else
			return m_pChild[1]->GetNode( index );
	}
	else
	{
		if( index == m_iIndex )
			return this;
		else
			return NULL;
	}
}


} // namespace amorphous
