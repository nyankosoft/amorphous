#ifndef  __LinkNode_H__
#define  __LinkNode_H__


template<class T>
class CLinkNode
{
public:

	T *pOwner;
	CLinkNode *pNext;
	CLinkNode *pPrev;

public:

	CLinkNode()
		:
	pNext(NULL), pPrev(NULL), pOwner(NULL)
	{}

	void Unlink()
	{
		if( pNext )
		{
			pNext->pPrev = pPrev;
		}

		if( pPrev )
		{
			pPrev->pNext = pNext;
		}

		pNext = NULL;
		pPrev = NULL;
	}

	void InsertNext( CLinkNode *pNode )
	{
		pNode->pNext = pNext;
		pNode->pPrev = this;
		if( pNext )
			pNext->pPrev = pNode;
		pNext = pNode;
	}
};


#endif		/*  __LinkNode_H__  */
