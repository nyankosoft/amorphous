
#include "Support/memory_helpers.h"

#include "RectTree.h"
using namespace Graphics;

//==================================================================================
// CRectTree
//==================================================================================

CRectTree::CRectTree()
{
	m_pRootNode = NULL;
}


CRectTree::CRectTree( const SRect& rect )
{
	SetRectangle( rect );
}


void CRectTree::SetRectangle( const SRect& rect )
{
	SafeDelete( m_pRootNode );
	m_pRootNode = new CRectNode;
	m_pRootNode->m_Rectangle = rect;
}


CRectTree::~CRectTree()
{
	SafeDelete( m_pRootNode );
}

int CRectTree::Insert( const SRect& rect, const int index )
{
	CRectNode *pNode = m_pRootNode->Insert( rect );

	if( pNode != NULL )
	{
		pNode->m_iIndex = index;
		return 0;
	}
	else
		return CRectNode::INVALID_INDEX;	// couldn't find any place to put the rectangle
}




//==================================================================================
// CRectNode
//==================================================================================

CRectNode::CRectNode()
{
	m_pChild[0] = m_pChild[1] = NULL;

	m_iIndex = INVALID_INDEX;
}


CRectNode::~CRectNode()
{
	SafeDelete( m_pChild[0] );
	SafeDelete( m_pChild[1] );
}

bool CRectNode::IsLeaf() const
{
	if( m_pChild[0] == NULL && m_pChild[1] == NULL )
		return true;
	else
		return false;
}

CRectNode *CRectNode::Insert( const SRect& rect )
{
	CRectNode *pNewNode = NULL;

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
		m_pChild[0] = new CRectNode;
		m_pChild[1] = new CRectNode;

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


CRectNode *CRectNode::GetNode( const int index )
{
	if( !IsLeaf() )
	{
		CRectNode *pNode = m_pChild[0]->GetNode( index );

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
