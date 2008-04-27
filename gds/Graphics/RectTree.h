
#ifndef  __RECTTREE_H__
#define  __RECTTREE_H__



#include "Rect.h"


namespace Graphics
{


// packs rectangular images to one larger rectangle

class CRectTree;



class CRectNode
{
	CRectNode *m_pChild[2];
	SRect m_Rectangle;
	int m_iIndex;
	
public:

	CRectNode();
	~CRectNode();

//	CRectNode operator=(CRectNode node);

	CRectNode *Insert( const SRect& rect );

	CRectNode *GetNode( const int index );

	bool IsLeaf() const;

	enum eIndex
	{
		INVALID_INDEX = -1,
	};

	friend class CRectTree;
};


class CRectTree
{
	CRectNode *m_pRootNode;

public:

	CRectTree();

	CRectTree( const SRect& rect );

	~CRectTree();

	void SetRectangle( const SRect& rect );

	int Insert( const SRect& rect, const int index );

	inline SRect *GetRectangle( const int index );
};


//=============================== inline implementations ===============================


inline SRect *CRectTree::GetRectangle( const int index )
{
	CRectNode *pNode = m_pRootNode->GetNode( index );

	if( pNode )
		return &(pNode->m_Rectangle);
	else
		return NULL;
}


}  /*  Graphics  */


#endif		/*  __RECTTREE_H__  */
