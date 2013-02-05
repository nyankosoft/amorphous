#ifndef  __BSPTREEFORBOX_H__
#define  __BSPTREEFORBOX_H__

#include "Vector3.hpp"


namespace amorphous
{


class BSPTreeForBox
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

	BSPTreeForBox();
	~BSPTreeForBox();

	inline void SetSize( float width, float height, float depth );

	bool ClipTrace( Vector3& vEndPos,
		            float& fEndFrac,
					const Vector3& vStart,
					const Vector3& vGoal,
					const float fEpsilon );

};


//========================= inline implementations =========================

inline void BSPTreeForBox::SetSize( float width, float height, float depth )
{
	m_aDist[0] = width / 2.0f;
	m_aDist[1] = height / 2.0f;
	m_aDist[2] = depth / 2.0f;
}


} // namespace amorphous



#endif		/*  __BSPTREEFORBOX_H__  */
