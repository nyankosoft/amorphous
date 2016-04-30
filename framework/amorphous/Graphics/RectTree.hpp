#ifndef  __RECTTREE_H__
#define  __RECTTREE_H__


#include <memory>
#include "Rect.hpp"


namespace amorphous
{


// packs rectangular images to one larger rectangle

class RectTree;



class RectNode
{
	std::unique_ptr<RectNode> m_pChild[2];
	SRect m_Rectangle;
	int m_iIndex;
	
public:

	RectNode();
	~RectNode();

//	RectNode operator=(RectNode node);

	RectNode *Insert( const SRect& rect );

	RectNode *GetNode( const int index );

	bool IsLeaf() const;

	enum eIndex
	{
		INVALID_INDEX = -1,
	};

	friend class RectTree;
};


class RectTree
{
	std::unique_ptr<RectNode> m_pRootNode;

public:

	RectTree();

	RectTree( const SRect& rect );

	~RectTree();

	void SetRectangle( const SRect& rect );

	int Insert( const SRect& rect, const int index );

	inline SRect *GetRectangle( const int index );
};


//=============================== inline implementations ===============================


inline SRect *RectTree::GetRectangle( const int index )
{
	RectNode *pNode = m_pRootNode->GetNode( index );

	if( pNode )
		return &(pNode->m_Rectangle);
	else
		return nullptr;
}


} // namespace amorphous


#endif		/*  __RECTTREE_H__  */
