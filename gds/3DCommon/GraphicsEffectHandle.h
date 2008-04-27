#ifndef __GraphicsEffectHandle_H__
#define __GraphicsEffectHandle_H__


#include "3DCommon/fwd.h"


class CGraphicsEffectHandle
{
	CAnimatedGraphicsManager *m_pManager;

	int m_EffectIndex;

	int m_EffectID;

	static const CGraphicsEffectHandle ms_NullHandle; /// definition in GraphicsEffectManager.cpp

	CGraphicsEffectHandle( CAnimatedGraphicsManager *pMgr, int index, int id )
		:
	m_pManager(pMgr), m_EffectIndex(index), m_EffectID(id)
	{}

public:

	CGraphicsEffectHandle()
		:
	m_pManager(NULL), m_EffectIndex(-1), m_EffectID(-1)
	{}

	static const CGraphicsEffectHandle Null() { return ms_NullHandle; }

	friend class CAnimatedGraphicsManager;
};


#endif /* __GraphicsEffectHandle_H__ */
