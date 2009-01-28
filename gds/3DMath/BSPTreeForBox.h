#ifndef  __BSPTREEFORBOX_H__
#define  __BSPTREEFORBOX_H__

#include "3DMath/Vector3.h"


class CBSPTreeForBox
{
	float m_aDist[3];

public:

	struct SBinaryNode
	{
		int child[2];

		enum eChild
		{
			CHILD_FRONT		= 0,
			CHILD_BACK		= 1,
			SOLID	= -1,
			EMPTY	= -2,
		};
	};

	CBSPTreeForBox();
	~CBSPTreeForBox();

	inline void SetSize( float width, float height, float depth );

	bool ClipTrace( Vector3& vEndPos,
		            float& fEndFrac,
					const Vector3& vStart,
					const Vector3& vGoal,
					const float fEpsilon );

};


//========================= inline implementations =========================

inline void CBSPTreeForBox::SetSize( float width, float height, float depth )
{
	m_aDist[0] = width / 2.0f;
	m_aDist[1] = height / 2.0f;
	m_aDist[2] = depth / 2.0f;
}



#endif		/*  __BSPTREEFORBOX_H__  */
